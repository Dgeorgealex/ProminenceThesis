#ifndef AREA_H
#define AREA_H

#include "AreaRep.h"
#include <utility>

class Area {
public:
    int id;
    AreaRep rep;
    AreaRep island_parent;
    int height;
    bool is_edge, is_peak;
    int saddle_height;
    bool true_saddle;

    Area() = default;

    Area(int id, AreaRep rep, int height, bool is_edge, bool is_peak);

    void updateSaddle(int h, bool ts, const AreaRep &new_island_parent);
};


Area::Area(int id, AreaRep rep, int height, bool is_edge, bool is_peak) {
    this->id = id;
    this->rep = rep;
    this->island_parent = rep;
    this->height = height;
    this->is_edge = is_edge;
    this->is_peak = is_peak;
    this->saddle_height = 0;
    this->true_saddle = false;
}

void Area::updateSaddle(int h, bool ts, const AreaRep &new_island_parent) {
    if (saddle_height == h)
        island_parent = std::min(island_parent, new_island_parent);
    else if (saddle_height < h) {
        island_parent = new_island_parent;
        saddle_height = h;
    }

    true_saddle = ts;
}


#endif //AREA_H
