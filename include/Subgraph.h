#ifndef PARALLELTCTN_SUBGRAPH_H
#define PARALLELTCTN_SUBGRAPH_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "ankerl/unordered_dense.h"

typedef struct EdgeTemp {
    int u;
    int v;
    int time;
    bool operator==(const EdgeTemp& e) const {
        return ((this->u == e.u) && (this->v == e.v) && (this->time == e.time));
    }
} EdgeTemp;

typedef struct NeighTemp {
    std::vector<int> timestamps;
    std::vector<bool> weights;
    std::vector<int> dirs;
} NeighTemp;

#define MAX_N_NODES 500000000
#define MAX_T_TIMES 2000000000

struct hash_edge {

    size_t operator()(const EdgeTemp &et) const {

        int nu = (et.u < et.v ? et.u : et.v);
        int nv = (et.u < et.v ? et.v : et.u);
        unsigned long long id = (static_cast<unsigned long long>(MAX_N_NODES)
                                * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv))
                                        + et.time;
        return id;
    }

};

class Subgraph {

public:

    bool add_edge(const EdgeTemp &e, bool heaviness);

    bool remove_edge(const int u, const int v);

    void clear();

    void return_neighbors(const int u, std::vector<std::pair<int, NeighTemp>> &u_neighs);

    void return_edges(std::vector<EdgeTemp> &subgraph_edges) const;

    void return_nodes(std::vector<int> &subgraph_nodes) const;

    int get_degree_node(const int u);

    int num_edges() const;

    int num_nodes() const;

    Subgraph();

    ~Subgraph();

private:

    // -- subgraph of edges
    ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, NeighTemp>> subgraph_;

    // -- nodes degree
    ankerl::unordered_dense::map<int, int> nodes_degree_;

    int num_edges_;

};

#endif
