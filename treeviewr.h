/***********************************************************************
* @filename treeviewr.h
* @brief    自定义TreeView，
*           实现自定义右键菜单 &
*           双击编辑 &
*           右键currentIndex实时改变
* @author   wanghp
* @date     2022-04-27
************************************************************************/

#ifndef TREEVIEWR_H
#define TREEVIEWR_H

#include <QTreeView>
class MainWindow;

class TreeViewR : public QTreeView
{
    Q_OBJECT
public:
    TreeViewR(QWidget *parent = nullptr);
    ~TreeViewR();
    int getSelectedIndexsSize();

protected:

    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);


signals:
    void editCommand(QModelIndex index);
    void editFormula(QModelIndex index);
    void edit();

    void newBeat();
    void newCommand();
    void addBeatBefore(int, int);
    void addBeatAfter(int);
    void sigChanged();
//    void sigCurrentIndexChanged(const QModelIndex & current, const QModelIndex & previous);

private slots:
    void slot_addBeatBefore();
    void slot_addBeatAfter();
    void slot_expanded(bool expanded);

private:
    MainWindow *findRootObject();
    QObject*    findRootObject(QObject *obj);

private:
    QAction* _action_copy;
    QAction* _action_paste;
    QAction* _action_newBeat;
    QAction* _action_newCommand;
    QAction* _action_addBeatBefore;
    QAction* _action_addBeatAfter;
    QAction* _action_edit;
};

#endif // TREEVIEWR_H
