#include "add_command_dlg.h"
#include "ui_add_command_dlg.h"
#include "select_signal_dlg.h"

#include "WRP_Project/SingletonProject.h"
#include "icd_data.h"
#include "beat_arrow.h"


#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>

AddCommandDlg::AddCommandDlg(QSharedPointer<ICDData> icd_data, VVFile *vvFile, VVCommand* vvCommand, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCommandDlg),
    _infoList(icd_data->getIcdsInfo()),
    _vvFile(vvFile),
    _icd_data(icd_data),
    _vvCommand(vvCommand)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowTitle(QStringLiteral("添加命令"));

    ui->stackedWidget->setCurrentIndex(0);
    QStringList commandTypeList;
    //    commandTypeList << QStringLiteral("SET") << QStringLiteral("RESET") << QStringLiteral("CHECK") << QStringLiteral("SVPC_CHECK") << QStringLiteral("COMBINATION");
    //    ui->comboBox_operator->addItems(commandTypeList);
    initUiVisible();

    int cycle; QString unit;
    BeatArrow::ConvertTimeStr2ValueAndUnit(_vvFile->_time_interval, cycle, unit);
    ui->lineEdit_continuousTime->setText(QString::number(ui->spinBox_continuousBeat->value() * cycle));
    ui->label_unit->setText(unit);

    //! 初始化下标的lineEdit控件（下标的填写）
    ui->lineEdit_index->setToolTip(QStringLiteral("不填默认为零,{block}下标为B。操作多下标时，使用无空格的英文格式“;”间隔，如：0;1"));
    QRegExp reg(tr("^([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);([0-9]+|%1);$").arg("B"));
    ui->lineEdit_index->setValidator(new QRegExpValidator(reg));
    ui->lineEdit_index->setText("0");
    ui->lineEdit_varIndex->setToolTip(QStringLiteral("不填默认为零,{block}下标为B。"));
    QRegExp regVarIndex(tr("^([0-9]+|%1)$").arg("B"));
    ui->lineEdit_varIndex->setValidator(new QRegExpValidator(regVarIndex));
    ui->lineEdit_varIndex->setText("0");

    //! 约等于的范围
    QRegExp reg_range("^(-?[0]|-?[1-9][0-9]{0,9})(?:\\.\\d{0,2})?$|(^\\t?$)");
    ui->lineEdit_range->setValidator(new QRegExpValidator(reg_range));//nisz add 2022-6-27
    //! 常量的输入
    QRegExp reg_const("^(-?[0]|-?[1-9][0-9]{0,9})(?:\\.\\d{0,2})?$|(^\\t?$)");
    ui->lineEdit_command_2->setValidator(new QRegExpValidator(reg_const));
    //! 普通的输入
    QRegExp reg_common("^(-?[0]|-?[1-9][0-9]{0,9})(?:\\.\\d{0,2})?$|(^\\t?$)");//nisz add 2022-6-22
    ui->lineEdit_command->setValidator(new QRegExpValidator(reg_common));

    initData();//初始化数据
    initView();//初始化界面
    initViewData();//初始化界面数据


    initUi();
    ui->spinBox_continuousBeat->setRange(1, MAX_CONTINUOUS_BEAT);
    initSineWave();
    initPhaseStep();
    initSlope();
    initSquareWave();
    initFx();
    initWhiteNoise();
    initRandomNum();

    if(_vvCommand != nullptr)
    {
        setVVCommand(_vvCommand);
    }
    else
    {
        _vvCommand = new VVCommand();
    }


    ui->frame_check->setVisible(false);
    ui->pushButton_chooseVariable->setVisible(false);
    ui->label_range->setVisible(false);
    ui->lineEdit_range->setVisible(false);
    ui->label_FIFO->setVisible(false);
    ui->lineEdit_FIFO->setVisible(false);
    ui->label_varIndex->setVisible(false);
    ui->lineEdit_varIndex->setVisible(false);
}

AddCommandDlg::~AddCommandDlg()
{
    delete ui;
}

VVCommand *AddCommandDlg::getInfo() //! 从控件获取数据
{
    VVCommand *vvCommand = _vvCommand;
    vvCommand->_command_type_str        = ui->comboBox_commandType->currentText();
    vvCommand->_data_indexs             = ui->lineEdit_index->text();
    vvCommand->_physical_address        = ui->lineEdit_physicalAddr->text();
    vvCommand->_data_value              = ui->lineEdit_command->text();
    if(vvCommand->_command_type_str == "SVPC_CHECK" || vvCommand->_command_type_str == "RESET")
    {
        vvCommand->_data_value          = "";
    }
    else if(vvCommand->_command_type_str == "COMBINATION")
    {
        vvCommand->_data_value          = getFormulaExpression();
    }
    else if(vvCommand->_command_type_str == "CHECK")
    {
        if(ui->comboBox_operator_2->currentIndex() == 6)
        {
            vvCommand->_data_value      =  ui->lineEdit_command_2->text() +";" + ui->lineEdit_range->text();
        }
        else
        {
            vvCommand->_data_value      =  ui->lineEdit_command_2->text();
        }
        vvCommand->_data_indexs_r = ui->lineEdit_varIndex->text();
        if(vvCommand->_data_indexs_r == "")
        {
            vvCommand->_data_indexs_r = "0";
        }
    }
    vvCommand->_operator                = ui->comboBox_operator->currentText();
    if(vvCommand->_command_type_str == "CHECK")
    {
        vvCommand->_operator            = ui->comboBox_operator_2->currentText();
    }
    vvCommand->_continuous_beat         = ui->spinBox_continuousBeat->value();
    vvCommand->_svpc_check_begin_offset = ui->spinBox_startOffset->value();
    vvCommand->_svpc_check_end_offset   = ui->spinBox_endOffset->value();
    vvCommand->_condition_expression    = ui->lineEdit_conditionExpression->text();
    getTargetInfoFromView(vvCommand);

    return vvCommand;
}

void AddCommandDlg::getTargetInfoFromView(VVCommand *vvCommand)
{
    if(!vvCommand) return;

    if(!architectureModel_) return;

    vvCommand->_target_list.clear();//情况目标列表

    for(int row=0;row<architectureModel_->rowCount();row++)
    {
        getTargetInfoFromItem(architectureModel_->item(row), vvCommand);
    }
}

