#include "Utils.h"
#include "WRPSampling.h"

void Utils::read_oracle(std::string &oracle_filename, const char delimiter, int skip,
                        ankerl::unordered_dense::map<long, int> &oracle) {

    std::ifstream file(oracle_filename);
    std::string line;
    oracle.clear();

    if (file.is_open()) {

        long nline = 0;
        while (std::getline(file, line)) {

            std::stringstream ss(line);

            if (nline < skip) continue;
            int u, v, h;
            ss >> u >> v >> h;

            oracle.emplace(WRPSampling::edge_to_id(u, v), h);

            nline++;

        }

    } else {
        std::cerr << "Error! Oracle file not opened.\n";
    }

}


bool Utils::sort_edge_map_comp(const std::pair<Edge, int> &a, const std::pair<Edge, int> &b) {
    return a.second > b.second;
}

bool Utils::sort_edge_time(const std::pair<Edge, int> &a, const std::pair<Edge, int> &b) {
    return a.second < b.second;
}

void Utils::sort_edgemap(const ankerl::unordered_dense::map<Edge, int, hash_edge>& map, std::vector<std::pair<Edge, int>> &edge_map) {

    for (auto elem : map) {
        edge_map.emplace_back(elem);
    }

    std::sort(edge_map.begin(), edge_map.end(), sort_edge_map_comp);

}

void Utils::preprocess_data(const std::string& dataset_filepath, const char* delimiter, int skip, const std::string &output_path) {

    std::cout << "Preprocessing Dataset...\n";
    std::ifstream file(dataset_filepath);
    std::string line, su, sv;

    // -- edge stream
    ankerl::unordered_dense::map<Edge, int, hash_edge> edge_stream;

    // - graph
    ankerl::unordered_dense::map<int, ankerl::unordered_dense::set<int>> graph_stream;

    int u, v, t, du, dv, n_min, n_max;
    ankerl::unordered_dense::set<int> min_neighbors;

    int total_T = 0;
    if (file.is_open()) {

        int nline = 0;
        int num_nodes = 0;
        int num_edges = 0;

        t = 0;
        while (std::getline(file, line)) {
            nline++;
            if (nline % 2000000 == 0) {
                std::cout << "Processed " << nline << " edges...\n";
            }
            if (nline <= skip) continue;

            std::stringstream ss(line);
            char del = ' ';
            if (strcmp(delimiter, "\t") == 0) {
                del = '\t';
            }
            std::getline(ss, su, del);
            std::getline(ss, sv, del);

            u = stoi(su);
            v = stoi(sv);

            // -- check self-loops
            if (u == v) continue;
            t++;
            int v1 = (u < v) ? u : v;
            int v2 = (u < v) ? v : u;
            std::pair uv = std::make_pair(v1, v2);
            if (graph_stream[u].find(v) != graph_stream[u].end() and graph_stream[v].find(u) != graph_stream[v].end()){
                edge_stream[uv] = t;
                continue;
            }

            // -- add edge to graph stream
            graph_stream[u].emplace(v);
            graph_stream[v].emplace(u);
            num_edges ++;
            edge_stream[uv] = t;

            // -- count triangles
            du = (int) graph_stream[u].size();
            dv = (int) graph_stream[v].size();
            n_min = (du < dv) ? u : v;
            n_max = (du < dv) ? v : u;
            min_neighbors = graph_stream[n_min];
            for (auto neigh: min_neighbors) {
                if (graph_stream[n_max].find(neigh) != graph_stream[n_max].end()) {
                    // -- triangle {n_min, neigh, n_max} discovered
                    total_T += 1;
                }
            }

        }

        // -- eof
        num_nodes = (int) graph_stream.size();
        printf("Preprocessed dataset with n = %d, m = %d, T = %d\n", num_nodes, num_edges, total_T);
        std::cout << "Sorting edge map...\n";
        std::vector<std::pair<Edge, int>> ordered_edge_stream;
        for (auto elem : edge_stream) {
            ordered_edge_stream.emplace_back(elem);
        }

        std::sort(ordered_edge_stream.begin(), ordered_edge_stream.end(), sort_edge_time);

        // -- write results
        std::cout << "Done!\nWriting results...\n";

        std::ofstream out_file(output_path);

        int cnt = 0;
        for (auto elem: ordered_edge_stream) {
            out_file << elem.first.first << " " << elem.first.second << " " << ++cnt << "\n";
        }

        out_file.close();

    } else {
        std::cerr << "DataPreprocessing - Error! Graph filepath not opened.\n";
    }

}

