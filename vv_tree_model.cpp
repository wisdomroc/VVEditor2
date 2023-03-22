#include "vv_tree_model.h"
#include "beat_arrow.h"
#include <QPixmap>
#include <QDebug>

VVTreeModel::VVTreeModel(const QStringList& headers, QObject *parent)
    : QAbstractItemModel(parent),
      _basedType(0)
{
    _headers = headers;
    _rootItem = new VVTreeItem();
}

VVTreeModel::~VVTreeModel()
{
    delete _rootItem;
}

void VVTreeModel::setBasedType(int basedType)
{
    beginResetModel();
    if(basedType == 0)
        _headers.replace(0, QStringLiteral("节拍"));
    else if(basedType == 1)
        _headers.replace(0, QStringLiteral("时间"));
    else if(basedType == 2)
        _headers.replace(0, QStringLiteral("标准时间"));
    _basedType = basedType;
    endResetModel();
}

VVTreeItem *VVTreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (index.isValid())
    {
        VVTreeItem *item = static_cast<VVTreeItem*>(index.internalPointer());
        return item;
    }
    return _rootItem;
}

VVTreeItem *VVTreeModel::root()
{
    return _rootItem;
}

// 获取表头数据
QVariant VVTreeModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return _headers.at(section);
        }
    }
    return QVariant();
}

// 获取index.row行，index.column列数据
QVariant VVTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    VVTreeItem *item = itemFromIndex(index);
    item->setBasedType(_basedType);
    if (role == Qt::DisplayRole)
    {
        if(itemFromIndex(index.sibling(index.row(), 0))->getType() == VVTreeItem::BEAT && index.column() == 1)
        {
            VVBeat* vvBeat = reinterpret_cast<VVBeat*>(item->ptr());
//            QStringList strList = vvBeat->_description.split("\r\n", QString::SkipEmptyParts);

            return  vvBeat->_description;
        }

        return item->data(index.column());
    }
    if(role == Qt::DecorationRole)
    {
        VVTreeItem *item = itemFromIndex(index);
        if(item->getType() == VVTreeItem::DIR)
        {
            QPixmap pixmap(":/img/dir.png");
            return QVariant::fromValue(pixmap.scaledToHeight(9));
        }
        else if(item->getType() == VVTreeItem::FILE)
        {
            QPixmap pixmap(":/img/file.png");
            return QVariant::fromValue(pixmap.scaledToHeight(9));
        }
//        else if(item->getType() == VVTreeItem::BEAT && index.column() == 0)
//        {
//            QPixmap pixmap(":/img/beat.png");
//            return QVariant::fromValue(pixmap.scaledToHeight(9));
//        }
        else if(item->getType() == VVTreeItem::COMMAND && index.column() == 0)
        {
            QPixmap pixmap(":/img/command.png");
            return QVariant::fromValue(pixmap.scaledToHeight(13));
        }
    }
//    if(role == Qt::TextAlignmentRole)
//    {
//        return Qt::AlignCenter;
//    }
    if(role == Qt::ToolTipRole)
    {
        VVTreeItem *item = itemFromIndex(index);
        if(item->getType() == VVTreeItem::BEAT)
        {
            VVBeat* vvBeat = reinterpret_cast<VVBeat*>(item->ptr());
            return QVariant::fromValue(tr("%1%2").arg(QStringLiteral("节拍描述：\n")).arg(vvBeat->_description));
        }
        else if(item->getType() == VVTreeItem::COMMAND && index.column() == 8)
        {
            VVCommand* vvCommand = reinterpret_cast<VVCommand*>(item->ptr());
            return QVariant::fromValue(vvCommand->_data_value);
        }
    }
    return QVariant();
}

