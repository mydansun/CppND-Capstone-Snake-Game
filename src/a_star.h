//
// Created by sun on 2023/12/30.
//

// XY coordinates: ↓ (y+) → (x+)

#ifndef SNAKE_GAME_PROJECT_A_STAR_H
#define SNAKE_GAME_PROJECT_A_STAR_H

#include <vector>
#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <memory>

enum class GridState {
    Empty,
    Obstacle,
    Visited,
    Path
};

struct GridPoint {
    int x;
    int y;

    bool operator==(const GridPoint &r) const {
        return ((x == r.x) && (y == r.y));
    }
};

struct GridNode {
    explicit GridNode(std::shared_ptr<GridNode> _parent, int _from_parent_x, int _from_parent_y,
                      const GridPoint &_point, int _g,
                      int _h);

    std::shared_ptr<GridNode> parent;
    int from_parent_x;
    int from_parent_y;
    GridPoint point{};
    int g;
    int h;
};

class AStar {
public:
    void initGrid(int width, int height);

    void resetGrid();

    void fillObstacle(GridPoint point);

    std::array<int, 2> FindFirstDirection(const std::shared_ptr<GridNode> &finalNode);

    std::array<int, 2> Search(const GridPoint &start, const GridPoint &goal);

private:
    const int delta[4][2]{{-1, 0},
                          {0,  -1},
                          {1,  0},
                          {0,  1}};
    std::vector<std::vector<GridState>> grid;
    std::vector<std::shared_ptr<GridNode>> open_nodes;

    void PrintGrid(const GridPoint &goal);

    std::string ResolveState(GridState state);

    int Heuristic(GridPoint p1, GridPoint p2);

    bool CheckValidPoint(GridPoint p);

    void ExpandNeighbors(const std::shared_ptr<GridNode> &current, const GridPoint &goal);

    void SortOpenNodes();

    void
    addToOpenNodes(const std::shared_ptr<GridNode> &parent_node, int from_parent_x, int from_parent_y,
                   const GridPoint &point,
                   int g, int h);
};


#endif //SNAKE_GAME_PROJECT_A_STAR_H
