#include "graphics_view.h"
#include "graphics_scene.h"
#include <QScrollBar>

GraphicsView::GraphicsView(QWidget *parent)
{

}

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
{

}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    GraphicsScene *scene = qobject_cast<GraphicsScene *>(this->scene());
    scene->adjustSceneRect(this->width(), this->height());
}
