#include "data_factory.h"
#include <QString>
#include <QDir>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QDomDocument>
#include <QTextStream>
#include <QRegularExpression>
#include "beat_arrow.h"

DataFactory *DataFactory::_dataFactory = nullptr;
QMutex DataFactory::_mutex;

DataFactory::DataFactory():
    _work_space("")
{
}

DataFactory *DataFactory::get_instance()
{
    if(_dataFactory == nullptr)
    {
        QMutexLocker locker(&_mutex);
        if(_dataFactory == nullptr)
        {
            _dataFactory = new DataFactory();
        }
    }
    return _dataFactory;
}

void DataFactory::setWorkspace(const QString &workspace)
{
    _work_space = workspace;
}

bool DataFactory::removeAllFiles(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::Dirs);
    QFileInfoList fileFolders = dir.entryInfoList();
    bool all_remove_ok = true;
    for (auto fileInfoIter : fileFolders)
    {
        QString folderName = fileInfoIter.fileName();
        if (!fileInfoIter.isDir() || folderName == "." || folderName == ".." || folderName == "cmdFuc" || folderName == "tapFuc")
            continue;
        QDir tmp_dir(fileInfoIter.filePath());
        bool ok = tmp_dir.removeRecursively();
        if(!ok)
        {
            all_remove_ok = false;
            break;
        }
    }

    if(!all_remove_ok)
    {
        return false;
    }
    return true;
}

bool DataFactory::save(VVTreeItem *root)
{
    _debugInfos.clear();
    QDir dir;
    QString path = _work_space + QString("/%1/").arg(VV_DIR_NAME);


    //![0] 先Check一下，如果可以保存成功，才进行下面的删除
    for (int i = 0; i < root->childCount(); ++i)
    {
        QString folderPath = path + root->child(i)->data(0).toString();
        //创建文件夹
        if (!dir.exists(folderPath))
        {
            if ( !dir.mkpath(folderPath) )
                return false;
        }

        //创建文件
        for (int j = 0; j < root->child(i)->childCount(); ++j)
        {
            if (!writeFilePreCheck(root->child(i)->child(j), folderPath))
            {
                VVDir*  vvDir  = static_cast<VVDir*>(root->child(i)->ptr());
                VVFile* vvFile = static_cast<VVFile*>(root->child(i)->child(j)->ptr());
                _debugInfos.append(QString("%1\\%2").arg(vvDir->_dir_name).arg(vvFile->_file_name));
                return false;
            }
        }
    }
    //! [0]


    //! [1]删除之前的文件，
    bool ok = removeAllFiles(path);
    if(!ok)
    {
        return false;
    }
    //! [1]

    //! 开始保存新文件
    for (int i = 0; i < root->childCount(); ++i)
    {
        QString folderPath = path + root->child(i)->data(0).toString();
        //创建文件夹
        if (!dir.exists(folderPath))
        {
            if ( !dir.mkpath(folderPath) )
                return false;
        }

        //创建文件
        for (int j = 0; j < root->child(i)->childCount(); ++j)
        {
#ifdef USE_FIXED_FILE
            //! 写入小文件（即不展开持续节拍的文件）
            if (!writeFile(root->child(i)->child(j), folderPath))
            {
                VVDir*  vvDir  = static_cast<VVDir*>(root->child(i)->ptr());
                VVFile* vvFile = static_cast<VVFile*>(root->child(i)->child(j)->ptr());
                _debugInfos.append(QString("%1%2\\%3%4").arg(QStringLiteral("文件：")).arg(vvDir->_dir_name).arg(vvFile->_file_name).arg(QStringLiteral("中存在空白项，请检查！")));
                return false;
            }

#else
#endif

            //! 写入大文件（即展开持续节拍的文件）
            if (!writeFileFixed(root->child(i)->child(j), folderPath))
            {
                VVDir*  vvDir  = static_cast<VVDir*>(root->child(i)->ptr());
                VVFile* vvFile = static_cast<VVFile*>(root->child(i)->child(j)->ptr());
                _debugInfos.append(QString("%1%2\\%3%4").arg(QStringLiteral("文件：")).arg(vvDir->_dir_name).arg(vvFile->_file_name).arg(QStringLiteral("中存在空白项，请检查！")));
                return false;
            }
        }
    }
    return true;
}

