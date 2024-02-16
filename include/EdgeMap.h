#ifndef TCTN_EDGEMAP_H
#define TCTN_EDGEMAP_H

#include <vector>
#include <algorithm>
#include "ankerl/unordered_dense.h"
#include "Hashing.h"

class EdgeMap {

private:
    ankerl::unordered_dense::map<Edge, int, edge_hashing> edge_map;
public:
    void add_edge(const Edge& e) { edge_map[e] = 0; }
    void increment_value(const Edge& e, int val = 1) { edge_map.at(e) += val; }
    bool contains(const Edge& e) { return edge_map.find(e) != edge_map.end(); }
    std::vector<Edge> get_heavy_edges(int threshold);
    std::tuple<std::vector<Edge>, float> get_heavy_edges(float percentage_retain);
    int get_heaviness(const Edge& e) { if(this->contains(e)) return edge_map.at(e); else return 0;}
    std::size_t size() { return edge_map.size(); }
    ankerl::unordered_dense::map<Edge, int, edge_hashing> get_map() const { return edge_map; }
    EdgeMap get_heavy_map(float percentage_retain);
    ~EdgeMap() = default;

};

#endif