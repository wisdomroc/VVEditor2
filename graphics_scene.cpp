#include "graphics_scene.h"
#include <QGraphicsRectItem>
#include <QDebug>
#include "icd_data.h"
#include "vv_typedef.h"
#include "command_item.h"
#include "add_command_dlg.h"

const int CommandRectWidth   = 80;
const int CommandRectHeight  = 40;
const int HorizontalInterval = 100;
const int VerticalInterval   = 50;

GraphicsScene::GraphicsScene():
    _xMinWidth(0),
    _yMinHeight(0),
    _highLightItem(nullptr)
{

}

void GraphicsScene::highLight(int beatIndex)
{
    if(_highLightItem == nullptr)
    {
        _highLightItem = new QGraphicsLineItem(0, 0, width(), 0);
        _highLightItem->setPen(QPen(QBrush("blue"), 2));
        addItem(_highLightItem);
    }
    _highLightItem->setPos(0, beatIndex * VerticalInterval);
}

void GraphicsScene::setICDData(QSharedPointer<ICDData> icd_data)
{
    _icd_data = icd_data;
}

void GraphicsScene::addCommand(int beatIndex, VVCommand *command)
{
    if(_beatCommandsList.size() == beatIndex )
    {
        _beatCommandsList.append(QList<VVCommand*>() << command);
        _beatList.append(beatIndex);
    }
    else
    {
        QList<VVCommand*> curCommands = _beatCommandsList.at(beatIndex);
        curCommands.append(command);
        _beatCommandsList.replace(beatIndex, curCommands);
    }
    int xOffsetIndex = (_beatCommandsList.at(beatIndex).size() - 1);
    //! 在场景中创建一个新的item
    CommandItem *item = new CommandItem(beatIndex, command, QRect(QPoint(0, 0), QSize(CommandRectWidth, CommandRectHeight)));
    connect(item, &CommandItem::modify, this, &GraphicsScene::modifyCommand);
    connect(item, &CommandItem::remove, this, &GraphicsScene::deleteOneCommand);
    //! 将item添加至场景中
    addItem(item);
    //! 设置item的位置
    item->setPos(xOffsetIndex*HorizontalInterval, beatIndex * VerticalInterval);
    QList<CommandItem*> oldList = _items.at(beatIndex);
    oldList.append(item);
    _items.replace(beatIndex, oldList);


    adjustSceneRect();
}

void GraphicsScene::adjustSceneRect(int width, int height/*, bool hBarVisible, bool vBarVisible*/)
{
    int maxIndexX = 0;
    for(QList<VVCommand*> vvCommands: _beatCommandsList)
    {
        if(vvCommands.size() - 1 > maxIndexX)
            maxIndexX = vvCommands.size() - 1;
    }
    int needWidth  = maxIndexX*HorizontalInterval + CommandRectWidth;
    int needHeight = _beatCommandsList.size()*VerticalInterval;
    qDebug() << "Y count:" << _beatCommandsList.count() << endl;

    bool hBarVisible = false;
    bool vBarVisible = false;
    if(needWidth > width)
        vBarVisible = true;
    if(needHeight > height)
        hBarVisible = true;

    int xMore = vBarVisible ? 0: 18;
    int yMore = hBarVisible ? 0: 18;

    if( needWidth > width)
    {
        QRectF oldRect = sceneRect();
        oldRect.setWidth(needWidth - 1);
        setSceneRect(oldRect);
        oldRect.setWidth(needWidth);
        setSceneRect(oldRect);
        qDebug() << "++++++++ needWidth > width" << endl;
    }
    else
    {
        QRectF oldRect = sceneRect();
        oldRect.setWidth(width - 2 - 18 + xMore);
        setSceneRect(oldRect);
        qDebug() << "-------- needWidth < width" << endl;
    }

    if(needHeight > height)
    {
        QRectF oldRect = sceneRect();
        oldRect.setHeight(needHeight - 1);
        setSceneRect(oldRect);
        oldRect.setHeight(needHeight);
        setSceneRect(oldRect);
        qDebug() << "++++++++ needHeight > height" << endl;
    }
    else
    {
        QRectF oldRect = sceneRect();
        oldRect.setHeight(height - 2 - 18 + yMore);
        setSceneRect(oldRect);
        qDebug() << "-------- needHeight < height" << endl;
    }
}

