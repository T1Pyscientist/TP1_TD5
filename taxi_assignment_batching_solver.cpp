#include "taxi_assignment_batching_solver.h"


BatchingSolver::BatchingSolver() {}

BatchingSolver::BatchingSolver(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
    this->_objective_value = 0;
    this->_solution_status = 0;
    this->_solution_time = 0;
    this->_km_dolar = 0;
}

void BatchingSolver::setInstance(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
}

void BatchingSolver::solve() {

    auto start = std::chrono::high_resolution_clock::now();
    this->_solution = TaxiAssignmentSolution(this->_instance.n);

    // Create the min cost network flow instance.
    this->_createMinCostFlowNetwork();

    // Obtain the solve the problem.
    this->_solution_status = this->_min_cost_flow.Solve();

    // Obtain the solution, construct the corresponding object and record the desired parameters.
    if (this->_solution_status == operations_research::MinCostFlow::OPTIMAL) {
        this->_objective_value = double((this->_min_cost_flow.OptimalCost())/10);  // Divido por diez porque al armar el grafo los pesos se multiplicaron por 10
        
        for (int i = 0; i < this->_min_cost_flow.NumArcs(); ++i) {
            int64_t flow = this->_min_cost_flow.Flow(i);
            if (flow == 0) continue;
            this->_solution.assign(this->_min_cost_flow.Tail(i), this->_min_cost_flow.Head(i)-this->_instance.n);
            this->_km_dolar += this->_instance.dist[this->_min_cost_flow.Tail(i)][this->_min_cost_flow.Head(i)-this->_instance.n] / this->_instance.pax_tot_fare[this->_min_cost_flow.Head(i)-this->_instance.n];
        }
    } else {
        std::cout << "Solving the min cost flow problem failed. Solver status: "
                << this->_solution_status << std::endl;
    }

    // Registrar el tiempo de finalización
    auto end = std::chrono::high_resolution_clock::now();

    // Calcular la duración en milisegundos
    int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    this->_solution_time = double(duration);

}

double BatchingSolver::getObjectiveValue() const {
    return this->_objective_value;
}

TaxiAssignmentSolution BatchingSolver::getSolution() const {
    return this->_solution;
}

int BatchingSolver::getSolutionStatus() const {
    return this->_solution_status;
}

double BatchingSolver::getSolutionTime() const {
    return this->_solution_time;
}

double BatchingSolver::getKmDolar() const  {
    return this->_km_dolar /  this->_instance.n;
}


void BatchingSolver::_createMinCostFlowNetwork() {

    // Initialize graph structures.
    int n = this->_instance.n;
    std::vector<int64_t> start_nodes(n*n, -1);
    std::vector<int64_t> end_nodes(n*n, -1);
    std::vector<int64_t> capacities(n*n, 1);
    std::vector<int64_t> unit_costs(n*n, -1);

    // Complete the graph structures. 
    // Origin vertices (taxis) indexed from 0...n-1. 
    // Destination vertices (paxs) indexed from n...2n-1
    // unit_cost of (i,j) = dist[i][j]
    int cnt = 0;
    for (int i = 0; i < this->_instance.n; i++) {
        for (int j = this->_instance.n; j < 2*this->_instance.n; j++) {
            // capacities are always 1, defined when initialized.
            start_nodes[cnt] = i;
            end_nodes[cnt] = j;
            unit_costs[cnt] = 10*this->_instance.dist[i][j - n];
            cnt++;
        }
    }

    // Create the supplies.
    // supplies[i] = 1 for taxis, i = 0,...,n-1.
    // supplies[i] = -1 for paxs, i = n,...,2n-1.
    std::vector<int64_t> supplies(2*n, 0);
    for (int i = 0; i < this->_instance.n; i++) {
        supplies[i] = 1;
        supplies[n + i] = -1;
    }

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