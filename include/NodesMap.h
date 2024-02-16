#ifndef TCTN_NODESMAP_H
#define TCTN_NODESMAP_H

#include <string>
#include <vector>
#include "ankerl/unordered_dense.h"
#include <algorithm>
#include "Hashing.h"

#define MAX_N_NODES 9000000

class NodesMap {

private:
    ankerl::unordered_dense::map<unsigned long long, std::vector<std::pair<Edge, bool>>> nodes_map;
    int size = 0;
    static unsigned long long edge_to_id(int u, int v);

public:
    void add_edge(const Edge& e, bool heavy = false);
    void prune(int curr_time, int delta);
    std::vector<std::pair<Edge, bool>> get_edges(int u, int v);
    bool contains_pair(int u, int v) {return nodes_map.find(edge_to_id(u, v)) != nodes_map.end();}
    int get_size() const { return size; }
    ~NodesMap() = default;

};

#endif //TCTN_NODESMAP_H