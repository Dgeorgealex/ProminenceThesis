#ifndef OUTPUTBUFFER_H
#define OUTPUTBUFFER_H

#include "Area.h"
#include "Utils.h"
#include <vector>
#include <algorithm>

#define BUFFER_SIZE 100000

using std::vector;

class OutputBuffer {
public:
    vector<Area> areas;

    void emptyBuffer();

    void addArea(const Area &area);
} outputBuffer;

void OutputBuffer::emptyBuffer() {
    std::cout << "\n\n\nEmptying output buffer\n\n\n";
    if (areas.empty())
        return;

    std::sort(areas.begin(), areas.end(), [](const Area &a, const Area &b) {
        return a.rep < b.rep;
    });

    int lat = areas[0].rep.lat, lon = areas[0].rep.lon;
    string filename = outputFile(lat, lon);
    FILE *output_file = fopen(filename.c_str(), "a");
    for (auto area: areas) {
        if (lat != area.rep.lat || lon != area.rep.lon) {
            fclose(output_file);
            lat = area.rep.lat;
            lon = area.rep.lon;
            filename = outputFile(lat, lon);
            output_file = fopen(filename.c_str(), "a");
        }

        auto [true_lat, true_lon] = area.rep.getCoords();
        auto [true_lat_i, true_lon_i] = area.island_parent.getCoords();
        if (area.height - area.saddle_height >= THRESHOLD)
            fprintf(output_file, "%.6f,%.6f,%d,%d,%.6f,%.6f\n", true_lat, true_lon, area.height,
                    area.height - area.saddle_height, true_lat_i, true_lon_i);
    }
    fclose(output_file);
    areas.clear();
}

void OutputBuffer::addArea(const Area &area) {
    areas.push_back(area);
    if (areas.size() == BUFFER_SIZE)
        emptyBuffer();
}

#endif //OUTPUTBUFFER_H
