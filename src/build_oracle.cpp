#include "../include/algorithms.h"

int main(int argc, char *argv[]) {

    if(argc < 5) {
        std::cout << "Usage: ./build_oracle "
                     "<dataset_path> "
                     "<delta> "
                     "<percentage_retain> "
                     "<oracle type> "
                     "<oracle_file> "
                     "<edge_map_file> "<< std::endl;
        return 1;
    }

    std::string dataset_path = argv[1];
    int delta = std::stoi(argv[2]);
    float percentage_retain = std::stof(argv[3]);
    std::string oracle_type = argv[4];
    std::string oracle_file = argv[5];

    std::string delimiter = " ";
    auto temporal_edges = load_preprocessed_edges(dataset_path, delimiter);

    if(oracle_type == "min_deg") {
        auto temporal_degrees = compute_temporal_degrees(temporal_edges, delta);
        auto static_degrees = compute_static_degrees(temporal_edges, delta);
        EdgeSet oracle = degree_oracle(temporal_edges, temporal_degrees, static_degrees, percentage_retain);
        save_oracle(oracle, oracle_file);
    }
    else {

        EdgeMap edge_map;
        std::string edge_map_file;

        if(oracle_type != "min_deg" && argc > 6) {
            edge_map_file = argv[6];
            edge_map = load_edge_map(edge_map_file);
            std::cout << "Loaded edge_map successfully" << std::endl;
        }
        else {
            std::tuple<counts, chrono_t, double, double, EdgeMap> res = exact_algorithm(temporal_edges, delta);
            edge_map = std::get<4>(res);
        }

        EdgeSet oracle = perfect_oracle(edge_map, percentage_retain, false);
        save_oracle(oracle, oracle_file);
    }

    return 0;
}

