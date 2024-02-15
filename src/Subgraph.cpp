#include "Subgraph.h"

Subgraph::Subgraph() : num_edges_(0) {};

Subgraph::~Subgraph() {
    subgraph_.clear();
}

bool Subgraph::add_edge(const EdgeTemp &e, bool heaviness) {

    int src = e.u;
    int dst = e.v;
    int t = e.time;

    NeighTemp nt = { dst, t, heaviness, 1};
    subgraph_[src].emplace_back(nt);
    nt = { src, t, heaviness, -1};
    subgraph_[dst].emplace_back(nt);

    num_edges_++;
    return true;

}

void Subgraph::prune(int curr_time, int delta) {

    ankerl::unordered_dense::map<int, std::vector<NeighTemp>> new_sg;
    for (auto pair : subgraph_){
        for (int idx_prune = (int) pair.second.size() - 1; idx_prune >= 0; --idx_prune) {
            if (curr_time - (pair.second)[idx_prune].timestamp >= delta) {
                break;
            }
            new_sg.emplace(pair);
        }
    }

    this->subgraph_ = new_sg;
//    for (auto &pair : subgraph_) {
//        neighbors = &pair.second;
//        auto neigh_it = neighbors->begin();
//        while (neigh_it != neighbors->end()) {
//            if (curr_time - neigh_it->timestamp < delta)
//                break;
//            neigh_it = neighbors->erase(neigh_it);
//        }
//
//    }

}



void Subgraph::clear() {
    subgraph_.clear();
    num_edges_ = 0;
}

std::vector<NeighTemp>* Subgraph::return_neighbors(const int u) {
    return &(subgraph_[u]);
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
