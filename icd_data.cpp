#include "icd_data.h"

ICDData::ICDData(const QString &work_space)
{
    setWrokSpace(work_space);


}

void ICDData::initICDData()
{

}

void ICDData::setWrokSpace(const QString& work_space)
{

}

QList<ICDInfo> ICDData::getIcdsInfo()
{
    return _icds_info;
}

void* ICDData::findICDInstanceByID(const QString& name)
{
    return nullptr;
}

short ICDData::getICDBuffers(void *icdHandle)
{
     return 0;
}

QHash<QString, QString> ICDData::getSigNamesHash(const QString &msgID)
{
    int index = -1;
    Q_UNUSED(index)
    QHash<QString, QString> hash;
    for(int i = 0; i < _icds_info.size(); ++i)
    {
        if(_icds_info.at(i).messageName_ == msgID)    //!TODO. 这里是根据的ICD_ID，但是会有问题，因为历史问题，这里暂使用ICD_NAME
        {
            for(auto value : _icds_info.at(i).signals_)
            {
                hash.insert(value.signalName_, QString("%1,%2,%3").arg(value.signalID_).arg(value.lenght_).arg(value.strType_));
            }
            return hash;
        }
    }
    return hash;
}

QHash<QString, QString> ICDData::getSigIDsHash(const QString &msgID)
{
    int index = -1;
    Q_UNUSED(index)
    QHash<QString, QString> hash;
    for(int i = 0; i < _icds_info.size(); ++i)
    {
        if(_icds_info.at(i).messageName_ == msgID)    //!TODO. 这里是根据的ICD_ID，但是会有问题，因为历史问题，这里暂使用ICD_NAME
        {
            for(auto value : _icds_info.at(i).signals_)
            {
                hash.insert(value.signalID_, QString("%1,%2,%3").arg(value.signalName_).arg(value.lenght_).arg(value.strType_));
            }
            return hash;
        }
    }
    return hash;
}

QHash<QString, QString> ICDData::getMsgNamesHash() const
{
    QHash<QString, QString> hash;
    for(int i = 0; i < _msgNameList.size(); ++i)
    {
        hash.insert(_msgNameList.at(i), _msgIDList.at(i));
    }
    return hash;
}

QHash<QString, QString> ICDData::getMsgIDsHash() const
{
    QHash<QString, QString> hash;
    for(int i = 0; i < _msgIDList.size(); ++i)
    {
        hash.insert(_msgIDList.at(i), _msgNameList.at(i));
    }
    return hash;
}
