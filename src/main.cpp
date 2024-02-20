#include <iostream>
#include "GraphStream.h"
#include "Utils.h"
#include "WRPSampling.h"
#include <string>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <cassert>
#include <omp.h>

WRPSampling wrp_sampling(const std::string &filename, const char &delimiter, int skip, int memory_budget,
                         int random_seed, double alpha, double beta,
                         const ankerl::unordered_dense::map<long, int> &heaviness_oracle) {

    GraphStream graph_stream(filename, delimiter, skip);

    WRPSampling WRP_algo(memory_budget, random_seed, alpha, beta, heaviness_oracle);
    long t = 0;

    while (graph_stream.has_next()) {

        EdgeStream current_edge = graph_stream.next();

        WRP_algo.process_edge(current_edge.u, current_edge.v);
        t++;
        // -- output log
        if (t % 5000000 == 0) {
            std::cout << "Processed " << t << " edges || Estimated count T = " << WRP_algo.get_global_triangles()
                      << "\n";
        }

    }

    return WRP_algo;

}

/* Use if POSIX basename() is unavailable */
char *base_name(char *s)
{
    char *start;

    /* Find the last '/', and move past it if there is one.  Otherwise return
       a copy of the whole string. */
    /* strrchr() finds the last place where the given character is in a given
       string.  Returns NULL if not found. */
    if ((start = strrchr(s, '/')) == NULL) {
        start = s;
    } else {
        ++start;
    }
    /* If you don't want to do anything interesting with the returned value,
       i.e., if you just want to print it for example, you can just return
       'start' here (and then you don't need dup_str(), or to free
       the result). */
    return start;
}

int main(int argc, char **argv) {

    // -- get the last path after backslash
    char* project = base_name(argv[0]);
    char delimiter;
    int skip;
    if (strcmp(project, "DataPreprocessing") == 0) {
        if (argc != 5) {
            std::cerr << "Usage: DataPreprocessing (dataset_path) (delimiter) (skip)"
                         " (output_path)\n";
            return 0;
        } else {
            std::string dataset_path(argv[1]);
            char* delim = (argv[2]);
            // delimiter = ' ';
            skip = atoi(argv[3]);
            std::string output_path(argv[4]);
            auto start = std::chrono::high_resolution_clock::now();
            Utils::preprocess_data(dataset_path, delim, skip, output_path);
            auto stop = std::chrono::high_resolution_clock::now();
            double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
            std::cout << "Dataset preprocessed in time: " << time << " s\n";
            return 0;

        }

    }

    if (strcmp(project, "BuildOracle") == 0) {
        if (argc != 7) {
            std::cerr << "Usage: BuildOracle (dataset_path) (delimiter) (skip)"
                         " (type = [Exact, MinDeg]) (retaining_fraction) (output_path)\n";
            return 0;
        } else {
            std::string dataset_path(argv[1]);
            delimiter = *(argv[2]);
            skip = atoi(argv[3]);
            std::string type_oracle(argv[4]);
            double perc_retain = atof(argv[5]);
            std::string output_path(argv[6]);
            if (strcmp(type_oracle.c_str(), "Exact") != 0 and strcmp(type_oracle.c_str(), "MinDeg") != 0) {
                std::cerr << "Build Oracle - Error! Type of Oracle must be Exact or MinDeg.\n";
                return 0;
            }
            auto start = std::chrono::high_resolution_clock::now();
            Utils::build_oracle(dataset_path, delimiter, skip, type_oracle, output_path, perc_retain);
            auto stop = std::chrono::high_resolution_clock::now();
            double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
            std::cout << "Oracle " << type_oracle << " successfully built in time: " << time << " s\n";
            return 0;
        }


    }

    if (argc < 8) {
        std::cerr << "Usage: Tonic (random_seed) (memory_budget) (alpha)"
                     " (beta) (dataset_path) (output_path) ([list of oracles...])\n";
        return 0;
    }

    int random_seed = atoi(argv[1]);
    long memory_budget = atol(argv[2]);
    double alpha = atof(argv[3]);
    double beta = atof(argv[4]);

    srand(random_seed);
    std::string filename(argv[5]);

    // -- by default, since the data should have been preprocessed, delimiter = ' ', number of lines to be skipped = 0
    // -- change here if you have other types of input datasets !!
    delimiter = ' ';
    skip = 0;
    std::string out_path(argv[6]);


    int n_oracles = (argc - 7);
    std::vector<std::string> oracle_list;
    for (int idx_oracle=7; idx_oracle < argc; idx_oracle++) {
	std::string oracle_filename(argv[idx_oracle]);
	oracle_list.push_back(oracle_filename);
	std::cout << oracle_list[idx_oracle - 7] << "\n";
    }


    std::vector<double> partial_results;
    std::vector<double> partial_times;

    omp_set_dynamic(0);
    omp_set_num_threads(n_oracles);

    #pragma omp parallel for
    for (auto oracle_path : oracle_list){
    
	    // -- read oracle
	    ankerl::unordered_dense::map<long, int> heaviness_oracle;
	    auto start = std::chrono::high_resolution_clock::now();
	    Utils::read_oracle(oracle_path, delimiter, skip, heaviness_oracle);
	    auto stop = std::chrono::high_resolution_clock::now();
		    
	    double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
	    std::cout << "Oracle " << oracle_path << " successfully read in time: " << time << " s\n";
	    // -- run main algo
	    start = std::chrono::high_resolution_clock::now();
	    WRPSampling WRP_algo = wrp_sampling(filename, delimiter, skip, memory_budget, random_seed, alpha, beta,
                                       heaviness_oracle);
	    stop = std::chrono::high_resolution_clock::now();
	    time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
	    
	    // -- write results
	    double global_count = WRP_algo.get_global_triangles();
	    partial_results.push_back(global_count); 
	    partial_times.push_back(time);
	    
	    
	    std::cout << "# Thread " << omp_get_thread_num() << "---> Estimated global count: " << global_count <<
		         " || Cpu Time elapsed: " << time << " s\n";
    }


    // -- output_files
    std::ofstream outFile(out_path + "_global_count.txt");
    // std::ofstream outFile_local(out_path + "_local_counts.txt");


    for (int idx=0; idx < partial_results.size(); idx++)
    	outFile << "Global Count: " << std::fixed << partial_results[idx] << "\nTime Elapsed: " << partial_times[idx] << "(s)\n";

    /*
    ankerl::unordered_dense::map<int, double> local_triangles;
    WRP_algo.get_local_triangles_map(local_triangles);
    outFile_local << "Local Triangles Counts:\n";
    for (auto &node_to_triangle: local_triangles) {
        outFile_local << node_to_triangle.first << "\t" << std::fixed << node_to_triangle.second << "\n";
    }
    */


    outFile.close();
    // outFile_local.close();

    
    return 0;


}
