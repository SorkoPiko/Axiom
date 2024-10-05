#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "../types/Node.hpp"
#include "../types/InstanceResult.hpp"

using namespace geode::prelude;

class Pathfinder {
    int n;
    std::unique_ptr<Node*> root;

    static bool findBranch(Node* rootNode, Node* target, NodeBranch& path, NodeBranch& branch);
    static void generateBranch(NodeBranch &currentBranch, bool direction, size_t depth);
    static void markFailed(NodeBranch &nodes, bool check, int index);
    static void pruneAfterIndex(NodeBranch &nodes, size_t index);
    static void setBranchSuccess(const NodeBranch &nodes, bool completion);
    static NodeBranch &analyseResults(const std::vector<InstanceResult> &results);

    void generatePaths(Node *node, std::vector<bool> &currentPath, std::vector<std::vector<bool>> &paths, size_t depth);

public:
    explicit Pathfinder(const int n)
        : n(n), root(std::make_unique<Node*>(new Node(false))) {}
};

#endif
