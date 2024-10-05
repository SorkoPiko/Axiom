#ifndef NODE_HPP
#define NODE_HPP

enum NodeStatus {
    Unvisited,
    Success, // Didn't die
    Failure, // Died
    Completion // Finished the level
};

struct Node {
    bool input;
    NodeStatus status;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node(const bool input) : input(input), status(Unvisited), left(nullptr), right(nullptr) {}
};

using NodeBranch = std::vector<Node*>;

#endif
