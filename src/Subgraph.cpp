#include "Subgraph.h"
#include <algorithm>
#include <cassert>

Subgraph::Subgraph() : num_edges_(0) {};

Subgraph::~Subgraph() {
    subgraph_.clear();
}

bool Subgraph::add_edge(const EdgeTemp &e, bool heaviness) {

    int src = e.u;
    int dst = e.v;
    int t = e.time;

    // -- src
    auto src_it = subgraph_.find(src);
    if (src_it == subgraph_.end()) {
        nodes_degree_[src] = 0;
        // -- init vectors
        NeighTemp src_neigh;
        src_neigh.timestamps = std::vector({t});
        src_neigh.weights = std::vector({heaviness});
        src_neigh.dirs = std::vector({1});

        ankerl::unordered_dense::map<int, NeighTemp> src_map {};
        src_map.emplace(dst, src_neigh);
        subgraph_.emplace(src, src_map);
    } else {
        assert(get_degree_node(src) > 0);
        nodes_degree_[src] ++;
        // -- push back to vecs
        subgraph_[src][dst].timestamps.emplace_back(t);
        subgraph_[src][dst].weights.emplace_back(heaviness);
        subgraph_[src][dst].dirs.emplace_back(1);
    }

    // -- dst
    auto dst_it = subgraph_.find(dst);
    if (dst_it == subgraph_.end()) {
        nodes_degree_[dst] = 0;
        // -- init vectors
        NeighTemp dst_neigh;
        dst_neigh.timestamps = std::vector({t});
        dst_neigh.weights = std::vector({heaviness});
        dst_neigh.dirs = std::vector({-1});

        ankerl::unordered_dense::map<int, NeighTemp> dst_map {};
        dst_map.emplace(dst, dst_neigh);
        subgraph_.emplace(dst, dst_map);
    } else {
        assert(get_degree_node(dst) > 0);
        nodes_degree_[dst] ++;
        // -- push back to vecs
        subgraph_[dst][src].timestamps.emplace_back(t);
        subgraph_[dst][src].weights.emplace_back(heaviness);
        subgraph_[dst][src].dirs.emplace_back(-1);
    }

    num_edges_++;
    return true;

}

bool Subgraph::remove_edge(const int u, const int v) {
    // -- TO BE IMPLEMENTED
}

void Subgraph::clear() {
    subgraph_.clear();
    num_edges_ = 0;
}

void Subgraph::return_neighbors(const int u, std::vector<std::pair<int, NeighTemp>> &u_neighs) {
    u_neighs.clear();
    for (const auto &u_neigh : subgraph_[u]) {
        u_neighs.emplace_back(u_neigh);
    }
}


int Subgraph::get_degree_node(const int u) {

    auto u_it = nodes_degree_.find(u);
    if (u_it == nodes_degree_.end()) {
        return 0;
    }
    return u_it->second;
}

int Subgraph::num_nodes() const {
    return (int) subgraph_.size();
}

int Subgraph::num_edges() const {
    return num_edges_;
}
