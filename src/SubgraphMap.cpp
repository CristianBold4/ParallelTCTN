//
// Created by Cristian Boldrin on 16/02/24.
//

#include "SubgraphMap.h"

SubgraphMap::SubgraphMap() : num_edges_(0) {};

SubgraphMap::~SubgraphMap() {
    subgraph_.clear();
}

bool SubgraphMap::add_edge(const EdgeTemp &e, bool heaviness) {

    int src = e.u;
    int dst = e.v;
    int t = e.time;

    NeighTempMap nt = {  t, heaviness, 1};
    subgraph_[src][dst].push_back(nt);
    nt = {t, heaviness, -1};
    subgraph_[dst][src].emplace_back(nt);

    num_edges_++;
    // -- increment nodes degrees
    nodes_degree_[src] ++;
    nodes_degree_[dst] ++;
    return true;

}


void SubgraphMap::prune(int curr_time, int delta) {

    ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, std::vector<NeighTempMap>>> new_sg;
    for (const auto &pair: subgraph_) {
        for (const auto &node_entry: pair.second) {
            for (int idx_prune = (int) node_entry.second.size() - 1; idx_prune >= 0; --idx_prune) {
                if (curr_time - node_entry.second[idx_prune].timestamp >= delta)
                    break;
                new_sg[pair.first][node_entry.first].emplace_back(node_entry.second[idx_prune]);
            }
        }
    }

    this->subgraph_ = new_sg;
}

bool SubgraphMap::find_edge(const int u, const int v) const {
    return (subgraph_.at(u).find(v) != subgraph_.at(u).end());
}

std::vector<NeighTempMap> *SubgraphMap::return_entries_edge(const int u, const int v) {
    return &(subgraph_[u][v]);
}


void SubgraphMap::clear() {
    subgraph_.clear();
    nodes_degree_.clear();
    num_edges_ = 0;
}

ankerl::unordered_dense::map<int, std::vector<NeighTempMap>> * SubgraphMap::return_neighbors(const int u) {
    return &(subgraph_[u]);
}


int SubgraphMap::get_degree_node(const int u) {

    auto u_it = nodes_degree_.find(u);
    if (u_it == nodes_degree_.end()) {
        return 0;
    }
    return u_it->second;
}

int SubgraphMap::num_nodes() const {
    return (int) subgraph_.size();
}

int SubgraphMap::num_edges() const {
    return num_edges_;
}

