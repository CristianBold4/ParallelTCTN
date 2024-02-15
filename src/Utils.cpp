#include "Utils.h"

void Utils::sort_edgemap(const ankerl::unordered_dense::map<EdgeTemp, int, hash_edge> &map,
                         std::vector<std::pair<EdgeTemp, int>> &edge_map) {

    for (auto elem: map) {
        edge_map.emplace_back(elem);
    }

    std::sort(edge_map.begin(), edge_map.end(), sort_edge_map_comp);

}

void Utils::build_oracle(std::string &dataset_path, int delta, std::string &type_oracle, double perc_retain,
                         std::string &output_path) {


    std::cout << "Building " << type_oracle << " Oracle...\n";

    GraphStream stream(dataset_path, ' ', 0);

    // -- oracles
    ankerl::unordered_dense::map<EdgeTemp, int, hash_edge> oracle_heaviness;
    // ankerl::unordered_dense::map<EdgeTemp , int, hash_edge> oracle_min_degree;

    // - graph
    ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, std::vector<int>>> graph_stream;

    int u, v, t, old_t, du, dv, n_min, n_max, w;
    ankerl::unordered_dense::map<int, std::vector<int>> *min_neighbors;
    ankerl::unordered_dense::map<int, std::vector<int>> *neighbors;
    // , neighbors;
    std::vector<int>* timestamps;
    EdgeTemp e1, e2, e3;

    long total_T = 0;
    int common_neighs;
    long nline = 0;
    bool first = true;
    int it_idx = 0;

    old_t = 0;

    EdgeStream curr;
    auto start = std::chrono::high_resolution_clock::now();
    while (stream.has_next()) {

        curr = stream.next();
        u = (curr.u < curr.v) ? curr.u : curr.v;
        v = (curr.u < curr.v) ? curr.v : curr.u;
        t = curr.time;

//        if (first) {
//            old_t = t;
//            first = false;
//        }
//
        // -- graph pruning
        if (t - old_t >= 20*delta) {
            // std::cout << "Pruning subgraph ...\n";
            old_t = t;
            // -- perform pruning from graph stream
            for (const auto &node : graph_stream) {
                neighbors = &graph_stream[node.first];
                for (auto neigh : *neighbors) {
                    // -- loop through timestamps
//                    neigh.second.erase(std::remove_if(neigh.second.begin(), neigh.second.end(),
//                                           [&](int time) { return t - time >=  delta; }),
//                                            neigh.second.end());
                    timestamps = &neigh.second;
                    auto neigh_time_it = (*timestamps).begin();
                    while (neigh_time_it != (*timestamps).end()) {
                        if (t - *neigh_time_it < delta) {
                            break;
                        } else {
                            neigh_time_it = (*timestamps).erase(neigh_time_it);
                            // neigh_time_it = timestamps.erase(neigh_time_it);
                        }

                    }
                }
            }

        }

        // -- insert into gs
        graph_stream[u][v].emplace_back(t);
        graph_stream[v][u].emplace_back(t);

        // -- count triangles
        du = (int) graph_stream[u].size();
        dv = (int) graph_stream[v].size();

        n_min = (du < dv) ? u : v;
        n_max = (du < dv) ? v : u;

        min_neighbors = &graph_stream[n_min];
        common_neighs = 0;

        for (const auto &neigh: *min_neighbors) {
            w = neigh.first;

            auto n_max_it = graph_stream[n_max].find(w);
            if (n_max_it != graph_stream[n_max].end()) {
                // -- triangle found
                int timestamps_size = (int) n_max_it->second.size();
                timestamps = &(n_max_it->second);

                for (int idx_neigh = (int) neigh.second.size() - 1; idx_neigh >= 0; idx_neigh--) {
                    // check times
                    int neigh_time = neigh.second[idx_neigh];
                    if (t - neigh_time >= delta)
                        break;

                    for (int idx = timestamps_size - 1; idx >= 0; idx--) {
                        int time = (*timestamps)[idx];
                        // check times
                        if (t - time >= delta)
                            break;

                        // -- check if timestamps are equal
                        if (t == time || t == neigh_time || time == neigh_time)
                            continue;
                        total_T++;
                        common_neighs++;

                        // -- sort entries
                        int entry_11 = (n_min < w) ? n_min : w;
                        int entry_12 = (n_min < w) ? w : n_min;
                        int entry_21 = (n_max < w) ? n_max : w;
                        int entry_22 = (n_max < w) ? w : n_max;

                        e1.u = entry_11;
                        e1.v = entry_12;
                        e1.time = neigh_time;

                        e2.u = entry_21;
                        e2.v = entry_22;
                        e2.time = time;

                        oracle_heaviness[e1] += 1;
                        oracle_heaviness[e2] += 1;
                    }

                }
            }

        }

        e3.u = u;
        e3.v = v;
        e3.time = t;
        oracle_heaviness[e3] = common_neighs;

        nline++;
        if (nline % 200000 == 0) {
            std::cout << "Processed " << nline << " edges...\n";
        }

    }

    auto stop = std::chrono::high_resolution_clock::now();
    double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
    std::cout << "Exact Algo built in " << time << " s\n";

    // -- eof
    // -- sorting the map
    std::cout << "Sorting the oracle and retrieving the top " << perc_retain << " values...\n";
    std::vector<std::pair<EdgeTemp, int>> sorted_oracle;
    sorted_oracle.reserve(oracle_heaviness.size());

    if (strcmp(type_oracle.c_str(), "Exact") == 0) {
        sort_edgemap(oracle_heaviness, sorted_oracle);
    } else if (strcmp(type_oracle.c_str(), "MinDeg") == 0) {
        // -- compute min_deg
        int deg_u, deg_v;
        // ......
    } else {
        std::cerr << "Error: Oracle type not specified!";
    }

    // -- write results
    std::cout << "Done!\nWriting results...\n";
    int stop_idx = (int) (perc_retain * (int) sorted_oracle.size());

    std::ofstream out_file(output_path);
    std::cout << "Total Delta-Triangles Instances -> " << total_T << "\n";
    std::cout << "Retained Oracle Size = " << stop_idx << "\n";

    int cnt = 0;
    for (auto elem: sorted_oracle) {
        if (cnt > stop_idx) break;
        out_file << elem.first.u << " " << elem.first.v << " " << elem.first.time << " " << elem.second << "\n";
        cnt++;
    }

    out_file.close();


}


