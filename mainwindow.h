/***********************************************************************
* @filename mainwindow.h
* @brief    程序主窗口类
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndexList>
#include "vv_typedef.h"

namespace Ui {
class MainWindow;
}

class GraphicsScene;
class TreeModel;
class VVTreeModel;
class ICDData;
class QLineEdit;
class QModelIndex;
class VVTreeItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString workspace, QWidget *parent = nullptr);
    ~MainWindow();

public:
    bool isCopy();                                              //之前知否已有复制操作（左侧树）
    bool isCopyR();                                             //之前知否已有复制操作（右侧树）
    void setStatusBarInfo(const QString& workspace, const QString& iwrp);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void initConnections();                                     //初始化槽函数连接
    void initUi();                                              //初始化UI
    void initBeatAndCommandView(VVFile* vvFile);                //初始化右侧的treeView
    QVector<VVDir*> initData();                                 //初始化数据
    void setModel(const QVector<VVDir*>& proList);              //初始化左侧的treeView

    bool checkIndexValid();

    void updateRuleBar();                                       //更新rulebar
    void resetSceneHeight();                                    //重新计算更新 视口的scene的高度updateRuleBar
    void updateShowType();                                      //根据不同的type进行：0：新的轴线展示；1旧的表格展示
    void updateBeatArrowBasedType(int basedType);               //根据不同的baseType，切换轴线的展示方式

    void adjustDirFileSelection(VVFile *vvFile);
    bool getFileIntervalAndUnit(int &interval, QString &unit);  //获得当前选中文件的调度周期与单位
    void checkIcdExist(VVCommand* vvCommand,QStringList &icdList);

private slots:
    void slot_newDir();                                 //新建文件夹
    void slot_newFile();                                //新建文件
    void slot_newBeat();                                //新建节拍
    void slot_newCommand();                             //新建命令
    void slot_copy();                                   //复制（当前只是Beat & Command）
    void slot_paste();                                  //粘贴（当前只是Beat & Command）
    void slot_copyDirOrFile();                          //复制（文件夹 & 文件）
    void slot_pasteDirOrFile();                         //粘贴（文件夹 & 文件）

    void slot_delete();                                 //删除（左侧树 & 右侧树）
    void slot_remove();                                 //删除（右侧树）
    void slot_deleteCommandR(const QModelIndex &index); //删除命令R
    void slot_modify();                                 //编辑文件夹，或者编辑文件

    void slot_editR();                                  //右侧节拍命令树中的右键修改
    void slot_modifyBeat();                             //编辑节拍
    void slot_modifyCommand();                          //编辑命令

    void slot_save();                                   //保存

    void slot_beatBasedTriggered(bool checked);         //切换：节拍 / 时间 显示
    void slot_timeBasedTriggered(bool checked);         //切换：节拍 / 时间 显示
    void slot_basedTriggered(bool checked);             //作为一个按钮，来切换：箭头 / treeView显示
    void slot_arrowTriggered(bool checked);             //切换：箭头 / treeView 显示
    void slot_tableTriggered(bool checked);             //切换：箭头 / treeView 显示

    void slot_activeClicked(int activeIndex);                                       //节拍轴，点击了当前节拍
    void slot_addOneCommand(int beatIndex);                                         //节拍轴，点击添加命令，对应的槽函数

    void slot_graphicsViewHValueChanged();                                          //右侧视口的水平变化，对应的槽函数
    void slot_graphicsViewVValueChanged();                                          //右侧视口的垂直变化，对应的槽函数
    void slot_showToolTip(QDialog &dlg, QLineEdit *lineEdit, const QString &text);  //右侧视口中的浮动显示

    void slot_treeViewClicked(const QModelIndex &index);                            //左侧文件夹/文件结构树中点击，对应的槽函数

    void slot_addBeatBefore(int lastBeatNumber, int curBeatNumber);                 //在当前节拍 之前添加节拍
    void slot_addBeatAfter(int curBeatNumber);                                      //在当前节拍 之后添加节拍


    void slot_editBeat(const QModelIndex &index);       //右侧treeView，编辑节拍
    void slot_editCommand(const QModelIndex &index);    //右侧treeView，双击编辑命令
    void slot_editFormula(const QModelIndex &index);    //右侧treeView，双击编辑公式
    void slot_splitterMoved(int pos, int index);
    void slot_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);                            //model的dataChanged对应的槽函数
    void slot_currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);
    void slot_changed();
private:
    Ui::MainWindow*         ui;
    QSharedPointer<ICDData> _icd_data;
    VVTreeModel*            _vvTreeModelRight;  //右侧的model
    VVTreeModel*            _vvTreeModel;       //左侧的model
    GraphicsScene*          _graphicsScene;     //右侧的视口
    VVTreeItem*             _copyVVTreeItem;    //右侧的treeView中，复制的内容
    VVTreeItem*             _copyDirAndFile;    //左侧的treeView中，复制的内容
    VVFile*                 _vvFile;            //当前左侧树，点击后选中的脚本文件

    QString                 _workspace;         //WRP当前的工作空间
    int                     _basedType;         //0:基于节拍（调度周期）1:基于时间
    int                     _showType;          //0:新的轴线显示；1：旧的表格显示
    bool                    _hasModified;       //是否有修改
    int                     _whichFocus;        //当前的焦点在左边还是在右边：左边是0，右边是1，没有是-1
};



#endif // MAINWINDOW_H
