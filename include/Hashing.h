#ifndef TCTN_HASHING_H
#define TCTN_HASHING_H

#include <utility>
#include "Edge.h"

struct pair_hashing {
    size_t operator()(const std::pair<int, int>& p) const
    {
        auto hash1 = std::hash<int>{}(p.first);
        auto hash2 = std::hash<int>{}(p.second);
        return hash1 ^ hash2;
    }
};

struct edge_hashing {

    size_t operator()(const Edge& p) const
    {
        auto hash1 = std::hash<int>{}(p.get_src());
        auto hash2 = std::hash<int>{}(p.get_dst());
        auto hash3 = std::hash<int>{}(p.get_time());

        return hash1 ^ hash2 ^ hash3;
    }
};

#endif //TCTN_HASHING_H
