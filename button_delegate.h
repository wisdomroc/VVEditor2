/***********************************************************************
* @filename button_delegate.h
* @brief    提供一个Button的代理类
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef BUTTON_DELEGATE_H
#define BUTTON_DELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ButtonDelegate(QStyledItemDelegate *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clicked(QModelIndex row);
};


#endif // BUTTON_DELEGATE_H
