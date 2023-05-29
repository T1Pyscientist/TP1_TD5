#include "taxi_assignment_instance.h"
#include "taxi_assignment_solution.h"
#include "checker.h"
#include "greedy_solver.h"
#include "taxi_assignment_batching_solver.h"
#include "taxi_assignment_batching_solver_ratio.h"
#include <fstream>


// Loops over all the files in the input folder
// and runs both algorithms on each instance.
// Saves results in results.csv file
int main(int argc, char** argv) {

    std::vector<std::string> n_sizes = {"xl", "large", "medium", "small"};
    std::fstream file;
    
    // Open file and write header
    file.open ("results.csv");
    file << "size,n,timeG,timeB,ObjValG,ObjValB,ObjValBR,PrecioKmG,PrecioKmB,PrecioKmBRatio" << std::endl; 

    TaxiAssignmentInstance instance;

    for (int j = 0; j < n_sizes.size(); j++) {
        for(int i=0; i<10; i++) 
        {
            // Create an instance of the problem for the current file
            std::string filename = "input/" + n_sizes[j] + "_" + std::to_string(i) + ".csv";
            instance = TaxiAssignmentInstance(filename);

            BatchingSolver batchingSolver(instance);
            BatchingSolverRatio batchingSolverRatio(instance);
            GreedySolver greedySolver(instance);

            // Solve instance with each algorithm
            batchingSolver.solve();
            batchingSolverRatio.solve();
            greedySolver.solve();

            TaxiAssignmentSolution solutionBatching = batchingSolver.getSolution();
            TaxiAssignmentSolution solutionGreedy = greedySolver.getSolution();

            // TODO: CHECK SOLUTIONS WITH CHECKER CLASS??

            // Write results into the file
            file << n_sizes[j] << "," <<instance.n << "," << greedySolver.getSolutionTime() << "," << batchingSolver.getSolutionTime()
            << "," << greedySolver.getObjectiveValue() << "," <<batchingSolver.getObjectiveValue() << ',' << batchingSolverRatio.getObjectiveValue()
            << "," << greedySolver.getDolarKm() << "," << batchingSolver.getDolarKm() << ',' << batchingSolverRatio.getDolarKm()
            << std::endl;
        }
    }

    file.close();


    return 0;
}