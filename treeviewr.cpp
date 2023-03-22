#include "treeviewr.h"

#include <QDebug>
#include <QAction>
#include <QMouseEvent>
#include "vv_tree_model.h"
#include "mainwindow.h"
#include "w_headerview.h"

TreeViewR::TreeViewR(QWidget *parent):QTreeView (parent)
{
    WHeaderView *wheader = new WHeaderView(Qt::Horizontal,this);
    connect(wheader, &WHeaderView::expanded, this, &TreeViewR::slot_expanded);
    setHeader(wheader);


    _action_newBeat       = new QAction(QIcon(":/img/new_beat.png"), QStringLiteral("新建节拍"));
    _action_newCommand    = new QAction(QIcon(":/img/new_command.png"), QStringLiteral("新建命令"));
    _action_edit          = new QAction(QIcon(":/img/beat_modify.png"), QStringLiteral("修改"));
    _action_addBeatBefore = new QAction(QStringLiteral("在之前插入节拍"));
    _action_addBeatAfter  = new QAction(QStringLiteral("在之后插入节拍"));
    connect(_action_newBeat,       &QAction::triggered, this, [this](){ emit newBeat(); });
    connect(_action_newCommand,    &QAction::triggered, this, [this](){ emit newCommand(); });
    connect(_action_edit,          &QAction::triggered, this, [this](){ emit edit(); });
    connect(_action_addBeatBefore, &QAction::triggered, this, &TreeViewR::slot_addBeatBefore);
    connect(_action_addBeatAfter,  &QAction::triggered, this, &TreeViewR::slot_addBeatAfter);
    addAction(_action_newBeat);
    addAction(_action_newCommand);
    addAction(_action_edit);
    addAction(_action_addBeatBefore);
    addAction(_action_addBeatAfter);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
}

TreeViewR::~TreeViewR()
{

}

int TreeViewR::getSelectedIndexsSize()
{
    QModelIndexList indexList = selectedIndexes();
    QList<int> rowList;
    for(auto i: indexList)
    {
        if(!rowList.contains(i.row()))
        {
            rowList.append(i.row());
        }
    }
    return rowList.size();
}

void TreeViewR::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    /**
     * @brief actionList:
     *        第一个：新建节拍
     *        第二个：新建命令
     *        第三个：修改
     *        第四个：在之前插入节拍
     *        第五个：在之后插入节拍
     *        第六个：删除
     *        第七个：复制
     *        第八个：粘贴
     */

    VVTreeModel *vvTreeModel = dynamic_cast<VVTreeModel*>(model());
    if(actions().size() != 8) return;

    if(current.isValid() && vvTreeModel->dataType(current) == VVTreeItem::BEAT)
    {
        _action_newBeat->setVisible(true);
        _action_newCommand->setVisible(true);
        _action_edit->setVisible(true);
        _action_addBeatBefore->setVisible(true);
        _action_addBeatAfter->setVisible(true);
        actions().at(5)->setVisible(true);
        actions().at(6)->setVisible(true);
        actions().at(7)->setVisible(true);

        if(current.data(0).toInt() == 0)
            _action_addBeatBefore->setEnabled(false);
        else
            _action_addBeatBefore->setEnabled(true);
    }
    else if(current.isValid() && vvTreeModel->dataType(current) == VVTreeItem::COMMAND)
    {
        _action_newBeat->setVisible(false);
        _action_newCommand->setVisible(true);
        _action_edit->setVisible(true);
        _action_addBeatBefore->setVisible(false);
        _action_addBeatAfter->setVisible(false);
        actions().at(5)->setVisible(true);
        actions().at(6)->setVisible(true);
        actions().at(7)->setVisible(true);
    }
    else
    {
        _action_newBeat->setVisible(true);
        _action_newCommand->setVisible(false);
        _action_edit->setVisible(false);
        _action_addBeatBefore->setVisible(false);
        _action_addBeatAfter->setVisible(false);
        actions().at(5)->setVisible(false);
        actions().at(6)->setVisible(false);
        actions().at(7)->setVisible(false);

        MainWindow* mainwindow = findRootObject();
        if(mainwindow->isCopyR())
        {
            actions().at(7)->setVisible(true);
        }
    }
    QTreeView::currentChanged(current,previous);
    emit clicked(current);
