/***********************************************************************
* @filename vv_typedef.h
* @brief    从旧的定义文件继承而来，修改了部分定义，新添了部分定义
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef VV_TYPEDEF_H
#define VV_TYPEDEF_H

#include <QVector>
#include <sstream>

const QString VV_DIR_NAME           = "testCase";
const QString VV_FILE_PRE           = "testCase-";
const int     MAX_BEAT              = 99999;
const int     MAX_CONTINUOUS_BEAT   = 99999;
const double  MAX_DOUBLE             = 99999.99;

struct TargetInfo {
    QString type;
    QString parentUuid;
    QString uuid;       //id
    QString name;       //名称
    QString path;       //路径
    QString msgIndex;   //消息包下标

    TargetInfo()
    {
        type = "";
        parentUuid = "";
        uuid = "";
        name = "";
        path = "";
        msgIndex = "";
    }
};

struct VVCommand {
    QString                 _command_type_str;          //命令类型string：

    QString                 _time_interval;             //时间间隔(调度周期)：仅当基于时间时有效

    QString                 _msg_name;                  //MSG_NAME
    QString                 _msg_id;                    //MSG_ID
    QString                 _sig_name;                  //SIGNAL_NAME
    QString                 _sig_id;                    //SIGNAL_ID
    QString                 _offset;                    //字节偏移
    QString                 _word_offset;               //字内偏移
    QString                 _data_type;                 //数据类型
    QString                 _data_type_r;               //数据类型R
    int                     _data_length;               //数据长度
    QString                 _data_indexs;               //数组下标
    QString                 _data_indexs_r;             //数组下标（check后者变量的下标）

    QString                 _physical_address;          //物理地址
    QString                 _data_value;                //值
    QString                 _operator;                  //比较运算符
    int                     _continuous_beat;           //持续调度周期（持续节拍）

    int                     _svpc_check_begin_offset;   //SVPC_CHECK时有用，是起始偏移
    int                     _svpc_check_end_offset;     //SVPC_CHECK时有用，是结束偏移

    QString                 _condition_expression;      //RESET时有用，是条件配置
    QVector<TargetInfo *>   _target_list;               //RESET时有用，是选中的ICD或者FMU

    VVCommand()
    {
        _data_length             = 0;
        _continuous_beat         = 1;
        _svpc_check_begin_offset = 0;
        _svpc_check_end_offset   = 0;
    }

    VVCommand& operator=(const VVCommand &other)
    {
        _command_type_str           = other._command_type_str;

        _time_interval              = other._time_interval;

        _msg_name                   = other._msg_name;
        _msg_id                     = other._msg_id;
        _sig_name                   = other._sig_name;
        _sig_id                     = other._sig_id;
        _offset                     = other._offset;
        _word_offset                = other._word_offset;
        _data_type                  = other._data_type;
        _data_type_r                = other._data_type_r;
        _data_length                = other._data_length;
        _data_indexs                = other._data_indexs;
        _data_indexs_r              = other._data_indexs_r;

        _physical_address           = other._physical_address;
        _data_value                 = other._data_value;
        _operator                   = other._operator;
        _continuous_beat            = other._continuous_beat;

        _svpc_check_begin_offset    = other._svpc_check_begin_offset;
        _svpc_check_end_offset      = other._svpc_check_end_offset;

        _condition_expression       = other._condition_expression;

        for(int i = 0; i < other._target_list.size(); ++i)
        {
            TargetInfo* targetInfo = new TargetInfo();
            *targetInfo = *other._target_list.at(i);
            _target_list.append(targetInfo);
        }
        return *this;
    }
};

struct VVBeat{
    int                  _beat_number;      //第几个调度周期（第几个节拍）
    int                  _type;             //0:基于调度周期；1:基于时间
    int                  _commands_count;   //命令个数
    QString              _time_interval;    //时间间隔(调度周期)：仅当基于时间时有效
    QString              _description;      //描述
    QVector<VVCommand *> _commands;         //所有命令

    VVBeat(int beat_number = -1, int type = -1, QString des = ""):_beat_number(beat_number),_type(type),_description(des)
    {
        _commands_count = -1;
    }

    VVBeat& operator=(const VVBeat& other)
    {
        _beat_number    = other._beat_number;
        _type           = other._type;
        _time_interval  = other._time_interval;
        _commands_count = other._commands_count;
        _description    = other._description;
        for(int i = 0; i < other._commands.size(); ++i)
        {
            VVCommand* newVVCommand = new VVCommand();
            *newVVCommand           = *other._commands.at(i);
            _commands.append(newVVCommand);
        }
        return *this;
    }
};

struct VVFile{
    QString             _file_name;     //文件名称
    int                 _type;          //文件类型：0——基于调度周期（节拍）；1——基于时间
    QString             _time_interval; //时间间隔(调度周期)：仅当基于时间时有效
    int                 _num_of_exe;    //执行次数
    int                 _beats_count;   //节拍个数
    QVector<VVBeat *>   _beats;         //所有节拍

    VVFile()
    {
        _type        = 1;
        _num_of_exe  = 0;
        _beats_count = 0;
    }

    VVFile & operator=(const VVFile& other)
    {
        _file_name = other._file_name;
        _type = other._type;
        _time_interval = other._time_interval;
        _num_of_exe = other._num_of_exe;
        _beats_count = other._beats_count;
        for(int i = 0; i < other._beats.size(); ++i)
        {
            VVBeat* newVVBeat = new VVBeat();
            *newVVBeat        = *other._beats.at(i);
            _beats.append(newVVBeat);
        }
        return *this;
    }
};

struct VVDir{
    QString             _dir_name;  //目录名称
    QVector<VVFile *>   _files;     //所有VV脚本文件

    VVDir()
    {
    }

    VVDir& operator=(const VVDir& other)
    {
        _dir_name = other._dir_name;
        for(int i = 0; i < other._files.size(); ++i)
        {
            VVFile* newVVFile = new VVFile();
            *newVVFile = *other._files.at(i);
            _files.append(newVVFile);
        }
        return *this;
    }
};

struct SingalInfo
{
    QString signalName_;
    QString signalID_;
    QString strType_;
    QString strValue_;
    QString strAlias_;
    int     offset_ = 0;     //字节偏移
    int     wordOffset_ = 0; //字内偏移
    int     lenght_ = 0;
    QString formula_;
};

struct ICDSignal
{
    QString     messageName_;
    QString     messageID_;
    SingalInfo  signalInfo_;
};

struct ICDInfo
{
    QString             messageName_;
    QString             messageID_;
    QString             dataPkgSummary_;
    QVector<SingalInfo> signals_;
};

#endif // VV_TYPEDEF_H
