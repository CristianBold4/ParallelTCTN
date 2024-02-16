#ifndef TCTN_EDGE_H
#define TCTN_EDGE_H

#include <string>
#include <iostream>

class Edge {

private:
    int src;
    int dst;
    int t;

public:
    Edge() {};
    Edge(int source, int destination, int time);
    int get_src() const { return src; }
    int get_dst() const { return dst; }
    int get_time() const { return t; }
    bool operator==(const Edge& e) const;
    bool operator>(const Edge& e) const;
    bool operator<(const Edge& e) const;
    ~Edge() = default;
};

std::ostream& operator<<(std::ostream& os, const Edge& e);

#endif //TCTN_EDGE_H
