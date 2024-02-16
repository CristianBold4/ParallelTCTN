//
// Created by Cristian Boldrin on 16/02/24.
//
#define MAX_N_NODES 500000000
#include <vector>
#include <algorithm>
#include <iostream>
#include "SubgraphMap.h"

#ifndef PARALLELTCTN_HASHING_H
#define PARALLELTCTN_HASHING_H

struct hash_pair {
    size_t operator()(const std::pair<int, int> &p) const {
        auto hash1 = std::hash<int>{}(p.first);
        auto hash2 = std::hash<int>{}(p.second);

        return hash1 ^ hash2;
    }
};

struct hash_edge {

    size_t operator()(const EdgeTemp &et) const {

        int nu = (et.u < et.v ? et.u : et.v);
        int nv = (et.u < et.v ? et.v : et.u);
        unsigned long long id = (static_cast<unsigned long long>(MAX_N_NODES)
                                 * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv))
                                + et.time;
        return id;
    }

};

#endif //PARALLELTCTN_HASHING_H
