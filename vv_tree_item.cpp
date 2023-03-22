#include "vv_tree_item.h"
#include "beat_arrow.h"
#include <QDebug>
#include <TimeLib.h>
#include <QTime>

VVTreeItem::VVTreeItem(VVTreeItem *parent)
    : _parent(parent),
      _type(UNKNOWN),
      _ptr(nullptr),
      _row(-1),
      _basedType(0)
{

}

VVTreeItem::~VVTreeItem()
{
    removeChildren();
}

void VVTreeItem::setBasedType(int basedType)
{
    _basedType = basedType;
}

// 在本节点下添加子节点
void VVTreeItem::addChild(VVTreeItem *item)
{
    item->setRow(_children.size());
    _children.append(item);
}

void VVTreeItem::insertChild(int index, VVTreeItem *item)
{
    item->setRow(index);
    _children.insert(index, item);
    for(int i = index + 1; i < _children.count(); ++i)
    {
        _children.at(i)->setRow(_children.at(i)->row() + 1);
    }
}

void VVTreeItem::removeChild(VVTreeItem *item)
{
    _children.removeOne(item);
}

// 清空所有子节点
void VVTreeItem::removeChildren()
{
    //TODO.这里会发生崩溃
    qDeleteAll(_children);
    _children.clear();
}

// 获取本节点第column列的数据
QVariant VVTreeItem::data(int column) const
{
    if(_type == DIR)
    {
        if (column == 0)
        {
            VVDir* p = (VVDir*)_ptr;
            return p->_dir_name;
        }
    }
    else if(_type == FILE)
    {
        VVFile* p = (VVFile*)_ptr;
        switch (column)
        {
        case 0:       return p->_file_name;
        default:      return QVariant();
        }
    }
    else if(_type == BEAT)
    {
        VVBeat* p = (VVBeat*)_ptr;
        switch (column) {
        case 0:
            if(_basedType == 0)
            {
                return p->_beat_number;
            }
            else if(_basedType == 1)
            {
                int cycle; QString unit;
                BeatArrow::ConvertTimeStr2ValueAndUnit(p->_time_interval, cycle, unit);
                return QString("%1%2").arg(p->_beat_number * cycle).arg(unit);
            }
            else if(_basedType == 2)
            {
                int cycle; QString unit;
                BeatArrow::ConvertTimeStr2ValueAndUnit(p->_time_interval, cycle, unit);

                QString rightValStr = QString("%1%2").arg(p->_beat_number * cycle).arg(unit);

                double val1 = TimeValue(rightValStr).getValueWithUnit(Millisecond);
                QTime tmpTime(0,0,0,0);
                tmpTime = tmpTime.addMSecs((int)val1);
                QString retStr = tmpTime.toString("hh:mm:ss.zzz");

                return retStr;
            }
            break;
        default:      return QVariant();
        }
    }
    else if(_type == COMMAND)
    {
        VVCommand* p = (VVCommand*)_ptr;
        switch (column) {
        case 2:       return p->_command_type_str;
        case 3:       if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_msg_name;
        case 4:       if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_msg_id;
        case 5:       if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_sig_name;
        case 6:       if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_sig_id;
        case 7:       if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_data_length;
        case 8:       if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_data_type;
        case 9:
                      if(p->_command_type_str=="CHECK")
                          return p->_operator+p->_data_value;
                      else if(p->_command_type_str == "RESET")
                          return "/";
                      else
                          return p->_data_value;
        case 10:       if(p->_command_type_str=="RESET")
                          return "/";
                      else
                          return p->_data_indexs;
        default:      return QVariant();
        }
    }

    return QVariant();
}