bool GraphicsScene::deleteBeat(int beatIndex)
{
    Q_UNUSED(beatIndex)
    return true;
}

QMap<int, int> GraphicsScene::deleteCurrentCommandsPre()
{
    QMap<int, int> aboutToDeleteCommandMap;
    QList<QGraphicsItem*> toDeleteItems = selectedItems();
    for(QGraphicsItem* item: toDeleteItems)
    {
        CommandItem *commItem = dynamic_cast<CommandItem*>(item);
        int row = commItem->getCommandRow();
        if(aboutToDeleteCommandMap.contains(row))
            aboutToDeleteCommandMap[row] = aboutToDeleteCommandMap.value(row) + 1;
        else
            aboutToDeleteCommandMap[row] = 1;
    }
    return aboutToDeleteCommandMap;
}

bool GraphicsScene::deleteCurrentCommands()
{
    QList<QGraphicsItem*> toDeleteItems = selectedItems();
    for(QGraphicsItem* item: toDeleteItems)
    {
        CommandItem *commItem = dynamic_cast<CommandItem*>(item);
        int row = commItem->getCommandRow();
        VVCommand *vvCommand = commItem->getVVCommand();
        //step1.删除树中数据
        bool ok = _vvFile->_beats.at(row)->_commands.removeOne(vvCommand);
        _vvFile->_beats.at(row)->_commands_count = _vvFile->_beats.at(row)->_commands.size();
        if(ok)
        {
            //step2.看后面有没有其他item，如果有，那么往前移
            QList<CommandItem*> oldItems = _items.at(row);
            int index = oldItems.indexOf(commItem);
            for(int i = oldItems.size() - 1; i > index ; --i)
            {
                oldItems.at(i)->setPos(oldItems.at(i - 1)->pos());
            }
            //step3.树中数据删除成功，这里进行item的删除
            removeItem(item);
            oldItems.removeOne(commItem);
            _items.replace(row, oldItems);
            commItem->deleteLater();
        }
    }
    return true;
}

bool GraphicsScene::modifyCommand(int row, VVCommand *vvCommand)
{
    int column = -1;
    for(int i = 0; i < _vvFile->_beats.at(row)->_commands.size(); ++i)
    {
        if(_vvFile->_beats.at(row)->_commands.at(i) == vvCommand)
        {
            column = i;
            break;
        }
    }
    if(column == -1) return false;


    AddCommandDlg addCommandDlg(_icd_data, _vvFile, vvCommand);
    addCommandDlg.setVVCommand(vvCommand);
    int ret = addCommandDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
    {   //step1.在数据里将信息修改
        VVCommand *oldCommand = _vvFile->_beats.at(row)->_commands.at(column);
        Q_UNUSED(oldCommand)
        VVCommand *vvCommandNew = addCommandDlg.getInfo();

        //step2.在场景中找到即将要修改的item
        CommandItem *item = qobject_cast<CommandItem *>(sender());

        //step3.对场景中的item进行信息修正
        item->setVVCommand(vvCommandNew);
    }
        break;
    case QDialog::Rejected:

        break;
    }
}

