#include "taxi_assignment_instance.h"
#include "taxi_assignment_solution.h"
#include "checker.h"
#include "greedy_solver.h"
#include "taxi_assignment_batching_solver.h"
#include "min_cost_flow_solver.h"


int main(int argc, char** argv) {
    std::vector<std::string> n_sizes = {"xl", "large", "medium", "small"};

    std::cout << "size\ttimeG\ttimeB\tObjValG\tObjValB" << std::endl; 

    TaxiAssignmentInstance instance1;
    TaxiAssignmentInstance instance2;

    for (int j = 0; j < n_sizes.size(); j++) {
        for(int i=0; i<10; i++) 
        {
            std::string filename = "input/" + n_sizes[j] + "_" + std::to_string(i) + ".csv";

            instance1 = TaxiAssignmentInstance(filename);
            instance2 = TaxiAssignmentInstance(filename);
            BatchingSolver batchingSolver(instance1);
            GreedySolver greedySolver(instance2);

            batchingSolver.solve();
            greedySolver.solve();

            TaxiAssignmentSolution solutionBatching = batchingSolver.getSolution();
            TaxiAssignmentSolution solutionGreedy = greedySolver.getSolution();

            std::cout << n_sizes[j] << "\t" << greedySolver.getSolutionTime() << "\t" << batchingSolver.getSolutionTime()
            << "\t" << greedySolver.getObjectiveValue() << "\t" <<batchingSolver.getObjectiveValue()
            << std::endl;
        }
    }

    return 0;
}