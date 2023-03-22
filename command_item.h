/***********************************************************************
* @filename command_item.h
* @brief    右侧视口中的一个个单独的item
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef COMMAND_ITEM_H
#define COMMAND_ITEM_H

#include <QGraphicsRectItem>
#include "vv_typedef.h"

class QMenu;
class QAction;
struct VVCommand;

class CommandItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    CommandItem(int row, VVCommand *vvCommand, const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR);

    void setVVCommand(VVCommand *vvCommand);
    inline VVCommand *getVVCommand(){ return _vvCommand; }
    inline int getCommandRow(){ return _row; }
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    void init();
    QString getToolTip(VVCommand *command);

private:
    QMenu* _menu;
    QAction* _action_modify;
    QAction* _action_remove;
    VVFile* _vvFile;
    VVCommand* _vvCommand;
    int _row;
    QGraphicsTextItem *_item;

signals:
    void modify(int row, VVCommand *vvCommand);
    void remove(int row, VVCommand *vvCommand);
};

#endif // COMMAND_ITEM_H
