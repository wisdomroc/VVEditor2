#include "ConditionCommon.h"

#include "common_md.h"

ConditionCommon* ConditionCommon::m_conditionCommon = nullptr;


ConditionCommon *ConditionCommon::getInstance()
{
    if(!m_conditionCommon)
        m_conditionCommon = new ConditionCommon();
    return m_conditionCommon;
}


const QStandardItemModel *ConditionCommon::getModel() const
{
    return m_model;
}

void ConditionCommon::setModel(const QStandardItemModel *value)
{
    m_model = value;

    //处理FMU信息
    extractFMUInfo();
}

void ConditionCommon::extractFMUInfo()
{
    m_fumInfo.clear();

    if(!m_model)
        return;

    for(int row=0;row<m_model->rowCount();row++){
        for(int column=0;column<m_model->columnCount();column++){
            QStandardItem *item = m_model->item(row,column);
            if(item){
                extractFMUInfoFromItem(item);
            }
        }
    }

}

void ConditionCommon::extractFMUInfoFromItem(QStandardItem *item)
{
    TreeNode *treeNode = dynamic_cast<TreeNode*>(item);
    if(!treeNode)
        return;
    switch (treeNode->moduleType) {
    case ModuleType::FMU:{
        FMU_info fmu_info;
        fmu_info.name = treeNode->text();
        m_fumInfo.insert(treeNode->uuid,fmu_info);
        break;
    }
    default:
        break;
    }

    for(int row=0;row<item->rowCount();row++){
        for(int column=0;column<item->columnCount();column++){
            QStandardItem *child = item->child(row,column);
            if(child){
                extractFMUInfoFromItem(child);
            }
        }
    }

}

QMap<QString, FMU_info> ConditionCommon::getFumInfo() const
{
    return m_fumInfo;
}

ConditionCommon::ConditionCommon()
{
    m_model = nullptr;
}
