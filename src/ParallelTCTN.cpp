//
// Created by Cristian Boldrin on 13/02/24.
//

#include "ParallelTCTN.h"

ParallelTCTN::ParallelTCTN(int delta, double p, EdgeSet &oracle) :
        delta_(delta), p_(p), oracle_(oracle){}

ParallelTCTN::~ParallelTCTN() { subgraph_.clear(); }

std::array<double, 8> ParallelTCTN::get_triangles_estimates() {
    return triangles_estimates_;
}

double ParallelTCTN::get_total_triangles() {
    double total_cnt = 0.0;
    for (const auto &cnt : triangles_estimates_) {
        total_cnt += cnt;
    }
    return total_cnt;
}

void ParallelTCTN::add_edge_subgraph(int u, int v, int t, bool heaviness) {
    EdgeTemp e = {u, v, t};
    subgraph_.add_edge(e, heaviness);
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


void ParallelTCTN::count_triangles(int u, int v, int t) {

    // -- count triangles
    int du, dv, n_min, n_max, w;
    du = subgraph_.get_degree_node(u);
    dv = subgraph_.get_degree_node(v);

    n_min = (du < dv) ? u : v;
    n_max = (du < dv) ? v : u;

    std::vector<NeighTemp> *min_neighbors, *neighbors;
    NeighTemp neigh_i, neigh_j;

    EdgeTemp e1, e2, e3;

    min_neighbors = subgraph_.return_neighbors(n_min);

    for (int n_i_idx = (int)(*min_neighbors).size() - 1; n_i_idx >= 0; --n_i_idx) {

        neigh_i = (*min_neighbors)[n_i_idx];

        if (t - neigh_i.timestamp >= delta_)
            break;

        w = neigh_i.node;

        neighbors = subgraph_.return_neighbors(w);

        for (int n_j_idx = (int)(*neighbors).size() - 1; n_j_idx >= 0; --n_j_idx) {

            neigh_j = (*neighbors)[n_j_idx];

            if (t - neigh_j.timestamp >= delta_)
                break;

            if (neigh_j.node == n_max) {
                // -- triangle discovered
                // -- check times
                if (neigh_i.timestamp == neigh_j.timestamp || neigh_i.timestamp == t || neigh_j.timestamp == t)
                    continue;

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

                if (motif_idx != -1) {
                    bool h_i = neigh_i.weight;
                    bool h_j = neigh_j.weight;
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

}

void ParallelTCTN::process_edge(const int u, const int v, const int t) {

    curr_time_ = t;

    // -- prune subgraph if needed
    if (prev_time_ == -1)
        prev_time_ = curr_time_;

    if(curr_time_ - prev_time_ > delta_) {
        prune();
        prev_time_ = curr_time_;
    }

    // -- count triangles
    count_triangles(u, v, t);
    // -- sample edge
    sample_edge(u, v, t);

}

void ParallelTCTN::prune() {

    subgraph_.prune(curr_time_, delta_);
}


double ParallelTCTN::next_double() {
    int a = mt() >> 5;
    int b = mt() >> 6;
    return (a * 67108864.0 + b) / 9007199254740992.0;
}
