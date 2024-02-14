#include "Utils.h"

void Utils::sort_edgemap(const ankerl::unordered_dense::map<EdgeTemp, int, hash_edge>& map, std::vector<std::pair<EdgeTemp, int>> &edge_map) {

    for (auto elem : map) {
        edge_map.emplace_back(elem);
    }

    std::sort(edge_map.begin(), edge_map.end(), sort_edge_map_comp);

}

void Utils::build_oracle(std::string &dataset_path, int delta, std::string &type_oracle, double perc_retain,
                         std::string &output_path) {


    std::cout << "Building " << type_oracle << " Oracle...\n";

    GraphStream stream(dataset_path, ' ', 0);

    // -- oracles
    ankerl::unordered_dense::map<EdgeTemp , int, hash_edge> oracle_heaviness;
    // ankerl::unordered_dense::map<EdgeTemp , int, hash_edge> oracle_min_degree;

    // - graph
    ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, std::vector<int>>> graph_stream;

    int u, v, t, du, dv, n_min, n_max, w;
    ankerl::unordered_dense::map<int, std::vector<int>> min_neighbors;
    EdgeTemp e1, e2, e3;

    long total_T = 0;
    int common_neighs;
    long nline = 0;

    EdgeStream curr;
    while(stream.has_next()) {

        curr = stream.next();
        u = (curr.u < curr.v) ? curr.u : curr.v;
        v = (curr.u < curr.v) ? curr.v : curr.u;
        t = curr.time;

        // -- insert into gs
        graph_stream[u][v].emplace_back(t);
        graph_stream[v][u].emplace_back(t);

        // -- count triangles
        du = (int) graph_stream[u].size();
        dv = (int) graph_stream[v].size();

        n_min = (du < dv) ? u : v;
        n_max = (du < dv) ? v : u;

        min_neighbors = graph_stream[n_min];
        common_neighs = 0;

        for (const auto &neigh : min_neighbors) {
            w = neigh.first;
            for (auto time_w = neigh.second.rbegin(); time_w != neigh.second.rend(); time_w++) {
                // check times
                int neigh_time = *time_w;
                if (t - neigh_time >= delta)
                    break;

                auto n_max_it = graph_stream[n_max].find(w);
                if (n_max_it != graph_stream[n_max].end()) {

                    std::vector timestamps = n_max_it->second;

                    for (auto time_nmax = timestamps.rbegin(); time_nmax != timestamps.rend(); time_nmax++) {
                        int time = *time_nmax;
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
    int stop_idx = (int) (perc_retain* (int)sorted_oracle.size());

    std::ofstream out_file(output_path);
    std::cout << "Total Delta-Triangles Instances -> " << total_T << "\n";
    std::cout << "Retained Oracle Size = " << stop_idx << "\n";

    int cnt = 0;
    for (auto elem: sorted_oracle) {
        if (cnt > stop_idx) break;
        out_file << elem.first.u << " " << elem.first.v << " " << elem.first.time << " " << elem.second << "\n";
        cnt ++;
    }

    out_file.close();



}
