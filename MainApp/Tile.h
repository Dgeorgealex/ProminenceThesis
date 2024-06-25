#ifndef TILE_H
#define TILE_H

#include "Area.h"
#include "TileTree.h"
#include "ForestNode.h"
#include "Utils.h"
#include <stack>
#include <queue>
#include <vector>
#include <utility>

const int d_lin[] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int d_col[] = {0, 1, 1, 1, 0, -1, -1, -1};

using std::vector;
using std::stack;
using std::queue;

class Tile {
public:
    vector<vector<int>> elem, viz;
    vector<Area> areas;
    vector<vector<int>> graph;
    vector<int> order[H_MAX];
    int lat, lon;
    vector<pair<pair<int, int>, int>> tree;

    Tile() = default;

    void resize();

    [[nodiscard]] static bool isOnEdge(int lin, int col);

    [[nodiscard]] static bool isInside(int lin, int col);

    void getAreas();

    Area fillPlateau(int lin, int col, int id);

    void createGraph();

    void sortEdges();

    int uniteSameKind(int a, int b, int h, bool type, vector<ForestNode> &forest);

    int uniteDifferentKind(int a, int b, int h, vector<ForestNode> &forest);

    TileTree createTileTree();

    void writeInsidePeaks();

    string getTileCoordinates() const;
};

void Tile::resize() {
    elem = vector<vector<int>>(TILE_SIZE, vector<int>(TILE_SIZE));
    viz = vector<vector<int>>(TILE_SIZE, vector<int>(TILE_SIZE));
}

bool Tile::isOnEdge(int lin, int col) {
    return lin == 0 || col == 0 || lin == TILE_SIZE - 1 || col == TILE_SIZE - 1;
}

bool Tile::isInside(int lin, int col) {
    return lin >= 0 && col >= 0 && lin < TILE_SIZE && col < TILE_SIZE;
}

Area Tile::fillPlateau(int lin, int col, int id) {
    bool is_edge = false;
    bool is_peak = true;
    if (elem[lin][col] == 0)
        is_peak = false;

    queue<pair<int, int>> q;
    q.emplace(lin, col);
    viz[lin][col] = id;

    while (!q.empty()) {
        auto u = q.front();
        q.pop();

        is_edge = is_edge || isOnEdge(u.first, u.second);

        for (int d = 0; d < 8; d++) {
            pair<int, int> v;
            v.first = u.first + d_lin[d];
            v.second = u.second + d_col[d];

            if (!isInside(v.first, v.second))
                continue;

            if (elem[v.first][v.second] > elem[u.first][u.second]) {
                is_peak = false;
                continue;
            }

            if (viz[v.first][v.second] == -1 && elem[v.first][v.second] == elem[u.first][u.second]) {
                viz[v.first][v.second] = id;
                q.push(v);
            }
        }
    }
    return {id, AreaRep(lin, col, lat, lon), elem[lin][col], is_edge, is_peak};
}

void Tile::getAreas() {
    areas.clear();
    for (int i = 0; i < TILE_SIZE; i++)
        for (int j = 0; j < TILE_SIZE; j++)
            if (viz[i][j] == -1)
                areas.push_back(fillPlateau(i, j, (int) areas.size()));
}

void Tile::createGraph() {
    graph.resize((int) areas.size());
    for (auto &g: graph)
        g.clear();

    for (int i = 0; i < TILE_SIZE; i++)
        for (int j = 0; j < TILE_SIZE; j++)
            for (int d = 0; d < 8; d++) {
                int next_lin = i + d_lin[d], next_col = j + d_col[d];
                if (!isInside(next_lin, next_col))
                    continue;

                if (viz[next_lin][next_col] != viz[i][j])
                    graph[viz[i][j]].push_back(viz[next_lin][next_col]);
            }
}

void Tile::sortEdges() {
    for (int h = H_MAX - 1; h >= 0; h--)
        order[h].clear();

    for (auto area: areas)
        order[area.height].push_back(area.id);
}

int Tile::uniteSameKind(int a, int b, int h, bool type, vector<ForestNode> &forest) {
    int p_a = ForestNode::findParent(a, forest), p_b = ForestNode::findParent(b, forest);

    if (p_a == p_b)
        return p_a;

    if (forest[p_a].height == forest[p_b].height) {
        if(forest[p_a].sz < forest[p_b].sz)
            std::swap(p_a, p_b);
        areas[forest[p_b].highest_id].updateSaddle(h, type, areas[forest[p_a].highest_id].rep);
    } else {
        if (forest[p_a].height > forest[p_b].height)
            std::swap(p_a, p_b);

        areas[forest[p_a].highest_id].updateSaddle(h, type, areas[forest[p_b].highest_id].rep);
    }
    if (!type) {
        areas[forest[p_a].highest_id].true_saddle = false;
        areas[forest[p_b].highest_id].true_saddle = false;
        tree.push_back({{forest[p_a].highest_id, forest[p_b].highest_id}, h});
    }

    return ForestNode::setUnion(p_a, p_b, forest);
}

