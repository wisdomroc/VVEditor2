#ifndef VV_COMMON_H
#define VV_COMMON_H

//TODO.只是为了编译通过，将来要废弃，to obsolete

#include <QString>
#include <QSharedPointer>

//            文件夹     文件    预编译节点        正常节点   节拍     命令
enum NODETYPE{ FOLDER=1, FILEE=2, PRETREATMENT=4, NORMAL=5, BEAT=6, COMMAND=6
              ,COMSET=7, COMSETFILE=8, COMSETCOMMAND=9
              ,BEATSET=10, BEATSETFILE=11, BEATSETBEAT=12, BEATSETCOMMAND=13 };

enum VVCOMMAND{ SET=1, CHECK=2, COMBINATION=3, DELAY=4, MULTIPLE=5, DEFAULT=6,RESET=7,SVPC_CHECK};

struct TargetInfo{
    QString type;
    QString parentUuid;
    QString uuid;//id
    QString name;//名称
    QString path;//路径
    QString msgIndex;//消息包下标
};

struct SVPC_Check{
    int begin = 0;
    int end = 0;
    int continuanceBeats = 1;
};

// tree信息 // ////////////////////////////////////////////////
// 命令层的信息
struct VVCommand
{
    VVCommand(){}

    inline bool operator != (const QSharedPointer<VVCommand> othercommand_)const
    {
        return !( serialNumber_ == othercommand_->serialNumber_
                  && commandName_ == othercommand_->commandName_
                  && deviceName_ == othercommand_->deviceName_
                  && msgName_ == othercommand_->msgName_
                  && msgID_ == othercommand_->msgID_
                  && signalName_ == othercommand_->signalName_
                  && singalID_ == othercommand_->singalID_
                  && value_ == othercommand_->value_
                  && valueLenght_ == othercommand_->valueLenght_
                  && strValueType_ == othercommand_->strValueType_
                  && cmdFunctionID_ == othercommand_->cmdFunctionID_
                  && alias_ == othercommand_->alias_
                  && physicalAddress_ == othercommand_->physicalAddress_
                  && conditionExpression_ == othercommand_->conditionExpression_
                  && data_index_ == othercommand_->data_index_
                  && comparison_operator_ == othercommand_->comparison_operator_);
    }

    inline bool operator==(const int& otherSerialNumber_)const
    {
        return serialNumber_ == otherSerialNumber_;
    }

    VVCommand& operator=(const QSharedPointer<VVCommand>& other)
    {
        serialNumber_ = other->serialNumber_;
        commandName_ = other->commandName_;
        strCommandName_ = other->strCommandName_;
        deviceName_ = other->deviceName_;
        msgName_ = other->msgName_;
        msgID_ = other->msgID_;
        signalName_ = other->signalName_;
        singalID_ = other->singalID_;
        physicalAddress_ = other->physicalAddress_;
        alias_ = other->alias_;
        value_ = other->value_;
        byteOffset = other->byteOffset;
        inWordOffset = other->inWordOffset;
        valueLenght_ = other->valueLenght_;
        strValueType_ = other->strValueType_;
        cmdFunctionID_ = "NULL";
        targetType_ = other->targetType_;
        targetList_ = other->targetList_;
        conditionExpression_ = other->conditionExpression_;
        comparison_operator_ = other->comparison_operator_;
        svpcCheck_ = other->svpcCheck_;
        data_index_ = other->data_index_;
        return *this;
    }

    int serialNumber_;
    VVCOMMAND commandName_ = VVCOMMAND::SET;
    QString strCommandName_ = "SET";
    QString deviceName_;
    QString msgName_;
    QString msgID_;
    QString signalName_;
    QString singalID_;
    QString physicalAddress_;
    QString alias_;
    QString value_;
    QString byteOffset;//字节偏移
    QString inWordOffset;//字内偏移
    int valueLenght_ = 0;
    QString strValueType_;
    QString cmdFunctionID_ = "NULL";    //! whp.是否节拍集导入的？
    QString targetType_;
    QList<TargetInfo> targetList_;      //! 目标
    QString conditionExpression_;
    QString comparison_operator_;       //! 比较符

    SVPC_Check svpcCheck_;              //! svpc校验

    QString data_index_;                //! 数据索引

