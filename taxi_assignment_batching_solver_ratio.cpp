#include "taxi_assignment_batching_solver_ratio.h"


BatchingSolverRatio::BatchingSolverRatio() {}

BatchingSolverRatio::BatchingSolverRatio(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
    this->_objective_value = 0;
    this->_solution_status = 0;
    this->_solution_time = 0;
    this->_ratio_precio_km = std::vector<double>(this->_instance.n);
}

void BatchingSolverRatio::setInstance(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
}

void BatchingSolverRatio::solve() {

    auto start = std::chrono::high_resolution_clock::now();
    this->_solution = TaxiAssignmentSolution(this->_instance.n);

    // Create the network flow instance.
    this->_createMinCostFlowNetworkRatio();


    // Compute the solution for min cost flow
    this->_solution_status = this->_min_cost_flow.Solve();

    if (this->_solution_status == operations_research::MinCostFlow::OPTIMAL) {
        
        for (int i = 0; i < this->_min_cost_flow.NumArcs(); ++i) {
            int64_t flow = this->_min_cost_flow.Flow(i);
            // Ignore arcs with flow 0 (no assignment) and unit cost 0 (arcs from source and sink nodes)
            if (flow == 0 || _min_cost_flow.UnitCost(i) ==0) continue;
            int car = this->_min_cost_flow.Tail(i)-1;
            int request = this->_min_cost_flow.Head(i)-this->_instance.n-1;
            this->_solution.assign(car, request);
            this->_objective_value += this->_instance.dist[car][request];
            this->_ratio_precio_km[car] = this->_instance.pax_tot_fare[request] / (this->_instance.dist[car][request] + this->_instance.pax_trip_dist[request]);
        }
    } else {
        std::cout << "Solving the min cost flow problem failed. Solver status: "
                << this->_solution_status << std::endl;
    }

    // Registrar el tiempo de finalización
    auto end = std::chrono::high_resolution_clock::now();

    // Calcular la duración en microsegundos
    int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    this->_solution_time = double(duration);

}

double BatchingSolverRatio::getObjectiveValue() const {
    return this->_objective_value;
}

TaxiAssignmentSolution BatchingSolverRatio::getSolution() const {
    return this->_solution;
}

int BatchingSolverRatio::getSolutionStatus() const {
    return this->_solution_status;
}

double BatchingSolverRatio::getSolutionTime() const {
    return this->_solution_time;
}

double BatchingSolverRatio::getDolarKm() const  {

     double total = 0;
     for (int j=0; j<this->_instance.n; j++){
        total += this->_ratio_precio_km[j];
     }

    return total / this->_instance.n;
}

void BatchingSolverRatio::_createMinCostFlowNetworkRatio() {
    // Creates a network flow with source and sink nodes

    int n = this->_instance.n;
    int total_arcs = n*n + (2*n);
    int total_nodes = 2*n + 2;

    // Initialize graph structures.
    std::vector<int64_t> start_nodes(total_arcs, -1);
    std::vector<int64_t> end_nodes(total_arcs, -1);
    std::vector<int64_t> capacities(total_arcs, 1);
    std::vector<int64_t> unit_costs(total_arcs, 0);

    // Arcs from source node to cars, and  requests to sink node 
    for (int i=0; i < n; i++){
        // source -> cars
        start_nodes[i] = 0;
        end_nodes[i] = i + 1; // Offset 1 bc of source node

        // requests -> sink
        start_nodes[total_arcs - n + i] = (n+1) + i ; // Offset (n+1) bc of source node and n car nodes
        end_nodes[total_arcs - n + i] = total_nodes - 1;
    }

    // Arcs from cars to requests
    int arc_index = n;
    for (int i = 0; i < n; i++) {
        for (int j = n; j < 2*n; j++) {
            // Offset 1 because of source node
            start_nodes[arc_index] = i+1;
            end_nodes[arc_index] = j+1;
            // cost x10 to compute with integers
            unit_costs[arc_index] = int(-100*(this->_instance.pax_tot_fare[j-n] / (this->_instance.dist[i][j - n] + this->_instance.pax_trip_dist[j-n])));
            arc_index++;
        }
    }


    // Create the supplies.
    // All supplies are 0, except for source and sink nodes
    std::vector<int64_t> supplies(total_nodes, 0);
    supplies[0] = n;
    supplies[total_nodes-1] = -n;

    // Create the digraph
    // Add each arc.
    for (int i = 0; i < start_nodes.size(); ++i) {
        int arc = this->_min_cost_flow.AddArcWithCapacityAndUnitCost(start_nodes[i], end_nodes[i], capacities[i], unit_costs[i]);
        if (arc != i) LOG(FATAL) << "Internal error";
    }

    // Add node supplies.
    for (int i = 0; i < supplies.size(); ++i) {
        this->_min_cost_flow.SetNodeSupply(i, supplies[i]);
    }
}