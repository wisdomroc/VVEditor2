#include "treeview.h"

#include <QDebug>
#include <QAction>
#include <QMouseEvent>
#include "vv_tree_model.h"
#include "mainwindow.h"
#include "w_headerview.h"

TreeView::TreeView(QWidget *parent):QTreeView (parent)
{
    WHeaderView *wheader = new WHeaderView(Qt::Horizontal,this);
    connect(wheader, &WHeaderView::expanded, this, &TreeView::slot_expanded);
    setHeader(wheader);

    _action_newDir       = new QAction(QIcon(":/img/new_dir.png"),  QStringLiteral("新建文件夹"));
    _action_newFile      = new QAction(QIcon(":/img/new_file.png"), QStringLiteral("新建文件"));
    _action_modify       = new QAction(QIcon(":/img/modify.png"),   QStringLiteral("修改"));

    connect(_action_newDir,    &QAction::triggered, this, [this](){ emit newDir(); });
    connect(_action_newFile,   &QAction::triggered, this, [this](){ emit newFile(); });
    connect(_action_modify,    &QAction::triggered, this, [this](){ emit modify(currentIndex()); });

    addAction(_action_newDir);
    addAction(_action_newFile);
    addAction(_action_modify);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
}

TreeView::~TreeView()
{

}

QModelIndexList TreeView::getSelectedIndexs()
{
    return selectedIndexes();
}

MainWindow* TreeView::findRootObject()
{
    QObject* root = findRootObject(this);
    MainWindow* mainwindow = reinterpret_cast<MainWindow*>(root);
    return mainwindow;
}

QObject *TreeView::findRootObject(QObject *obj)
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

int TreeView::getSelectedIndexsSize()
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

void TreeView::slot_expanded(bool expanded)
{
    if(expanded)
        expandAll();
    else
        collapseAll();
}

void TreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    /**
     * @brief actionList:
     *        第一个：新建文件夹
     *        第二个：新建文件
     *        第三个：修改
     *        第四个：删除
     *        第五个：复制
     *        第六个：粘贴
     */

    VVTreeModel *vvTreeModel = dynamic_cast<VVTreeModel*>(model());
    if(actions().size() != 6) return;

    if(current.isValid() && vvTreeModel->dataType(current) == VVTreeItem::DIR)
    {
        _action_newDir->setVisible(false);
        _action_newFile->setVisible(true);
        _action_modify->setVisible(true);
        actions().at(3)->setVisible(true);
        actions().at(4)->setVisible(true);
        actions().at(5)->setVisible(true);
    }
    else if(current.isValid() && vvTreeModel->dataType(current) == VVTreeItem::FILE)
    {
         _action_newDir->setVisible(false);
         _action_newFile->setVisible(true);
        _action_modify->setVisible(true);
        actions().at(3)->setVisible(true);
        actions().at(4)->setVisible(true);
        actions().at(5)->setVisible(true);
    }
    else
    {
        _action_newDir->setVisible(true);
        _action_newFile->setVisible(false);
        _action_modify->setVisible(false);
        actions().at(3)->setVisible(false);
        actions().at(4)->setVisible(false);
        actions().at(5)->setVisible(false);

        MainWindow* mainwindow = findRootObject();
        if(mainwindow->isCopy())
        {
            actions().at(5)->setVisible(true);
        }
    }
    QTreeView::currentChanged(current,previous);

    emit currentIndexChanged(current,previous);
}

void TreeView::mousePressEvent(QMouseEvent *event)
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
