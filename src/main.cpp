#include "../include/algorithms.h"

int main(int argc, char *argv[]) {

    if(argc < 5) {
        std::cout << "Usage:\n./TCTN <dataset_path> <delta> exact <output_file>\n"
                     "./TCTN <dataset_path> <delta> sampling <sampling probability> <trials> <seed> <output_file>\n"
                     "./TCTN <dataset_path> <delta> oracle <sampling probability> <oracle_file> <trials> <seed> <output_file>\n"
                  << std::endl;
        return 1;
    }

    std::string dataset_path = argv[1];
    int delta = std::stoi(argv[2]);

    auto start = std::chrono::high_resolution_clock::now();

    std::string delimiter = " ";
    auto temporal_edges = load_preprocessed_edges(dataset_path, delimiter);

    auto end = std::chrono::high_resolution_clock::now();

    chrono_t diff = end - start;

    std::cout << "Time to load edges " << diff.count() << std::endl;


    std::cout << "Dataset: " << dataset_path << std::endl;
    std::cout << temporal_edges.size() << " edges loaded" << std::endl;
    std::cout << "delta : " << delta << " s" << std::endl;

    std::string algorithm = argv[3];

    if(algorithm == "exact") {

        std::cout << "Running exact algorithm" << std::endl;

        std::string exact_out_file = argv[4];

        std::tuple<counts, chrono_t, double, double, EdgeMap> res = exact_algorithm(temporal_edges, delta);

        counts exact_counts = std::get<0>(res);
        chrono_t exact_time = std::get<1>(res);
        double exact_avg_memory = std::get<2>(res);
        double exact_max_memory = std::get<3>(res);

        save_results(exact_counts,
                     exact_time,
                     exact_avg_memory,
                     exact_max_memory,
                     exact_out_file);

        std::cout << "Exact algorithm done" << std::endl;
    }

    else if(algorithm == "sampling") {

        if(argc < 8) {
            std::cout << "Usage:./TCTN <dataset_path> <delta> sampling <sampling probability> <trials> <seed> <output_file>\n";
            return 1;
        }

        float p = std::stof(argv[4]);
        int trials = std::stoi(argv[5]);
        int seed = std::stoi(argv[6]);
        std::string sampling_out_file = argv[7];

        std::cout << "Running sampling algorithm" << std::endl;
        std::cout << "sampling probability: " << p << std::endl;
        std::cout << "trials: " << trials << std::endl;
        std::cout << "seed: " << seed << std::endl;

        std::vector<counts > all_sampling_counts;
        std::vector<chrono_t > all_sampling_times;
        std::vector<double> all_sampling_avg_memory;
        std::vector<double> all_sampling_max_memory;

        for (int i = 0; i < trials; i++) {

            std::default_random_engine rand_eng(seed + i);

            auto [sampling_counts,
                    sampling_time,
                    sampling_avg_memory,
                    sampling_max_memory] = sampling_algorithm(temporal_edges, delta, p, rand_eng);

            all_sampling_counts.push_back(sampling_counts);
            all_sampling_times.push_back(sampling_time);
            all_sampling_avg_memory.push_back(sampling_avg_memory);
            all_sampling_max_memory.push_back(sampling_max_memory);
        }

        save_multiple_results(all_sampling_counts,
                              all_sampling_times,
                              all_sampling_avg_memory,
                              all_sampling_max_memory,
                              sampling_out_file);

        std::cout << "Sampling algorithm done" << std::endl;
    }

    else {

        if(argc < 9) {
            std::cout << "Usage:./TCTN <dataset_path> <delta> oracle <sampling probability> <oracle_file> <trials> <seed> <output_file>\n";
            return 1;
        }

        float p = std::stof(argv[4]);
        std::string oracle_file = argv[5];
        int trials = std::stoi(argv[6]);
        int seed = std::stoi(argv[7]);
        std::string oracle_out_file = argv[8];

        std::cout << "Running oracle algorithm" << std::endl;
        std::cout << "sampling probability: " << p << std::endl;
        std::cout << "trials: " << trials << std::endl;
        std::cout << "seed: " << seed << std::endl;

        std::vector<counts> all_oracle_counts;
        std::vector<chrono_t> all_oracle_times;
        std::vector<double> all_oracle_avg_memory;
        std::vector<double> all_oracle_max_memory;
        auto oracle = load_oracle(oracle_file);
        std::cout << "Oracle size " << oracle.size() << std::endl;

        for (int i = 0; i < trials; i++) {

            std::default_random_engine rand_eng(seed + i);

            auto [oracle_counts,
                    oracle_time,
                    oracle_avg_memory,
                    oracle_max_memory] = oracle_algorithm(temporal_edges, delta, p, oracle, rand_eng);

            all_oracle_counts.push_back(oracle_counts);
            all_oracle_times.push_back(oracle_time);
            all_oracle_avg_memory.push_back(oracle_avg_memory);
            all_oracle_max_memory.push_back(oracle_max_memory);
        }

        save_multiple_results(all_oracle_counts,
                              all_oracle_times,
                              all_oracle_avg_memory,
                              all_oracle_max_memory,
                              oracle_out_file);

        std::cout << "Oracle algorithm done" << std::endl;

    }

    return 0;
}
