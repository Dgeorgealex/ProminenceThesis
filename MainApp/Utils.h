#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <thread>
#include <mutex>
#include "CLI11.hpp"

using std::string;
using std::mutex;

enum DataType {
    SRTM30,
    SRTM90
};

const int H_MAX = 8850;
int TILE_SIZE = 0;
int LAST_BAD = 0;

int MIN_LAT = 0;
int MAX_LAT = -1;
int MIN_LON = 0;
int MAX_LON = -1;
int NR_THREADS = 1;
bool EXPORT = false;
int THRESHOLD = 0;
string INPUT_DIRECTORY = ".";
string OUTPUT_DIRECTORY = ".";
DataType DATA_TYPE = SRTM90;

string outputFile(int lat, int lon) {
    char buf[100];
    string filename = OUTPUT_DIRECTORY + "prominence-%c%02d%c%03d.txt";
    snprintf(buf, sizeof(buf), filename.c_str(),
             (lat >= 0) ? 'N' : 'S',
             abs(static_cast<int>(lat)),
             (lon >= 0) ? 'E' : 'W',
             abs(static_cast<int>(lon)));

    filename = buf;
    return filename;
}

mutex cout_mutex;

void safe_print(const std::string &message) {
    std::lock_guard<mutex> guard(cout_mutex);
    std::cout << message;
}

string treeExportFile() {
    return OUTPUT_DIRECTORY + "tree.txt";
}

mutex info_mutex;

void print_info(int lat, int lon, int total_peaks, int outside_peaks) {
    std::lock_guard<mutex> guard(info_mutex);
    string filename = OUTPUT_DIRECTORY + "info.txt";
    FILE *info_file = fopen(filename.c_str(), "a");
    fprintf(info_file, "%d,%d,%d,%d\n", lat, lon, total_peaks, outside_peaks);
    fclose(info_file);
//    return;
}


int parseArguments(int argc, char **argv) {
    if (argc == 1) {
        MIN_LAT = 40;
        MAX_LAT = 51;
        MIN_LON = 18;
        MAX_LON = 29;
        EXPORT = true;
        NR_THREADS = 5;
        THRESHOLD = 500;
        DATA_TYPE = SRTM90;
        INPUT_DIRECTORY = "../../data/Romania/";
        OUTPUT_DIRECTORY = "../../data/RomaniaANS/";
        return 0;
    }

    CLI::App app{"Prominence algorithm"};

    app.add_option("minLat", MIN_LAT, "Minimum latitude")->required();
    app.add_option("maxLat", MAX_LAT, "Maximum latitude")->required();
    app.add_option("minLon", MIN_LON, "Minimum longitude")->required();
    app.add_option("maxLon", MAX_LON, "Maximum longitude")->required();

    string dataType;

    app.add_option("-i,--inputDir", INPUT_DIRECTORY, "Input directory, default = \".\"");
    app.add_option("-o,--outputDir", OUTPUT_DIRECTORY, "Output directory, default = \".\"");
    app.add_option("-d,--dataType", dataType, R"(Data type: "SRTM30", "SRTM90", default = "SRTM90")");
    app.add_option("-p,--pThreshold", THRESHOLD, "Prominence threshold, default = 0");
    app.add_option("-t,--threadNum", NR_THREADS, "Number of threads used, default = 1");
    app.add_flag("-e,--expTree", EXPORT, "Export tree flag, default = false");


    CLI11_PARSE(app, argc, argv);


    if (!dataType.empty()) {
        if (dataType == "SRTM90")
            DATA_TYPE = SRTM90;
        else if (dataType == "SRTM30")
            DATA_TYPE = SRTM30;
        else {
            std::cout << "Invalid data type " + dataType << '\n';
            exit(-1);
        }
    }
    INPUT_DIRECTORY += "/";
    OUTPUT_DIRECTORY += "/";
    return 0;
}

#endif //UTILS_H
