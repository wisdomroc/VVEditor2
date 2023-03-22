#ifndef CONDITIONCOMMON_H
#define CONDITIONCOMMON_H

#include <QMap>
#include <QStandardItemModel>


struct FMU_info
{
    QString name;
};

class  ConditionCommon
{
public:
    static ConditionCommon* getInstance();

    QMap<QString, FMU_info> getFumInfo() const;

    const QStandardItemModel *getModel() const;
    void setModel(const QStandardItemModel *value);
private:
    void extractFMUInfo();
    void extractFMUInfoFromItem(QStandardItem *item);
private:
    ConditionCommon();
    static ConditionCommon *m_conditionCommon;
    QMap<QString,FMU_info> m_fumInfo;
    const QStandardItemModel * m_model;
};

#endif // CONDITIONCOMMON_H
