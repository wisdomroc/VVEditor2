/***********************************************************************
* @filename data_factory.h
* @brief    提供原始数据的一个类：
*           1.从文件读入内存   2.将内存中的数据存入本地文件
*           部分代码由旧代码继承而来
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef DATA_FACTORY_H
#define DATA_FACTORY_H
#include "vv_tree_item.h"

class QDomElement;
class QMutex;
class QMutexLocker;

class DataFactory
{
public:
    static DataFactory *get_instance();         //获取单例

    QVector<VVDir *> init();                    //初始化，并且返回所有信息

    bool save(VVTreeItem *root);                //根据treeView根节点进行所有数据的保存

    void setWorkspace(const QString& workspace);//设置工作空间

    void setDataSource(VVTreeItem *vvTreeItem); //设置根节点

    QString getDebugInfo();                     //获取警告信息

private:
    DataFactory();

private:
    static QMutex _mutex;
    static DataFactory *_dataFactory;
    QString _work_space;
    VVTreeItem *_vvTreeItem;

private:
    //写入数据
    bool writeFilePreCheck(VVTreeItem *file, QString folder_path);
    bool writeFile(VVTreeItem *file, QString folder_path);
    bool writeFileFixed(VVTreeItem *file, QString folder_path);
    void FixBeatInfo(QVector<VVBeat> &dest_beats_list, const QVector<VVBeat *> &beat_info);

    bool removeAllFiles(const QString &path);
    //[0] 重载
    void appendBeatsNode(QDomElement &node, const QVector<VVBeat> &beat_list);
    void appendBeatsNode(QDomElement &node, const QVector<VVBeat *> &beat_list);
    //[1] 重载
    void appendStepCmdNode(QDomElement &beatNode, const VVBeat& beat);
    void appendStepCmdNode(QDomElement &beatNode, VVBeat *beat);

    void appendChildNode(QDomElement &node, QString nodeName, QVariant nodeValue);
    //读取数据
    bool initFile(const QString &file_path, VVFile *file_data);
    void initBeat(const QDomElement &beats_node, QVector<VVBeat *> &beat_list);
    QString initCommand(const QDomElement &stepNode, VVBeat *beat);

private:
    QStringList _debugInfos;
};

#endif // DATA_FACTORY_H
