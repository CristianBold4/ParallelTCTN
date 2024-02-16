#include "../include/Edge.h"

Edge::Edge(int source, int destination, int time) {
    src = source;
    dst = destination;
    t = time;
}

bool Edge::operator==(const Edge& e) const {

    return (this->get_src() == e.get_src() &&
            this->get_dst() == e.get_dst() &&
            this->get_time() == e.get_time());
}
bool Edge::operator>(const Edge& e) const { return this->get_time() > e.get_time(); }

bool Edge::operator<(const Edge& e) const { return this->get_time() < e.get_time(); }

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << e.get_src() << " " << e.get_dst() << " " << e.get_time();
    return os;
}