void AddCommandDlg::getTargetInfoFromItem(QStandardItem *item, VVCommand *vvCommand)
{
    if(!item) return;
    if(!vvCommand) return;


    if(item->checkState() == Qt::Checked )
    {
        qDebug() << item->data(Qt::DisplayRole).toString() << endl;
        QString type = item->data(TYPE_ROLE).toString();
        /**
          * 1: FMU
          * 5: ICD In
          * 6: ICD Out
          **/
        if(type.compare("1") == 0)
        {
            TargetInfo *targetInfo = new TargetInfo();

            targetInfo->uuid = item->data(UUID_ROLE).toString();
            targetInfo->name = item->text();
            targetInfo->path = getItemPath(item);
            targetInfo->type = "fmu";
            targetInfo->parentUuid = item->parent() ? item->parent()->data(UUID_ROLE).toString() : "";
            vvCommand->_target_list.append(targetInfo);
        }
        else if(type.compare("2") == 0)
        {
            //            TargetInfo *targetInfo = new TargetInfo();
            //            targetInfo->uuid = item->data(UUID_ROLE).toString();
            //            targetInfo->name = item->text();
            //            targetInfo->path = getItemPath(item);
            //            targetInfo->type = "block";
            //            targetInfo->parentUuid = item->parent() ? item->data(UUID_ROLE).toString() : "";
            //            vvCommand->_target_list.append(targetInfo);
        }
        else if(type.compare("99") == 0)
        {
            QString indexStr;
            for(int k = 0;k<item->rowCount();k++)
            {
                QStandardItem *indexItem = item->child(k);
                if(indexItem && indexItem->checkState() == Qt::Checked)
                {
                    indexStr += indexItem->data(FULL_PATH_ROLE).toString() + ";";
                }
            }
            TargetInfo *targetInfo = new TargetInfo();
            targetInfo->type = "icd";
            targetInfo->path = getItemPath(item);
            targetInfo->name = item->text();
            targetInfo->uuid = item->text();
            targetInfo->msgIndex = indexStr;
            vvCommand->_target_list.append(targetInfo);
        }
    }
    else if(item->checkState() == Qt::PartiallyChecked)
    {
        QString type = item->data(TYPE_ROLE).toString();
        if(type.compare("99") == 0)
        {
            QString indexStr;
            for(int k = 0;k<item->rowCount();k++)
            {
                QStandardItem *indexItem = item->child(k);
                if(indexItem && indexItem->checkState() == Qt::Checked)
                {
                    indexStr += indexItem->data(FULL_PATH_ROLE).toString() + ";";
                }
            }
            TargetInfo *targetInfo = new TargetInfo();
            targetInfo->type = "icd";
            targetInfo->path = getItemPath(item);
            targetInfo->name = item->text();
            targetInfo->uuid = item->text();
            targetInfo->msgIndex = indexStr;
            vvCommand->_target_list.append(targetInfo);
        }
    }

    for(int row = 0;row<item->rowCount();row++)
    {
        QStandardItem *child = item->child(row);
        if(child)
        {
            getTargetInfoFromItem(child, vvCommand);
        }
    }
}

int AddCommandDlg::getCommandType(const QString &commandType)
{
    QStringList commandTypeList;
    int rowCount = ui->comboBox_commandType->count();
    for(int i = 0; i < rowCount; ++i)
    {
        if(ui->comboBox_commandType->itemText(i) == commandType)
            return i;
    }
    return -1;
}

void AddCommandDlg::setVVCommand(VVCommand *vvCommand) //! 根据数据进行控件的初始化
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->frame_target->setVisible(true);
    int type = getCommandType(vvCommand->_command_type_str);
    if(type == -1)
    {
        return;
    }
    else if(type == 1)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->frame_target->setVisible(false);
    }
    else if(type == 4)
    {
        vvCommand->_continuous_beat = 1;
//        ui->lineEdit_command->setReadOnly(true);
//        ui->lineEdit_command->installEventFilter(this);
    }



    if(vvCommand->_continuous_beat == 0) vvCommand->_continuous_beat = 1;

    ui->comboBox_commandType->setCurrentIndex(type);
    ui->lineEdit_dest->setText(tr("%1,%2").arg(_vvCommand->_msg_name).arg(_vvCommand->_sig_name));

    ui->lineEdit_index->setText(vvCommand->_data_indexs);
    ui->lineEdit_physicalAddr->setText(vvCommand->_physical_address);
    ui->lineEdit_command->setText(vvCommand->_data_value);
    ui->comboBox_operator->setCurrentText(vvCommand->_operator);
    ui->comboBox_operator_2->setCurrentText(vvCommand->_operator);
    ui->spinBox_continuousBeat->setValue(vvCommand->_continuous_beat);


    if(type == 2)
    {
        ui->frame_check->setVisible(true);
        QStringList strList = vvCommand->_data_value.split(";");
        if(strList.size() == 1)
        {
            if(strList.first().contains(","))
            {
                ui->label_varIndex->setVisible(true);
                ui->lineEdit_varIndex->setVisible(true);
                ui->lineEdit_varIndex->setText(vvCommand->_data_indexs_r);
                ui->radioButton_variable->setChecked(true);
                ui->pushButton_chooseVariable->setVisible(true);
                ui->lineEdit_command_2->setReadOnly(true);
                ui->lineEdit_command_2->setValidator(new QRegExpValidator(QRegExp()));
                ui->lineEdit_command_2->setMaxLength(100);
            }
            else
            {
                ui->lineEdit_command_2->setReadOnly(false);
//                QRegExp reg("^(-?[0-9]{0,5})(.\\d{0,2})?)$");
                QRegExp reg("^(-?[0]|-?[1-9][0-9]{0,9})(?:\\.\\d{0,2})?$|(^\\t?$)");
                ui->lineEdit_command_2->setValidator(new QRegExpValidator(reg));
//                ui->lineEdit_command_2->setMaxLength(5);
            }
            ui->lineEdit_command_2->setText(strList.first());
            return;
        }
        if(vvCommand->_operator == QStringLiteral("≈"))
        {
            ui->comboBox_operator_2->setCurrentText(vvCommand->_operator);
            ui->label_range->setVisible(true);
            ui->lineEdit_command_2->setText(strList.at(0));
            ui->lineEdit_range->setVisible(true);
            ui->lineEdit_range->setText(strList.at(1));
        }
        if(vvCommand->_data_value.contains(","))
        {
            ui->label_varIndex->setVisible(true);
            ui->lineEdit_varIndex->setVisible(true);
            ui->lineEdit_varIndex->setText(vvCommand->_data_indexs_r);
            ui->radioButton_variable->setChecked(true);
            ui->pushButton_chooseVariable->setVisible(true);
            qDebug() << vvCommand->_data_value << endl;
            ui->lineEdit_command_2->setText(strList.at(0));
            ui->lineEdit_command_2->setToolTip(strList.at(0));
        }
    }
    else if(type == 5)
    {
        ui->label_FIFO->setVisible(true);
        ui->lineEdit_FIFO->setVisible(true);
        ui->lineEdit_dest->setText(vvCommand->_msg_name);
        ui->lineEdit_FIFO->setText(vvCommand->_sig_name);
    }
    else if(type == 6)
    {
        ui->label_FIFO->setVisible(true);
        ui->lineEdit_FIFO->setVisible(true);
        ui->lineEdit_dest->setText(vvCommand->_sig_name);
        ui->lineEdit_FIFO->setText(vvCommand->_sig_name);
    }




    int cycle; QString unit;
    BeatArrow::ConvertTimeStr2ValueAndUnit(_vvFile->_time_interval, cycle, unit);
    QString continuousTime = QString::number(vvCommand->_continuous_beat * cycle);
    ui->lineEdit_continuousTime->setText(continuousTime);
    ui->spinBox_endOffset->setValue(vvCommand->_svpc_check_end_offset);
    ui->spinBox_startOffset->setValue(vvCommand->_svpc_check_begin_offset);
    ui->lineEdit_conditionExpression->setText(vvCommand->_condition_expression);
    if(vvCommand->_command_type_str == "COMBINATION")
        setFormulaExpression(vvCommand->_data_value);
    //! 下面函数将在vvCommand->_target_list中写入信息
    updateTargetInfoToView(vvCommand);
}

