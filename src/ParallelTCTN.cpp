//
// Created by Cristian Boldrin on 13/02/24.
//

#include "ParallelTCTN.h"
#include <cassert>

ParallelTCTN::ParallelTCTN(int delta, double p, EdgeSet &oracle) :
        delta_(delta), p_(p), oracle_(oracle) {}

ParallelTCTN::~ParallelTCTN() { subgraph_.clear(); }

std::array<double, 8> ParallelTCTN::get_triangles_estimates() {
    return triangles_estimates_;
}

double ParallelTCTN::get_total_triangles() {
    double total_cnt = 0.0;
    for (const auto &cnt: triangles_estimates_) {
        total_cnt += cnt;
    }
    return total_cnt;
}

unsigned long long ParallelTCTN::edge_to_id(int u, int v) {
    int nu = (u < v ? u : v);
    int nv = (u < v ? v : u);
    unsigned long long id = (static_cast<unsigned long long>(MAX_N_NODES)
                             * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv));
    return id;
}

void ParallelTCTN::add_edge_subgraph(int u, int v, int t, bool heaviness) {
    EdgeTemp e = {u, v, t};
    subgraph_.add_edge(e, heaviness);
    // -- add to node map
    unsigned long long id = edge_to_id(u, v);
    node_map_[id].push_back({{u, v, t }, heaviness});
    // node_map_[id].push_back({{v, u, t }, heaviness});
}

void ParallelTCTN::sample_edge(int u, int v, int t) {
    EdgeTemp e = {u, v, t};
    bool heaviness = (oracle_.find(e) != oracle_.end());
    if (heaviness)
        add_edge_subgraph(u, v, t, true);
    else {
        double rand = next_double();
        if (rand < p_)
            add_edge_subgraph(u, v, t, false);
    }
}

int ParallelTCTN::check_triangle(EdgeTemp e1, EdgeTemp e2, EdgeTemp e3) {

//    std::cout << "*********\n" << e1.u << " " << e1.v << " " << e1.time << "\n"
//              << e2.u << " " << e2.v << " " << e2.time << "\n"
//              << e3.u << " " << e3.v << " " << e3.time << "\n*************\n";

    int u_1, u_2, u_3, v_1, v_2, v_3;
    u_1 = e1.u;
    v_1 = e1.v;
    u_2 = e2.u;
    v_2 = e2.v;
    u_3 = e3.u;
    v_3 = e3.v;

    if ((u_1 == u_3) && (v_1 == v_2) && (u_2 == v_3))
        return 0;
    else if ((u_1 == v_3) && (v_1 == v_2) && (u_2 == u_3))
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


void ParallelTCTN::count_triangles(int u, int v, int t) {

    // -- count triangles
    int du, dv, n_min, n_max, w;
    du = subgraph_.get_degree_node(u);
    dv = subgraph_.get_degree_node(v);

    n_min = (du < dv) ? u : v;
    n_max = (du < dv) ? v : u;

    std::vector<NeighTemp> *min_neighbors, *neighbors;
    NeighTemp neigh_i;
    std::vector<PairEdges> *pair_edges;

    EdgeTemp e1, e2, e3;

    min_neighbors = subgraph_.return_neighbors(n_min);

    for (int n_i_idx = (int) (*min_neighbors).size() - 1; n_i_idx >= 0; --n_i_idx) {

        neigh_i = (*min_neighbors)[n_i_idx];

        if (t - neigh_i.timestamp >= delta_)
            break;

        w = neigh_i.node;

        // neighbors = subgraph_.return_neighbors(w);
        // assert(edge_to_id(n_max, w) == edge_to_id(w, n_max));
        pair_edges = &node_map_[edge_to_id(n_max, w)];
        PairEdges p_e{};

        for (int n_j_idx = (int) (*pair_edges).size() - 1; n_j_idx >= 0; --n_j_idx) {
            // std::cout << (*pair_edges).size();
            p_e = (*pair_edges)[n_j_idx];
            if (t - p_e.e.time >= delta_)
                break;

            if (neigh_i.timestamp == p_e.e.time || neigh_i.timestamp == t || p_e.e.time == t)
                continue;

            // std::cout << "Right Triangle cntd\n";

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
            e2 = p_e.e;
            // std::cout << e2.u << " " << e2.v << " " << e2.time << "\n";

            // -- sort edges by dir
            if (u == n_min) {
                e3.u = n_min;
                e3.v = n_max;
                e3.time = t;
            } else {
                e3.v = n_min;
                e3.u = n_max;
                e3.time = t;
            }

            // -- get motif idx
            int motif_idx;

            if (e1.time < e2.time)
                motif_idx = check_triangle(e1, e2, e3);
            else
                motif_idx = check_triangle(e2, e1, e3);

            // assert(motif_idx != -1);
            if (motif_idx != -1) {
                bool h_i = neigh_i.weight;
                bool h_j = p_e.weight;
                double increment = 1.0;
                if (!h_i and !h_j)
                    increment = 1.0 / (p_ * p_);
                else if (!h_i or !h_j)
                    increment = 1.0 / (p_);

                triangles_estimates_[motif_idx] += increment;
            }

        }


    }

}

void ParallelTCTN::process_edge(const int u, const int v, const int t) {

    curr_time_ = t;

    // -- prune subgraph if needed
    if (prev_time_ == -1)
        prev_time_ = curr_time_;

    if (curr_time_ - prev_time_ > delta_) {
        prune();
        prev_time_ = curr_time_;
    }

    // -- count triangles
    count_triangles(u, v, t);
    // -- sample edge
    sample_edge(u, v, t);

}

void ParallelTCTN::prune() {

    // std::cout << "Pruning...\n";

    subgraph_.prune(curr_time_, delta_);
    // -- node map prune
    std::vector<PairEdges> *pair_edges;
    for (auto &pair_nodes: node_map_) {
        pair_edges = &pair_nodes.second;
        auto edge_it = pair_edges->begin();
        while (edge_it != pair_edges->end()) {
            if (curr_time_ - edge_it->e.time < delta_)
                break;
            edge_it = pair_edges->erase(edge_it);
        }
    }
}


double ParallelTCTN::next_double() {
    int a = mt() >> 5;
    int b = mt() >> 6;
    return (a * 67108864.0 + b) / 9007199254740992.0;
}
