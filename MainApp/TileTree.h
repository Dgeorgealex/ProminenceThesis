#ifndef TILETREE_H
#define TILETREE_H

#include "Area.h"
#include "ForestNode.h"
#include "Utils.h"
#include "OutputBuffer.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

using std::vector;
using std::pair;
using std::unordered_map;
using std::unordered_set;

class TileTree {

public:
    vector<Area> areas;
    vector<pair<int, int>> up, down, left, right;
    vector<pair<pair<int, int>, int>> edges;

    TileTree() = default;

    TileTree(const vector<Area> &a, const vector<vector<int>> &elem, const vector<vector<int>> &viz,
             const vector<pair<pair<int, int>, int>> &tree);

    static void normVector(vector<pair<int, int>> &a, const unordered_map<int, int> &t, vector<Area> &areas);

    void uniteSameKind(int a, int b, int h, bool type, vector<ForestNode> &forest,
                       vector<pair<pair<int, int>, int>> &new_edges);

    void uniteDifferentKind(int a, int b, int h, vector<ForestNode> &forest,
                            vector<pair<pair<int, int>, int>> &new_edges);

    void addAreasOutsideUnion(vector<Area> &new_areas, const unordered_set<int> &m, unordered_map<int, int> &t);

    void addAreasFromUnion(vector<Area> &new_areas, const vector<pair<int, int>> &v, const unordered_map<int, int> &t);

    static int uniteSide(const vector<pair<int, int>> &a, const vector<pair<int, int>> &b, unordered_map<int, int> &t,
                         unordered_map<int, int> &t_otr, int id);

    static void dfs(int x, int k, const vector<vector<int>> &v, vector<int> &group);

    void mergeLatitude(TileTree &otr);

    void mergeLongitude(TileTree &otr);

    void runAlgorithm();

    void printRemoveInsidePeaks(vector<pair<pair<int, int>, int>> &new_edges);

    void printAllPeaks();

    void exportTileTree();
};

void TileTree::normVector(vector<pair<int, int>> &a, const unordered_map<int, int> &t, vector<Area> &areas) {
    for (auto &it: a) {
        it.second = t.find(it.second)->second;
        areas[it.second].is_edge = true;
    }
}

void TileTree::addAreasOutsideUnion(vector<Area> &new_areas, const unordered_set<int> &m,
                                    unordered_map<int, int> &t) {
    for (auto area: areas)
        if (!m.contains(area.id)) {
            Area new_area = Area(area);
            new_area.id = (int) new_areas.size();

            t[area.id] = new_area.id;

            new_areas.push_back(new_area);
        }
}

void TileTree::addAreasFromUnion(vector<Area> &new_areas, const vector<pair<int, int>> &v,
                                 const unordered_map<int, int> &t) {
    for (auto it: v) {
        int pos = t.find(it.second)->second;
        if (new_areas[pos].id == -1) {
            new_areas[pos] = areas[it.second];
            new_areas[pos].id = pos;
            new_areas[pos].is_edge = false;
        } else {
            new_areas[pos].rep = std::min(new_areas[pos].rep, areas[it.second].rep);
            new_areas[pos].saddle_height = std::max(new_areas[pos].saddle_height, areas[it.second].saddle_height);
            new_areas[pos].is_peak = new_areas[pos].is_peak && areas[it.second].is_peak;
        }
    }
}

void TileTree::dfs(int x, int k, const vector<vector<int>> &v, vector<int> &group) {
    group[x] = k;
    for (auto it: v[x])
        if (group[it] == -1)
            dfs(it, k, v, group);
}

