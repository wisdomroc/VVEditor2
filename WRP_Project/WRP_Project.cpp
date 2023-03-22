#include "WRP_Project.h"
#include <QFile>
#include <QDomDocument>

WRP_Project::WRP_Project()
{

}

QString WRP_Project::getFilePath() const
{
    return m_filePath;
}

bool WRP_Project::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
    return loadFile();
}

QStandardItemModel *WRP_Project::getSystemArchitectureModel()
{
    return &m_systemArchitectureModel;
}

const QMap<QString, QString> &WRP_Project::getIcdContainer()
{
    return m_icdContainer;
}

bool WRP_Project::loadFile()
{
    m_systemArchitectureModel.clear();
    m_icdContainer.clear();

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    QDomElement docRootElement = doc.documentElement();//获取root节点
    QDomElement systemArchitectureElement = docRootElement.firstChildElement("systemarchitecture");
    if(systemArchitectureElement.isNull())
        return false;

    loadIcdFormicds(systemArchitectureElement.firstChildElement("icds"));

    //DataSource
    QList<QString> module_dataSource;
    QDomElement dataSoutce_msgid = systemArchitectureElement.firstChildElement("icds").firstChildElement("msgid");
    while(!dataSoutce_msgid.isNull()){
        module_dataSource.append(dataSoutce_msgid.text());
        dataSoutce_msgid = dataSoutce_msgid.nextSiblingElement("msgid");
    }

    //DataOutputer
    QList<QString> module_dataOutputer;
    QDomElement dataOutputer_msgid = systemArchitectureElement.firstChildElement("icds_output").firstChildElement("msgid");
    while(!dataOutputer_msgid.isNull()){
        module_dataOutputer.append(dataOutputer_msgid.text());
        dataOutputer_msgid = dataOutputer_msgid.nextSiblingElement("msgid");
    }

    for(auto item : getArchitectureItemFromModules(systemArchitectureElement.firstChildElement("modules"),module_dataSource,module_dataOutputer))
    {
        m_systemArchitectureModel.appendRow(item);
    }
    return true;
}

QList<QStandardItem *> WRP_Project::getArchitectureItemFromModules(QDomElement modulesElement,QList<QString> dataSource,QList<QString> dataOutputer)
{
    QList<QStandardItem*> itemList;
    //遍历module
    QDomElement moduleElement = modulesElement.firstChildElement("module");
    while(!moduleElement.isNull()){
        loadIcdFormicds(moduleElement.firstChildElement("icds"));//加载icd

        //DataSource
        QList<QString> module_dataSource;
        QDomElement dataSoutce_msgid = moduleElement.firstChildElement("icds").firstChildElement("msgid");
        while(!dataSoutce_msgid.isNull()){
            module_dataSource.append(dataSoutce_msgid.text());
            dataSoutce_msgid = dataSoutce_msgid.nextSiblingElement("msgid");
        }

        //DataOutputer
        QList<QString> module_dataOutputer;
        QDomElement dataOutputer_msgid = moduleElement.firstChildElement("icds_output").firstChildElement("msgid");
        while(!dataOutputer_msgid.isNull()){
            module_dataOutputer.append(dataOutputer_msgid.text());
            dataOutputer_msgid = dataOutputer_msgid.nextSiblingElement("msgid");
        }

        //whp add 2022-04-06
        QString tmpStr = moduleElement.firstChildElement("name").text();
        if(tmpStr.isEmpty() || tmpStr.contains("Note"))
        {
            moduleElement = moduleElement.nextSiblingElement("module");
            continue;
        }
        //创建item
        QStandardItem *item = new QStandardItem(tmpStr);//名称
        //whp add end

        item->setData(moduleElement.firstChildElement("uuid").text(),UUID_ROLE);//uuid
        QString type = moduleElement.firstChildElement("type").text();
        item->setData(type,TYPE_ROLE);//type

        //package:0;fmu:1;block:2;
        if(type.compare("0") == 0 || type.compare("1") == 0 || type.compare("2") == 0)
        {
            item->setCheckable(true);
            item->setTristate(true);
        }
        else if(type.compare("5") == 0)
        {
            item->setData(QVariant(dataSource),MSG_LIST_ROLE);
            item->setCheckable(true);
            item->setTristate(true);
        }
        else if(type.compare("6") == 0)
        {
            item->setData(QVariant(dataOutputer),MSG_LIST_ROLE);
            item->setCheckable(true);
            item->setTristate(true);
        }

        item->appendRows(getArchitectureItemFromModules(moduleElement.firstChildElement("submodule").firstChildElement("modules"),module_dataSource,module_dataOutputer));
        itemList.append(item);

        moduleElement = moduleElement.nextSiblingElement("module");
    }
    return itemList;
}

void WRP_Project::loadIcdFormicds(QDomElement icdsElement)
{
    //遍历所有msgid
    QDomElement msgidElement = icdsElement.firstChildElement("msgid");
    while(!msgidElement.isNull()){
        m_icdContainer.insert(msgidElement.text(),msgidElement.text());
        msgidElement = msgidElement.nextSiblingElement("msgid");
    }
}
