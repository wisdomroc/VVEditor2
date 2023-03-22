#include "command_item.h"
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>


CommandItem::CommandItem(int row, VVCommand *vvCommand, const QRectF &rect, QGraphicsItem *parent): QGraphicsRectItem (rect,parent),
    _vvCommand(vvCommand), _row(row)
{
    init();
}

void CommandItem::setVVCommand(VVCommand *vvCommand)
{
    _vvCommand = vvCommand;
    _item->setPlainText(vvCommand->_command_type_str);
}

void CommandItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_UNUSED(event)
    _menu->exec(event->screenPos());
}

void CommandItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    emit modify(_row, _vvCommand);
}

QString CommandItem::getToolTip(VVCommand* command)
{
    QString formatStr;
    formatStr.append(tr("%1%2\n").arg(QStringLiteral("命令类型    ")).arg(command->_command_type_str));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("消息  ID    ")).arg(command->_msg_id).arg(QStringLiteral("消息名称    ")).arg(command->_msg_name));
    formatStr.append(tr("%1%2\t\t\t%3%4\n").arg(QStringLiteral("信号  ID    ")).arg(command->_sig_id).arg(QStringLiteral("信号名称    ")).arg(command->_sig_name));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("字节偏移    ")).arg(command->_word_offset).arg(QStringLiteral("字内偏移    ")).arg(command->_offset));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("类    型    ")).arg(command->_data_type).arg(QStringLiteral("长    度    ")).arg(command->_data_length));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("下    标    ")).arg(command->_data_indexs).arg(QStringLiteral("物理地址    ")).arg(command->_physical_address));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("起始偏移    ")).arg(command->_svpc_check_begin_offset).arg(QStringLiteral("结束偏移    ")).arg(command->_svpc_check_end_offset));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("比较符号    ")).arg(command->_operator).arg(QStringLiteral("命 令 值    ")).arg(command->_data_value));
    formatStr.append(tr("%1%2\t\t\t\t%3%4\n").arg(QStringLiteral("持续节拍    ")).arg(command->_continuous_beat).arg(QStringLiteral("持续时间    ")).arg(""));

    //! 以下两个信息，是RESET命令时有用的：暂不提供显示
    QString             _condition_expression;  //RESET时有用，是条件配置
    QVector<TargetInfo *> _target_list;         //RESET时有用，是选中的ICD或者FMU

    return formatStr;
}

void CommandItem::init()
{
    _menu = new QMenu();
    _action_modify = _menu->addAction(QStringLiteral("修改"));
    _action_remove = _menu->addAction(QStringLiteral("删除"));
    connect(_action_modify, &QAction::triggered, [this](){ emit modify(_row, _vvCommand); });
    connect(_action_remove, &QAction::triggered, [this](){ emit remove(_row, _vvCommand); });

    _item = new QGraphicsTextItem(_vvCommand->_command_type_str, this);

    setToolTip(getToolTip(_vvCommand));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}
