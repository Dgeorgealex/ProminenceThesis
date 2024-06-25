#ifndef AREAREP_H
#define AREAREP_H

#include <utility>
#include "Utils.h"

using std::pair;

class AreaRep {
public:
    int lin, col, lat, lon;

    AreaRep() = default;

    AreaRep(int lin, int col, int lat, int lon);

    bool operator<(const AreaRep &otr) const;

    [[nodiscard]] pair<double, double> getCoords() const;
};

AreaRep::AreaRep(int lin, int col, int lat, int lon) {
    this->lin = lin;
    this->col = col;
    this->lat = lat;
    this->lon = lon;
}

pair<double, double> AreaRep::getCoords() const {
    double true_lat = lat + 1 - (double) lin / TILE_SIZE;
    double true_lon = lon + (double) col / TILE_SIZE;
    return {true_lat, true_lon};
}

bool AreaRep::operator<(const AreaRep &otr) const {
    if (lat == otr.lat && lon == otr.lon)
        return lin < otr.lin || (lin == otr.lin && col < otr.col);

    if (lat == otr.lat) {
        if (lon < otr.lon)
            return true;
        else
            return false;
    }

    return lat < otr.lat;
}

#endif //AREAREP_H