    int continuousBeat_ = -1;           //! 持续节拍
    QString continuousTime_;            //! 持续时间
};
// command命令list的别名
typedef QList<QSharedPointer<VVCommand>> List_VVCommand;

// 节拍层的信息
struct VVBeat
{
    VVBeat(){}

    inline bool operator != (const QSharedPointer<VVBeat> otherbeat_)const
    {
        bool command = true;
        if (otherbeat_->comands_.count() != comands_.count())
            return true;

        for (auto i = 0; i < comands_.count(); i++)
        {
            if ((*comands_.at(i)) != otherbeat_->comands_.at(i))
                return true;
        }

        return !( beatName_ == otherbeat_->beatName_
                  && startBeatNum_ == otherbeat_->startBeatNum_
                  && continuousBeat_ == otherbeat_->continuousBeat_
                  && remark_ == otherbeat_->remark_
                  && commandName_ == otherbeat_->commandName_
                  && tapFunctionID_ == otherbeat_->tapFunctionID_
                  && command);
    }

    inline bool operator==(const QString& bName)const
    {
        return beatName_ == bName;
    }

    VVBeat& operator=(const VVBeat other)
    {
        beatName_ = other.beatName_;
        startBeatNum_ = other.startBeatNum_;
        continuousBeat_ = other.continuousBeat_;
        remark_ = other.remark_;
        commandName_ = other.commandName_;
        strCommandName_ = other.strCommandName_;
        comands_ = other.comands_;
        tapFunctionID_ = other.tapFunctionID_;
    }

    VVBeat& operator=(const QSharedPointer<VVBeat> other)
    {
        beatName_ = other->beatName_;
        startBeatNum_ = other->startBeatNum_;
        continuousBeat_ = other->continuousBeat_;
        remark_ = other->remark_;
        commandName_ = other->commandName_;
        strCommandName_ = other->strCommandName_;
        tapFunctionID_ = "NULL";

        for (auto com_iter : other->comands_)
        {
            QSharedPointer<VVCommand> com(new VVCommand);
            *com = com_iter;

            comands_ << com;
        }

        return *this;
    }

    QString getViewName() const{
        if(beatName_.isEmpty()){
            return QStringLiteral("第") + QString::number(startBeatNum_) + QStringLiteral("拍") + "(" + QString::number(startBeatNum_) + ")";
        }else{
            return beatName_ + "(" + QString::number(startBeatNum_) + ")";
        }
    }

    QString beatName_;                      //! 节拍名称
    uint64_t startBeatNum_ = -1;            //! 开始节拍
    QString startBeatTime_;                 //! 开始节拍String, 带单位的字符串
    uint64_t continuousBeat_ = 1;           //! 持续节拍,其实没有用
    QString betweenLastBeatIntervalTime_;   //! 与上一节拍间隔时间String
    //QString standardBeatIntervalTime_;      //! 所有节拍间隔时间String
    QString remark_;                        //! 节拍备注
    VVCOMMAND commandName_;
    QString strCommandName_;
    List_VVCommand comands_ = List_VVCommand();
    QString tapFunctionID_ = "NULL";
};
// 节拍list的别名
typedef QList<QSharedPointer<VVBeat>> List_VVBeat;

//文件层的信息
struct VVFile
{
    VVFile(){}

    inline bool operator != (const QSharedPointer<VVFile> otherfile_)const
    {
        bool file = true;
        if (otherfile_->beats_.count() != beats_.count())
            return true;

        for (auto i = 0; i < beats_.count(); i++)
        {
            if ((*beats_.at(i)) != otherfile_->beats_.at(i))
                return true;
        }

        return !( fileName_ == otherfile_->fileName_
                  && file);
    }

    inline bool operator==(const QString& fName)const
    {
        return fileName_ == fName;
    }

   // int type_=1; //type=1:normal;  type=2:init
    QString fileName_;
    QString stepTime_;      //! 节拍时间间隔
    QString stepTimeOld_;   //whp add 2021-09-23
    int circleExecute;      //节拍循环次数，-1表示循环执行
    List_VVBeat pretreatment_beats_ = List_VVBeat();
    List_VVBeat beats_ = List_VVBeat();               //正常的beat
    List_VVBeat fixed_beats_ = List_VVBeat();           //whp add 2022-02-24,修正后的beat
};
// 文件的list的别名
typedef QList<QSharedPointer<VVFile>> List_VVFile;