bool VVTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole)
    {
        VVTreeItem *item = itemFromIndex(index);
        if(item->getType() == VVTreeItem::COMMAND)
        {
            VVCommand *vvCommand = reinterpret_cast<VVCommand*>(item->ptr());
            switch (index.column()) {
            case 3:
                vvCommand->_msg_name = value.toString();
                break;
            case 4:
                vvCommand->_msg_id = value.toString();
                break;
            case 5:
                vvCommand->_sig_name = value.toString();
                break;
            case 6:
                vvCommand->_sig_id = value.toString();
                break;
            case 7:
                vvCommand->_data_length = value.toInt();
                break;
            case 8:
                vvCommand->_data_type = value.toString();
                break;
            case 9:
                vvCommand->_data_value = value.toString();
                break;
            case 10:
                vvCommand->_data_indexs = value.toString();//nisz add 2022-6-17
                break;
            }
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

// 在parent节点下，第row行，第column列位置上创建索引
QModelIndex VVTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    VVTreeItem *parentItem = itemFromIndex(parent);
    VVTreeItem *item = parentItem->child(row);
    if (item)
        return createIndex(row, column, item);
    else
        return QModelIndex();
}

// 创建index的父索引
QModelIndex VVTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    VVTreeItem *item = itemFromIndex(index);
    VVTreeItem *parentItem = item->parent();

    if (parentItem == _rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

// 获取索引parent下有多少行
int VVTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    VVTreeItem* item = itemFromIndex(parent);
    return item->childCount();
}

// 返回索引parent下有多少列
int VVTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _headers.size();
}

Qt::ItemFlags VVTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    VVTreeItem *item = itemFromIndex(index);
    if (item->getType() == VVTreeItem::COMMAND)
    {
        if(index.column() == 3 || index.column() == 4 || index.column() == 5 || index.column() == 6 || index.column() == 9)
            flags |= Qt::ItemIsEditable;
    }
    return flags;
}

VVTreeItem::Type VVTreeModel::dataType(QModelIndex index)
{
    VVTreeItem *vvTreeItem = itemFromIndex(index);
    return vvTreeItem->getType();
}

bool VVTreeModel::addDir(QString name)
{
    for(int i = 0; i < _rootItem->childCount(); i ++)
    {
        VVDir *vvDir = static_cast<VVDir *>(_rootItem->child(i)->ptr());
        if(vvDir->_dir_name == name)
            return false;
    }

    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    VVDir* dir = new VVDir();
    dir->_dir_name = name;
    VVTreeItem* dir_item = new VVTreeItem(_rootItem);
    dir_item->setPtr(dir);
    dir_item->setType(VVTreeItem::DIR);
    _rootItem->addChild(dir_item);
    endInsertRows();
    return true;
}

bool VVTreeModel::addDir(VVDir* vvDir)
{
    QStringList dirNameList;
    for(int i = 0; i  < _rootItem->childCount(); ++i)
    {
        VVDir* vvDir = reinterpret_cast<VVDir*>(_rootItem->child(i)->ptr());
        dirNameList.append(vvDir->_dir_name);
    }
    while (dirNameList.contains(vvDir->_dir_name)) {
        vvDir->_dir_name = vvDir->_dir_name + QStringLiteral("_副本");
    }

    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    VVTreeItem* dir_item = new VVTreeItem(_rootItem);
    dir_item->setPtr(vvDir);
    dir_item->setType(VVTreeItem::DIR);
    for(int i = 0; i < vvDir->_files.size(); ++i)
    {
        VVTreeItem* file_item = new VVTreeItem(dir_item);
        file_item->setPtr(vvDir->_files.at(i));
        file_item->setType(VVTreeItem::FILE);
        dir_item->addChild(file_item);
    }
    _rootItem->addChild(dir_item);
    endInsertRows();
    return true;
}

