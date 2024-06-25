#include "SRTM90Loader.h"
#include "SRTM30Loader.h"
#include "Tile.h"
#include "OutputBuffer.h"
#include <chrono>

using namespace std;

vector<Tile> tile;
vector<TileTree> tile_tree;
stack<TileTree> stack_tile_tree;
vector<TileLoader *> tile_loader;
int nr_tiles = 0;
vector<pair<int, pair<int, int>>> operations;

void run_thread(int lat, int lon, Tile &t, TileLoader *tl, TileTree &tt) {
    tl->loadTile(lat, lon, t);
    tt = t.createTileTree();
}

void div_et_imp(int minLat, int maxLat, int minLon, int maxLon) {
    if (minLat == maxLat && minLon == maxLon) {
        nr_tiles++;
        operations.push_back({0, {minLat, minLon}});
        return;
    }
    int mijLat = minLat + (maxLat - minLat) / 2;
    int mijLon = minLon + (maxLon - minLon) / 2;

    if (minLat != maxLat && minLon != maxLon) {
        div_et_imp(minLat, mijLat, minLon, mijLon);
        div_et_imp(minLat, mijLat, mijLon + 1, maxLon);
        operations.push_back({1, {0, 0}});


        div_et_imp(mijLat + 1, maxLat, minLon, mijLon);
        div_et_imp(mijLat + 1, maxLat, mijLon + 1, maxLon);
        operations.push_back({1, {0, 0}});

        operations.push_back({2, {0, 0}});
        return;
    }

    if (minLat == maxLat) {
        div_et_imp(minLat, maxLat, minLon, mijLon);
        div_et_imp(minLat, maxLat, mijLon + 1, maxLon);
        operations.push_back({1, {0, 0}});
        return;
    }

    div_et_imp(minLat, mijLat, minLon, maxLon);
    div_et_imp(mijLat + 1, maxLat, minLon, maxLon);
    operations.push_back({2, {0, 0}});
}

int main(int argc, char **argv) {
    parseArguments(argc, argv);
    tile.resize(NR_THREADS), tile_tree.resize(NR_THREADS), tile_loader.resize(NR_THREADS);

    for (int i = 0; i < NR_THREADS; i++)
        if (DATA_TYPE == SRTM30)
            tile_loader[i] = new SRTM30Loader();
        else
            tile_loader[i] = new SRTM90Loader();

    for (auto &t: tile)
                t.resize();

    auto start = chrono::steady_clock::now();

    div_et_imp(MIN_LAT, MAX_LAT, MIN_LON, MAX_LON);

    int nr_buckets = nr_tiles / NR_THREADS + (nr_tiles % NR_THREADS != 0), op = 0, tiles_processed = 0;
    for (int i = 1; i <= nr_buckets; i++) {
        vector<thread> threads;
        int busy = 0;

        cout << "\n\nProcessing the next " << min(nr_buckets - i + 1, NR_THREADS) << " Tiles:" << '\n';
        int first_op = op;
        for (; op < operations.size(); op++)
            if (operations[op].first == 0) {
                if (busy == NR_THREADS)
                    break;
                auto [lat, lon] = operations[op].second;
                threads.emplace_back(run_thread, lat, lon, std::ref(tile[busy]), tile_loader[busy],
                                     std::ref(tile_tree[busy]));
                busy++;
            }
        for (auto &t: threads)
            t.join();

        int tl = 0;
        cout << "Done processing Tiles; Start TileTree merges:\n";
        for (int j = first_op; j < op; j++)
            if (operations[j].first == 0) {
                cout << "\tPush TileTree (" << operations[j].second.first << ", " << operations[j].second.second
                     << ")\n";
                stack_tile_tree.push(tile_tree[tl]);
                tiles_processed++;
                tl++;
            } else if (operations[j].first == 1) {
                cout << "\tMerge TileTrees with same Latitude\n";
                TileTree top_tile_tree2 = stack_tile_tree.top();
                stack_tile_tree.pop();
                TileTree top_tile_tree1 = stack_tile_tree.top();
                stack_tile_tree.pop();
                top_tile_tree1.mergeLatitude(top_tile_tree2);
                stack_tile_tree.push(top_tile_tree1);
            } else {
                cout << "\tMerge TileTrees with same Longitude\n";
                TileTree top_tile_tree2 = stack_tile_tree.top();
                stack_tile_tree.pop();
                TileTree top_tile_tree1 = stack_tile_tree.top();
                stack_tile_tree.pop();
                top_tile_tree1.mergeLongitude(top_tile_tree2);
                stack_tile_tree.push(top_tile_tree1);
            }
        cout << "Tiles processed = " << tiles_processed << '\n';
    }

    TileTree last_tile = stack_tile_tree.top();
    last_tile.printAllPeaks();
    cout << "LAST BAD PEAKS = " << LAST_BAD << '\n';

    if(EXPORT)
        last_tile.exportTileTree();

    outputBuffer.emptyBuffer();

    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<std::chrono::seconds>(end - start);
    cout << "\n\nTime elapsed: " << elapsed << '\n';

    for (TileLoader *loader: tile_loader)
        delete loader;
    return 0;
}