#include "../include/algorithms.h"

std::tuple<counts, chrono_t, double, double, EdgeMap> exact_algorithm(const t_edges& temporal_edges, int delta) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts exact_counts{};
    EdgeMap edge_map;
    Subgraph sg;

    int c = 0;
    int prev_t = temporal_edges[0].get_time();

    long counter = 0;
    double max_memory = 0;

    for(const Edge &e: temporal_edges) {

        counter += sg.get_size();

        if((int)sg.get_size() > max_memory)
            max_memory = sg.get_size();

        c++;
        if(c % ((int)((double)temporal_edges.size() * 0.01)) == 0) {
            double percentage = 100 * (double)c / (double)temporal_edges.size();
            print_progress_bar((int)percentage);
        }

        if(e.get_time() - prev_t >= delta) {
            sg.subgraph_prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        edge_map.add_edge(e);
        sg.add_edge(e);

        std::vector<wedge> wedges = collect_wedges(sg, e, delta);

        if(wedges.empty())
            continue;

        for(const auto &w : wedges) {

            int tr = check_triangles(w, e, delta);

            if(tr != -1) {
                exact_counts[tr]++;
                edge_map.increment_value(w.first);
                edge_map.increment_value(w.second);
                edge_map.increment_value(e);
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    chrono_t time = end_time - start_time;

    double avg_memory = (double)counter / (double)temporal_edges.size();

    return {exact_counts, time, avg_memory, max_memory, edge_map};
}


std::tuple<counts, chrono_t, double, double> sampling_algorithm(const t_edges& temporal_edges, int delta, float p, std::default_random_engine rand_eng) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts sampling_counts{};
    Subgraph sg;
    NodesMap nm;

    long counter = 0;
    double max_memory = 0;

    std::uniform_real_distribution<double> uni_dist(0.0,1.0);

    int prev_t = temporal_edges[0].get_time();

    for(const auto &e: temporal_edges) {

        counter += sg.get_size();

        if((int)sg.get_size() > max_memory)
            max_memory = sg.get_size();

        count_triangles(sg, nm, e, delta, sampling_counts);

        if(e.get_time() - prev_t > delta) {
            sg.subgraph_prune(e.get_time(), delta);
            nm.prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        double rnd = uni_dist(rand_eng);

        if(p >= rnd) {
            sg.add_edge(e);
            nm.add_edge(e);
        }
    }

    for(int i = 0; i < n_motifs; i++)
        sampling_counts[i] = sampling_counts[i] / (p*p);

    auto end_time = std::chrono::high_resolution_clock::now();
    chrono_t time = end_time - start_time;

    double avg_memory = (double)counter / (double)temporal_edges.size();

    return {sampling_counts, time, avg_memory, max_memory};
}

std::tuple<counts, chrono_t, double, double> oracle_algorithm(const t_edges& temporal_edges, int delta, float p, EdgeSet& oracle, std::default_random_engine rand_eng) {

    auto start_time = std::chrono::high_resolution_clock::now();

    counts oracle_counts{};
    counts l1{}, l2{}, l3{};
    Subgraph sg;
    NodesMap nm;

    long counter = 0;
    double max_memory = 0;

    std::uniform_real_distribution<double> uni_dist(0.0,1.0);

    int prev_t = temporal_edges[0].get_time();

    for(const auto& e: temporal_edges) {

        counter += sg.get_size();

        if((int)sg.get_size() > max_memory)
            max_memory = sg.get_size();

        count_triangles_oracle(sg, nm, e, delta, l1, l2, l3);

        if(e.get_time() - prev_t > delta) {
            sg.subgraph_prune(e.get_time(), delta);
            nm.prune(e.get_time(), delta);
            prev_t = e.get_time();
        }

        if(oracle.contains(e)) {
            sg.add_edge(e, true);
            nm.add_edge(e, true);
        }
        else {
            double rnd = uni_dist(rand_eng);
            if (p >= rnd) {
                sg.add_edge(e, false);
                nm.add_edge(e, false);
            }
        }
    }

    for(int i = 0; i < n_motifs; i++) {
        l1[i] = l1[i] / (p * p);
        l2[i] = l2[i] / p;
        oracle_counts[i] = l1[i] + l2[i] + l3[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end_time - start_time;

    double avg_memory = (double)counter / (double)temporal_edges.size();

    return {oracle_counts, time, avg_memory, max_memory};
}

std::vector<wedge> collect_wedges(const Subgraph &sg, const Edge &e, int delta) {

    std::vector<wedge> wedges;
    std::vector<neighbor> neighborhood_i, neighborhood_j;

    if (sg.contains_node(e.get_src()))
        neighborhood_i = sg.get_neighborhood(e.get_src());
    if (sg.contains_node(e.get_dst()))
        neighborhood_j = sg.get_neighborhood(e.get_dst());

    for (int i = (int)neighborhood_i.size() - 1; i >=0; i--) {

        neighbor n_i = neighborhood_i[i];

        if(e.get_time() - n_i.time > delta)
            break;

        for (int j = (int)neighborhood_j.size() - 1; j >=0; j--) {

            neighbor n_j = neighborhood_j[j];

            if(e.get_time() - n_j.time > delta)
                break;

            if (n_i.id == e.get_dst() || n_j.id == e.get_src())
                continue;

            if (n_i.id == n_j.id) {

                Edge e_i, e_j;

                if (n_i.direction)
                    e_i = Edge(e.get_src(), n_i.id, n_i.time);
                else
                    e_i = Edge(n_i.id, e.get_src(), n_i.time);

                if (n_j.direction)
                    e_j = Edge(e.get_dst(), n_j.id, n_j.time);
                else
                    e_j = Edge(n_j.id, e.get_dst(), n_j.time);

                if (n_i.time < n_j.time)
                    wedges.emplace_back(e_i, e_j);
                else
                    wedges.emplace_back(e_j, e_i);
            }
        }
    }

    return wedges;
}

void count_triangles_oracle(const Subgraph &sg,
                            NodesMap &nm,
                            const Edge &e,
                            int delta,
                            counts &l1,
                            counts &l2,
                            counts &l3) {

    std::vector<neighbor> neighborhood_i, neighborhood_j;

    if (sg.contains_node(e.get_src()) && sg.contains_node(e.get_dst())) {

        neighborhood_i = sg.get_neighborhood(e.get_src());
        neighborhood_j = sg.get_neighborhood(e.get_dst());

        int x, y;
        std::vector<neighbor> neighborhood_x;

        std::size_t min_deg = std::min(neighborhood_i.size(), neighborhood_j.size());

        if (min_deg == neighborhood_i.size()) {
            x = e.get_src();
            y = e.get_dst();
            neighborhood_x = neighborhood_i;
        } else {
            x = e.get_dst();
            y = e.get_src();
            neighborhood_x = neighborhood_j;
        }

        for(int i = (int)neighborhood_x.size() - 1; i >= 0; i--) {

            neighbor z = neighborhood_x[i];

            if(e.get_time() - z.time >= delta)
                break;

            std::vector<std::pair<Edge, bool>> pairs = nm.get_edges(y, z.id);

            for (auto pair: pairs) {

                if(e.get_time() - pair.first.get_time() >= delta)
                    continue;

                Edge e_x;
                Edge e_y = pair.first;

                if (z.direction)
                    e_x = Edge(x, z.id, z.time);
                else
                    e_x = Edge(z.id, x, z.time);

                wedge w;

                if (e_x.get_time() < e_y.get_time())
                    w = {e_x, e_y};
                else
                    w = {e_y, e_x};

                if (z.heavy && pair.second) {
                    int tr = check_triangles(w, e, delta);
                    if(tr != -1)
                        l3[tr]++;
                }
                else if (z.heavy || pair.second) {
                    int tr = check_triangles(w, e, delta);
                    if(tr != -1)
                        l2[tr]++;
                }
                else {
                    int tr = check_triangles(w, e, delta);
                    if(tr != -1)
                        l1[tr]++;
                }
            }
        }
    }
}

void count_triangles(const Subgraph &sg,
                     NodesMap &nm,
                     const Edge &e,
                     int delta,
                     counts &l) {

    std::vector<neighbor> neighborhood_i, neighborhood_j;

    if (sg.contains_node(e.get_src()) && sg.contains_node(e.get_dst())) {

        neighborhood_i = sg.get_neighborhood(e.get_src());
        neighborhood_j = sg.get_neighborhood(e.get_dst());

        int x, y;
        std::vector<neighbor> neighborhood_x;

        std::size_t min_deg = std::min(neighborhood_i.size(), neighborhood_j.size());

        if (min_deg == neighborhood_i.size()) {
            x = e.get_src();
            y = e.get_dst();
            neighborhood_x = neighborhood_i;
        } else {
            x = e.get_dst();
            y = e.get_src();
            neighborhood_x = neighborhood_j;
        }

        for(int i = (int)neighborhood_x.size() - 1; i >= 0; i--) {

            neighbor n_x = neighborhood_x[i];

            if(e.get_time() - n_x.time >= delta)
                break;

            std::vector<std::pair<Edge, bool>> pairs = nm.get_edges(y, n_x.id);

            for (auto pair: pairs) {

                if(e.get_time() - pair.first.get_time() >= delta)
                    continue;

                Edge e_x;
                Edge e_y = pair.first;

                if (n_x.direction)
                    e_x = Edge(x, n_x.id, n_x.time);
                else
                    e_x = Edge(n_x.id, x, n_x.time);

                wedge w;

                if (e_x.get_time() < e_y.get_time())
                    w = {e_x, e_y};
                else
                    w = {e_y, e_x};

                int tr = check_triangles(w, e, delta);
                if(tr != -1)
                    l[tr]++;
            }
        }
    }
}

int check_triangles(const wedge& w, const Edge &e, int delta) {

    if(e.get_time() - w.first.get_time() >= delta)
        return -1;

    //Mackey's definition of delta-instance does not allow for edges with same timestamps
    if(e.get_time() == w.first.get_time() ||
       e.get_time() == w.second.get_time() ||
       w.first.get_time() == w.second.get_time())
        return -1;

    int u_1 = w.first.get_src();
    int v_1 = w.first.get_dst();
    int u_2 = w.second.get_src();
    int v_2 = w.second.get_dst();
    int u_3 = e.get_src();
    int v_3 = e.get_dst();

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