int TileTree::uniteSide(const vector<pair<int, int>> &a, const vector<pair<int, int>> &b, unordered_map<int, int> &t,
                        unordered_map<int, int> &t_otr, int id) {
    int cnt = 0;

    for (int i = 0; i < a.size(); i++) {
        if (!t.contains(a[i].second)) {
            t[a[i].second] = cnt;
            cnt++;
        }
        if (!t_otr.contains(b[i].second)) {
            t_otr[b[i].second] = cnt;
            cnt++;
        }
    }

    vector<vector<int>> v(cnt);
    vector<int> group(cnt, -1);

    for (int i = 0; i < a.size(); i++) {
        int x = t[a[i].second], y = t_otr[b[i].second];
        v[x].push_back(y);
        v[y].push_back(x);
    }

    int k = 0;
    for (int i = 0; i < cnt; i++)
        if (group[i] == -1) {
            dfs(i, k, v, group);
            k++;
        }

    for (int i = 0; i < a.size(); i++) {
        int pa = t[a[i].second], pb = t_otr[b[i].second];
        if (pa < id)
            t[a[i].second] = id + group[pa];
        if (pb < id)
            t_otr[b[i].second] = id + group[pb];
    }

    return k;
}

void TileTree::mergeLatitude(TileTree &otr) {
    for (int i = 0; i < (int) right.size(); i++)
        assert(right[i].first == otr.left[i].first);

    unordered_set<int> m, m_otr;
    unordered_map<int, int> t, t_otr;

    vector<Area> new_areas;
    for (int i = 0; i < (int) right.size(); i++) {
        m.insert(right[i].second);
        m_otr.insert(otr.left[i].second);
    }

    this->addAreasOutsideUnion(new_areas, m, t);
    otr.addAreasOutsideUnion(new_areas, m_otr, t_otr);

    int new_ids = uniteSide(right, otr.left, t, t_otr, (int) new_areas.size());
    for (int i = 0; i < new_ids; i++) {
        new_areas.emplace_back();
        new_areas.back().id = -1;
    }

    this->addAreasFromUnion(new_areas, right, t);
    otr.addAreasFromUnion(new_areas, otr.left, t_otr);

    areas = std::move(new_areas);
    normVector(up, t, areas);
    normVector(down, t, areas);
    normVector(left, t, areas);
    normVector(otr.up, t_otr, areas);
    normVector(otr.down, t_otr, areas);
    normVector(otr.right, t_otr, areas);

    for (auto &e: edges) {
        e.first.first = t[e.first.first];
        e.first.second = t[e.first.second];
        assert(areas[e.first.first].height >= e.second && areas[e.first.second].height >= e.second);
    }

    for (auto &e: otr.edges) {
        e.first.first = t_otr[e.first.first];
        e.first.second = t_otr[e.first.second];
        assert(areas[e.first.first].height >= e.second && areas[e.first.second].height >= e.second);
    }

    edges.insert(edges.end(), otr.edges.begin(), otr.edges.end());
    up.insert(up.end(), otr.up.begin(), otr.up.end());
    down.insert(down.end(), otr.down.begin(), otr.down.end());
    right = std::move(otr.right);

    otr.areas.clear();
    otr.up.clear();
    otr.down.clear();
    otr.left.clear();
    otr.edges.clear();

    runAlgorithm();
}

void TileTree::mergeLongitude(TileTree &otr) {
    for (int i = 0; i < (int) up.size(); i++)
        assert(up[i].first == otr.down[i].first);

    unordered_set<int> m, m_otr;
    unordered_map<int, int> t, t_otr;

    vector<Area> new_areas;
    for (int i = 0; i < (int) up.size(); i++) {
        m.insert(up[i].second);
        m_otr.insert(otr.down[i].second);
    }

    this->addAreasOutsideUnion(new_areas, m, t);
    otr.addAreasOutsideUnion(new_areas, m_otr, t_otr);

    int new_ids = uniteSide(up, otr.down, t, t_otr, (int) new_areas.size());
    for (int i = 0; i < new_ids; i++) {
        new_areas.emplace_back();
        new_areas.back().id = -1;
    }

    this->addAreasFromUnion(new_areas, up, t);
    otr.addAreasFromUnion(new_areas, otr.down, t_otr);

    areas = std::move(new_areas);
    normVector(right, t, areas);
    normVector(down, t, areas);
    normVector(left, t, areas);
    normVector(otr.up, t_otr, areas);
    normVector(otr.left, t_otr, areas);
    normVector(otr.right, t_otr, areas);

    for (auto &e: edges) {
        e.first.first = t[e.first.first];
        e.first.second = t[e.first.second];
    }

    for (auto &e: otr.edges) {
        e.first.first = t_otr[e.first.first];
        e.first.second = t_otr[e.first.second];
    }

    edges.insert(edges.end(), otr.edges.begin(), otr.edges.end());
    left.insert(left.end(), otr.left.begin(), otr.left.end());
    right.insert(right.end(), otr.right.begin(), otr.right.end());
    up = std::move(otr.up);

    otr.areas.clear();
    otr.down.clear();
    otr.left.clear();
    otr.right.clear();
    otr.edges.clear();

    runAlgorithm();
}