int Tile::uniteDifferentKind(int a, int b, int h, vector<ForestNode> &forest) {
    int p_a = ForestNode::findParent(a, forest), p_b = ForestNode::findParent(b, forest);

    if (p_a == p_b)
        return p_a;

    if (forest[p_a].height < forest[p_b].height)
        areas[forest[p_a].highest_id].updateSaddle(h, true, areas[forest[p_b].highest_id].rep);
    else if (forest[p_a].height == forest[p_b].height) {
        areas[forest[p_a].highest_id].updateSaddle(h, true, areas[forest[p_b].highest_id].rep);
        areas[forest[p_b].highest_id].true_saddle = false;
    } else {
        areas[forest[p_b].highest_id].updateSaddle(h, false, areas[forest[p_a].highest_id].rep);
        areas[forest[p_a].highest_id].true_saddle = false;
        areas[forest[p_b].highest_id].true_saddle = false;
        tree.push_back({{forest[p_a].highest_id, forest[p_b].highest_id}, h});
    }
    return ForestNode::setUnion(p_a, p_b, forest);
}

TileTree Tile::createTileTree() {
    getAreas();
    createGraph();
    sortEdges();
    tree.clear();

    vector<ForestNode> forest((int) areas.size());
    for (auto it: areas)
        forest[it.id] = ForestNode(it);

    for (int h = H_MAX - 1; h >= 0; h--) {
        if (order[h].empty())
            continue;

        for (auto area: order[h]) {
            stack<int> inside, outside;
            for (auto it: graph[area]) {
                if (areas[it].height < h)
                    continue;

                int p_it = ForestNode::findParent(it, forest);

                if (forest[p_it].is_edge)
                    outside.push(p_it);
                else
                    inside.push(p_it);
            }

            int p_area = ForestNode::findParent(area, forest);
            if (forest[p_area].is_edge)
                outside.push(p_area);
            else
                inside.push(p_area);

            while (inside.size() > 1) {
                int a = inside.top();
                inside.pop();
                int b = inside.top();
                inside.pop();
                int aux = uniteSameKind(a, b, h, true, forest);
                inside.push(aux);
            }

            while (outside.size() > 1) {
                int a = outside.top();
                outside.pop();
                int b = outside.top();
                outside.pop();
                int aux = uniteSameKind(a, b, h, false, forest);
                outside.push(aux);
            }

            if (!outside.empty() && !inside.empty()) {
                int a = inside.top();
                int b = outside.top();
                uniteDifferentKind(a, b, h, forest);
            }
        }
    }

    writeInsidePeaks();

    return {areas, elem, viz, tree};
}

void Tile::writeInsidePeaks() {
    int total_peaks = 0, inside_peaks = 0;
    string filename = outputFile(lat, lon);
    FILE *output_file = fopen(filename.c_str(), "w");

    for (auto area: areas) {
        if (area.is_peak)
            total_peaks++;

        if (area.is_peak && area.true_saddle) {
            auto [true_lat, true_lon] = area.rep.getCoords();
            auto [true_lat_i, true_lon_i] = area.island_parent.getCoords();

            if (area.height - area.saddle_height >= THRESHOLD)
                fprintf(output_file, "%.6f,%.6f,%d,%d,%.6f,%.6f\n", true_lat, true_lon, area.height,
                        area.height - area.saddle_height, true_lat_i, true_lon_i);

            inside_peaks++;
        }
    }
    string to_print = "----------------------------------------\n";
    to_print += "Processed Tile " + getTileCoordinates() + "\n";
    to_print += "Total peaks = " + std::to_string(total_peaks) + "; Outside peaks = " +
                std::to_string(total_peaks - inside_peaks) + "\n";
    to_print += "----------------------------------------\n";
    safe_print(to_print);
    fclose(output_file);


    print_info(lat, lon, total_peaks, total_peaks - inside_peaks);
}

string Tile::getTileCoordinates() const {
    return "(" + std::to_string(lat) + ", " + std::to_string(lon) + ")";
}

#endif //TILE_H
