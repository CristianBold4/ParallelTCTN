#ifndef TRIANGLESWITHPREDICTIONS_WRPSAMPLING_H
#define TRIANGLESWITHPREDICTIONS_WRPSAMPLING_H

#include "Subgraph.h"
#include <queue>
#include <map>
#include <random>
#include <iomanip>
#include "ankerl/unordered_dense.h"
#include "FixedSizePQ.h"
#include <omp.h>

#define EdgeHeaviness std::pair<Edge, int>

#define MAX_N_NODES 100000000

// -- comparator for lightness of edges
class LightnessComparator {

public:
    bool operator()(EdgeHeaviness a, EdgeHeaviness b) {
        return a.second > b.second;
    }
};

class WRPSampling {

public:

    // -- getter for counters
    double get_global_triangles() const;
    double get_local_triangles(int node) const;
    void get_local_triangles_map(ankerl::unordered_dense::map<int, double> &local_triangles) const;

    // -- "hashing" functions
    static unsigned long long edge_to_id(const int u, const int v);

    WRPSampling(long k, int random_seed, double alpha, double beta,
                const ankerl::unordered_dense::map<long, int>& oracle);

    ~WRPSampling();

    inline int size_subgraph() const {
        return subgraph_.num_edges();
    }

    void process_edge(int u, int v);

private:

    // -- Mersenne Twister RG
    std::mt19937 e2;

    unsigned long long t_;
    double alpha_;
    double beta_;
    long k_;
    long WR_size_;
    long H_size_;
    long SL_size_;

    // -- init current dimensions of sets of the subgraph
    long WR_cur_ = 0;
    long H_cur_ = 0;
    long SL_cur_ = 0;

    // -- counter of global triangles
    double global_triangles_cnt_ = 0.0;
    // -- counter of local triangles
    ankerl::unordered_dense::map<int, double> local_triangles_cnt_;

    // -- subgraph
    Subgraph subgraph_;
    Edge* waiting_room_;
    FixedSizePQ<EdgeHeaviness, LightnessComparator> heavy_edges_pq_;
    Edge* light_edges_sample_;

    // -- edge oracle
    const ankerl::unordered_dense::map<long, int>& oracle_;

    // -- used for fastest lookup of edges
    ankerl::unordered_dense::set<unsigned long long> set_edges_ids_;
    // -- hash set to track light edges
    ankerl::unordered_dense::set<unsigned long long> set_light_edges_ids_;

    bool add_edge_subgraph(const int u, const int v, char label);

    bool remove_edge_subgraph(const int u, const int v);

    void count_triangles(const int u, const int v);

    char sample_edge(int u, int v);

    double next_double();

};


#endif //TRIANGLESWITHPREDICTIONS_WRPSAMPLING_H
