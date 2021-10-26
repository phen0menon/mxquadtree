#pragma once
#include <QMainWindow>

#include "mxquadtree.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QGraphicsScene;
QT_END_NAMESPACE

class QuadTreeViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene* _scene;
    QuadTreeViewer* _viewer;

    MxQuadTree _qtree;
};