void AddCommandDlg::updateTargetInfoToView(VVCommand *command_)
{
    if(!command_) return;

    if(!architectureModel_) return;

    for(int row=0;row<architectureModel_->rowCount();row++){
        updateTargetInfoToItem(architectureModel_->item(row), command_);
    }
}

void AddCommandDlg::updateTargetInfoToItem(QStandardItem *item, VVCommand *command_)
{
    if(!item) return;
    if(!command_) return;

    QString uuid = item->data(UUID_ROLE).toString();
    QString path = getItemPath(item);
    for(int index=0;index<command_->_target_list.size();index++)
    {
        if(uuid.compare(command_->_target_list.at(index)->uuid) == 0 && path.compare(command_->_target_list.at(index)->path) == 0)
        {
            if(item->data(TYPE_ROLE).toInt() == 99)
            {
                QList<QString> indexList = command_->_target_list.at(index)->msgIndex.split(";",QString::SkipEmptyParts);
                for(int j=0;j<item->rowCount();j++)
                {
                    QStandardItem *indexItem = item->child(j);
                    if(indexItem)
                    {
                        QString fullPath = indexItem->data(FULL_PATH_ROLE).toString();
                        if(indexList.contains(fullPath))
                        {
                            indexItem->setCheckState(Qt::Checked);
                        }
                    }
                }
            }
            else
            {
                item->setCheckState(Qt::Checked);
            }
            break;
        }
    }

    for(int row = 0;row<item->rowCount();row++){
        QStandardItem *child = item->child(row);
        if(child){
            updateTargetInfoToItem(child, command_);
        }
    }
}

QString AddCommandDlg::getItemPath(QStandardItem *item)
{
    if(!item)
        return QString();
    QString path = item->text();

    QStandardItem *parent = item->parent();
    while(parent)
    {
        path = parent->text() + "/" + path;
        parent = parent->parent();
    }
    return path;
}

void AddCommandDlg::initUiVisible()
{
    ui->label_command->setVisible(true);
    ui->lineEdit_command->setVisible(true);
    ui->stackedWidget_formula->setVisible(false);
    ui->comboBox_type->setVisible(false);

    ui->label_physicalAddr->setVisible(true);
    ui->lineEdit_physicalAddr->setVisible(true);
    //CHECK
    ui->label_operator->setVisible(false);
    ui->comboBox_operator->setVisible(false);
    //SVPC_CHECK
    ui->label_endOffset->setVisible(false);
    ui->label_startOffset->setVisible(false);
    ui->spinBox_endOffset->setVisible(false);
    ui->spinBox_startOffset->setVisible(false);
}

void AddCommandDlg::setICDData(QSharedPointer<ICDData> icd_data)
{
    _icd_data = icd_data;
}

void AddCommandDlg::on_pushButton_selectTarget_clicked()
{

}

