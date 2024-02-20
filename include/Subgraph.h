#ifndef TRIANGLESWITHPREDICTIONS_SUBGRAPH_H
#define TRIANGLESWITHPREDICTIONS_SUBGRAPH_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "ankerl/unordered_dense.h"

#define Edge std::pair<int, int>

class Subgraph {

public:

    bool add_edge(const int u, const int v, char label);

    bool remove_edge(const int u, const int v);

    void clear();

    void return_neighbors(const int u, std::vector<std::pair<int, bool>> &u_neighs) const;

    void return_edges(std::vector<Edge> &subgraph_edges) const;

    void return_nodes(std::vector<int> &subgraph_nodes) const;

    void change_edge_label(int u, int v, char new_label);

    int get_degree_node(const int u) const;

    int num_edges() const;

    int num_nodes() const;

    Subgraph(const long k);

    Subgraph();

    ~Subgraph();

private:

    // -- subgraph of edges: unordered map of unordered map (int -> bool == heavy)
    ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, bool>> subgraph_;
    int num_edges_;

};

#endif //TRIANGLESWITHPREDICTIONS_SUBGRAPH_H
