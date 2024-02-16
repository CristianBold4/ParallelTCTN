#include "../include/NodesMap.h"

void NodesMap::add_edge(const Edge& e, bool heavy) {
    int u = e.get_src();
    int v = e.get_dst();
    unsigned long long id = edge_to_id(u ,v);
    nodes_map[id].emplace_back(e, heavy);
    size++;
}

unsigned long long NodesMap::edge_to_id(int u, int v) {
    int nu = (u < v ? u : v);
    int nv = (u < v ? v : u);
    unsigned long long id = (static_cast<unsigned long long>(MAX_N_NODES)
                             * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv));
    return id;
}

void NodesMap::prune(int curr_time, int delta) {
    ankerl::unordered_dense::map<unsigned long long, std::vector<std::pair<Edge, bool>>> new_nodes_map;
    int new_size = 0;
    for(const auto& N: nodes_map) {
        unsigned long long id = N.first;
        std::vector<std::pair<Edge, bool>> pairs = N.second;
        for (const std::pair<Edge, bool>& p: pairs)
            if (curr_time - p.first.get_time() <= delta) {
                new_nodes_map[id].emplace_back(p.first, p.second);
                new_size++;
            }
    }
    this->nodes_map = new_nodes_map;
    this->size = new_size/2;
}

std::vector<std::pair<Edge, bool>> NodesMap::get_edges(int u, int v) {

    auto edges = nodes_map.find(edge_to_id(u, v));

    if(edges != nodes_map.end())
        return edges->second;
    else
        return {};
}