void AddCommandDlg::on_comboBox_commandType_currentIndexChanged(int index)
{
    initUiVisible();

    switch (index) {
    case 0://SET
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_formula->setVisible(false);
        ui->lineEdit_command->setVisible(true);
        ui->comboBox_type->setVisible(false);
        ui->label_continuousBeat->setVisible(true);
        ui->spinBox_continuousBeat->setVisible(true);
        ui->label_continuousTime->setVisible(true);
        ui->lineEdit_continuousTime->setVisible(true);
        ui->label_unit->setVisible(true);
        ui->frame_other->setVisible(true);
        ui->frame_check->setVisible(false);
        ui->label_index->setVisible(true);
        ui->lineEdit_index->setVisible(true);
        ui->frame_target->setVisible(true);
        ui->label_physicalAddr->setVisible(true);
        ui->lineEdit_physicalAddr->setVisible(true);
        ui->label_FIFO->setVisible(false);
        ui->lineEdit_FIFO->setVisible(false);
        ui->label_dest->setVisible(true);
        ui->lineEdit_dest->setVisible(true);
        ui->lineEdit_command->clear();
        break;
    case 1://RESET
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_formula->setVisible(false);
        ui->lineEdit_command->setVisible(true);
        ui->comboBox_type->setVisible(false);
        ui->label_continuousBeat->setVisible(true);
        ui->spinBox_continuousBeat->setVisible(true);
        ui->label_continuousTime->setVisible(true);
        ui->lineEdit_continuousTime->setVisible(true);
        ui->label_unit->setVisible(true);
        ui->frame_other->setVisible(true);
        ui->frame_check->setVisible(false);
        ui->label_index->setVisible(true);
        ui->lineEdit_index->setVisible(true);
        ui->frame_target->setVisible(false);
        ui->label_physicalAddr->setVisible(false);
        ui->lineEdit_physicalAddr->setVisible(false);
        ui->label_FIFO->setVisible(false);
        ui->lineEdit_FIFO->setVisible(false);
        ui->label_dest->setVisible(true);
        ui->lineEdit_dest->setVisible(true);
        ui->lineEdit_dest->clear();
        ui->lineEdit_command_2->clear();
        break;
    case 2://CHECK
        ui->label_operator->setVisible(true);
        ui->comboBox_operator->setVisible(true);
        ui->stackedWidget_formula->setVisible(false);
        ui->lineEdit_command->setVisible(true);
        ui->comboBox_type->setVisible(false);
        ui->label_continuousBeat->setVisible(true);
        ui->spinBox_continuousBeat->setVisible(true);
        ui->label_continuousTime->setVisible(true);
        ui->lineEdit_continuousTime->setVisible(true);
        ui->label_unit->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);
        ui->frame_other->setVisible(false);
        ui->frame_check->setVisible(true);
        ui->label_index->setVisible(true);
        ui->lineEdit_index->setVisible(true);
        ui->frame_target->setVisible(true);
        ui->label_physicalAddr->setVisible(true);
        ui->lineEdit_physicalAddr->setVisible(true);
        ui->label_FIFO->setVisible(false);
        ui->lineEdit_FIFO->setVisible(false);
        ui->label_dest->setVisible(true);
        ui->lineEdit_dest->setVisible(true);
        break;
    case 3://SVPC_CHECK
        ui->label_endOffset->setVisible(true);
        ui->label_startOffset->setVisible(true);
        ui->spinBox_endOffset->setVisible(true);
        ui->spinBox_startOffset->setVisible(true);
        ui->label_physicalAddr->setVisible(false);
        ui->lineEdit_physicalAddr->setVisible(false);
        ui->label_command->setVisible(false);
        ui->lineEdit_command->setVisible(false);
        ui->stackedWidget_formula->setVisible(false);
        ui->comboBox_type->setVisible(false);
        ui->label_continuousBeat->setVisible(true);
        ui->spinBox_continuousBeat->setVisible(true);
        ui->label_continuousTime->setVisible(true);
        ui->lineEdit_continuousTime->setVisible(true);
        ui->label_unit->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);
        ui->frame_other->setVisible(true);
        ui->frame_check->setVisible(false);
        ui->label_index->setVisible(true);
        ui->lineEdit_index->setVisible(true);
        ui->frame_target->setVisible(true);
        ui->label_physicalAddr->setVisible(true);
        ui->lineEdit_physicalAddr->setVisible(true);
        ui->label_FIFO->setVisible(false);
        ui->lineEdit_FIFO->setVisible(false);
        ui->label_dest->setVisible(true);
        ui->lineEdit_dest->setVisible(true);
        break;
    case 4://COMBINATION
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_formula->setVisible(true);
        ui->lineEdit_command->setVisible(false);
        ui->comboBox_type->setVisible(true);
        ui->label_continuousBeat->setVisible(false);
        ui->spinBox_continuousBeat->setVisible(false);
        ui->label_continuousTime->setVisible(false);
        ui->lineEdit_continuousTime->setVisible(false);
        ui->label_unit->setVisible(false);
//        ui->lineEdit_command->removeEventFilter(this);
//        ui->lineEdit_command->installEventFilter(this);
        ui->spinBox_continuousBeat->setValue(1);
        ui->frame_other->setVisible(true);
        ui->frame_check->setVisible(false);
        ui->label_index->setVisible(true);
        ui->lineEdit_index->setVisible(true);
        ui->frame_target->setVisible(true);
        ui->label_physicalAddr->setVisible(true);
        ui->lineEdit_physicalAddr->setVisible(true);
        ui->label_FIFO->setVisible(false);
        ui->lineEdit_FIFO->setVisible(false);
        ui->label_dest->setVisible(true);
        ui->lineEdit_dest->setVisible(true);
        break;
    case 5://PUSH_INTO_FIFO
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_formula->setVisible(false);
        ui->lineEdit_command->setVisible(false);
        ui->comboBox_type->setVisible(false);
        ui->label_continuousBeat->setVisible(true);
        ui->spinBox_continuousBeat->setVisible(true);
        ui->label_continuousTime->setVisible(true);
        ui->lineEdit_continuousTime->setVisible(true);
        ui->label_unit->setVisible(false);
        ui->frame_other->setVisible(false);
        ui->frame_check->setVisible(false);
        ui->label_index->setVisible(false);
        ui->lineEdit_index->setVisible(false);
        ui->frame_target->setVisible(true);
        ui->label_physicalAddr->setVisible(false);
        ui->lineEdit_physicalAddr->setVisible(false);
        ui->label_FIFO->setVisible(true);
        ui->lineEdit_FIFO->setVisible(true);
        ui->label_dest->setVisible(true);
        ui->lineEdit_dest->setVisible(true);
        ui->lineEdit_dest->clear();
        ui->lineEdit_FIFO->clear();
        break;
    case 6://CLEAR_FIFO
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_formula->setVisible(false);
        ui->lineEdit_command->setVisible(false);
        ui->comboBox_type->setVisible(false);
        ui->label_continuousBeat->setVisible(true);
        ui->spinBox_continuousBeat->setVisible(true);
        ui->label_continuousTime->setVisible(true);
        ui->lineEdit_continuousTime->setVisible(true);
        ui->label_unit->setVisible(false);
        ui->frame_other->setVisible(false);
        ui->frame_check->setVisible(false);
        ui->label_index->setVisible(false);
        ui->lineEdit_index->setVisible(false);
        ui->frame_target->setVisible(true);
        ui->label_physicalAddr->setVisible(false);
        ui->lineEdit_physicalAddr->setVisible(false);
        ui->label_FIFO->setVisible(true);
        ui->lineEdit_FIFO->setVisible(true);
        ui->label_dest->setVisible(false);
        ui->lineEdit_dest->setVisible(false);
        ui->lineEdit_dest->clear();
        ui->lineEdit_FIFO->clear();
        break;
    }
}

