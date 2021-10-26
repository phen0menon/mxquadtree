#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "quadtreeviewer.h"

#include <QButtonGroup>
#include <QGraphicsSceneMouseEvent>

const int tree_size = 3;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _scene(new QGraphicsScene(this)),
      _qtree(-1., -1., 1., 1., tree_size),
      _viewer(new QuadTreeViewer(&_qtree))
{
    ui->setupUi(this);
    ui->view->setScene(_scene);
    _scene->addItem(_viewer);
    _scene->installEventFilter(this);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->add);
    group->addButton(ui->select);
    connect(ui->add, &QRadioButton::toggled, [this]()
    {
        _viewer->setMouseMode(QuadTreeViewer::MouseMode::Add);
    });
    connect(ui->select, &QRadioButton::toggled, [this]()
    {
        _viewer->setMouseMode(QuadTreeViewer::MouseMode::Select);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject* /*watched*/, QEvent* event)
{
    if(event->type() == QEvent::GraphicsSceneMouseMove)
    {
        auto mouse_event = static_cast<QGraphicsSceneMouseEvent*>(event);
        ui->statusbar->showMessage(QString("x:%1 y:%2").arg(mouse_event->scenePos().x()).arg(mouse_event->scenePos().y()));
        _viewer->mouseMoveEvent(mouse_event);  // плохо. как заставить вызываться mouseMoveEvent (private метод)?
                                               // приходиться вызывать самому с помощью friend.
        return mouse_event->isAccepted();
    }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    ui->view->fitInView(_viewer);  // бесячая штука
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    ui->view->fitInView(_viewer);
}
