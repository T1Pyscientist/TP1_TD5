#include "taxi_assignment_instance.h"
#include "taxi_assignment_solution.h"
#include "checker.h"
#include "greedy_solver.h"


int main(int argc, char** argv) {
    std::vector<std::string> n_sizes = {"xl", "large", "medium", "small"};

    for (int j = 0; j < 4; j++)
        for(int i=0; i<10; i++) {
        {
            std::string filename = "input/" + n_sizes[j] + "_" + std::to_string(i) + ".csv";
            std::cout << filename << std::endl;
            TaxiAssignmentInstance instance(filename);
            TaxiAssignmentSolution solution(instance.n);
        }
    }


    // GreedySolver solver(instance);

    // solver.solve();

    // std::cout << solver.getObjectiveValue() << std::endl;
    // std::cout << solver.getSolutionTime() << std::endl;

    return 0;
}