#include "quadtreeviewer.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <float.h>
#include <stack>

const int tree_size = 3;

QuadTreeViewer::QuadTreeViewer(MxQuadTree* qtree, QGraphicsItem* parent)
    : QGraphicsObject(parent),
      _qtree(qtree),
      _step_grid(2 / std::pow(2, tree_size)),
      _hover_pos(DBL_MAX, DBL_MAX), _m(Add), _selecting(false)
{}

QRectF QuadTreeViewer::boundingRect() const
{
    double l, t, r, b;
    _qtree->rect(l, t, r, b, _qtree->root());
    return QRectF(QPointF(l, t), QPointF(r, b));
}

void QuadTreeViewer::setMouseMode(MouseMode m)
{
    _m = m;
    _selected.clear();
}

void QuadTreeViewer::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    drawGrid(painter);
    drawTree(painter);
    drawHover(painter);
    drawSelection(painter);
}

void QuadTreeViewer::drawGrid(QPainter* painter)
{
    double l, t, r, b;
    _qtree->rect(l, t, r, b, _qtree->root());

    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::lightGray);
    pen.setCosmetic(true);
    painter->setPen(pen);
    for(float line_grid = l + _step_grid; line_grid < r; line_grid += _step_grid)
    {
        painter->drawLine(QPointF(l, line_grid), QPointF(r, line_grid));
        painter->drawLine(QPointF(line_grid, t), QPointF(line_grid, b));
    }
}

void QuadTreeViewer::drawTree(QPainter* painter)
{
    if(_qtree->is_leaf(_qtree->root()))
        return;

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(0, 0, 0));
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(Qt::black);

    // имитируем работу рекурсивной функции
    static std::stack<MxQuadTree::NodePtr> nodes;
    nodes.push(_qtree->root());

    while(!nodes.empty())
    {
        MxQuadTree::NodePtr node = nodes.top();
        nodes.pop();
        Q_ASSERT(!_qtree->is_leaf(node));

        double l, t, r, b;
        _qtree->rect(l, t, r, b, node);
        double c_x = (l + r) / 2, c_y = (t + b) / 2;

        painter->drawLine(QPointF(l, c_y), QPointF(r, c_y));
        painter->drawLine(QPointF(c_x, t), QPointF(c_x, b));

        for(int i=3; i >= 0; --i)
        {
            MxQuadTree::NodePtr child = _qtree->child(node, (MxQuadTree::QUAD)i);
            if(child != nullptr)
            {
                if(_qtree->is_leaf(child))
                {
                    if(_qtree->data(child) == 1)
                    {
                        double l, t, r, b;
                        _qtree->rect(l, t, r, b, child);
                        painter->drawRect(QRectF(QPointF(l, t), QPointF(r, b)));
                    }
                }
                else
                    nodes.push(child);
            }
        }
    }

    painter->setBrush(Qt::NoBrush);
}

void QuadTreeViewer::drawHover(QPainter* painter)
{
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(255, 127, 0));
    pen.setCosmetic(true);
    painter->setPen(pen);

    QRectF rect = boundingRect();
    painter->drawLine(QPointF(rect.left(), _hover_pos.y()), QPointF(rect.right(), _hover_pos.y()));
    painter->drawLine(QPointF(_hover_pos.x(), rect.top()), QPointF(_hover_pos.x(), rect.bottom()));

    if(_hover_node == nullptr)
        return;

    double l, t, r, b;
    _qtree->rect(l, t, r, b, _hover_node);
    double c_x = (l + r) / 2, c_y = (t + b) / 2;
    painter->drawEllipse(QPointF(c_x, c_y), 0.05, 0.05);
}

void QuadTreeViewer::drawSelection(QPainter* painter)
{
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::green);
    pen.setCosmetic(true);
    painter->setPen(pen);
    for(auto node : _selected)
    {
        double l, t, r, b;
        _qtree->rect(l, t, r, b, node);
        double c_x = (l + r) / 2, c_y = (t + b) / 2;
        painter->drawEllipse(QPointF(c_x, c_y), 0.05, 0.05);
    }

    if(!_selecting)
        return;

    pen.setColor(Qt::lightGray);
    painter->setPen(pen);
    painter->drawRect(QRectF(_sel_pt1, _sel_pt2));
}

void QuadTreeViewer::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(_m == Select)
        {
            _selecting = true;
            _sel_pt1 = event->scenePos();
            _sel_pt2 = event->scenePos();
        }
        else
            _qtree->insert(event->scenePos().x(), event->scenePos().y(), 1);
    }
    else if(event->button() == Qt::RightButton)
    {
        _qtree->remove(event->scenePos().x(), event->scenePos().y());
    }
    update();
    event->accept();
}

void QuadTreeViewer::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(_m == Select)
        _sel_pt2 = event->scenePos();

    _hover_pos = event->scenePos();
    _hover_node = _qtree->find_leaf(_hover_pos.x(), _hover_pos.y());
    update();
    event->accept();
}

void QuadTreeViewer::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(_m == Select)
    {
        _selecting = false;
        _sel_pt2 = event->scenePos();
        _selected = _qtree->find_nodes(_sel_pt1.x(), _sel_pt1.y(), _sel_pt2.x(), _sel_pt2.y());
    }
}
