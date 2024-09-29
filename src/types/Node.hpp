#ifndef NODE_HPP
#define NODE_HPP

struct Node {
    bool input;
    bool success;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node(bool input) : input(input), success(false), left(nullptr), right(nullptr) {}
};

#endif
