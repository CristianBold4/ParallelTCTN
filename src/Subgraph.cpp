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

    std::vector<NeighTemp> *neighbors;
    for (auto &pair : subgraph_) {
        neighbors = &pair.second;
        auto neigh_it = neighbors->begin();
        while (neigh_it != neighbors->end()) {
            if (curr_time - neigh_it->timestamp < delta)
                break;
            neigh_it = neighbors->erase(neigh_it);
        }

    }
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