bool GraphicsScene::deleteOneCommand(int row, VVCommand *vvCommand)
{
    QVector<VVCommand*>::const_iterator it = qFind(_vvFile->_beats.at(row)->_commands, vvCommand);
    if(it == _vvFile->_beats.at(row)->_commands.end())
        return false;

    //step1.删除树中数据
    bool ok = _vvFile->_beats.at(row)->_commands.removeOne(vvCommand);
    _vvFile->_beats.at(row)->_commands_count = _vvFile->_beats.at(row)->_commands.size();
    if(ok)
    {
        //step2.在场景中找到即将要删除的item
        CommandItem *item = qobject_cast<CommandItem *>(sender());
        //step3.看后面有没有其他item，如果有，那么往前移
        QList<CommandItem*> oldItems = _items.at(row);
        int index = oldItems.indexOf(item);
        for(int i = oldItems.size() - 1; i > index ; --i)
        {
            oldItems.at(i)->setPos(oldItems.at(i - 1)->pos());
        }
        //step4.树中数据删除成功，这里进行item的删除
        removeItem(item);
        oldItems.removeOne(item);
        _items.replace(row, oldItems);
        item->deleteLater();
    }
    return false;
}

void GraphicsScene::modifyCurrentCommand()
{
    QList<QGraphicsItem*> currentItems = selectedItems();
    if(currentItems.size() != 1) return;

    QGraphicsItem* item = currentItems.first();
    CommandItem *commItem = dynamic_cast<CommandItem*>(item);
    int row = commItem->getCommandRow();
    VVCommand *vvCommand = commItem->getVVCommand();

    modifyCommand(row, vvCommand);
}

void GraphicsScene::initFromFile(VVFile *vvFile)
{
    _vvFile = vvFile;
    //根据vvFile初始化成员变量_beatCommandsList
    _beatCommandsList.clear();
    for(auto beat: vvFile->_beats)
    {
        QList<VVCommand *> commands;
        for(int i = 0; i < beat->_commands_count; i ++)
        {
            commands.append(beat->_commands.at(i));
        }
        _beatCommandsList.append(commands);
        _beatList.append(beat->_beat_number);
    }

    //先清空场景中所有的item
    for(QList<CommandItem*> itemList : _items)
    {
        for(CommandItem* item : itemList)
        {
            removeItem(item);
        }
    }
    _items.clear();

    //初始化Ui
    for(int i = 0; i < _beatCommandsList.size(); i ++)
    {
        QList<VVCommand*>   commands = _beatCommandsList.at(i);
        QList<CommandItem*> commandItems;
        for(int j = 0; j < commands.size(); j ++)
        {
            int xOffsetIndex = j;
            //! 在场景中创建一个新的item
            CommandItem *item = new CommandItem(i, commands.at(j), QRect(QPoint(0, 0), QSize(CommandRectWidth, CommandRectHeight)));
            connect(item, &CommandItem::modify, this, &GraphicsScene::modifyCommand);
            connect(item, &CommandItem::remove, this, &GraphicsScene::deleteOneCommand);
            //! 将item添加至场景中
            addItem(item);
            //! 设置item的位置
            item->setPos(xOffsetIndex*HorizontalInterval, i*VerticalInterval);
            commandItems.append(item);
        }
        _items.append(commandItems);
    }

    adjustSceneRect();
}

void GraphicsScene::modifyBeat(int oldBeat, int newBeat)
{
    // step1.修改记录节拍
    int index = _beatList.indexOf(oldBeat);
    _beatList.replace(index, newBeat);

    // step2.移动图元
    QList<CommandItem*> commandItems = _items.at(oldBeat);
    for(CommandItem *commandItem: commandItems)
    {
        QPointF posF = commandItem->pos();
        posF.setY(newBeat*VerticalInterval);
        commandItem->setPos(posF);
    }
}

void GraphicsScene::addBeat()
{
    _beatCommandsList.append(QList<VVCommand*>());
    _items.append(QList<CommandItem*>());
}

void GraphicsScene::insertBeat(int pos)
{
    _beatCommandsList.insert(pos, QList<VVCommand*>());
    _items.insert(pos, QList<CommandItem*>());

    for(int i = pos + 1; i < _beatCommandsList.size(); ++i)
    {
        QList<CommandItem*> commandItems = _items.at(i);
        for(CommandItem *commandItem: commandItems)
        {
            QPointF posF = commandItem->pos();
            posF.setY(posF.y() + VerticalInterval);
            commandItem->setPos(posF);
        }
    }
}
