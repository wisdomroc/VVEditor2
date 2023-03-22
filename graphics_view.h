/***********************************************************************
* @filename graphics_view.h
* @brief    配合graphics_scene实现右侧视口的所有功能
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef GRAPHICS_VIEW_H
#define GRAPHICS_VIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphicsView(QWidget *parent = Q_NULLPTR);
    GraphicsView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void changeSize(QSize);
};

#endif // GRAPHICS_VIEW_H
