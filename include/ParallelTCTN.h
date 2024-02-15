//
// Created by Cristian Boldrin on 13/02/24.
//

#ifndef BUILDORACLE_PARALLELTCTN_H
#define BUILDORACLE_PARALLELTCTN_H

#include "Subgraph.h"
#include <ankerl/unordered_dense.h>
#include <random>

#define EdgeSet ankerl::unordered_dense::set<EdgeTemp, hash_edge>

class ParallelTCTN {

    int curr_time_{};
    int prev_time_ = -1;
    double p_;
    int delta_;
    // -- oracle
    EdgeSet oracle_;
    Subgraph subgraph_{};

    std::array<double, 8> triangles_estimates_{};

    // -- Mersenne Twister RG
    std::mt19937 mt;

    void count_triangles(int u, int v, int t);
    static int check_triangle(EdgeTemp e1, EdgeTemp e2, EdgeTemp e3);
    void sample_edge(int u, int v, int t);

    void add_edge_subgraph(int u, int v, int t, bool heaviness);
    void prune();

    double next_double();


public:

    ParallelTCTN(int delta, double p, EdgeSet &oracle);
    ~ParallelTCTN();

    std::array<double, 8> get_triangles_estimates();
    double get_total_triangles();

    void process_edge(int u, int v, int t);
};


#endif //BUILDORACLE_PARALLELTCTN_H
