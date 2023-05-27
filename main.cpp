#include "taxi_assignment_instance.h"
#include "taxi_assignment_solution.h"
#include "checker.h"
#include "greedy_solver.h"
#include "taxi_assignment_batching_solver.h"
#include <fstream>


// Loops over all the files in the input folder
// and runs both algorithms on each instance.
// Saves results in results.csv file
int main(int argc, char** argv) {

    std::vector<std::string> n_sizes = {"xl", "large", "medium", "small"};
    std::fstream file;
    
    // Open file and write header
    file.open ("results.csv");
    file << "size,n,timeG,timeB,ObjValG,ObjValB,PrecioKmG,PrecioKmB" << std::endl; 

    TaxiAssignmentInstance instance;

    for (int j = 0; j < n_sizes.size(); j++) {
        for(int i=0; i<10; i++) 
        {
            // Create an instance with of the problem
            std::string filename = "input/" + n_sizes[j] + "_" + std::to_string(i) + ".csv";
            instance = TaxiAssignmentInstance(filename);

            BatchingSolver batchingSolver(instance);
            GreedySolver greedySolver(instance);

            // Solve instance with each algorithm
            batchingSolver.solve();
            greedySolver.solve();

            TaxiAssignmentSolution solutionBatching = batchingSolver.getSolution();
            TaxiAssignmentSolution solutionGreedy = greedySolver.getSolution();

            // Write results into the file
            file << n_sizes[j] << "," <<instance.n << "," << greedySolver.getSolutionTime() << "," << batchingSolver.getSolutionTime()
            << "," << greedySolver.getObjectiveValue() << "," <<batchingSolver.getObjectiveValue() 
            << "," << greedySolver.getPrecioKm() << "," << batchingSolver.getKmDolar()
            << std::endl;
        }
    }

    file.close();

    return 0;
}