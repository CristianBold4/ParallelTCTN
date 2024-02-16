#ifndef TCTN_EDGESET_H
#define TCTN_EDGESET_H

#include <ankerl/unordered_dense.h>
#include "Hashing.h"

class EdgeSet {

private:
    ankerl::unordered_dense::set<Edge, edge_hashing> edge_set;
public:
    void add_edge(const Edge& e) { edge_set.insert(e); }
    bool contains(const Edge& e) const { return edge_set.find(e) != edge_set.end(); }
    std::size_t size() { return edge_set.size(); }
    void remove_edge(const Edge& e) { edge_set.erase(e);}
    ankerl::unordered_dense::set<Edge, edge_hashing> get_set() { return edge_set; }
    ~EdgeSet() = default;
};


#endif //TCTN_EDGESET_H