//    qDebug() <<"currentChanged-->" << current.row() << current.column() << ";" << previous.row() << previous.column() << endl;
//    emit sigCurrentIndexChanged(current,previous);
}

void TreeViewR::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
    {
        clearSelection();
        currentChanged(QModelIndex(), QModelIndex());
    }
    else
    {
        currentChanged(index, QModelIndex());
    }
    QTreeView::mousePressEvent(event);
}

void TreeViewR::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
        QTreeView::mouseDoubleClickEvent(event);

    VVTreeItem* vvTreeItem = reinterpret_cast<VVTreeModel*>(model())->itemFromIndex(index);
    if(vvTreeItem->getType() == VVTreeItem::COMMAND)
    {
        VVCommand* vvCommand = reinterpret_cast<VVCommand*>(vvTreeItem->ptr());
        if(vvCommand != nullptr)
        {
            if(index.column() == 2)
            {
                emit editCommand(index);
            }
            else if(index.column() == 9 && index.sibling(index.row(), 2).data().toString() == "COMBINATION")
            {
                emit editFormula(index);
            }
            else if (index.column()==3||index.column()==4||index.column()==5||index.column()==6)
            {
               emit sigChanged();
            }
        }
    }

    QTreeView::mouseDoubleClickEvent(event);
}

void TreeViewR::slot_addBeatBefore()
{
    QModelIndex curIndex = currentIndex();
    int curIndexRowNumber = curIndex.row();
    QModelIndex lastIndex = QModelIndex();
    if(curIndex.row() > 0)
        lastIndex = curIndex.sibling(curIndexRowNumber - 1, 0);

    VVTreeModel* vvTreeModel = qobject_cast<VVTreeModel*>(model());
    VVBeat* curVVBeat  = nullptr;
    VVBeat* lastVVBeat = nullptr;
    if(curIndex.isValid())
        curVVBeat = reinterpret_cast<VVBeat*>(vvTreeModel->itemFromIndex(curIndex)->ptr());
    if(lastIndex.isValid())
        lastVVBeat = reinterpret_cast<VVBeat*>(vvTreeModel->itemFromIndex(lastIndex)->ptr());

    int lastNumber = -1;
    int curNumber  = -1;
    if(lastVVBeat)
        lastNumber = lastVVBeat->_beat_number;
    if(curVVBeat)
        curNumber  = curVVBeat->_beat_number;
    emit addBeatBefore(lastNumber, curNumber);
}

void TreeViewR::slot_addBeatAfter()
{
    QModelIndex curIndex = currentIndex();
    VVTreeModel* vvTreeModel = qobject_cast<VVTreeModel*>(model());
    VVBeat* curVVBeat  = nullptr;
    if(curIndex.isValid())
        curVVBeat = reinterpret_cast<VVBeat*>(vvTreeModel->itemFromIndex(curIndex)->ptr());
    int curNumber  = -1;
    if(curVVBeat)
        curNumber  = curVVBeat->_beat_number;
    emit addBeatAfter(curNumber);
}

void TreeViewR::slot_expanded(bool expanded)
{
    if(expanded)
        expandAll();
    else
        collapseAll();
}

MainWindow* TreeViewR::findRootObject()
{
    QObject* root = findRootObject(this);
    MainWindow* mainwindow = reinterpret_cast<MainWindow*>(root);
    return mainwindow;
}

QObject *TreeViewR::findRootObject(QObject *obj)
{
    if(obj->parent() == nullptr)
    {
        return obj;
    }
    else
    {
        return findRootObject(obj->parent());
    }
}
