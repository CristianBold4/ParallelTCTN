//
// Created by Cristian Boldrin on 13/02/24.
//

#ifndef PARALLEL_TCTN_ALGO_H
#define PARALLEL_TCTN_ALGO_H

// #include "Subgraph.h"
#include "SubgraphMap.h"
#include <ankerl/unordered_dense.h>
#include <random>
#include <vector>
#include "hashing.h"

#define EdgeSet ankerl::unordered_dense::set<EdgeTemp, hash_edge>

struct PairEdges {
    EdgeTemp e;
    bool weight;
};


class ParallelTCTN {

    int curr_time_{};
    int prev_time_ = -1;
    double p_;
    int delta_;
    // -- oracle
    EdgeSet oracle_;
    // Subgraph subgraph_{};
    SubgraphMap subgraph_map_{};

    std::array<double, 8> triangles_estimates_{};

    // ankerl::unordered_dense::map<unsigned long long, std::vector<PairEdges>> node_map_ {};

    // -- Mersenne Twister RG
    std::mt19937 mt;

    unsigned long long edge_to_id(int u, int v);

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


#endif