void DataFactory::setDataSource(VVTreeItem *vvTreeItem)
{
    _vvTreeItem = vvTreeItem;
}

QString DataFactory::getDebugInfo()
{
    return _debugInfos.join("\n");
}

bool DataFactory::writeFilePreCheck(VVTreeItem* file, QString folder_path)
{
    VVFile *vvFile = static_cast<VVFile *>(file->ptr());
    for(int i = 0; i < vvFile->_beats_count; ++i)
    {
        VVBeat* vvBeat = vvFile->_beats.at(i);
        for(int j = 0; j < vvBeat->_commands_count; ++j)
        {
            VVCommand* vvCommand = vvBeat->_commands.at(j);
            if((vvCommand->_msg_name.isEmpty()
                || vvCommand->_msg_id.isEmpty()
                || vvCommand->_sig_name.isEmpty()
                || vvCommand->_sig_id.isEmpty())
              &&
              (vvCommand->_command_type_str == "SET"
                || vvCommand->_command_type_str == "CHECK"
                || vvCommand->_command_type_str == "SVPC_CHECK"
                || vvCommand->_command_type_str == "COMBINATION"
                || vvCommand->_command_type_str == "PUSH_INTO_FIFO"))
            {
                return false;
            }
        }
    }
    return true;
}

bool DataFactory::writeFile(VVTreeItem *file, QString folder_path)
{
    VVFile *vvFile = static_cast<VVFile *>(file->ptr());
    for(int i = 0; i < vvFile->_beats_count; ++i)
    {
        VVBeat* vvBeat = vvFile->_beats.at(i);
        for(int j = 0; j < vvBeat->_commands_count; ++j)
        {
            VVCommand* vvCommand = vvBeat->_commands.at(j);
            if((vvCommand->_msg_name.isEmpty()
                || vvCommand->_msg_id.isEmpty()
                || vvCommand->_sig_name.isEmpty()
                || vvCommand->_sig_id.isEmpty())
              &&
              (vvCommand->_command_type_str == "SET"
                || vvCommand->_command_type_str == "CHECK"
                || vvCommand->_command_type_str == "SVPC_CHECK"
                || vvCommand->_command_type_str == "COMBINATION"
                || vvCommand->_command_type_str == "PUSH_INTO_FIFO"))
            {
                return false;
            }
        }
    }

    QDomDocument doc;
    QString strHeader("version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild( doc.createProcessingInstruction("xml", strHeader) );

    QDomElement rootNode = doc.createElement("root");
    doc.appendChild(rootNode);

    QDomElement stepTime = doc.createElement("stepTime");
    QDomText stepTime_text = doc.createTextNode(vvFile->_time_interval);//节拍时间间隔（调度周期）
    stepTime.appendChild(stepTime_text);

    QDomElement circleExecute = doc.createElement("circleExecute");
    QDomText circleExecute_text = doc.createTextNode(QString::number(vvFile->_num_of_exe));//执行次数
    circleExecute.appendChild(circleExecute_text);

    QDomElement basedType = doc.createElement("basedType");
    QDomText basedType_text = doc.createTextNode(QString::number(vvFile->_type));//类型
    basedType.appendChild(basedType_text);

    QDomElement general = doc.createElement("general");
    general.appendChild(stepTime);
    general.appendChild(circleExecute);
    general.appendChild(basedType);//其实已经没有用了，这里将类型放到了每一个节拍里面

    QDomElement scripFile = doc.createElement("scripFile");
    scripFile.appendChild(general);
    rootNode.appendChild(scripFile);

    QDomElement testCaseNode = doc.createElement("testCase");
    testCaseNode.setAttribute("type", "Object");
    rootNode.appendChild(testCaseNode);

    QDomElement initDataNode = doc.createElement("InitalData");
    initDataNode.setAttribute("type", "Array");
    testCaseNode.appendChild(initDataNode);

    //!说明：之前与caseData同级的还有“InitalData”节点，即预处理节点，经过询问目前及将来不再使用

    // 将节拍信息写到XML中
    QDomElement caseDataNode = doc.createElement("caseData");
    caseDataNode.setAttribute("type", "Array");
    testCaseNode.appendChild(caseDataNode);

    appendBeatsNode(caseDataNode, vvFile->_beats);

    // 打开文件保存文件
    QString filePath = folder_path + "/" + VV_FILE_PRE + vvFile->_file_name + "_fixed.xml";

    QFile fileSave(filePath);
    if( !fileSave.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

    QTextStream textStream(&fileSave);
    textStream.setCodec("utf-8");
    doc.save(textStream,4,QDomNode::EncodingFromTextStream);

    fileSave.flush();
    fileSave.close();

    return true;
}

/** 对数据进行修正 **/
void DataFactory::FixBeatInfo(QVector<VVBeat> &dest_beats_list, const QVector<VVBeat *> &beat_info)
{
    QList<int> dest_startbeat_num_list;

    for (int i = 0; i < beat_info.size(); i++)
    {
        VVBeat* beat = beat_info.at(i);

        //新建VVBeat，将其加入到目标list中
        VVBeat new_beat;
        new_beat._beat_number   = beat->_beat_number;
        new_beat._description   = beat->_description;
        new_beat._time_interval = beat->_time_interval;
        new_beat._type          = beat->_type;

        if(!dest_startbeat_num_list.contains(static_cast<int>(beat->_beat_number)))
        {   //! 添加现有的节拍 ①
            dest_startbeat_num_list.append(static_cast<int>(beat->_beat_number));
            dest_beats_list.append(new_beat);
        }
        else
        {   //! 在新添加的未来的节拍里，添加具体的数据
            qDebug() << "----------Need add old info----------" << endl;
            /*
            int index = dest_startbeat_num_list.indexOf(static_cast<int>(beat->startBeatNum_));
            dest_beats_list.at(index)->beatName_ = beat->beatName_;                 //info 1 节拍名称
            dest_beats_list.at(index)->startBeatNum_ = beat->startBeatNum_;         //info 2 开始节拍
            dest_beats_list.at(index)->startBeatTime_ = beat->startBeatTime_;       //info 3 开始时间
            dest_beats_list.at(index)->continuousBeat_ = beat->continuousBeat_;     //info 4 持续节拍
            dest_beats_list.at(index)->beatIntervalTime_ = beat->beatIntervalTime_; //info 5 节拍时间间隔
            dest_beats_list.at(index)->remark_ = beat->remark_;                     //info 6 备注
            dest_beats_list.at(index)->commandName_ = beat->commandName_;           //info 7 命令名
            dest_beats_list.at(index)->strCommandName_ = beat->strCommandName_;     //info 8 命令名的string
            dest_beats_list.at(index)->comands_ << beat->comands_; //TODO.这里需要将持续节拍置为1
            dest_beats_list.at(index)->tapFunctionID_ = beat->tapFunctionID_;
            */
        }

        QVector<VVCommand*> commands_list = beat->_commands;
        for(int j = 0; j < commands_list.size(); j ++)
        {
            VVCommand* command = commands_list.at(j);
            //! [0] CHECK命令超过100拍特殊处理
            if(command->_command_type_str == "CHECK" && command->_continuous_beat > 100)
            {
                //! 更新第一次插入的节拍 ① 里的命令
                int index = dest_startbeat_num_list.indexOf(beat->_beat_number);
                VVCommand* new_command = new VVCommand();
                *new_command = *command;
                dest_beats_list[index]._commands << new_command;
                continue;
            }
            //! [0]

            int need_add_number = command->_continuous_beat - 1;

            //! 更新第一次插入的节拍 ① 里的命令
            int index = dest_startbeat_num_list.indexOf(beat->_beat_number);
            VVCommand* new_command = new VVCommand();
            *new_command = *command;
            new_command->_continuous_beat = 1;
            dest_beats_list[index]._commands << new_command;

            for(int k = 0; k < need_add_number; k ++)
            {
                VVCommand* new_command = new VVCommand();
                *new_command = *command;
                new_command->_continuous_beat = 1;
                int new_startbeat = beat->_beat_number + k + 1;
                if(!dest_startbeat_num_list.contains(new_startbeat))
                {   //! 添加未来的节拍，只写了节拍的：开始节拍一个属性
                    VVBeat need_add_beat;
                    need_add_beat._beat_number = new_startbeat;
                    need_add_beat._commands << new_command;
                    dest_beats_list.append(need_add_beat);
                    dest_startbeat_num_list.append(new_startbeat);
                }
                else
                {   //! 更新现有的节拍里的命令
                    int index = dest_startbeat_num_list.indexOf(new_startbeat);
                    dest_beats_list[index]._commands << new_command;
                }
            }
            qDebug() << "process One Command OK" << endl;
        }
        qDebug() << "process One Beat OK" << endl;
    }

    qDebug() << "process All Info OK" << endl;
}

/** 经过修正后的数据的写入 **/
bool DataFactory::writeFileFixed(VVTreeItem *file, QString folder_path)
{
    VVFile *vvFile = static_cast<VVFile *>(file->ptr());
    for(int i = 0; i < vvFile->_beats_count; ++i)
    {
        VVBeat* vvBeat = vvFile->_beats.at(i);
        for(int j = 0; j < vvBeat->_commands_count; ++j)
        {
            VVCommand* vvCommand = vvBeat->_commands.at(j);
            if((vvCommand->_msg_name.isEmpty()
                || vvCommand->_msg_id.isEmpty()
                || vvCommand->_sig_name.isEmpty()
                || vvCommand->_sig_id.isEmpty())
              &&
              (vvCommand->_command_type_str == "SET"
                || vvCommand->_command_type_str == "CHECK"
                || vvCommand->_command_type_str == "SVPC_CHECK"
                || vvCommand->_command_type_str == "COMBINATION"
                || vvCommand->_command_type_str == "PUSH_INTO_FIFO"))
            {
                return false;
            }
        }
    }

    QDomDocument doc;
    QString strHeader("version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild( doc.createProcessingInstruction("xml", strHeader) );

    QDomElement rootNode = doc.createElement("root");
    doc.appendChild(rootNode);

    QDomElement stepTime = doc.createElement("stepTime");
    QDomText stepTime_text = doc.createTextNode(vvFile->_time_interval);
    stepTime.appendChild(stepTime_text);

    QDomElement circleExecute = doc.createElement("circleExecute");
    QDomText circleExecute_text = doc.createTextNode(QString::number(vvFile->_num_of_exe));
    circleExecute.appendChild(circleExecute_text);

    QDomElement basedType = doc.createElement("basedType");
    QDomText basedType_text = doc.createTextNode(QString::number(vvFile->_type));//类型
    basedType.appendChild(basedType_text);

    QDomElement general = doc.createElement("general");
    general.appendChild(stepTime);
    general.appendChild(circleExecute);
    general.appendChild(basedType);//其实已经没有用了，这里将类型放到了每一个节拍里面

    QDomElement scripFile = doc.createElement("scripFile");
    scripFile.appendChild(general);
    rootNode.appendChild(scripFile);

    QDomElement testCaseNode = doc.createElement("testCase");
    testCaseNode.setAttribute("type", "Object");
    rootNode.appendChild(testCaseNode);

    QDomElement initDataNode = doc.createElement("InitalData");
    initDataNode.setAttribute("type", "Array");
    testCaseNode.appendChild(initDataNode);

    //!说明：之前与caseData同级的还有“InitalData”节点，即预处理节点，经过询问目前及将来不再使用

    // 将节拍信息写到XML中
    QDomElement caseDataNode = doc.createElement("caseData");
    caseDataNode.setAttribute("type", "Array");
    testCaseNode.appendChild(caseDataNode);

    //! 将节拍信息写到XML中（修正后的）
    //! fixed_beats其实是展开后的文件
    QVector<VVBeat> fixed_beats;
    FixBeatInfo(fixed_beats, vvFile->_beats);

    /** 这两行，对树的数据源作了修改，会导致设置了持续节拍后，点击保存，然后不点击左侧树刷新右侧树时，操作右侧树，直接崩溃（因为 void* 地址已变）
    vvFile->_beats = fixed_beat_list;
    vvFile->_beats_count = vvFile->_beats.size();
    appendBeatsNode(caseDataNode, vvFile->_beats);
    **/

    appendBeatsNode(caseDataNode, fixed_beats);


    // 打开文件保存文件
    QString filePath = folder_path + "/" + "testCase-" + vvFile->_file_name + ".xml";

    QFile fileSave(filePath);
    if(!fileSave.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream textStream(&fileSave);
    textStream.setCodec("utf-8");
    doc.save(textStream,4,QDomNode::EncodingFromTextStream);

    fileSave.flush();
    fileSave.close();

    return true;
}

void DataFactory::appendBeatsNode(QDomElement &node, const QVector<VVBeat> &beat_list)
{
    QDomDocument doc;

    for (auto &beatIter : beat_list)
    {
        QDomElement rowNode = doc.createElement("row");
        rowNode.setAttribute("type", "Object");

        int cycle; QString unit;
        BeatArrow::ConvertTimeStr2ValueAndUnit(beatIter._time_interval, cycle, unit);
        QString rightStr = QString::number(beatIter._beat_number * cycle) + unit;

        appendChildNode(rowNode, "currentStep", QString::number(beatIter._beat_number));
        appendChildNode(rowNode, "description", beatIter._description);

        appendChildNode(rowNode, "startTime", rightStr);
        //! 说明：将name去掉，化繁为简，直接index就是名字（基于周期），（基于调度周期的话，直接根据所填index进行时间显示）
        appendChildNode(rowNode, "beatType", beatIter._type);
        //! 说明：将isdelay去掉，这里是延迟的概念，原来VVEditor也没有设置的地方
        // 添加命令
        appendStepCmdNode(rowNode, beatIter);
        //! 说明：将tapFunctionID去掉，这里是节拍集的概念，后续设计新的节拍集
        node.appendChild(rowNode);

    }
}

void DataFactory::appendBeatsNode(QDomElement &node, const QVector<VVBeat *> &beat_list)
{
    QDomDocument doc;

    for (auto &beatIter : beat_list)
    {
        QDomElement rowNode = doc.createElement("row");
        rowNode.setAttribute("type", "Object");

        appendChildNode(rowNode, "currentStep", QString::number(beatIter->_beat_number));
        //! 说明：将description去掉，日常操作中这个字段从未填写过
        appendChildNode(rowNode, "description", beatIter->_description);
        //! 说明：将name去掉，化繁为简，直接index就是名字（基于周期），（基于调度周期的话，直接根据所填index进行时间显示）
        appendChildNode(rowNode, "beatType", beatIter->_type);
        //! 说明：将isdelay去掉，这里是延迟的概念，原来VVEditor也没有设置的地方
        // 添加命令
        appendStepCmdNode(rowNode, beatIter);
        //! 说明：将tapFunctionID去掉，这里是节拍集的概念，后续设计新的节拍集
        node.appendChild(rowNode);

    }
}

void DataFactory::appendStepCmdNode(QDomElement &beatNode, const VVBeat& beat)
{
    QDomDocument doc;
    QDomElement stepCmdNode = doc.createElement("stepCmd");
    stepCmdNode.setAttribute("type", "Array");
    beatNode.appendChild(stepCmdNode);

    for (auto comIter : beat._commands)
    {
        QDomElement rowNode = doc.createElement("row");
        rowNode.setAttribute("type", "Object");


        //! 说明：去掉cmdFunctionID
        appendChildNode(rowNode, "cmdType", comIter->_command_type_str);
        ///RESET
        appendChildNode(rowNode,"expression",comIter->_condition_expression);
        QDomElement targetList = doc.createElement("targetList");
        for(auto targetInfo : comIter->_target_list)
        {
            QDomElement target = doc.createElement(targetInfo->type);
            target.setAttribute("name",targetInfo->name);
            target.setAttribute("path",targetInfo->path);
            target.setAttribute("uuid",targetInfo->uuid);
            target.setAttribute("parentUuid",targetInfo->parentUuid);
            target.setAttribute("msgIndex",targetInfo->msgIndex);
            targetList.appendChild(target);
        }
        rowNode.appendChild(targetList);
        //! 新加：字节偏移&字内偏移
        appendChildNode(rowNode, "offset", comIter->_offset);
        appendChildNode(rowNode, "wordOffset", comIter->_word_offset);
        ///SVPC_CHECK:
        appendChildNode(rowNode, "dataLength", comIter->_data_length);
        appendChildNode(rowNode, "dataType", comIter->_data_type);
        appendChildNode(rowNode, "dataValue", comIter->_data_value);
        appendChildNode(rowNode, "dataTypeR", comIter->_data_type_r);
        //! 说明：去掉device
        appendChildNode(rowNode, "groupID", comIter->_msg_id);
        appendChildNode(rowNode, "groupname", comIter->_msg_name);
        appendChildNode(rowNode, "signalID", comIter->_sig_id);
        appendChildNode(rowNode, "signalName", comIter->_sig_name);
        QString indexStr = comIter->_data_indexs;
        indexStr.replace("B", "{block}");
        appendChildNode(rowNode, "dataIndex", indexStr);
        QString indexStrR = comIter->_data_indexs_r;
        indexStrR.replace("B", "{block}");
        appendChildNode(rowNode, "dataIndexR", indexStrR);
        //! 说明：去掉Alias
        appendChildNode(rowNode, "PhysicalAddress", comIter->_physical_address);
        appendChildNode(rowNode,"svpcBegin",comIter->_svpc_check_begin_offset);
        appendChildNode(rowNode,"svpcEnd",comIter->_svpc_check_end_offset);
        ///CHECK
        appendChildNode(rowNode, "comparisonOperator", comIter->_operator);
        appendChildNode(rowNode,"continuanceBeats",comIter->_continuous_beat);


        stepCmdNode.appendChild(rowNode);
    }
}

void DataFactory::appendStepCmdNode(QDomElement &beatNode, VVBeat *beat)
{
    QDomDocument doc;
    QDomElement stepCmdNode = doc.createElement("stepCmd");
    stepCmdNode.setAttribute("type", "Array");
    beatNode.appendChild(stepCmdNode);

    for (auto comIter : beat->_commands)
    {
        QDomElement rowNode = doc.createElement("row");
        rowNode.setAttribute("type", "Object");


        //! 说明：去掉cmdFunctionID
        appendChildNode(rowNode, "cmdType", comIter->_command_type_str);
        ///RESET
        appendChildNode(rowNode,"expression",comIter->_condition_expression);
        QDomElement targetList = doc.createElement("targetList");
        for(auto targetInfo : comIter->_target_list)
        {
            QDomElement target = doc.createElement(targetInfo->type);
            target.setAttribute("name",targetInfo->name);
            target.setAttribute("path",targetInfo->path);
            target.setAttribute("uuid",targetInfo->uuid);
            target.setAttribute("parentUuid",targetInfo->parentUuid);
            target.setAttribute("msgIndex",targetInfo->msgIndex);
            targetList.appendChild(target);
        }
        rowNode.appendChild(targetList);
        //! 新加：字节偏移&字内偏移
        appendChildNode(rowNode, "offset", comIter->_offset);
        appendChildNode(rowNode, "wordOffset", comIter->_word_offset);
        ///SVPC_CHECK:
        appendChildNode(rowNode, "dataLength", comIter->_data_length);
        appendChildNode(rowNode, "dataType", comIter->_data_type);
        appendChildNode(rowNode, "dataValue", comIter->_data_value);
        appendChildNode(rowNode, "dataTypeR", comIter->_data_type_r);
        //! 说明：去掉device
        appendChildNode(rowNode, "groupID", comIter->_msg_id);
        appendChildNode(rowNode, "groupname", comIter->_msg_name);
        appendChildNode(rowNode, "signalID", comIter->_sig_id);
        appendChildNode(rowNode, "signalName", comIter->_sig_name);
        QString indexStr = comIter->_data_indexs;
        indexStr.replace("B", "{block}");
        appendChildNode(rowNode, "dataIndex", indexStr);
        QString indexStrR = comIter->_data_indexs_r;
        indexStrR.replace("B", "{block}");
        appendChildNode(rowNode, "dataIndexR", indexStrR);
        //! 说明：去掉Alias
        appendChildNode(rowNode, "PhysicalAddress", comIter->_physical_address);
        appendChildNode(rowNode,"svpcBegin",comIter->_svpc_check_begin_offset);
        appendChildNode(rowNode,"svpcEnd",comIter->_svpc_check_end_offset);
        ///CHECK
        appendChildNode(rowNode, "comparisonOperator", comIter->_operator);
        appendChildNode(rowNode,"continuanceBeats",comIter->_continuous_beat);


        stepCmdNode.appendChild(rowNode);
    }
}

void DataFactory::appendChildNode(QDomElement &node, QString nodeName, QVariant nodeValue)
{
    QDomDocument doc;

    QDomElement childNode = doc.createElement(nodeName);

    QDomText textFileName = doc.createTextNode(nodeValue.toString());
    childNode.appendChild(textFileName);

    node.appendChild(childNode);
}

QVector<VVDir*> DataFactory::init()
{

    QString folderPath = _work_space + "/testCase";

    QDir dir(folderPath);

    dir.setFilter(QDir::Dirs);
    QFileInfoList fileFolders = dir.entryInfoList();

    QVector<VVDir *> vvVec;
    for (auto fileInfoIter : fileFolders)
    {
        QString folderName = fileInfoIter.fileName();
        if (!fileInfoIter.isDir() || folderName == "." || folderName == ".." || folderName == "cmdFuc" || folderName == "tapFuc")
            continue;

        VVDir *vvDir = new VVDir();
        vvDir->_dir_name = folderName;
        vvVec.append(vvDir);


        // 读取xml文件
        dir.setPath(folderPath + "/" + folderName);
        dir.setFilter(QDir::Files);
        QStringList nameFilters;
        nameFilters << "*.xml";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

        for (int i=0; i<files.count(); i++)
        {
            QString file_path = folderPath + "/" + folderName + "/" + files.at(i);

            //过滤下_fixed后缀文件
#ifdef USE_FIXED_FILE
            if(!QFileInfo(file_path).fileName().endsWith("_fixed.xml"))
#else
            if(QFileInfo(file_path).fileName().endsWith("_fixed.xml"))
#endif
            {
                qDebug() << QString("%1%2").arg(QStringLiteral("过滤掉------>")).arg(file_path) << endl;
                continue;
            }

            VVFile *tempFile = new VVFile();
            if ( !initFile(file_path, tempFile) )
                continue;

            vvDir->_files.append(tempFile);
        }

    }
    return vvVec;
}

bool DataFactory::initFile(const QString &file_path, VVFile *file_data)
{
    QFile file(file_path);
    if ( !file.open( QIODevice::ReadOnly ) )
        return false;

    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }

    //VVFile tempFile;
    QString fileName = file_path.split("/").last();
    if (file_path.split("-").count() < 2)
        return false;

    QString fileAllName = fileName.remove("testCase-");//fileName.split("-").last() --> testCase-
    fileName = fileAllName.left(fileAllName.count()-4);

#ifdef USE_FIXED_FILE
    fileName.remove("_fixed");
#else
#endif
    file_data->_file_name = fileName;


    QDomElement rootElement = doc.documentElement();

    QDomElement stepTime = rootElement.firstChildElement("scripFile").firstChildElement("general").firstChildElement("stepTime");
    file_data->_time_interval = stepTime.isNull() ? "15ms" : stepTime.text();
    if(QRegularExpression("^([1-9]\\d*(\\.\\d*)?|0\\.\\d*[1-9]\\d*|0)(?:\\s*)$").match(file_data->_time_interval).hasMatch()){
        file_data->_time_interval = file_data->_time_interval + "s";
    }
    QDomElement circleExecute = rootElement.firstChildElement("scripFile").firstChildElement("general").firstChildElement("circleExecute");
    file_data->_num_of_exe = circleExecute.isNull() ? 1 : circleExecute.text().toInt();

    //! 去掉读取初始化节点的信息

    //读取正常节点的信息
    QDomElement caseDataNode = rootElement.firstChildElement("testCase").firstChildElement("caseData");
    initBeat(caseDataNode, file_data->_beats);
    file_data->_beats_count = file_data->_beats.count();
    for(int i = 0; i < file_data->_beats_count; ++i)
    {
        file_data->_beats.at(i)->_time_interval = file_data->_time_interval;
    }

    file.close();

    return true;
}

void DataFactory::initBeat(const QDomElement &beats_node, QVector<VVBeat *> &beat_list)
{
    QDomNodeList beatNodes = beats_node.childNodes();

    for (int i=0; i<beatNodes.count(); i++)
    {
        VVBeat *tempBeat = new VVBeat();
        QDomElement beatNode = beatNodes.at(i).toElement();

        QString currentStep = beatNode.firstChildElement("currentStep").text();
        int beatType        = beatNode.firstChildElement("beatType").text().toInt();
        QString description = beatNode.firstChildElement("description").text();


        tempBeat->_beat_number = currentStep.toInt();
        tempBeat->_type  = beatType;
        tempBeat->_description = description;

        QString beatCom = initCommand(beatNode.firstChildElement("stepCmd"), tempBeat);

        beat_list << tempBeat;
    }
}

QString DataFactory::initCommand(const QDomElement &stepNode, VVBeat *beat)
{
    QString beatCom;

    QDomNodeList comNodes = stepNode.childNodes();
    for (int i=0; i<comNodes.count(); i++)
    {
        VVCommand *tempCom = new VVCommand;
        QDomElement comNode = comNodes.at(i).toElement();
        tempCom->_command_type_str = comNode.firstChildElement("cmdType").text();       //命令类型
        tempCom->_msg_name = comNode.firstChildElement("groupname").text();             //ICD name
        tempCom->_msg_id = comNode.firstChildElement("groupID").text();                 //ICD id
        tempCom->_sig_name = comNode.firstChildElement("signalName").text();            //signal name
        tempCom->_sig_id = comNode.firstChildElement("signalID").text();                //signal id
        tempCom->_data_value = comNode.firstChildElement("dataValue").text();           //signal data value
        tempCom->_data_length = comNode.firstChildElement("dataLength").text().toInt(); //signal data length
        tempCom->_data_type = comNode.firstChildElement("dataType").text();             //signal data type
        tempCom->_data_type_r = comNode.firstChildElement("dataTypeR").text();          //signal data type R
        QString indexStr = comNode.firstChildElement("dataIndex").text();               //signal data index
        indexStr.replace("{block}", "B");
        tempCom->_data_indexs = indexStr;
        QString indexStrR = comNode.firstChildElement("dataIndexR").text();             //signal data index
        indexStrR.replace("{block}", "B");
        if(indexStrR == "")
        {
            indexStrR = "0";
        }
        tempCom->_data_indexs_r = indexStrR;
        //! 新加：字节偏移&字内偏移
        tempCom->_offset = comNode.firstChildElement("offset").text();
        tempCom->_word_offset = comNode.firstChildElement("wordOffset").text();

        tempCom->_physical_address = comNode.firstChildElement("PhysicalAddress").text();           //物理地址
        tempCom->_svpc_check_begin_offset = comNode.firstChildElement("svpcBegin").text().toInt();  //SVPC校验起始字节偏移
        tempCom->_svpc_check_end_offset = comNode.firstChildElement("svpcEnd").text().toInt();      //SVPC校验结束字节偏移
        QString continuousBeats = comNode.firstChildElement("continuanceBeats").text();                 //该命令的持续节拍

        /** 这里是为了兼容之前在旧VVEditor上修改的一版添加了持续节拍，
         *  使用_fixed.xml文件进行记录，使用了continuousBeat
         *  原始的脚本文件里面用过continuanceBeats（拼写错误）为了兼容执行端，暂不做更改
         *  所以这里进行修正
         **/
        if(continuousBeats.isEmpty())
            continuousBeats = comNode.firstChildElement("continuousBeat").text();

        tempCom->_continuous_beat = continuousBeats.toInt();

        tempCom->_operator = comNode.firstChildElement("comparisonOperator").text();                //比较符

        tempCom->_condition_expression = comNode.firstChildElement("expression").text();            //RESET：条件表达式
        QDomElement targetElement = comNode.firstChildElement("targetList").firstChildElement();
        for(;!targetElement.isNull();targetElement=targetElement.nextSiblingElement())
        {
            TargetInfo *targetInfo = new TargetInfo();
            targetInfo->type = targetElement.tagName();
            targetInfo->name = targetElement.attribute("name");
            targetInfo->path = targetElement.attribute("path");
            targetInfo->uuid = targetElement.attribute("uuid");
            targetInfo->parentUuid = targetElement.attribute("parentUuid");
            targetInfo->msgIndex = targetElement.attribute("msgIndex");

            tempCom->_target_list.append(targetInfo);                                               //RESET：目标list
        }
        beat->_commands << tempCom;
    }
    beat->_commands_count = beat->_commands.size();

    return beatCom;
}
