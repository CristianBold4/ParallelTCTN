#include <iostream>
#include <iostream>
#include "GraphStream.h"
#include "Utils.h"
#include <string>
#include <cstdio>
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

    if (strcmp(project, "BuildOracle") == 0) {
        if (argc != 6) {
            std::cerr << "Usage: BuildOracle (dataset_path) (delta)"
                         " (type = [Exact, MinDeg]) (retaining_fraction) (output_path)\n";
            return 0;
        } else {
            std::string dataset_path(argv[1]);
            int delta = atoi(argv[2]);
            std::string type_oracle(argv[3]);
            double perc_retain = atof(argv[4]);
            std::string output_path(argv[5]);
            if (strcmp(type_oracle.c_str(), "Exact") != 0 and strcmp(type_oracle.c_str(), "MinDeg") != 0) {
                std::cerr << "Build Oracle - Error! Type of Oracle must be Exact or MinDeg.\n";
                return 0;
            }
            auto start = std::chrono::high_resolution_clock::now();
            Utils::build_oracle(dataset_path, delta, type_oracle, perc_retain, output_path);
            auto stop = std::chrono::high_resolution_clock::now();
            double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
            std::cout << "Oracle " << type_oracle << " successfully built in time: " << time << " s\n";
            return 0;
        }


    }

    // -- ParallelTCTN

    if (argc != 8) {
        std::cerr << "Usage: ParallelTCTN \n";
        return 0;
    }



    return 0;

}
