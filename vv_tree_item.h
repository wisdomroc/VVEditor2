/***********************************************************************
* @filename vv_tree_item.h
* @brief    本来为数据源，树形分支结构
*           每一个单独的item可以代表一个文件夹、一个文件、一个节拍、一个命令
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef VVTREEITEM_H
#define VVTREEITEM_H

#include "vv_typedef.h"
#include <QVariant>

class VVTreeItem
{
public:
    enum Type
    {
        UNKNOWN = -1,
        DIR,
        FILE,
        BEAT,
        COMMAND
    };


    explicit VVTreeItem(VVTreeItem *parent = nullptr);
    ~VVTreeItem();

    void setBasedType(int basedType);

    void addChild(VVTreeItem *item);
    void insertChild(int index, VVTreeItem *item);
    void removeChild(VVTreeItem *item);
    void removeChildren();

    VVTreeItem *child(int row) { return _children.value(row); }
    VVTreeItem *parent() { return _parent; }

    int childCount() const { return _children.count(); }

    QVariant data(int column) const;

    //设置、获取节点存的数据指针
    void setPtr(void* p) { _ptr = p; }
    void* ptr() const { return _ptr; }


    void setRow(int row) { _row = row; }    //保存该节点是其父节点的第几个子节点，查询优化所用
    int row() const { return _row; }        //返回本节点位于父节点下第几个子节点

    Type getType() const { return _type; }
    void setType(const Type &value) { _type = value; }

private:
    QList<VVTreeItem*> _children;  // 子节点
    VVTreeItem*        _parent;    // 父节点
    Type               _type;      // 此节点保存的数据类型
    void*              _ptr;       // 存储数据的指针
    int                _row;       // 此item位于父节点中第几个
    int                _basedType;
};



#endif // VVTREEITEM_H
