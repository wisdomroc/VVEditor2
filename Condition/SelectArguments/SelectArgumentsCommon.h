#ifndef SELECTARGUMENTSCOMMON_H
#define SELECTARGUMENTSCOMMON_H

#include <QString>

struct VariableGroup
{
    QString uuid;//uuid
    QString type;//类型
    QString name;//名称
    QStringList nameList;
    QString blockUuid;//blockUuid
    QString path;//路径
    //FUM
    //uuid:fum的uuid
    //type="fum"
    //name:fum的名称
    //nameList:fum端口列表
    //blockUuid:fum所在block的uuid

    //dataSource或dataOutPuter
    //uuid:dataSource或dataOutPuter所在block的uuid
    //type="dataSource"或"dataOutPuter"
    //name:dataSource或dataOutPuter的名称
    //nameList:ICD名称列表
    //blockUuid:dataSource或dataOutPuter所在block的uuid
};

#endif // SELECTARGUMENTSCOMMON_H