void AddCommandDlg::on_pushButton_conditionConfig_clicked()
{

}

void AddCommandDlg::initData()
{
    architectureModel_ = new QStandardItemModel(this);
    cloneModel(SingletonProject::project_.getSystemArchitectureModel(),architectureModel_);
}

void AddCommandDlg::cloneModel(QStandardItemModel *model,QStandardItemModel *newModel)
{
    if(!model) return;

    for(int row=0;row<model->rowCount();row++){
        QStandardItem *newItem = nullptr;
        cloneItem(model->item(row),newItem);

        if(!newItem) break;

        newModel->appendRow(newItem);
    }
}

void AddCommandDlg::cloneItem(QStandardItem *item, QStandardItem *&newItem)
{
    if(!item) return;

    newItem = item->clone();
    for(int row = 0;row<item->rowCount();row++){
        QStandardItem *child = item->child(row);

        if(!child) break;

        QStandardItem *newChild = nullptr;
        cloneItem(child,newChild);

        if(!newChild) return;

        newItem->appendRow(newChild);
    }
}

void AddCommandDlg::initView()
{
    ui->treeView_architecture->setHeaderHidden(true);
    ui->treeView_architecture->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void AddCommandDlg::initViewData()
{
    if(architectureModel_){
        ui->treeView_architecture->setModel(architectureModel_);
        ui->treeView_architecture->expandAll();

        connect(architectureModel_,&QStandardItemModel::itemChanged,this,&AddCommandDlg::slot_architecture_itemChanged);

        if(architectureModel_){
            for(int row=0;row<architectureModel_->rowCount();row++){
                addMsgInfoToView(architectureModel_->item(row));
            }
        }
    }
}

void AddCommandDlg::addMsgInfoToView(QStandardItem *item)
{
    if(!item) return;
    if(!_icd_data) return;

    //DataSource == 5;DataOutputer == 6
    if(item->data(TYPE_ROLE).toInt() == 5 ||item->data(TYPE_ROLE).toInt() == 6)
    {
        QString uuidStr = item->parent()->data(UUID_ROLE).toString();
        QList<QVariant> msgIdList = item->data(MSG_LIST_ROLE).toList();
        for(auto msgId:msgIdList)
        {
            QStandardItem *msgItem = new QStandardItem(msgId.toString());
            msgItem->setCheckable(true);
            msgItem->setTristate(true);
            msgItem->setData(msgId,UUID_ROLE);
            msgItem->setData(99,TYPE_ROLE);

            void *handle = _icd_data->findICDInstanceByID(msgId.toString()); //此处的msgId为实际为icdname，变量名没改过来
            if(handle != nullptr)
            {
                short bufferSize = _icd_data->getICDBuffers(handle);
                //! add special {block}
                QStandardItem *indexItemBlock = new QStandardItem("{block}");
                QString fullStr = tr("/%1/%2/%3").arg(msgId.toString()).arg("{block}").arg(uuidStr);
                indexItemBlock->setData(fullStr, FULL_PATH_ROLE);
                indexItemBlock->setCheckable(true);
                indexItemBlock->setTristate(true);
                msgItem->appendRow(indexItemBlock);
                //! add end
                for(int i=0;i<bufferSize;i++)
                {
                    QStandardItem *indexItem = new QStandardItem(QString::number(i));
                    indexItem->setData(i, FULL_PATH_ROLE);
                    indexItem->setCheckable(true);
                    indexItem->setTristate(true);
                    msgItem->appendRow(indexItem);
                }
            }
            item->appendRow(msgItem);
        }
    }

    for(int i=0;i<item->rowCount();i++)
    {
        QStandardItem * childItem = item->child(i);
        addMsgInfoToView(childItem);
    }
}

void AddCommandDlg::slot_architecture_itemChanged(QStandardItem *item)
{
    if(!architectureModel_) return;

    disconnect(architectureModel_,&QStandardItemModel::itemChanged,this,&AddCommandDlg::slot_architecture_itemChanged);

    //更新子节点CheckState
    updateChildCheckState(item);

    //更新父节点CheckState
    updateParentCheckState(item->parent());

    connect(architectureModel_,&QStandardItemModel::itemChanged,this,&AddCommandDlg::slot_architecture_itemChanged);
}

void AddCommandDlg::updateParentCheckState(QStandardItem *parent)
{
    if(!parent) return;

    bool haveChecked = false;
    bool haveUnChecked = false;
    for(int row = 0;row<parent->rowCount();row++){
        QStandardItem *sibling = parent->child(row);
        if(sibling){
            if(sibling->checkState() == Qt::Checked){
                haveChecked = true;
            }else if(sibling->checkState() == Qt::Unchecked){
                haveUnChecked = true;
            }else{
                haveChecked = true;
                haveUnChecked = true;
                break;
            }
        }
    }

    if(haveChecked && haveUnChecked){
        if(parent->isCheckable()){
            parent->setCheckState(Qt::PartiallyChecked);
        }

    }else if(haveChecked){
        if(parent->isCheckable()){
            parent->setCheckState(Qt::Checked);
        }
    }else if(haveUnChecked){
        if(parent->isCheckable()){
            parent->setCheckState(Qt::Unchecked);
        }
    }
    updateParentCheckState(parent->parent());
}

void AddCommandDlg::updateChildCheckState(QStandardItem *item)
{
    if(!item) return;

    if(item->checkState() != Qt::PartiallyChecked){
        for(int row = 0;row<item->rowCount();row++){
            QStandardItem *child = item->child(row);
            if(child && child->isCheckable()){
                child->setCheckState(item->checkState());
                updateChildCheckState(child);
            }
        }
    }
}

QStringList AddCommandDlg::getAllFormulaStr()
{
    QStringList strList;
    strList.append("SineWave");
    strList.append("StepOffset");
    strList.append("Ramp");
    strList.append("SquareWave");
    strList.append("Linear");
    strList.append("WhiteNoise");
    strList.append("RandomNumber");
    return strList;
}

bool AddCommandDlg::setFormulaExpression(const QString &expression)
{
    // 正弦波   阶跃        斜坡    方波     一元一次方程   白噪声    随机数
    // SineWave、StepOffset、Ramp、SquareWave、Linear、WhiteNoise、RandomNumber------------这里保留旧版本的标志

    if(expression.isEmpty()) return true;
    _curFormula = expression.split(";").first();

    int index = getAllFormulaStr().indexOf(_curFormula);
    ui->comboBox_type->setCurrentIndex(index);
    ui->stackedWidget_formula->setCurrentIndex(index);

    if(_curFormula == "SineWave")
        return setFormulaExpressionSineWave(expression);
    else if(_curFormula == "StepOffset")
        return setFormulaExpressionPhaseStep(expression);
    else if(_curFormula == "Ramp")
        return setFormulaExpressionSlope(expression);
    else if(_curFormula == "SquareWave")
        return setFormulaExpressionSquareWave(expression);
    else if(_curFormula == "Linear")
        return setFormulaExpressionFx(expression);
    else if(_curFormula == "WhiteNoise")
        return setFormulaExpressionWhiteNoise(expression);
    else if(_curFormula == "RandomNumber")
        return setFormulaExpressionRandomNum(expression);
}

QString AddCommandDlg::getFormulaExpression()
{
    if(_curFormula == "SineWave")
        return getFormulaExpressionSineWave();
    else if(_curFormula == "StepOffset")
        return getFormulaExpressionPhaseStep();
    else if(_curFormula == "Ramp")
        return getFormulaExpressionSlope();
    else if(_curFormula == "SquareWave")
        return getFormulaExpressionSquareWave();
    else if(_curFormula == "Linear")
        return getFormulaExpressionFx();
    else if(_curFormula == "WhiteNoise")
        return getFormulaExpressionWhiteNoise();
    else if(_curFormula == "RandomNumber")
        return getFormulaExpressionRandomNum();
}

void AddCommandDlg::initUi()
{
    QStringList headerList;
    headerList << QStringLiteral("正弦波") << QStringLiteral("阶跃")
               << QStringLiteral("斜坡") << QStringLiteral("方波")
               << QStringLiteral("一元一次方程")<< QStringLiteral("白噪声")
               << QStringLiteral("随机数");
    ui->comboBox_type->addItems(headerList);
    _curFormula = "SineWave";
}

void AddCommandDlg::initSineWave()
{
    ui->doubleSpinBox_amplitude->setRange(0,MAX_DOUBLE);
    ui->doubleSpinBox_frequency->setRange(0,MAX_DOUBLE);
    ui->spinBox_durationSineWave->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepSineWave->setChecked(true);
}

void AddCommandDlg::initPhaseStep()
{
    ui->doubleSpinBox_stepValue->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->spinBox_durationPhaseStep->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepPhaseStep->setChecked(true);
}

void AddCommandDlg::initSlope()
{
    ui->doubleSpinBox_initalValue->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->doubleSpinBox_targetValue->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->spinBox_durationSlope->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepSlope->setChecked(true);
}

void AddCommandDlg::initSquareWave()
{
    ui->doubleSpinBox_positiveValue->setRange(0,MAX_DOUBLE);
    ui->spinBox_positiveDuration->setRange(1,MAX_DOUBLE);
    ui->doubleSpinBox_negativeValue->setRange(0,MAX_DOUBLE);
    ui->spinBox_negativeDuration->setRange(1,MAX_DOUBLE);
    ui->spinBox_durationSquareWave->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepSquareWave->setChecked(true);
}

void AddCommandDlg::initFx()
{
    ui->doubleSpinBox_slope->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->doubleSpinBox_intercept->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->spinBox_durationFx->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepFx->setChecked(true);
}

void AddCommandDlg::initWhiteNoise()
{
    ui->doubleSpinBox_meanValue->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->doubleSpinBox_standardDeviation->setRange(0,MAX_DOUBLE);
    ui->spinBox_durationWhiteNoise->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepWhiteNoise->setChecked(true);
}

void AddCommandDlg::initRandomNum()
{
    ui->doubleSpinBox_max->setRange(-MAX_DOUBLE,MAX_DOUBLE);
    ui->doubleSpinBox_min->setRange(-MAX_DOUBLE,MAX_DOUBLE);

    connect(ui->doubleSpinBox_max, &QDoubleSpinBox::editingFinished, this, [this](){
        double maxValue = ui->doubleSpinBox_max->text().toDouble();
        double minValue = ui->doubleSpinBox_min->text().toDouble();
        if(maxValue < minValue)
        {
            ui->doubleSpinBox_max->setValue(ui->doubleSpinBox_min->text().toDouble());
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("最大值不能小于最小值！"), QStringLiteral("确定"));
            return;
        }
    });
    connect(ui->doubleSpinBox_min, &QDoubleSpinBox::editingFinished, this, [this](){
        double maxValue = ui->doubleSpinBox_max->text().toDouble();
        double minValue = ui->doubleSpinBox_min->text().toDouble();
        if(maxValue < minValue)
        {
            ui->doubleSpinBox_min->setValue(ui->doubleSpinBox_max->text().toDouble());
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("最小值不能大于最大值！"), QStringLiteral("确定"));
            return;
        }
    });

    ui->spinBox_durationRandomNum->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepRandomNum->setChecked(true);
}

