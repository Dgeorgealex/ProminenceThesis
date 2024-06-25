#ifndef TILELOADER_H
#define TILELOADER_H

#include <vector>
#include <string>
#include <iostream>
#include "Tile.h"

using std::vector;
using std::string;
using std::cout;

class TileLoader {
public:
    virtual void loadTile(int lat, int lon, Tile &tile) = 0;

    virtual string getFilename(int lat, int lon) = 0;

    virtual ~TileLoader(){};
};


#endif //TILELOADER_H