TileTree::TileTree(const vector<Area> &a, const vector<vector<int>> &elem, const vector<vector<int>> &viz,
                   const vector<pair<pair<int, int>, int>> &tree) {
    unordered_map<int, int> transform;

    up = vector<pair<int, int>>(TILE_SIZE);
    right = vector<pair<int, int>>(TILE_SIZE);
    down = vector<pair<int, int>>(TILE_SIZE);
    left = vector<pair<int, int>>(TILE_SIZE);

    for (auto area: a)
        if (area.is_edge || (area.is_peak && !area.true_saddle)) {
            transform[area.id] = (int) areas.size();
            Area new_area(area);
            new_area.id = (int) areas.size();
            areas.push_back(new_area);
        }

    for (int i = 0; i < TILE_SIZE; i++) {
        up[i] = std::make_pair(elem[0][i], transform[viz[0][i]]);
        right[i] = std::make_pair(elem[i][TILE_SIZE - 1], transform[viz[i][TILE_SIZE - 1]]);
        down[i] = std::make_pair(elem[TILE_SIZE - 1][i], transform[viz[TILE_SIZE - 1][i]]);
        left[i] = std::make_pair(elem[i][0], transform[viz[i][0]]);
    }

    for (auto e: tree)
        edges.push_back({{transform[e.first.first], transform[e.first.second]}, e.second});
}

void TileTree::uniteSameKind(int a, int b, int h, bool type, vector<ForestNode> &forest,
                             vector<pair<pair<int, int>, int>> &new_edges) {
    if (a == b)
        return;

    if (forest[a].height == forest[b].height) {
        areas[forest[a].highest_id].updateSaddle(h, type, areas[forest[b].highest_id].rep);
        areas[forest[b].highest_id].updateSaddle(h, type, areas[forest[b].highest_id].rep);
    } else {
        if (forest[a].height > forest[b].height)
            std::swap(a, b);

        areas[forest[a].highest_id].updateSaddle(h, type, areas[forest[b].highest_id].rep);
    }

    if (!type) {
        areas[forest[a].highest_id].true_saddle = false;
        areas[forest[b].highest_id].true_saddle = false;
        new_edges.push_back({{forest[a].highest_id, forest[b].highest_id}, h});
    }
    ForestNode::setUnion(a, b, forest);
}

void TileTree::uniteDifferentKind(int a, int b, int h, vector<ForestNode> &forest,
                                  vector<pair<pair<int, int>, int>> &new_edges) {
    if (a == b)
        return;

    if (forest[a].height < forest[b].height)
        areas[forest[a].highest_id].updateSaddle(h, true, areas[forest[b].highest_id].rep);
    else if (forest[a].height == forest[b].height) {
        areas[forest[a].highest_id].updateSaddle(h, true, areas[forest[b].highest_id].rep);
        areas[forest[b].highest_id].updateSaddle(h, false, areas[forest[a].highest_id].rep);
    } else {
        areas[forest[b].highest_id].updateSaddle(h, false, areas[forest[a].highest_id].rep);
        areas[forest[a].highest_id].true_saddle = false;
        areas[forest[b].highest_id].true_saddle = false;
        new_edges.push_back({{forest[a].highest_id, forest[b].highest_id}, h});
    }

    ForestNode::setUnion(a, b, forest);
}