void Utils::build_oracle_fast(std::string &dataset_path, int delta, std::string &type_oracle, double perc_retain,
                         std::string &output_path) {


    std::cout << "Building " << type_oracle << " Oracle...\n";

    GraphStream stream(dataset_path, ' ', 0);

    // -- oracles
    ankerl::unordered_dense::map<EdgeTemp, int, hash_edge> oracle_heaviness;
    // ankerl::unordered_dense::map<EdgeTemp , int, hash_edge> oracle_min_degree;

    // - graph
    ankerl::unordered_dense::map<int, std::vector<NeighTemp>> graph_stream;

    int u, v, t, t_i, t_j, old_t, du, dv, n_min, n_max, w;
    std::vector<NeighTemp> *min_neighbors, *neighbors;
    // , neighbors;
    std::vector<int>* timestamps;
    EdgeTemp e1, e2, e3;
    NeighTemp neigh_i, neigh_j;

    long total_T = 0;
    int common_neighs;
    long nline = 0;
    bool first = true;
    int it_idx = 0;

    old_t = 0;

    EdgeStream curr;
    auto start = std::chrono::high_resolution_clock::now();
    while (stream.has_next()) {

        curr = stream.next();
        u = (curr.u < curr.v) ? curr.u : curr.v;
        v = (curr.u < curr.v) ? curr.v : curr.u;
        t = curr.time;

//        // -- graph pruning
//        if (t - old_t >= 20*delta) {
//            // std::cout << "Pruning subgraph ...\n";
//            old_t = t;
//            // -- perform pruning from graph stream
//            for (const auto &node : graph_stream) {
//                neighbors = &graph_stream[node.first];
//                for (auto neigh : *neighbors) {
//                    // -- loop through timestamps
////                    neigh.second.erase(std::remove_if(neigh.second.begin(), neigh.second.end(),
////                                           [&](int time) { return t - time >=  delta; }),
////                                            neigh.second.end());
//                    timestamps = &neigh.second;
//                    auto neigh_time_it = (*timestamps).begin();
//                    while (neigh_time_it != (*timestamps).end()) {
//                        if (t - *neigh_time_it < delta) {
//                            break;
//                        } else {
//                            neigh_time_it = (*timestamps).erase(neigh_time_it);
//                            // neigh_time_it = timestamps.erase(neigh_time_it);
//                        }
//
//                    }
//                }
//            }
//
//        }

        // -- insert into gs
        NeighTemp nt;
        nt.node = v;
        nt.timestamp = t;
        nt.weight = false;
        nt.dir = 1;
        graph_stream[u].emplace_back(nt);
        nt.node = u;
        nt.timestamp = t;
        nt.weight = false;
        nt.dir = -1;
        graph_stream[v].emplace_back(nt);

        // -- count triangles
        du = (int) graph_stream[u].size();
        dv = (int) graph_stream[v].size();

        n_min = (du < dv) ? u : v;
        n_max = (du < dv) ? v : u;

        min_neighbors = &graph_stream[n_min];
        common_neighs = 0;

        for (int n_i_idx = (int)(*min_neighbors).size() - 1; n_i_idx >= 0; --n_i_idx) {
            neigh_i = (*min_neighbors)[n_i_idx];
            if (t - neigh_i.timestamp >= delta)
                break;
            w = neigh_i.node;
            neighbors = &graph_stream[w];
            for (int n_j_idx = (int)(*neighbors).size() - 1; n_j_idx >= 0; --n_j_idx) {
                neigh_j = (*neighbors)[n_j_idx];
                if (t - neigh_j.timestamp >= delta)
                    break;
                if (neigh_j.node == n_max) {
                    // -- triangle discovered
//                    // -- check times
                    if (neigh_i.timestamp == neigh_j.timestamp || neigh_i.timestamp == t || neigh_j.timestamp == t)
                        continue;
                    common_neighs ++;
                    total_T++;

                    // -- sort entries
                    int entry_11 = (n_min < w) ? n_min : w;
                    int entry_12 = (n_min < w) ? w : n_min;
                    int entry_21 = (n_max < w) ? n_max : w;
                    int entry_22 = (n_max < w) ? w : n_max;

                    e1.u = entry_11;
                    e1.v = entry_12;
                    e1.time = neigh_i.timestamp;

                    e2.u = entry_21;
                    e2.v = entry_22;
                    e2.time = neigh_j.timestamp;

                    oracle_heaviness[e1] += 1;
                    oracle_heaviness[e2] += 1;

                }
            }

        }

        e3.u = u;
        e3.v = v;
        e3.time = t;
        oracle_heaviness[e3] = common_neighs;

        nline++;
        if (nline % 200000 == 0) {
            std::cout << "Processed " << nline << " edges...\n";
        }

    }

    auto stop = std::chrono::high_resolution_clock::now();
    double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
    std::cout << "Exact Algo built in " << time << " s\n";

    // -- eof
    // -- sorting the map
    std::cout << "Sorting the oracle and retrieving the top " << perc_retain << " values...\n";
    std::vector<std::pair<EdgeTemp, int>> sorted_oracle;
    sorted_oracle.reserve(oracle_heaviness.size());

    if (strcmp(type_oracle.c_str(), "Exact") == 0) {
        sort_edgemap(oracle_heaviness, sorted_oracle);
    } else if (strcmp(type_oracle.c_str(), "MinDeg") == 0) {
        // -- compute min_deg
        int deg_u, deg_v;
        // ......
    } else {
        std::cerr << "Error: Oracle type not specified!";
    }

    // -- write results
    std::cout << "Done!\nWriting results...\n";
    int stop_idx = (int) (perc_retain * (int) sorted_oracle.size());

    std::ofstream out_file(output_path);
    std::cout << "Total Delta-Triangles Instances -> " << total_T << "\n";
    std::cout << "Retained Oracle Size = " << stop_idx << "\n";

    int cnt = 0;
    for (auto elem: sorted_oracle) {
        if (cnt > stop_idx) break;
        out_file << elem.first.u << " " << elem.first.v << " " << elem.first.time << " " << elem.second << "\n";
        cnt++;
    }

    out_file.close();


}

