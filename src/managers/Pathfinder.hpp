#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <memory>
#include <vector>
#include "../types/Node.hpp"

using BinaryInstructions = std::vector<bool>;
using NodePath = std::vector<Node*>;

class TrainManager;

class Pathfinder {
    std::unique_ptr<Node> root;
    TrainManager* trainManager;
    size_t n;

    static void nodeFailed(std::vector<Node *> &nodes);
    static void checkChildNodes(std::vector<Node *> &nodes);

    void generatePaths(Node* node, BinaryInstructions &currentPath, std::vector<BinaryInstructions> &paths, size_t depth);
    static NodePath findNodeBranch(Node *root, const Node *target);
    static std::pair<std::vector<Node *>, bool> analyseResults(Node *rootNode, const std::vector<BinaryInstructions > &paths, const std::vector<std::pair<bool, size_t> > &results);
    static NodePath findNodesFromInstructions(Node *currentNode, const BinaryInstructions &instructions, NodePath &nodes);
    static Node* generatePathsDown(Node* node, bool input, size_t remaining);
    static BinaryInstructions findSuccessfulPathFromNode(Node* node);

public:
    Pathfinder(
        TrainManager* trainManager,
        const size_t depth
    ) : root(std::make_unique<Node>(false)), trainManager(trainManager), n(depth) {}

    BinaryInstructions findSuccessfulPath();

    void test();
};

#endif