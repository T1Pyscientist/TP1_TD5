#include "greedy_solver.h"
#include <chrono>
#include "checker.h"

GreedySolver::GreedySolver() {}

GreedySolver::GreedySolver(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
    this->_objective_value = 0;
    this->_solution_status = 0;
    this->_solution_time = 0;
    this->_precio_km = 0;
}

void GreedySolver::setInstance(TaxiAssignmentInstance &instance) {
    this->_instance = instance;
}

void GreedySolver::solve() {

    auto start = std::chrono::high_resolution_clock::now();

    this->_solution = TaxiAssignmentSolution(this->_instance.n);


    for (int j = 0; j < this->_instance.n; j++)
    {
        double min = std::numeric_limits<double>::infinity();
        int conductor = 0;
        for (int i = 0; i < this->_instance.n; i++)
        {
            if (!this->_solution.isTaxiAssigned(i)) {
                if(this->_instance.dist[i][j] < min) {
                    min  = this->_instance.dist[i][j];
                    conductor = i;
                }
            }
        }
        this->_solution.assign(conductor, j);
        this->_objective_value += min;
        this->_precio_km += (min / this->_instance.pax_tot_fare[j]);
    }

    // Registrar el tiempo de finalización
    auto end = std::chrono::high_resolution_clock::now();

    // Calcular la duración en milisegundos
    int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    this->_solution_time = double(duration);

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

double GreedySolver::getPrecioKm() const{
    return this->_precio_km / this->_instance.n;
}


