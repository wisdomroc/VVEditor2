/***********************************************************************
* @filename icd_data.h
* @brief    提供对当前WRP工作空间中所有ICD信息的访问途经
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef ICDDATA_H
#define ICDDATA_H

#include <QList>
#include "vv_typedef.h"

#include "ICDController.h"

class ICDData
{
public:
    ICDData(const QString& work_space);

    void initICDData();
    void setWrokSpace(const QString& work_space);

    QList<ICDInfo> getIcdsInfo();
    void *findICDInstanceByID(const QString &name);
    short getICDBuffers(void * icdHandle);

    QHash<QString, QString> getSigNamesHash(const QString &msgID);
    QHash<QString, QString> getSigIDsHash(const QString &msgID);
    QHash<QString, QString> getMsgNamesHash() const;
    QHash<QString, QString> getMsgIDsHash() const;

private:
    ICDController  _icdController;
    QList<ICDInfo> _icds_info;

    QStringList    _msgNameList;
    QStringList    _msgIDList;
    QStringList    _sigNameList;
    QStringList    _sigIDList;

    QString        _work_space;
};

#endif // ICDDATA_H