bool AddCommandDlg::setFormulaExpressionSineWave(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("SineWave") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_amplitude->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_frequency->setValue(argList.at(2).toDouble());
            ui->spinBox_durationSineWave->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepSineWave->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepSineWave->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool AddCommandDlg::setFormulaExpressionPhaseStep(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("StepOffset") == 0)
    {
        if(argList.size() == 4)
        {
            ui->doubleSpinBox_stepValue->setValue(argList.at(1).toDouble());
            ui->spinBox_durationPhaseStep->setValue(argList.at(2).toInt());

            if(argList.at(3).toInt())
            {
                ui->radioButton_keepPhaseStep->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepPhaseStep->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool AddCommandDlg::setFormulaExpressionSlope(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("Ramp") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_initalValue->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_targetValue->setValue(argList.at(2).toDouble());
            ui->spinBox_durationSlope->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepSlope->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepSlope->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool AddCommandDlg::setFormulaExpressionSquareWave(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("SquareWave") == 0)
    {
        if(argList.size() == 7)
        {
            ui->doubleSpinBox_positiveValue->setValue(argList.at(1).toDouble());
            ui->spinBox_positiveDuration->setValue(argList.at(2).toInt());
            ui->doubleSpinBox_negativeValue->setValue(qAbs(argList.at(3).toDouble()));
            ui->spinBox_negativeDuration->setValue(argList.at(4).toInt());
            ui->spinBox_durationSquareWave->setValue(argList.at(5).toInt());

            if(argList.at(6).toInt())
            {
                ui->radioButton_keepSquareWave->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepSquareWave->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool AddCommandDlg::setFormulaExpressionFx(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("Linear") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_slope->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_intercept->setValue(argList.at(2).toDouble());
            ui->spinBox_durationFx->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepFx->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepFx->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool AddCommandDlg::setFormulaExpressionWhiteNoise(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("WhiteNoise") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_meanValue->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_standardDeviation->setValue(argList.at(2).toDouble());
            ui->spinBox_durationWhiteNoise->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepWhiteNoise->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepWhiteNoise->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool AddCommandDlg::setFormulaExpressionRandomNum(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("RandomNumber") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_max->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_min->setValue(argList.at(2).toDouble());
            ui->spinBox_durationRandomNum->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepRandomNum->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepRandomNum->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

QString AddCommandDlg::getFormulaExpressionSineWave()
{
    QString expression;
    QString amplitude = ui->doubleSpinBox_amplitude->text();
    QString frequency = ui->doubleSpinBox_frequency->text();
    QString duration = ui->spinBox_durationSineWave->text();
    QString keep = ui->radioButton_keepSineWave->isChecked() ? "1" :"0";

    expression = "SineWave;"
            + amplitude + ";"
            + frequency + ";"
            + duration  + ";"
            + keep;
    return expression;
}

QString AddCommandDlg::getFormulaExpressionPhaseStep()
{
    QString expression;
    QString stepValue = ui->doubleSpinBox_stepValue->text();
    QString duration = ui->spinBox_durationPhaseStep->text();
    QString keep = ui->radioButton_keepPhaseStep->isChecked() ? "1" :"0";

    expression = "StepOffset;"
            + stepValue + ";"
            + duration  + ";"
            + keep;
    return expression;
}

QString AddCommandDlg::getFormulaExpressionSlope()
{
    QString expression;
    QString initalValue = ui->doubleSpinBox_initalValue->text();
    QString targetValue = ui->doubleSpinBox_targetValue->text();
    QString duration = ui->spinBox_durationSlope->text();
    QString keep = ui->radioButton_keepSlope->isChecked() ? "1" :"0";

    expression = "Ramp;"
            + initalValue + ";"
            + targetValue + ";"
            + duration    + ";"
            + keep;
    return expression;
}

QString AddCommandDlg::getFormulaExpressionSquareWave()
{
    QString expression;
    QString positiveValue = ui->doubleSpinBox_positiveValue->text();
    QString positiveDuration = ui->spinBox_positiveDuration->text();
    QString negativeValue = "-" + ui->doubleSpinBox_negativeValue->text();
    QString negativeDuration = ui->spinBox_negativeDuration->text();
    QString duration = ui->spinBox_durationSquareWave->text();
    QString keep = ui->radioButton_keepSquareWave->isChecked() ? "1" :"0";

    expression = "SquareWave;"
            + positiveValue    + ";"
            + positiveDuration + ";"
            + negativeValue    + ";"
            + negativeDuration + ";"
            + duration         + ";"
            + keep;
    return expression;
}

QString AddCommandDlg::getFormulaExpressionFx()
{
    QString expression;
    QString slope = ui->doubleSpinBox_slope->text();
    QString intercept = ui->doubleSpinBox_intercept->text();
    QString duration = ui->spinBox_durationFx->text();
    QString keep = ui->radioButton_keepFx->isChecked() ? "1" :"0";

    expression = "Linear;"
            + slope     + ";"
            + intercept + ";"
            + duration  + ";"
            + keep;
    return expression;
}

QString AddCommandDlg::getFormulaExpressionWhiteNoise()
{
    QString expression;
    QString meanValue = ui->doubleSpinBox_meanValue->text();
    QString standardDeviation = ui->doubleSpinBox_standardDeviation->text();
    QString duration = ui->spinBox_durationWhiteNoise->text();
    QString keep = ui->radioButton_keepWhiteNoise->isChecked() ? "1" :"0";

    expression = "WhiteNoise;"
            + meanValue         + ";"
            + standardDeviation + ";"
            + duration          + ";"
            + keep;
    return expression;
}

QString AddCommandDlg::getFormulaExpressionRandomNum()
{
    QString expression;
    QString max = ui->doubleSpinBox_max->text();
    QString min = ui->doubleSpinBox_min->text();
    QString duration = ui->spinBox_durationRandomNum->text();
    QString keep = ui->radioButton_keepRandomNum->isChecked() ? "1" :"0";

    expression = "RandomNumber;"
            + max      + ";"
            + min      + ";"
            + duration + ";"
            + keep;
    return expression;
}

void AddCommandDlg::on_comboBox_type_currentIndexChanged(int index)
{
    // SineWave、StepOffset、Ramp、SquareWave、Linear、WhiteNoise、RandomNumber------------这里保留旧版本的标志
    if(index == -1) return;

    _curFormula = getAllFormulaStr().at(index);
    ui->stackedWidget_formula->setCurrentIndex(index);
}

void AddCommandDlg::on_pushButton_chooseDest_clicked()
{
    bool isFIFO = false;
    bool isClearFIFO = false;
    if(ui->comboBox_commandType->currentIndex() == 5)
    {
        isFIFO = true;
    }
    if(ui->comboBox_commandType->currentIndex() == 6)
    {
        isFIFO = true;
        isClearFIFO = true;
    }

    SelectSignalDlg selectSignalDlg(_infoList, isFIFO, isClearFIFO);
    QString destStr = ui->lineEdit_dest->text();

    QString param1 = "";
    QString param2 = "";
    if(isFIFO)
    {
        param1 = ui->lineEdit_dest->text();
        param2 = ui->lineEdit_FIFO->text();
    }
    else
    {
        if(destStr.split(",").size() == 2)
        {
            param1 = destStr.split(",").at(0);
            param2 = destStr.split(",").at(1);
        }
    }
    selectSignalDlg.setInfo(param1, param2);






    int ret = selectSignalDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
        ICDSignal icdSignal = selectSignalDlg.getInfo();
        _vvCommand->_msg_id = icdSignal.messageID_;
        _vvCommand->_msg_name = icdSignal.messageName_;
        _vvCommand->_sig_id = icdSignal.signalInfo_.signalID_;
        _vvCommand->_sig_name = icdSignal.signalInfo_.signalName_;
        _vvCommand->_offset = QString::number(icdSignal.signalInfo_.offset_);
        _vvCommand->_word_offset = QString::number(icdSignal.signalInfo_.wordOffset_);
        _vvCommand->_data_type = icdSignal.signalInfo_.strType_;
        _vvCommand->_data_length = icdSignal.signalInfo_.lenght_;

        ui->lineEdit_dest->setText(tr("%1,%2").arg(_vvCommand->_msg_name).arg(_vvCommand->_sig_name));
        if(isFIFO)
        {
            ui->lineEdit_dest->setText(_vvCommand->_msg_name);
            ui->lineEdit_FIFO->setText(_vvCommand->_sig_name);
        }
        if(isClearFIFO)
        {
            ui->lineEdit_dest->setText(_vvCommand->_sig_name);
            ui->lineEdit_FIFO->setText(_vvCommand->_sig_name);
            _vvCommand->_msg_id   = _vvCommand->_sig_name;
            _vvCommand->_msg_name = _vvCommand->_sig_name;
        }
        break;
    }
}

void AddCommandDlg::on_spinBox_continuousBeat_valueChanged(int arg1)
{
    int cycle; QString unit;
    BeatArrow::ConvertTimeStr2ValueAndUnit(_vvFile->_time_interval, cycle, unit);
    ui->lineEdit_continuousTime->setText(QString::number(arg1 * cycle));
}

void AddCommandDlg::on_spinBox_startOffset_valueChanged(int arg1)
{
    if(arg1 > ui->spinBox_endOffset->value())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("起始偏移不能大于结束偏移！"), QStringLiteral("确定"));
        ui->spinBox_startOffset->setValue(ui->spinBox_endOffset->value());
    }
}

void AddCommandDlg::on_spinBox_endOffset_valueChanged(int arg1)
{
    if(arg1 < ui->spinBox_startOffset->value())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("结束偏移不能小于起始偏移！"), QStringLiteral("确定"));
        ui->spinBox_endOffset->setValue(ui->spinBox_startOffset->value());
    }
}

void AddCommandDlg::on_pushButton_ok_clicked()
{
    if(ui->lineEdit_dest->text().isEmpty() && ui->comboBox_commandType->currentIndex() != 1 && ui->comboBox_commandType->currentIndex() != 6)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("命令目标不能为空！"), QStringLiteral("确定"));
        return;
    }
    //nisz add 2022-6-22
    if(ui->lineEdit_command_2->text().isEmpty()&& ui->comboBox_commandType->currentIndex() == 2)
    {
        QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("命令值不能为空！"),QStringLiteral("确定"));
        return;
    }
    if(ui->lineEdit_command->text().isEmpty()&& ui->comboBox_commandType->currentIndex() == 0)
    {
        QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("命令值不能为空！"),QStringLiteral("确定"));
        return;
    }
    //nisz add end
    //nisz add 2022-6-23
    if(ui->comboBox_commandType->currentIndex() == 2&&ui->comboBox_operator_2->currentIndex()==6&&ui->lineEdit_range->text().isEmpty())
    {
        QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral(" 浮动范围不能为空！"),QStringLiteral("确定"));
        return;
    }
    //nisz add end
    this->accept();
}

void AddCommandDlg::on_pushButton_cancel_clicked()
{
    this->reject();
}

void AddCommandDlg::on_comboBox_operator_2_currentIndexChanged(int index)
{
    if(index == 6)
    {
        ui->lineEdit_range->setVisible(true);
        ui->label_range->setVisible(true);
    }
    else
    {
        ui->lineEdit_range->setVisible(false);
        ui->label_range->setVisible(false);
    }
}

void AddCommandDlg::on_radioButton_const_toggled(bool checked)
{
    if(checked)
    {
        ui->pushButton_chooseVariable->setVisible(false);
        ui->label_varIndex->setVisible(false);
        ui->lineEdit_varIndex->setVisible(false);
        ui->lineEdit_command_2->setReadOnly(false);
        QRegExp reg("^(-?[0]|-?[1-9][0-9]{0,9})(?:\\.\\d{0,2})?$|(^\\t?$)");
        ui->lineEdit_command_2->setValidator(new QRegExpValidator(reg));
//        ui->lineEdit_command_2->setMaxLength(5);
        ui->lineEdit_command_2->clear();
    }
}

void AddCommandDlg::on_radioButton_variable_toggled(bool checked)
{
    if(checked)
    {
        ui->pushButton_chooseVariable->setVisible(true);
        ui->label_varIndex->setVisible(true);
        ui->lineEdit_varIndex->setVisible(true);
        ui->lineEdit_command_2->setReadOnly(true);
        ui->lineEdit_command_2->setValidator(new QRegExpValidator(QRegExp()));
        ui->lineEdit_command_2->setMaxLength(100);
        ui->lineEdit_command_2->clear();
        ui->lineEdit_varIndex->setText("0");
    }
}

void AddCommandDlg::on_pushButton_chooseVariable_clicked()
{
    SelectSignalDlg selectSignalDlg(_infoList);
    QString destStr = ui->lineEdit_command_2->text();

    QString param1 = "";
    QString param2 = "";
    if(!destStr.isEmpty())
    {
        param1 = destStr.split(",").at(0);
        param2 = destStr.split(",").at(1);
    }

    selectSignalDlg.setInfo(param1, param2);
    int ret = selectSignalDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
        ICDSignal icdSignal = selectSignalDlg.getInfo();
        _vvCommand->_data_type_r = icdSignal.signalInfo_.strType_;
        QString str = tr("%1,%2").arg(icdSignal.messageName_).arg(icdSignal.signalInfo_.signalID_); //!由于执行端代码问题，这里第一个使用Name第二个使用ID、
        ui->lineEdit_command_2->setText(str);
        ui->lineEdit_command_2->setToolTip(str);
        break;
    }
}
