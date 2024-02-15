#include "Utils.h"

void Utils::sort_edgemap(const ankerl::unordered_dense::map<EdgeTemp, int, hash_edge> &map,
                         std::vector<std::pair<EdgeTemp, int>> &edge_map) {

    for (auto elem: map) {
        edge_map.emplace_back(elem);
    }

    std::sort(edge_map.begin(), edge_map.end(), sort_edge_map_comp);

}

int Utils::count_triangle(EdgeTemp e1, EdgeTemp e2, EdgeTemp e3) {

    int u_1, u_2, u_3, v_1, v_2, v_3;
    u_1 = e1.u;
    v_1 = e1.v;
    u_2 = e2.u;
    v_2 = e2.v;
    u_3 = e3.u;
    v_3 = e3.v;

    if ((u_1 == u_3) && (v_1 == v_2) && (u_2 == v_3))
        return 0;
    else if((u_1 == v_3) && (v_1 == v_2) && (u_2 == u_3))
        return 1;
    else if ((u_1 == u_3) && (v_1 == u_2) && (v_2 == v_3))
        return 2;
    else if ((v_1 == u_2) && (v_2 == u_3) && (v_3 == u_1))
        return 3;
    else if ((u_1 == u_2) && (v_1 == v_3) && (v_2 == u_3))
        return 4;
    else if ((u_1 == v_2) && (v_1 == v_3) && (u_2 == u_3))
        return 5;
    else if ((u_1 == u_2) && (v_1 == u_3) && (v_2 == v_3))
        return 6;
    else if ((u_1 == v_2) && (v_1 == u_3) && (v_3 == u_2))
        return 7;
    else
        return -1;

}

void Utils::read_oracle(std::string &oracle_path, ankerl::unordered_dense::set<EdgeTemp, hash_edge> &oracle) {

    GraphStream stream(oracle_path, ' ', 0);
    EdgeStream curr;
    while (stream.has_next()) {
        curr = stream.next();
        oracle.insert({curr.u, curr.v, curr.time});
    }

}

void Utils::build_ground_truth(std::string &dataset_path, int delta, std::string &type_oracle, double perc_retain,
                               std::string &exact_output_path, std::string &oracle_output_path) {


    std::cout << "Building " << type_oracle << " Oracle...\n";

    GraphStream stream(dataset_path, ' ', 0);

    // -- oracles
    ankerl::unordered_dense::map<EdgeTemp, int, hash_edge> oracle_heaviness;
    // ankerl::unordered_dense::map<EdgeTemp , int, hash_edge> oracle_min_degree;

    // - graph
    ankerl::unordered_dense::map<int, std::vector<NeighTemp>> graph_stream;

    // -- motif counters
    std::array<double, 8> motif_count{};

    int u, v, t, old_t, du, dv, n_min, n_max, w;
    std::vector<NeighTemp> *min_neighbors, *neighbors;
    // , neighbors;
    std::vector<int>* timestamps;
    EdgeTemp e1, e2, e3;
    NeighTemp neigh_i, neigh_j;

    long total_T = 0;
    int common_neighs;
    long nline = 0;
    bool first = true;

    old_t = 0;

    EdgeStream curr;
    auto start = std::chrono::high_resolution_clock::now();
    while (stream.has_next()) {

        curr = stream.next();
        u = (curr.u < curr.v) ? curr.u : curr.v;
        v = (curr.u < curr.v) ? curr.v : curr.u;
        t = curr.time;

        if (first) {
            first = false;
            old_t = t;
        }

        // -- graph pruning
        if (t - old_t >= delta) {
            old_t = t;
            for (auto &pair : graph_stream) {
                neighbors = &pair.second;
                auto neigh_it = neighbors->begin();
                while (neigh_it != neighbors->end()) {
                    if (t - neigh_it->timestamp < delta)
                        break;
                    neigh_it = neighbors->erase(neigh_it);
                }

            }
        }

        // -- insert into gs
        NeighTemp nt;
        nt.node = curr.v;
        nt.timestamp = t;
        nt.weight = false;
        nt.dir = 1;
        graph_stream[curr.u].emplace_back(nt);

        nt.node = curr.u;
        nt.timestamp = t;
        nt.weight = false;
        nt.dir = -1;
        graph_stream[curr.v].emplace_back(nt);

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
                    // -- check times
                    if (neigh_i.timestamp == neigh_j.timestamp || neigh_i.timestamp == t || neigh_j.timestamp == t)
                        continue;
                    common_neighs ++;
                    total_T++;

                    // -- count motifs: we have triangle {n_min, w=neigh_i, n_max=neigh_j}
                    // -- sort edges by dir
                    if (neigh_i.dir == 1) {
                        e1.u = n_min;
                        e1.v = w;
                        e1.time = neigh_i.timestamp;
                    } else {
                        e1.v = n_min;
                        e1.u = w;
                        e1.time = neigh_i.timestamp;
                    }
                    // -- sort edges by dir
                    if (neigh_j.dir == 1) {
                        e2.u = w;
                        e2.v = n_max;
                        e2.time = neigh_j.timestamp;
                    } else {
                        e2.v = w;
                        e2.u = n_max;
                        e2.time = neigh_j.timestamp;
                    }
                    // -- sort edges by dir
                    if (curr.u == n_min) {
                        e3.u = n_min;
                        e3.v = n_max;
                        e3.time = curr.time;
                    } else {
                        e3.v = n_min;
                        e3.u = n_max;
                        e3.time = curr.time;
                    }

                    // -- get motif idx
                    int motif_idx;
                    if (e1.time < e2.time)
                        motif_idx = count_triangle(e1, e2, e3);
                    else
                        motif_idx = count_triangle(e2, e1, e3);

                    if (motif_idx != -1)
                        motif_count[motif_idx] ++;

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
        if (nline % 1000000 == 0) {
            std::cout << "Processed " << nline << " edges...\n";
        }

    }

    auto stop = std::chrono::high_resolution_clock::now();
    double time = (double) ((std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)).count()) / 1000;
    std::cout << "Exact Algo built in " << time << " s\n";

    // -- eof

    // -- writing ground truth
    std::ofstream exact_out_file(exact_output_path);
    for (auto cnt : motif_count)
        exact_out_file << cnt << " ";
    exact_out_file << time << "\n";
    exact_out_file.close();

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

    std::ofstream out_file(oracle_output_path);
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

