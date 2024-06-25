#ifndef SRTM90LOADER_H
#define SRTM90LOADER_H

#define HGT90_TILE_SIZE 1201

#include "TileLoader.h"
#include "Utils.h"
#include <cstdint>

class SRTM90Loader : public TileLoader {
public:
    SRTM90Loader();

    int num_elem;

    int16_t *in_buf;

    string getFilename(int lat, int lon) override;

    void loadTile(int lat, int lon, Tile &tile) override;

    static uint16_t swapByteOrderLittleEndian(uint16_t x);

    ~SRTM90Loader() override;
};

SRTM90Loader::SRTM90Loader() {
    TILE_SIZE = HGT90_TILE_SIZE;
    num_elem = HGT90_TILE_SIZE * HGT90_TILE_SIZE;
    in_buf = (int16_t *) malloc(sizeof(int16_t) * num_elem);
}

uint16_t SRTM90Loader::swapByteOrderLittleEndian(uint16_t x) {
    return (x >> 8) | (x << 8);
}

string SRTM90Loader::getFilename(int lat, int lon) {
    char buf[100];
    string filename = INPUT_DIRECTORY + "%c%02d%c%03d.hgt";
    snprintf(buf, sizeof(buf), filename.c_str(),
             (lat >= 0) ? 'N' : 'S',
             abs(static_cast<int>(lat)),
             (lon >= 0) ? 'E' : 'W',
             abs(static_cast<int>(lon)));

    filename = buf;
    return filename;
}

void SRTM90Loader::loadTile(int lat, int lon, Tile &tile) {
    tile.lat = lat;
    tile.lon = lon;
    for (int i = 0; i < HGT90_TILE_SIZE; i++)
        for (int j = 0; j < HGT90_TILE_SIZE; j++)
            tile.viz[i][j] = -1;

    string filename = getFilename(lat, lon);
    FILE *input_file = fopen(filename.c_str(), "rb");

    if (input_file == nullptr) {
        safe_print("Tile " + tile.getTileCoordinates() + " cannot be read\n");
        for (int i = 0; i < HGT90_TILE_SIZE; i++)
            for (int j = 0; j < HGT90_TILE_SIZE; j++)
                tile.elem[i][j] = 0;
    } else {
        safe_print("Tile " + tile.getTileCoordinates() + " successfully read\n");
        fread(in_buf, sizeof(int16_t), num_elem, input_file);
        for (int i = 0; i < HGT90_TILE_SIZE; i++)
            for (int j = 0; j < HGT90_TILE_SIZE; j++)
                tile.elem[i][j] = (int16_t) swapByteOrderLittleEndian(in_buf[i * HGT90_TILE_SIZE + j]);

        fclose(input_file);
    }

    if (lat - 1 >= MIN_LAT) {
        filename = getFilename(lat - 1, lon);
        FILE *down_input = fopen(filename.c_str(), "rb");
        if (down_input == nullptr) {
            for (int j = 0; j < HGT90_TILE_SIZE; j++)
                tile.elem[HGT90_TILE_SIZE - 1][j] = 0;
        } else {
            fread(in_buf, sizeof(int16_t), num_elem, down_input);
            for (int j = 0; j < HGT90_TILE_SIZE; j++)
                tile.elem[HGT90_TILE_SIZE - 1][j] = (int16_t) swapByteOrderLittleEndian(in_buf[j]);
            fclose(down_input);
        }
    }

    if (lon - 1 >= MIN_LON) {
        filename = getFilename(lat, lon - 1);
        FILE *left_input = fopen(filename.c_str(), "rb");
        if (left_input == nullptr) {
            for (int i = 0; i < HGT90_TILE_SIZE; i++)
                tile.elem[i][0] = 0;
        } else {
            fread(in_buf, sizeof(int16_t), num_elem, left_input);
            for (int i = 0; i < HGT90_TILE_SIZE; i++)
                tile.elem[i][0] = (int16_t) swapByteOrderLittleEndian(
                        in_buf[i * HGT90_TILE_SIZE + HGT90_TILE_SIZE - 1]);
            fclose(left_input);
        }
    }

    tile.elem[0][0] = tile.elem[0][HGT90_TILE_SIZE - 1] = tile.elem[HGT90_TILE_SIZE - 1][HGT90_TILE_SIZE - 1]
            = tile.elem[HGT90_TILE_SIZE - 1][0] = 0;

    int missing = 0, maximum = 0;
    for (int i = 0; i < HGT90_TILE_SIZE; i++)
        for (int j = 0; j < HGT90_TILE_SIZE; j++) {
            if (tile.elem[i][j] < 0) {
                tile.elem[i][j] = 0;
                missing++;
            }
            maximum = std::max(maximum, tile.elem[i][j]);
        }

    safe_print("Tile " + tile.getTileCoordinates() + ": missing = " + std::to_string(missing) + "; maximum = " +
               std::to_string(maximum) + "\n");
}

SRTM90Loader::~SRTM90Loader(){
    delete(in_buf);
}

#endif //SRTM90LOADER_H
