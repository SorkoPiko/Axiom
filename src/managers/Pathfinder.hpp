#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "../types/Node.hpp"
#include "../types/InstanceResult.hpp"

using namespace geode::prelude;

class TrainManager;

class Pathfinder {
    size_t n;
    TrainManager* manager;
    std::unique_ptr<Node> root;

    static bool findBranch(Node* rootNode, Node* target, NodeBranch& path, NodeBranch& branch);
    static void generateBranch(NodeBranch& currentBranch, bool direction, size_t depth);
    static void markFailed(NodeBranch& nodes, bool check, int index);
    static void pruneAfterIndex(NodeBranch& nodes, size_t index);
    static void setBranchSuccess(const NodeBranch& nodes, bool completion);
    static std::pair<NodeBranch &, InstanceResult &> analyseResults(std::vector<InstanceResult> &results);
    static void sanitiseBranches(std::vector<NodeBranch>& branches);
    static void generatePaths(Node *node, NodeBranch &currentBranch, const NodeBranch &existingBranch, std::vector<NodeBranch> &branches, const
                              size_t depth);
    NodeBranch& findPath(NodeBranch &currentBranch);

public:
    NodeBranch &startSearch();

    explicit Pathfinder(const size_t n, TrainManager* manager)
        : n(n), manager(manager), root(std::make_unique<Node>(new Node(false))) {
    }
};

#endif
