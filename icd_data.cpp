#include "icd_data.h"

ICDData::ICDData(const QString &work_space)
{
    setWrokSpace(work_space);


}

void ICDData::initICDData()
{
    QStringList msgNames = _icdController.getAllPackageNameList();
    for (auto msgIter : msgNames)
    {
        ICDInfo tempIcd;

        void *shmPackage = _icdController.findICDInstanceByID(msgIter);
        if (shmPackage == nullptr)
            continue;

        auto pack = _icdController.findICDDefinationByID(msgIter);
        if (pack == nullptr)
            continue;

        tempIcd.messageID_ =  pack->msgID;
        tempIcd.messageName_ = msgIter;
        tempIcd.dataPkgSummary_ = pack->PkgSummary;

        QList<QSharedPointer<SignalDef> > icdSignals =  _icdController.getAllSignalsDef(shmPackage);

        for (auto sigIter : icdSignals)
        {
            SingalInfo tempSigInfo;
            tempSigInfo.signalName_ = sigIter->signalName;
            tempSigInfo.signalID_ = sigIter->signalID;
            tempSigInfo.lenght_ = sigIter->length;
            tempSigInfo.offset_ = sigIter->offset;
            tempSigInfo.wordOffset_ = sigIter->wordOffset;
            tempSigInfo.formula_ = sigIter->formula;
            tempSigInfo.strType_ = sigIter->strDataType;
            tempSigInfo.strValue_ = sigIter->stringValue;

            tempIcd.signals_ << tempSigInfo;
        }

        _icds_info << tempIcd;
    }


    qSort(_icds_info.begin(), _icds_info.end(), [](ICDInfo &icdInfo1, ICDInfo &icdInfo2){
        if(icdInfo1.messageName_ != "" && icdInfo2.messageName_ != "")
            return icdInfo1.messageName_ < icdInfo2.messageName_;
        else
            return icdInfo1.messageID_ < icdInfo2.messageID_;
    });



    for (int i = 0; i < _icds_info.size(); i++)
    {
        _msgNameList.append(_icds_info.at(i).messageName_);
        _msgIDList.append(_icds_info.at(i).messageID_);
    }
}

void ICDData::setWrokSpace(const QString& work_space)
{
    _work_space = work_space;

    QString icdPath = _work_space + "/metaData";

    if ( !_icdController.setControllerDir(icdPath) )
        return;

    _icdController.initIcdData();
    initICDData();
}

QList<ICDInfo> ICDData::getIcdsInfo()
{
    return _icds_info;
}

void* ICDData::findICDInstanceByID(const QString& name)
{
    return _icdController.findICDInstanceByID(name);
}

short ICDData::getICDBuffers(void *icdHandle)
{
    return _icdController.getICDBuffers(icdHandle);
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
