#pragma once
#include <list>
#include <memory>

class MxQuadTree
{
public:
    enum QUAD { NW=0, NE=1, SW, SE };

    struct Node {
        friend class MxQuadTree;
    private:
        std::shared_ptr<Node> nw, ne, sw, se;
        int data;
        double l, t, r, b;
    };
    typedef std::shared_ptr<Node> NodePtr;

    MxQuadTree(double left, double top, double right, double bottom, int k=3);

    NodePtr root() const;

    bool is_leaf(const NodePtr node) const;
    bool is_empty_leaf(const NodePtr node) const;

    QUAD orient(double src_x, double src_y, double dest_x, double dest_y) const;
    QUAD orient(NodePtr src, NodePtr dest) const;

    NodePtr child(NodePtr node, QUAD q) const;

    NodePtr find_leaf(double x, double y) const;
    NodePtr find_leaf(int& k, double x, double y) const;
    std::list<NodePtr> find_nodes(double left, double top, double right, double bottom) const;
    NodePtr find_father(const NodePtr node) const;

    void insert(double x, double y, int data=-1);

    void remove(double x, double y);

    int data(const NodePtr node) const;
    void rect(double& left, double& top, double& right, double& bottom, const NodePtr node) const;

private:
    void cut_leaf(NodePtr node);

private:
    int _k;
    NodePtr _root;
};
