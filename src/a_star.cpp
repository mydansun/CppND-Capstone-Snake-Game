//
// Created by sun on 2023/12/30.
//

#include "a_star.h"

#include <utility>

void AStar::initGrid(int width, int height) {
    grid = std::vector<std::vector<GridState>>(height, std::vector<GridState>(width, GridState::Empty));
}

void AStar::resetGrid() {
    for (auto &row: grid) {
        row.assign(row.size(), GridState::Empty);
    }
}

void AStar::fillObstacle(GridPoint point) {
    if ((point.y >= 0 && point.y < grid.size()) && (point.x >= 0 && point.x < grid[0].size())) {
        grid[point.y][point.x] = GridState::Obstacle;
    }
}

int AStar::Heuristic(GridPoint p1, GridPoint p2) {
    return abs(p2.x - p1.x) + abs(p2.y - p1.y);
}

bool AStar::CheckValidPoint(GridPoint p) {
    if ((p.y >= 0 && p.y < grid.size()) && (p.x >= 0 && p.x < grid[0].size())) {
        return grid[p.y][p.x] == GridState::Empty;
    }
    return false;
}

void AStar::ExpandNeighbors(const std::shared_ptr<GridNode> &current, const GridPoint &goal) {
    for (auto i: delta) {
        GridPoint p2{current->point.x + i[0], current->point.y + i[1]};

        if (CheckValidPoint(p2)) {
            int g2 = current->g + 1;
            int h2 = Heuristic(p2, goal);
            addToOpenNodes(current, i[0], i[1], p2, g2, h2);
        }
    }
}

void AStar::addToOpenNodes(const std::shared_ptr<GridNode> &parent_node, int from_parent_x, int from_parent_y,
                           const GridPoint &point, int g,
                           int h) {
    open_nodes.push_back(std::make_shared<GridNode>(parent_node, from_parent_x, from_parent_y, point, g, h));
    grid[point.y][point.x] = GridState::Visited;
}

std::array<int, 2> AStar::Search(const GridPoint &start, const GridPoint &goal) {
    open_nodes.clear();
    int h = Heuristic(start, goal);
    addToOpenNodes(nullptr, 0, 0, start, 0, h);

    while (!open_nodes.empty()) {
        // Get the next node
        SortOpenNodes();
        auto current = open_nodes.back();
        open_nodes.pop_back();

        // Check if we're done.
        if (current->point == goal) {
            return FindFirstDirection(current);
        }
        ExpandNeighbors(current, goal);
        //PrintGrid(goal);
    }
    //PrintGrid(goal);
    return {0, 0};
}

void AStar::PrintGrid(const GridPoint &goal) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            if (i == goal.y && j == goal.x) {
                std::cout << "$   ";
            } else {
                std::cout << ResolveState(grid[i][j]);
            }
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

std::string AStar::ResolveState(GridState state) {
    switch (state) {
        case GridState::Obstacle:
            return "|   ";
        case GridState::Visited:
            return "X   ";
        case GridState::Path:
            return "*   ";
        default:
            return "0   ";
    }
}

std::array<int, 2> AStar::FindFirstDirection(const std::shared_ptr<GridNode> &finalNode) {
    grid[finalNode->point.y][finalNode->point.x] = GridState::Path;
    auto iter = finalNode;
    int from_parent_x = iter->from_parent_x, from_parent_y = iter->from_parent_y;
    while (iter->parent) {
        iter = iter->parent;
        grid[iter->point.y][iter->point.x] = GridState::Path;
        if (iter->parent) {
            from_parent_x = iter->from_parent_x;
            from_parent_y = iter->from_parent_y;
        }
    }
    if (from_parent_x == 0 && from_parent_y == 0) {
        throw std::runtime_error("Invalid direction.");
    }
    return {from_parent_x, from_parent_y};
}

void AStar::SortOpenNodes() {
    std::sort(open_nodes.begin(), open_nodes.end(), [](auto const &open1, auto const &open2) {
        return open1->g + open1->h > open2->g + open2->h;
    });
}

GridNode::GridNode(std::shared_ptr<GridNode> _parent, int _from_parent_x, int _from_parent_y, const GridPoint &_point,
                   int _g, int _h) {
    parent = std::move(_parent);
    from_parent_x = _from_parent_x;
    from_parent_y = _from_parent_y;
    point.x = _point.x;
    point.y = _point.y;
    g = _g;
    h = _h;
}
