#ifndef SELECTARGUMENTSCOMMON_H
#define SELECTARGUMENTSCOMMON_H

#include <QString>

struct VariableGroup
{
    QString uuid;//uuid
    QString type;//����
    QString name;//����
    QStringList nameList;
    QString blockUuid;//blockUuid
    QString path;//·��
    //FUM
    //uuid:fum��uuid
    //type="fum"
    //name:fum������
    //nameList:fum�˿��б�
    //blockUuid:fum����block��uuid

    //dataSource��dataOutPuter
    //uuid:dataSource��dataOutPuter����block��uuid
    //type="dataSource"��"dataOutPuter"
    //name:dataSource��dataOutPuter������
    //nameList:ICD�����б�
    //blockUuid:dataSource��dataOutPuter����block��uuid
};

#endif // SELECTARGUMENTSCOMMON_H
