#pragma once
#include "mxquadtree.h"

#include <QGraphicsObject>

class MainWindow;

class QuadTreeViewer : public QGraphicsObject
{
    friend class MainWindow;
public:
    QuadTreeViewer(MxQuadTree* qtree, QGraphicsItem* parent=nullptr);

    QRectF boundingRect() const override;

    enum MouseMode {Add, Select};  // Add - добавление вершин по клику ЛКМ;
                                   // Select - выделение области вершин при зажатой ЛКМ.
    void setMouseMode(MouseMode m);

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void drawGrid(QPainter* painter);
    void drawTree(QPainter* painter);
    void drawHover(QPainter* painter);
    void drawSelection(QPainter* painter);

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    MxQuadTree* _qtree;

    float _step_grid;

    QPointF _hover_pos;
    MxQuadTree::NodePtr _hover_node;

    MouseMode _m;
    bool _selecting;
    QPointF _sel_pt1;
    QPointF _sel_pt2;
    std::list<MxQuadTree::NodePtr> _selected;
};