bool VVTreeModel::modifyDir(const QString &oldDir, const QString &newDir, QModelIndex index)
{
    QStringList allNames;
    for(int i = 0; i < _rootItem->childCount(); i ++)
    {
        VVDir *vvDir = static_cast<VVDir *>(_rootItem->child(i)->ptr());
        allNames.append(vvDir->_dir_name);
    }

    for(int i = 0; i < _rootItem->childCount(); i ++)
    {
        VVDir *vvDir = static_cast<VVDir *>(_rootItem->child(i)->ptr());
        if(vvDir->_dir_name == oldDir)
        {
            allNames.removeOne(oldDir);
            if(allNames.contains(newDir)) return false;

            vvDir->_dir_name = newDir;
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

bool VVTreeModel::addFile(const QString &name, int executionNumber, const QString &cycleStr, QModelIndex parent)
{
    VVTreeItem *curVVDirItem = _rootItem->child(parent.row());
    for(int i = 0; i < curVVDirItem->childCount(); i ++)
    {
        VVFile *vvFile = static_cast<VVFile *>(curVVDirItem->child(i)->ptr());
        if(vvFile->_file_name == name)
            return false;
    }

    beginInsertRows(parent, curVVDirItem->childCount(), curVVDirItem->childCount());
    VVFile *file = new VVFile();
    file->_file_name = name;
    file->_num_of_exe = executionNumber;
    file->_time_interval = cycleStr;
    VVTreeItem *parent_item = itemFromIndex(parent);
    VVTreeItem *file_item = new VVTreeItem(parent_item);
    file_item->setPtr(file);
    file_item->setType(VVTreeItem::FILE);
    parent_item->addChild(file_item);
    endInsertRows();
    return true;
}

bool VVTreeModel::addFile(VVFile* vvFile, QModelIndex parent)
{
    VVTreeItem *curVVDirItem = _rootItem->child(parent.row());
    QStringList fileNameList;
    for(int i = 0; i < curVVDirItem->childCount(); ++i)
    {
        VVFile* vvFile = reinterpret_cast<VVFile*>(curVVDirItem->child(i)->ptr());
        fileNameList.append(vvFile->_file_name);
    }
    while (fileNameList.contains(vvFile->_file_name)) {
        vvFile->_file_name = vvFile->_file_name + QStringLiteral("_副本");
    }

    beginInsertRows(parent, curVVDirItem->childCount(), curVVDirItem->childCount());
    VVTreeItem *parent_item = itemFromIndex(parent);
    VVDir* vvDir = reinterpret_cast<VVDir*>(parent_item->ptr());
    vvDir->_files.append(vvFile);
    VVTreeItem *file_item = new VVTreeItem(parent_item);
    file_item->setPtr(vvFile);
    file_item->setType(VVTreeItem::FILE);
    parent_item->addChild(file_item);
    endInsertRows();
    return true;
}

bool VVTreeModel::modifyFile(VVFile *oldVVFile, const QString &newFile,
                             const int &newNumOfExe, const QString &newCycleStr,
                             QModelIndex index, int multiplier)
{
    QStringList allNames;
    VVTreeItem *curVVDirItem = _rootItem->child(index.parent().row());
    for(int i = 0; i < curVVDirItem->childCount(); i ++)
    {
        VVFile *vvFile = static_cast<VVFile *>(curVVDirItem->child(i)->ptr());
        allNames.append(vvFile->_file_name);
    }

    for(int i = 0; i < curVVDirItem->childCount(); i ++)
    {
        VVFile *vvFile = static_cast<VVFile *>(curVVDirItem->child(i)->ptr());
        if(vvFile == oldVVFile)
        {
            allNames.removeOne(oldVVFile->_file_name);
            if(allNames.contains(newFile)) return false;

            vvFile->_file_name = newFile;
            vvFile->_num_of_exe = newNumOfExe;
            vvFile->_time_interval = newCycleStr;
            //! 文件修改了 调度周期 以后，文件下的所有节拍里的相应_time_interval也应该进行更新
            for(auto beat: vvFile->_beats)
            {
                beat->_time_interval = newCycleStr;
            }

            //! 增加对调度周期减小，并且可以被之前整除的处理（即将节拍变大）
            if(multiplier != 0)
            {
                for(int i = 0; i < vvFile->_beats_count; ++i)
                {
                    vvFile->_beats[i]->_beat_number = vvFile->_beats[i]->_beat_number * multiplier;
                }
            }

            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

void VVTreeModel::addBeat(QModelIndex index, VVBeat *beat)
{
    VVTreeItem *file_item = itemFromIndex(index);
    VVFile *vvFile = reinterpret_cast<VVFile *>(file_item->ptr());
    vvFile->_beats.append(beat);
    vvFile->_beats_count = vvFile->_beats.size();

    emit dataChanged(index, index);
}

void VVTreeModel::addBeatRight(QModelIndex index, int start, VVBeat *beat)
{
    beginInsertRows(index, start, start);

    VVTreeItem *root_item = itemFromIndex(index);
    VVTreeItem* beat_item = new VVTreeItem(root_item);
    beat_item->setType(VVTreeItem::BEAT);
    beat_item->setPtr(beat);
    root_item->addChild(beat_item);

    endInsertRows();
}

void VVTreeModel::insertBeat(QModelIndex index, int row, VVBeat *beat)
{
    VVTreeItem *file_item = itemFromIndex(index);
    VVFile *vvFile = static_cast<VVFile *>(file_item->ptr());
    int addNumber = 0;
    if(row > 0)
        addNumber = beat->_beat_number - vvFile->_beats.at(row-1)->_beat_number;
    vvFile->_beats.insert(row, beat);

    emit dataChanged(index, index);
}

void VVTreeModel::insertBeatRight(QModelIndex index, int start, VVBeat *beat)
{
    beginInsertRows(index, start, start);

    VVTreeItem* parentVVTreeItem = itemFromIndex(index);
    VVTreeItem* vvTreeItem = new VVTreeItem(parentVVTreeItem);
    vvTreeItem->setType(VVTreeItem::BEAT);
    vvTreeItem->setPtr(beat);
    parentVVTreeItem->insertChild(start, vvTreeItem);
    for(int i = start+1; i < root()->childCount(); ++i)
    {
        parentVVTreeItem->child(i)->setRow(i);
    }


    int addNumber = 0;
    if(start > 0)
        addNumber = beat->_beat_number - reinterpret_cast<VVBeat*>(parentVVTreeItem->child(start - 1)->ptr())->_beat_number;
    for(int i = start + 1; i < parentVVTreeItem->childCount(); ++i)
    {
        int oldBeatNumber = reinterpret_cast<VVBeat*>(parentVVTreeItem->child(i)->ptr())->_beat_number;
        VVBeat* vvBeat = reinterpret_cast<VVBeat*>(parentVVTreeItem->child(i)->ptr());
        vvBeat->_beat_number = oldBeatNumber + addNumber;
    }


    endInsertRows();
}

void VVTreeModel::modifyBeat(VVBeat *beat, int index, int type, QString des, QModelIndex beat_index)
{
    VVTreeItem *curVVDirItem = _rootItem->child(beat_index.parent().row());
    VVFile *vvFile = static_cast<VVFile *>(curVVDirItem->child(beat_index.row())->ptr());

    int newInterval = index - beat->_beat_number;

    beat->_beat_number = index;
    beat->_type = type;
    beat->_description = des;
    emit dataChanged(beat_index, beat_index);

    int startIndex = vvFile->_beats.indexOf(beat);
    for(int i = startIndex + 1; i < vvFile->_beats_count; ++i)
    {
        VVBeat* vvBeat = vvFile->_beats.at(i);
        vvBeat->_beat_number = vvBeat->_beat_number + newInterval;

        emit dataChanged(beat_index.sibling(beat_index.row() + i - startIndex, beat_index.column()), beat_index);
    }



}

void VVTreeModel::modifyCommand(QModelIndex index, int beatIndex, VVCommand* vvCommand, VVCommand *vvCommandNew)
{
    VVTreeItem *file_item = itemFromIndex(index);
    VVFile *vvFile = static_cast<VVFile *>(file_item->ptr());
    VVBeat *curBeat = vvFile->_beats.at(beatIndex);
    int row = curBeat->_commands.indexOf(vvCommand);
    curBeat->_commands.replace(row, vvCommandNew);

    emit dataChanged(index, index);
}

void VVTreeModel::modifyCommandRight(QModelIndex index, VVCommand *vvCommand, VVCommand *vvCommandNew)
{
    VVTreeItem* modify_command_item = itemFromIndex(index);
    modify_command_item->setPtr(vvCommandNew);
    VVBeat* vvBeat = reinterpret_cast<VVBeat*>(itemFromIndex(index.parent())->ptr());
    int row = vvBeat->_commands.indexOf(vvCommand);
    vvBeat->_commands.replace(row, vvCommandNew);
    //    delete vvCommand;//TODO.
    emit dataChanged(index, index);
}

void VVTreeModel::addCommand(QModelIndex index, int beatIndex, VVCommand *command)
{
    VVTreeItem *file_item = itemFromIndex(index);
    VVFile *vvFile = static_cast<VVFile *>(file_item->ptr());
    VVBeat *curBeat = vvFile->_beats.at(beatIndex);

    curBeat->_commands.append(command);
    vvFile->_beats.replace(beatIndex, curBeat);

    emit dataChanged(index, index);
}

void VVTreeModel::addCommandRight(QModelIndex parent, int start, VVCommand *command)
{
    beginInsertRows(parent, start, start);

    VVTreeItem *beat_item = itemFromIndex(parent);
    VVTreeItem* command_item = new VVTreeItem(beat_item);
    command_item->setType(VVTreeItem::COMMAND);
    command_item->setPtr(command);
    beat_item->addChild(command_item);

    endInsertRows();
}

void VVTreeModel::pasteCommand(QModelIndex index, int start, VVCommand *command)
{
    beginInsertRows(index, start, start);

    VVTreeItem* beatItem = itemFromIndex(index);
    VVTreeItem* vvTreeItem = new VVTreeItem(beatItem);
    vvTreeItem->setType(VVTreeItem::COMMAND);
    vvTreeItem->setPtr(command);
    beatItem->addChild(vvTreeItem);

    endInsertRows();
}

void VVTreeModel::pasteBeat(QModelIndex index, int start, VVBeat *beat)
{
    beginInsertRows(index, start, start);

    VVTreeItem *beatItemNew = new VVTreeItem(root());
    beatItemNew->setType(VVTreeItem::BEAT);
    beatItemNew->setPtr(beat);
    for(int i = 0; i < beat->_commands.size(); ++i)
    {
        VVTreeItem *commandItemNew = new VVTreeItem(beatItemNew);
        commandItemNew->setType(VVTreeItem::COMMAND);
        commandItemNew->setPtr(beat->_commands.at(i));
        beatItemNew->addChild(commandItemNew);
    }

    root()->addChild(beatItemNew);

    endInsertRows();
}

void VVTreeModel::deleteDirOrFile(QModelIndex index)
{
    beginRemoveRows(index.parent(), index.row(), index.row());
    VVTreeItem *file_item = itemFromIndex(index);
    if(file_item->getType() == VVTreeItem::DIR)
    {
        _rootItem->removeChild(file_item);
    }
    else if(file_item->getType() == VVTreeItem::FILE)
    {
        VVTreeItem *dir_item = file_item->parent();
        dir_item->removeChild(file_item);
    }
    endRemoveRows();
}

void VVTreeModel::deleteBeat(QModelIndex index, VVBeat *beat)
{
    VVTreeItem *file_item = itemFromIndex(index);
    if(file_item->getType() == VVTreeItem::FILE)
    {
        VVFile* vvFile = reinterpret_cast<VVFile*>(file_item->ptr());
        vvFile->_beats.removeOne(beat);
        vvFile->_beats_count = vvFile->_beats.size();
    }

    emit dataChanged(index, index);
}

void VVTreeModel::deleteBeatRight(QModelIndex parent, int first, int last)
{
    beginRemoveRows(parent, first, last);

    VVTreeItem *item = itemFromIndex(parent);
    for(int i = first; i <= last; i ++)
    {
        item->removeChild(item->child(first));
    }

    endRemoveRows();
}

void VVTreeModel::deleteCommand(QModelIndex index, VVBeat* beat, VVCommand *command)
{
    VVTreeItem *file_item = itemFromIndex(index);
    if(file_item->getType() == VVTreeItem::FILE)
    {
        VVFile* vvFile = reinterpret_cast<VVFile*>(file_item->ptr());
        int i = vvFile->_beats.indexOf(beat);
        if(i == -1)
        {
            qDebug() << QStringLiteral("找不到该节拍！") << endl;
            return;
        }
        vvFile->_beats.at(i)->_commands.removeOne(command);
        vvFile->_beats.at(i)->_commands_count = vvFile->_beats.at(i)->_commands.size();
    }

    emit dataChanged(index, index);
}

void VVTreeModel::deleteCommandRight(QModelIndex parent, int first, int last)
{
    beginRemoveRows(parent, first, last);

    VVTreeItem *item = itemFromIndex(parent);
    for(int i = first; i <= last; i ++)
    {
        item->removeChild(item->child(first));
    }

    endRemoveRows();
}