// 文件夹层的信息
struct VVFolder
{
    VVFolder(){}

    inline bool operator==(const QString& fName)const
    {
        return folderName_ == fName;
    }

    inline bool operator!=(const QSharedPointer<VVFolder> otherFolder)const
    {
        if (folderName_ != otherFolder->folderName_ || files_.count() != otherFolder->files_.count())
            return true;

        for (int i=0; i<files_.count(); i++)
        {
            if ((*files_.at(i)) != otherFolder->files_.at(i))
                return true;
        }

        return false;
    }

    QString folderName_;
    List_VVFile files_ = List_VVFile();
};

// 文件夹list的别名
typedef QList<QSharedPointer<VVFolder>> List_VVFolder;

// ICD信息 // ////////////////////////////////////////////////
// 信号
struct SingalInfo
{
    QString singalName_;
    QString singalID_;
    QString strType_;
    QString strValue_;
    QString strAlias_;
    int offset_;     //字节偏移
    int wordOffset_; //字内偏移
    int lenght_;
    QString formula_;
};

// 消息
struct IcdInfo
{
    IcdInfo(){}

    inline bool operator==(const QString msgName)const
    {
        return messageName_ == msgName;
    }
public:
        static bool compare(const IcdInfo &s1, const IcdInfo &s2)
        {
            return s1.messageName_ < s2.messageName_; // This is just an example
        }

    QString messageName_;
    QString messageID_;
    QString dataPkgSummary_;
    QList<SingalInfo> singals_;
};
//ICD下的信息
struct IcdSignal
{
    QString messageName_;
    QString messageID_;
    SingalInfo signalInfo_;
};

// 命令函数集
struct CommandSet
{
    CommandSet(){}

    inline bool operator==(const QString commandSetName)const
    {
        return commandSetName_ == commandSetName;
    }

    inline bool operator!=(const QSharedPointer<CommandSet> otherCom)const
    {
        if (commandSetName_ != otherCom->commandSetName_ && comands_.count() != otherCom->comands_.count())
            return true;

        for (int i=0; i<comands_.count(); i++)
        {
            if ((*comands_.at(i)) != otherCom->comands_.at(i))
                return true;
        }

        return false;
    }

    QString commandSetName_;
    List_VVCommand comands_ = List_VVCommand();
};
// 命令集list的别名
typedef QList<QSharedPointer<CommandSet>> List_ComSet;

// 拍数函数集
struct BeatSet
{
    BeatSet(){}

    inline bool operator==(const QString& setName)const
    {
        return beatSetName_ == setName;
    }

    inline bool operator==(const BeatSet& otherSet)const
    {
        return beatSetName_ == otherSet.beatSetName_;
    }

    inline bool operator!=(const QSharedPointer<BeatSet> otherSet)const
    {
        if (beatSetName_ != otherSet->beatSetName_ || beats_.count() != otherSet->beats_.count())
            return true;

        for (int i=0; i<beats_.count(); i++)
        {
            if ( (*beats_.at(i)) != otherSet->beats_.at(i) )
                return true;
        }

        return false;
    }

    QString beatSetName_;
    List_VVBeat beats_ = List_VVBeat();
};
// 节拍集list的别名
typedef QList<QSharedPointer<BeatSet>> List_BeatSet;

Q_DECLARE_METATYPE(QSharedPointer<VVFolder>)   // vveidtor::VVFolder:  自己的类
Q_DECLARE_METATYPE(QSharedPointer<VVFile>)
Q_DECLARE_METATYPE(QSharedPointer<VVBeat>)
Q_DECLARE_METATYPE(QSharedPointer<VVCommand>)

Q_DECLARE_METATYPE(QSharedPointer<CommandSet>)
Q_DECLARE_METATYPE(QSharedPointer<BeatSet>)

Q_DECLARE_METATYPE(List_VVFolder)
Q_DECLARE_METATYPE(List_VVFile)
Q_DECLARE_METATYPE(List_VVBeat)
Q_DECLARE_METATYPE(List_VVCommand)

Q_DECLARE_METATYPE(List_ComSet)
Q_DECLARE_METATYPE(List_BeatSet)

Q_DECLARE_METATYPE(SingalInfo)
Q_DECLARE_METATYPE(IcdInfo)

#endif // VV_COMMON_H
