#include "button_delegate.h"

#include "button_delegate.h"
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>
#include <QDebug>

ButtonDelegate::ButtonDelegate(QStyledItemDelegate *parent) : QStyledItemDelegate(parent)
{
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    int offset = option.rect.size().width();
    QStyleOptionButton* button = new QStyleOptionButton();
    button->rect = option.rect.adjusted(offset/2 - 8, 2, -offset/2 + 8, -2);
    button->text = "X";
    button->state |= QStyle::State_Enabled;

    QFont font = painter->font();
    font.setPixelSize(11);
    painter->setFont(font);
    painter->save();
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    painter->restore();

    QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if(option.rect.contains(mouseEvent->pos()))
    {
        if(event->type() == QEvent::MouseButtonRelease)
            emit clicked(index);
    }
    return true;
}

