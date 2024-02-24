#include "Subgraph.h"
#include <algorithm>

Subgraph::Subgraph(long k) : num_edges_(0) {
    subgraph_ = ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, bool>>(k);
}

Subgraph::Subgraph() : num_edges_(0) {}

Subgraph::~Subgraph() {
    subgraph_.clear();
}

bool Subgraph::add_edge(const int u, const int v, char label) {

    auto u_it = subgraph_.find(u);
    if (u_it != subgraph_.end()) {
        // -- u is already in sg
        auto uv_it = u_it->second.find(v);
        if (uv_it != u_it->second.end()) {
            // -- edge uv is already in sg -> multiple edge
            return false;
        }
    }

    auto v_it = subgraph_.find(v);
    if (v_it != subgraph_.end()) {
        // -- v is already in sg
        auto vu_it = v_it->second.find(u);
        if (vu_it != v_it->second.end()) {
            // -- edge vu is already in sg -> multiple edge
            return false;
        }
    }

    // -- edge not present in the subgraph
    num_edges_++;
    bool is_det = (label != 'L');
//    u_it->second.emplace(v, is_det);
//    v_it->second.emplace(u, is_det);
    subgraph_[u].emplace(v, is_det);
    subgraph_[v].emplace(u, is_det);

    return true;

}

bool Subgraph::remove_edge(const int u, const int v) {

    auto u_it = subgraph_.find(u);
    auto v_it = subgraph_.find(v);
    if (u_it != subgraph_.end() && v_it != subgraph_.end()){
        // -- edge is present in sg
        num_edges_--;
        u_it->second.erase(v);
        v_it->second.erase(u);
        return true;

    } else {
        // -- edge is not present in sg
        return false;
    }

}

void Subgraph::clear() {
    subgraph_.clear();
    num_edges_ = 0;
}

void Subgraph::return_neighbors(const int u, std::vector<std::pair<int, bool>> &u_neighs) const {

    auto u_it = subgraph_.find(u);
    if (u_it != subgraph_.end()) {
        for (const auto &neigh: u_it->second) {
            u_neighs.emplace_back(neigh);
        }
    }
}

void Subgraph::return_edges(std::vector<Edge > &subgraph_edges) const {

    subgraph_edges.clear();
    for (auto &edge: subgraph_) {
        int src = edge.first;
        for (auto &src_neigh: edge.second) {
            subgraph_edges.emplace_back(src, src_neigh.first);
        }
    }
}

void Subgraph::return_nodes(std::vector<int> &subgraph_nodes) const {

    subgraph_nodes.clear();
    for (const auto& node : subgraph_) {
        subgraph_nodes.emplace_back(node.first);
    }

}


void Subgraph::change_edge_label(int u, int v, char new_label) {

    auto u_it = subgraph_.find(u);
    auto v_it = subgraph_.find(v);
    if (u_it != subgraph_.end() && v_it != subgraph_.end()) {
        bool is_det = (new_label != 'L');
        u_it->second.at(v) = is_det;
        v_it->second.at(u) = is_det;
    }

}

int Subgraph::get_degree_node(const int u) const {

    const auto u_it = subgraph_.find(u);
    if (u_it == subgraph_.end()) {
        return 0;
    }
    return (int) u_it->second.size();
}

int Subgraph::num_edges() const {
    return num_edges_;
}

int Subgraph::num_nodes() const {
    return subgraph_.size();
}
