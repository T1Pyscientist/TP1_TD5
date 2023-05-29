#include "greedy_solver.h"
#include <chrono>
#include "checker.h"


GreedySolver::GreedySolver() {}

GreedySolver::GreedySolver(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
    this->_objective_value = 0;
    this->_solution_status = 0;
    this->_solution_time = 0;
    this->_ratio_precio_km = std::vector<double>(this->_instance.n);
}

void GreedySolver::setInstance(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
}


/*
* For a given instance, finds an assigment solution
* --------------------- 
* It finds the closest car for each passenger, in the order
* of arriving requests
*/
void GreedySolver::solve() {

    // Initial execution time
    auto start = std::chrono::high_resolution_clock::now();

    // Instantiate solution
    this->_solution = TaxiAssignmentSolution(this->_instance.n);

    // Loop over each requests j
    for (int j = 0; j < this->_instance.n; j++)
    {
        // Find the closest available car
        double min_dist = std::numeric_limits<double>::infinity();
        int closest_car = 0;
        for (int i = 0; i < this->_instance.n; i++)
        {   
            // Check car is not assigned already and update
            if (!this->_solution.isTaxiAssigned(i)) {
                if(this->_instance.dist[i][j] < min_dist) {
                    min_dist  = this->_instance.dist[i][j];
                    closest_car = i;
                }
            }
        }

        // Assign closest car to request j in the solution
        this->_solution.assign(closest_car, j);

        // Set metrics of the assignment  
        this->_objective_value += min_dist;
        this->_ratio_precio_km[closest_car] = this->_instance.pax_tot_fare[j] / (min_dist + this->_instance.pax_trip_dist[j]);
    }

    // Final execution time
    auto end = std::chrono::high_resolution_clock::now();

    // Set execution time for the algorithm
    int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    this->_solution_time = double(duration);

    // Check feasibility of the solution
    TaxiAssignmentChecker checker = TaxiAssignmentChecker();
    checker.checkFeasibility(this->_instance, this->_solution);
    this->_solution_status = checker.getFeasibilityStatus();
    
}

double GreedySolver::getObjectiveValue() const {
    return this->_objective_value;
}

TaxiAssignmentSolution GreedySolver::getSolution() const {
    return this->_solution;
}

int GreedySolver::getSolutionStatus() const {
    return this->_solution_status;
}

double GreedySolver::getSolutionTime() const {
    return this->_solution_time;
}

double GreedySolver::getDolarKm() const{
    
     double total = 0;
     for (int j=0; j<this->_instance.n; j++){
        total += this->_ratio_precio_km[j];
     }

    return total / this->_instance.n;
}