void TileTree::printRemoveInsidePeaks(vector<pair<pair<int, int>, int>> &new_edges) {
    for (auto area: areas)
        if (area.is_peak && area.true_saddle && area.height - area.saddle_height >= THRESHOLD)
            outputBuffer.addArea(area);

    areas.erase(remove_if(areas.begin(), areas.end(),
                          [&](Area area) { return !(area.is_edge || (area.is_peak && !area.true_saddle)); }),
                areas.end());

    unordered_map<int, int> transform;

    int cnt = 0;
    for (auto &area: areas) {
        transform[area.id] = cnt;
        area.id = cnt;
        cnt++;
    }

    edges = std::move(new_edges);

    for (auto &e: edges) {
        e.first.first = transform[e.first.first];
        e.first.second = transform[e.first.second];
    }

    for (auto &it: up) it.second = transform[it.second];
    for (auto &it: down) it.second = transform[it.second];
    for (auto &it: left) it.second = transform[it.second];
    for (auto &it: right) it.second = transform[it.second];
}

void TileTree::runAlgorithm() {
    vector<ForestNode> forest((int) areas.size());
    vector<vector<pair<int, int>>> order(H_MAX);
    vector<pair<pair<int, int>, int>> new_edges;

    for (auto e: edges)
        order[e.second].push_back(e.first);

    for (auto area: areas)
        forest[area.id] = ForestNode(area);

    for (int h = H_MAX - 1; h >= 0; h--) {
        if (order[h].empty())
            continue;

        /// same kind
        for (auto e: order[h]) {
            int p_a = ForestNode::findParent(e.first, forest), p_b = ForestNode::findParent(e.second, forest);
            if (!forest[p_a].is_edge && !forest[p_b].is_edge)
                uniteSameKind(p_a, p_b, h, true, forest, new_edges);
            else if (forest[p_a].is_edge && forest[p_b].is_edge)
                uniteSameKind(p_a, p_b, h, false, forest, new_edges);
        }

        /// different kind
        for (auto e: order[h]) {
            int p_a = ForestNode::findParent(e.first, forest), p_b = ForestNode::findParent(e.second, forest);
            if (!forest[p_a].is_edge && forest[p_b].is_edge)
                uniteDifferentKind(p_a, p_b, h, forest, new_edges);
            else if (forest[p_a].is_edge && !forest[p_b].is_edge)
                uniteDifferentKind(p_b, p_a, h, forest, new_edges);
        }

        ///same again
        for (auto e: order[h]) {
            int p_a = ForestNode::findParent(e.first, forest), p_b = ForestNode::findParent(e.second, forest);
            if (forest[p_a].is_edge && forest[p_b].is_edge)
                uniteSameKind(p_a, p_b, h, false, forest, new_edges);
        }
    }

    printRemoveInsidePeaks(new_edges);
}

void TileTree::printAllPeaks() {
    for (auto area: areas)
        if (area.is_peak && area.height - area.saddle_height >= THRESHOLD) {
            outputBuffer.addArea(area);
            LAST_BAD++;
        }
}

void TileTree::exportTileTree() {
    string filename = treeExportFile();
    unordered_map<int, int> transform;
    vector<pair<double, double>> rep;
    vector<pair<int, int>> rep_edges;

    for (auto e: edges)
        if (!(areas[e.first.first].is_edge || areas[e.first.second].is_edge)) {
            auto [x, y] = e.first;
            if(!transform.contains(x))
                transform[x] = (int)transform.size();
            if(!transform.contains(y))
                transform[y] = (int)transform.size();
        }

    rep.resize(transform.size());
    for(auto area:areas)
        if(transform.contains(area.id))
            rep[transform[area.id]] = area.rep.getCoords();

    for(auto e: edges)
        if(transform.contains(e.first.first) && transform.contains(e.first.second))
            rep_edges.emplace_back(transform[e.first.first], transform[e.first.second]);

    FILE *tree_file = fopen(filename.c_str(), "w");
    for(auto it: rep)
        fprintf(tree_file, "%c,%.6f,%.6f\n", 'n', it.first, it.second);

    for(auto it:rep_edges)
        fprintf(tree_file, "%c,%d,%d\n", 'c', it.first, it.second);

    fclose(tree_file);
}
#endif //TILETREE_H
