#include "mxquadtree.h"

#include <cassert>
#include <cmath>
#include <stack>

MxQuadTree::MxQuadTree(double left, double top, double right, double bottom, int k)
    : _k(k), _root(std::make_shared<Node>())
{
    _root->data = -1;
    _root->l = left;
    _root->t = top;
    _root->r = right;
    _root->b = bottom;
}

MxQuadTree::NodePtr MxQuadTree::root() const
{
    return _root;
}

bool MxQuadTree::is_leaf(const NodePtr node) const
{
    return node->nw == nullptr && node->ne == nullptr && node->sw == nullptr && node->se == nullptr;  // если хоть один == nullptr, то - лист
}

bool MxQuadTree::is_empty_leaf(const NodePtr node) const {
    return is_leaf(node) && node->data == -1;
}

MxQuadTree::QUAD MxQuadTree::orient(double src_x, double src_y, double dest_x, double dest_y) const
{
    if(dest_x < src_x)
    {
        if(dest_y < src_y)
            return QUAD::NW;
        return QUAD::SW;
    }
    else
    {
        if(dest_y < src_y)
            return QUAD::NE;
        return QUAD::SE;
    }
}

MxQuadTree::QUAD MxQuadTree::orient(NodePtr src, NodePtr dest) const
{
    double c1_x = (src->l + src->r) / 2, c1_y = (src->t + src->b) / 2;
    double c2_x = (dest->l + dest->r) / 2, c2_y = (dest->t + dest->b) / 2;

    if(c2_x < c1_x)
    {
        if(c2_y < c1_y)
            return QUAD::NW;
        return QUAD::SW;
    }
    else
    {
        if(c2_y < c1_y)
            return QUAD::NE;
        return QUAD::SE;
    }
}

MxQuadTree::NodePtr MxQuadTree::child(NodePtr node, QUAD q) const
{
    if(q == QUAD::NW)
        return node->nw;
    if(q == QUAD::NE)
        return node->ne;
    if(q == QUAD::SW)
        return node->sw;
    return node->se;
}

MxQuadTree::NodePtr MxQuadTree::find_leaf(double x, double y) const
{
    NodePtr cur = _root;

    while(!is_leaf(cur))
    {
        double c_x = (cur->l + cur->r) / 2, c_y = (cur->t + cur->b) / 2;
        cur = child(cur, orient(c_x, c_y, x, y));
    }

    return cur;
}

MxQuadTree::NodePtr MxQuadTree::find_leaf(int& k, double x, double y) const
{
    NodePtr cur = _root;
    int step = 0;

    while(!is_leaf(cur))
    {
        double c_x = (cur->l + cur->r) / 2, c_y = (cur->t + cur->b) / 2;
        cur = child(cur, orient(c_x, c_y, x, y));
        ++step;
    }

    k = step;
    return cur;
}

std::list<MxQuadTree::NodePtr> MxQuadTree::find_nodes(double left, double top, double right, double bottom) const
{
    std::list<NodePtr> res;
    std::stack<NodePtr> nodes;
    nodes.push(_root);

    while(!nodes.empty())
    {
        NodePtr node = nodes.top();
        nodes.pop();

        double c_x = (node->l + node->r) / 2, c_y = (node->t + node->b) / 2;

        bool is_in_rect = std::fmin(left, right) <= c_x && c_x <= std::fmax(left, right) &&
                std::fmin(top, bottom) <= c_y && c_y <= std::fmax(top, bottom);

        if(is_in_rect && node->data == 1)
            res.push_back(node);

        if(is_leaf(node))
            continue;

        if(!(std::fmin(left, right) < c_x && std::fmax(left, right) < c_x))
        {
            if(!(std::fmin(top, bottom) < c_y && std::fmax(top, bottom) < c_y))
                nodes.push(node->se);
            if(!(std::fmin(top, bottom) > c_y && std::fmax(top, bottom) > c_y))
                nodes.push(node->ne);
        }

        if(!(std::fmin(left, right) > c_x && std::fmax(left, right) > c_x))
        {
            if(!(std::fmin(top, bottom) < c_y && std::fmax(top, bottom) < c_y))
                nodes.push(node->sw);
            if(!(std::fmin(top, bottom) > c_y && std::fmax(top, bottom) > c_y))
                nodes.push(node->nw);
        }
    }

    return res;
}

MxQuadTree::NodePtr MxQuadTree::find_father(const NodePtr node) const
{
    if(node == nullptr)
        return nullptr;

    NodePtr prev = nullptr;
    NodePtr cur = _root;
    while(cur != node)
    {
        assert(!is_leaf(cur));
        prev = cur;
        cur = child(cur, orient(cur, node));
    }

    return prev;
}

void MxQuadTree::insert(double x, double y, int data)
{

    NodePtr cur = _root;
    if (is_leaf(cur)) {
        cut_leaf(_root);
    }

    int step = 1;
    while(step < _k)
    {
        double c_x = (cur->l + cur->r) / 2, c_y = (cur->t + cur->b) / 2;
        cur = child(cur, orient(c_x, c_y, x, y));

        if (is_leaf(cur)) {
            cut_leaf(cur);
        }

        if (++step == _k) {
            c_x = (cur->l + cur->r) / 2, c_y = (cur->t + cur->b) / 2;
            NodePtr chld = child(cur, orient(c_x, c_y, x, y));
            chld->data = data;
        }
    }
}



void MxQuadTree::remove(double x, double y)
{
    NodePtr node = find_leaf(x, y);
    if(!node) return;

    NodePtr parent = find_father(node);

    node->data = -1;
    node = nullptr;

    if(parent == nullptr) {
        return;
    }

    if (is_empty_leaf(parent->nw) && is_empty_leaf(parent->sw) && is_empty_leaf(parent->se) && is_empty_leaf(parent->ne)) {
        parent->nw = nullptr;
        parent->ne = nullptr;
        parent->sw = nullptr;
        parent->se = nullptr;
    }

    if (is_leaf(parent)) {
        remove((parent->l + parent->r) / 2, (parent->t + parent->b) / 2);
    }
}

int MxQuadTree::data(const NodePtr node) const
{
    return node->data;
}

void MxQuadTree::rect(double& left, double& top, double& right, double& bottom, const NodePtr node) const
{
    left = node->l;
    top = node->t;
    right = node->r;
    bottom = node->b;
}

void MxQuadTree::cut_leaf(NodePtr node)
{
    assert(is_leaf(node));
    double c_x = (node->l + node->r) / 2, c_y = (node->t + node->b) / 2;

    node->nw = std::make_shared<Node>();
    node->nw->data = -1;
    node->nw->l = node->l;
    node->nw->t = node->t;
    node->nw->r = c_x;
    node->nw->b = c_y;

    node->ne = std::make_shared<Node>();
    node->ne->data = -1;
    node->ne->l = c_x;
    node->ne->t = node->t;
    node->ne->r = node->r;
    node->ne->b = c_y;

    node->sw = std::make_shared<Node>();
    node->sw->data = -1;
    node->sw->l = node->l;
    node->sw->t = c_y;
    node->sw->r = c_x;
    node->sw->b = node->b;

    node->se = std::make_shared<Node>();
    node->se->data = -1;
    node->se->l = c_x;
    node->se->t = c_y;
    node->se->r = node->r;
    node->se->b = node->b;
}
