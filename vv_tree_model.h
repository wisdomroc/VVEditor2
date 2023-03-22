/***********************************************************************
* @filename vv_tree_model.h
* @brief    提供一个自定义的model，
*           1.用来管理VVEditor左侧的文件夹/文件树，其中包含所有的原始数据
*           2.用来管理VVEditor右侧的treeView的节拍/命令树，其中仅包含当前选中的脚本文件中的所有内容
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef VVTREEMODEL_H
#define VVTREEMODEL_H
#include <QAbstractItemModel>
#include "vv_tree_item.h"

struct VVBeat;
struct VVCommand;

class VVTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit VVTreeModel(const QStringList& headers, QObject *parent = nullptr);
    ~VVTreeModel() override;

    void setBasedType(int basedType);

    VVTreeItem*         root();                        //获取根节点
    VVTreeItem::Type    dataType(QModelIndex index);   //获取当前index的类型

    QVariant            headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant            data(const QModelIndex &index, int role) const override;
    bool                setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex         index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex         parent(const QModelIndex &index) const override;
    int                 rowCount(const QModelIndex &parent) const override;
    int                 columnCount(const QModelIndex &parent) const override;
    Qt::ItemFlags       flags(const QModelIndex &index) const override;


    bool addDir(QString name);                                                                          //添加文件夹
    bool addDir(VVDir* vvDir);                                                                          //添加文件夹
    bool addFile(const QString &name, int executionNumber, const QString &cycleStr, QModelIndex parent);//添加文件
    bool addFile(VVFile* vvFile, QModelIndex parent);                                                   //添加文件

    void addBeat(QModelIndex index, VVBeat *beat);                                                      //添加节拍
    void addBeatRight(QModelIndex index, int start, VVBeat* beat);                                                 //添加节拍（节拍命令树）
    void addCommand(QModelIndex index, int beatIndex, VVCommand *command);                              //添加命令
    void addCommandRight(QModelIndex parent, int start, VVCommand *command);                            //添加命令（节拍命令树）

    void pasteBeat(QModelIndex index, int start, VVBeat *beat);                                                    //粘贴命令
    void pasteCommand(QModelIndex index, int start, VVCommand *command);                                           //粘贴命令

    void insertBeat(QModelIndex index, int row, VVBeat* beat);                                          //插入节拍
    void insertBeatRight(QModelIndex index, int start, VVBeat* beat);                                   //插入节拍（右侧）

    void deleteBeat(QModelIndex index, VVBeat* beat);                                                   //删除节拍
    void deleteBeatRight(QModelIndex parent, int first, int last);                                      //删除节拍（右侧）
    void deleteCommand(QModelIndex index, VVBeat *beat, VVCommand* command);                            //删除命令
    void deleteCommandRight(QModelIndex parent, int first, int last);                                   //删除命令（右侧）
    void deleteDirOrFile(QModelIndex index);                                                            //删除文件夹or文件

    bool modifyDir(const QString &oldDir, const QString &newDir, QModelIndex index);                    //修改文件夹名称
    bool modifyFile(VVFile *oldVVFile, const QString &newFile, const int &newNumOfExe,
                    const QString &newCycleStr, QModelIndex index, int multiplier = 0);                              //修改文件名称
    void modifyBeat(VVBeat *beat, int index, int type, QString des, QModelIndex beat_index);                         //修改节拍
    void modifyCommand(QModelIndex index, int beatIndex, VVCommand *vvCommand, VVCommand* vvCommandNew);//修改命令
    void modifyCommandRight(QModelIndex index, VVCommand *vvCommand, VVCommand* vvCommandNew);          //修改命令（右侧）
public:
    VVTreeItem *itemFromIndex(const QModelIndex &index) const;

private:
    QStringList _headers;   //水平表头
    VVTreeItem* _rootItem;  //根节点
    int _basedType;         //当前是基于节拍，还是基于时间
};



#endif // VVTREEMODEL_H
