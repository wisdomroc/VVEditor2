/***********************************************************************
* @filename treeview.h
* @brief    自定义TreeView，
*           实现自定义右键菜单 &
*           右键currentIndex实时改变
* @author   wanghp
* @date     2022-04-27
************************************************************************/

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
class MainWindow;

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    TreeView(QWidget *parent = nullptr);
    ~TreeView();
    QModelIndexList getSelectedIndexs();
    int getSelectedIndexsSize();

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void mousePressEvent(QMouseEvent *event);

private:
    MainWindow *findRootObject();
    QObject*    findRootObject(QObject *obj);

signals:
    void newDir();
    void newFile();
    void copyDirOrFile();
    void pasteDirOrFile();
    void modify(QModelIndex);
    void currentIndexChanged(const QModelIndex & current, const QModelIndex & previous);

private slots:
    void slot_expanded(bool expanded);

private:
    QAction* _action_newDir;
    QAction* _action_newFile;
    QAction* _action_copy;
    QAction* _action_paste;
    QAction* _action_modify;
};

#endif // TREEVIEW_H