void Utils::build_oracle(const std::string &dataset_filepath,
                         const std::string &type, const std::string &output_path, double perc_retain) {

    std::cout << "Building " << type << " Oracle...\n";

    std::ifstream file(dataset_filepath);
    std::string line;

    // -- oracles
    ankerl::unordered_dense::map<Edge, int, hash_edge> oracle_heaviness;
    ankerl::unordered_dense::map<Edge, int, hash_edge> oracle_min_degree;

    // - graph
    ankerl::unordered_dense::map<int, ankerl::unordered_dense::set<int>> graph_stream;

    int u, v, t, du, dv, n_min, n_max;
    ankerl::unordered_dense::set<int> min_neighbors;

    int total_T = 0;
    int skip = 0;

    if (file.is_open()) {

        int nline = 0;
        while (std::getline(file, line)) {

            std::stringstream ss(line);
            if (nline < skip) continue;
            ss >> u >> v >> t;

            // -- check self-loops
            if (u == v) continue;
            // -- check multiple edges
            if (graph_stream[u].find(v) != graph_stream[u].end() and graph_stream[v].find(u) != graph_stream[v].end())
                continue;
            graph_stream[u].emplace(v);
            graph_stream[v].emplace(u);
            // -- add edge to graph stream
            // -- count triangles
            du = (int) graph_stream[u].size();
            dv = (int) graph_stream[v].size();
            n_min = (du < dv) ? u : v;
            n_max = (du < dv) ? v : u;
            min_neighbors = graph_stream[n_min];
            int common_neighs = 0;
            for (auto neigh: min_neighbors) {
                if (graph_stream[n_max].find(neigh) != graph_stream[n_max].end()) {
                    // -- triangle {n_min, neigh, n_max} discovered
                    common_neighs += 1;
                    total_T += 1;
                    // -- sort edge entries
                    int entry_11 = (n_min < neigh) ? n_min : neigh;
                    int entry_12 = (n_min < neigh) ? neigh : n_min;
                    int entry_21 = (neigh < n_max) ? neigh : n_max;
                    int entry_22 = (neigh < n_max) ? n_max : neigh;

                    oracle_heaviness[std::make_pair(entry_11, entry_12)] += 1;
                    oracle_heaviness[std::make_pair(entry_21, entry_22)] += 1;

                }
            }

            int entry_31 = (u < v) ? u : v;
            int entry_32 = (u < v) ? v : u;
            oracle_heaviness[std::make_pair(entry_31, entry_32)] = common_neighs;

            nline++;
            if (nline % 2000000 == 0) {
                std::cout << "Processed " << nline << " edges...\n";
            }

        }

        // -- eof
        // -- sorting the map
        std::cout << "Sorting the oracle and retrieving the top " << perc_retain << " values...\n";
        std::vector<std::pair<Edge, int>> sorted_oracle;
        sorted_oracle.reserve(oracle_heaviness.size());

        if (strcmp(type.c_str(), "Exact") == 0) {
            sort_edgemap(oracle_heaviness, sorted_oracle);
        } else if (strcmp(type.c_str(), "MinDeg") == 0) {
            // -- compute min_deg
            int deg_u, deg_v;
            for (auto elem : oracle_heaviness) {
                u = elem.first.first;
                v = elem.first.second;
                deg_u = (int) graph_stream[u].size();
                deg_v = (int) graph_stream[v].size();
                oracle_min_degree.emplace(elem.first, std::min(deg_u, deg_v));
            }
            sort_edgemap(oracle_min_degree, sorted_oracle);
        } else {
            std::cerr << "Error: Oracle type not specified!";
        }

        // -- write results
        std::cout << "Done!\nWriting results...\n";
        int stop_idx = (int) (perc_retain* (int)sorted_oracle.size());

        std::ofstream out_file(output_path);
        std::cout << "Total Triangles -> " << total_T << "\n";
        std::cout << "Retained Oracle Size = " << stop_idx << "\n";

        int cnt = 0;
        for (auto elem: sorted_oracle) {
            if (cnt > stop_idx) break;
                out_file << elem.first.first << " " << elem.first.second << " " << elem.second << "\n";
                cnt ++;
        }

        out_file.close();

    } else {
        std::cerr << "Build Oracle - Error! Graph filepath not opened.\n";
    }
}
