#include "checker.h"
#include <set>

TaxiAssignmentChecker::TaxiAssignmentChecker() :_feasibility_status(std::vector<bool>(3,false)) {}

bool TaxiAssignmentChecker::checkFeasibility(const TaxiAssignmentInstance &instance, const TaxiAssignmentSolution &solution) {
    bool ret =  this->_checkValuesInRange(instance, solution) && 
                this->_checkTaxiUnique(instance, solution) &&  
                this->_checkPaxUnique(instance, solution);

    return ret;
}

double TaxiAssignmentChecker::getSolutionCost(const TaxiAssignmentInstance &instance, const TaxiAssignmentSolution &solution) {

    double ret = 0;

    for (int i = 0; i < instance.n; i++)
    {
        int j = solution.getAssignedPax(i);
        ret += instance.dist[i][j];
    }

    return ret;
}

int TaxiAssignmentChecker::getFeasibilityStatus() {
    int vr = 2;
    for (int i = 0; i < this->n_constraints; i++)
    {
        if (!this->_feasibility_status[i]) {
            vr = 3;
        }
    }
    return vr;   
}


bool TaxiAssignmentChecker::_checkValuesInRange(const TaxiAssignmentInstance &instance, const TaxiAssignmentSolution &solution) {
    bool vr = true;
    int n = instance.n;
    for (int i = 0; i < n; i++)
    {
        if(solution.getAssignedPax(i) >=n  || solution.getAssignedPax(i) < 0) {
            vr = false;
        }

        if(solution.getAssignedTaxi(i) >=n  || solution.getAssignedTaxi(i) < 0) {
            vr = false;
        }

    }

    this->_feasibility_status[0] = vr;
    
    return vr;
}

bool TaxiAssignmentChecker::_checkPaxUnique(const TaxiAssignmentInstance &instance, const TaxiAssignmentSolution &solution) {

    std::set<int> pasajeros = {};
    int  n = instance.n;

    for (int i = 0; i < n; i++)
    {
        pasajeros.insert(solution.getAssignedPax(i));
    }
    
    this->_feasibility_status[1] = pasajeros.size() == n;

    return pasajeros.size() == n;

}

bool TaxiAssignmentChecker::_checkTaxiUnique(const TaxiAssignmentInstance &instance, const TaxiAssignmentSolution &solution) {

    std::set<int> taxis = {};
    int  n = instance.n;

    for (int i = 0; i < n; i++)
    {
        taxis.insert(solution.getAssignedTaxi(i));
    }

    this->_feasibility_status[2] = taxis.size() == n;
    return taxis.size() == n;

}