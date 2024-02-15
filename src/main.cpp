#include <iostream>
#include "Utils.h"
#include "ParallelTCTN.h"
#include <string>
#include <cstdlib>
#include <chrono>
#include <cassert>


// -- used to retrieve absolute name of the executed program
char *base_name(char *s)
{
    char *start;
    if ((start = strrchr(s, '/')) == NULL) {
        start = s;
    } else {
        ++start;
    }
    return start;
}

int main(int argc, char** argv) {

    // -- get the last path after backslash
    char* project = base_name(argv[0]);
    char delimiter = ' ';
    int skip = 0;
    std::cout << "Cpp Version: " << __cplusplus << "\n";

    if (strcmp(project, "BuildExact") == 0) {
        if (argc != 7) {
            std::cerr << "Usage: BuildExact (dataset_path) (delta)"
                         " (type = [Exact, MinDeg]) (retaining_fraction) (exact_output_path) (oracle_output_path)\n";
            return 0;
        } else {
            std::string dataset_path(argv[1]);
            int delta = atoi(argv[2]);
            std::string type_oracle(argv[3]);
            double perc_retain = atof(argv[4]);
            std::string exact_output_path(argv[5]);
            std::string oracle_output_path(argv[6]);
            if (strcmp(type_oracle.c_str(), "Exact") != 0 and strcmp(type_oracle.c_str(), "MinDeg") != 0) {
                std::cerr << "Build Oracle - Error! Type of Oracle must be Exact or MinDeg.\n";
                return 0;
            }
            auto start = std::chrono::high_resolution_clock::now();
            Utils::build_ground_truth(dataset_path, delta, type_oracle, perc_retain, exact_output_path, oracle_output_path);
            auto stop = std::chrono::high_resolution_clock::now();
            double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
            std::cout << "Oracle " << type_oracle << " successfully built in time: " << time << " s\n";
            return 0;
        }


    }

    // -- ParallelTCTN

    if (argc != 6) {
        std::cerr << "Usage: ParallelTCTN (dataset_path) (delta) (p_sampling) "
                     "(oracle_path) (output_path)\n";
        return 0;
    } else {
        std::string dataset_path (argv[1]);
        int delta = atoi(argv[2]);
        double p = atof(argv[3]);
        std::string oracle_path (argv[4]);
        std::string output_path (argv[5]);
        EdgeSet oracle;
        std::cout << "Reading Oracle...\n";
        auto start = std::chrono::high_resolution_clock::now();
        Utils::read_oracle(oracle_path, oracle);
        auto stop = std::chrono::high_resolution_clock::now();
        double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
        std::cout << "Oracle read in time: " << time << " s | Size Oracle = " << oracle.size() << " edges\n";

        // -- run TCTN
        GraphStream gs (dataset_path, ' ', 0);
        ParallelTCTN algo (delta, p, oracle);
        EdgeStream curr;
        start = std::chrono::high_resolution_clock::now();
        long nline = 0;
        while (gs.has_next()) {
            curr = gs.next();
            algo.process_edge(curr.u, curr.v, curr.time);
            nline++;
            if (nline % 300000 == 0) {
                std::cout << "Processed " << nline << " edges...\n";
            }
        }

        stop = std::chrono::high_resolution_clock::now();
        time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
        std::cout << "Done!\nTotal Delta-Triangles Instances -> " << algo.get_total_triangles() << "\n";
        // -- write results
        std::ofstream out_file(output_path);
        for (auto cnt : algo.get_triangles_estimates())
            out_file << cnt << " ";
        out_file << time;
        out_file.close();
    }

    return 0;

}
