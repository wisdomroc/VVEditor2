#include "ConditionConfigWidget.h"
#include "ui_ConditionConfigWidget.h"

#include "SelectArguments/SelectArgumentsWidget.h"

#include "ConditionCommon.h"

#include "ConditionWidget.h"
#include "select_signal_dlg.h"
#include <QDebug>
QList<ICDInfo> ConditionConfigWidget::_icdInfoList={};
ConditionConfigWidget::ConditionConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConditionConfigWidget)
{
    ui->setupUi(this);

    initData();//初始化数据
    initView();//初始化界面
    initViewData();//初始化界面数据

//    //! 临时限制
//    QRegExp zeroRegExp(tr("^(0?)$"));
//    ui->lineEdit_index1->setValidator(new QRegExpValidator(zeroRegExp));
//    ui->lineEdit_index2->setValidator(new QRegExpValidator(zeroRegExp));
//    ui->lineEdit_index1->setToolTip(QStringLiteral("条件库暂不支持非零下标"));
//    ui->lineEdit_index2->setToolTip(QStringLiteral("条件库暂不支持非零下标"));
}

ConditionConfigWidget::~ConditionConfigWidget()
{
    delete ui;
}

void ConditionConfigWidget::setIcdsInfo(const QList<ICDInfo> &icdsInfo)
{
    _icdInfoList = icdsInfo;
}

void ConditionConfigWidget::setCondition(DataValueSubCondition *condition)
{
    m_condition = condition;
    if(m_condition){
        m_dataValueSubCondition = *m_condition;
    }

    initViewData();
}

DataValueSubCondition ConditionConfigWidget::getCondition()
{
    if(m_condition)
        return *m_condition;
    return m_dataValueSubCondition;
}

void ConditionConfigWidget::setArg1_configurable(bool configurable)
{
    ui->pushButton_selectArg1->setVisible(configurable);
}

void ConditionConfigWidget::initData()
{
    m_condition = nullptr;
}

void ConditionConfigWidget::initView()
{
    m_dataValueSubCondition.comparison_operator = ui->comboBox_comparisonOperators->currentText();
}

void ConditionConfigWidget::initViewData()
{
    QString valueKey;
    QString value;

    //参数1
    getArgumentsValue(m_dataValueSubCondition.parameter,valueKey,value);

    ui->lineEdit_arg1->setText(value);

    //比较符
    ui->comboBox_comparisonOperators->setCurrentText(m_dataValueSubCondition.comparison_operator);
    ui->lineEdit_index1->setText(m_dataValueSubCondition.index1);
    ui->lineEdit_index2->setText(m_dataValueSubCondition.index2);

    //参数2
    getArgumentsValue(m_dataValueSubCondition.comparison_value,valueKey,value);

    switch (getArgumentsType(valueKey)) {
    case ArgumentsType::FMU:
    case ArgumentsType::ICD:{
        m_variable = m_dataValueSubCondition.comparison_value;
        ui->radioButton_variable->setChecked(true);
        break;
    }
    default:{
        m_constant = m_dataValueSubCondition.comparison_value;
        ui->radioButton_constant->setChecked(true);
        break;
    }
    }

    ui->lineEdit_arg2->setText(value);
}

ArgumentsType ConditionConfigWidget::getArgumentsType(const QString &arguments)
{
    if(arguments.compare("Value") == 0){
        return ArgumentsType::Value;
    }else if(arguments.compare("ICD") == 0){
        return ArgumentsType::ICD;
    }else if(arguments.compare("FMU") == 0){
        return ArgumentsType::FMU;
    }else
    {
        return ArgumentsType::NONE;
        qDebug()<<__FUNCTION__<<QStringLiteral("未识别参数种类");
    }
}

void ConditionConfigWidget::getArgumentsValue(QString arguments, QString &key, QString &value)
{
    QRegularExpression regExp("\{\"(?<key>.+)\":\"(?<value>.+)\"\}");

    QRegularExpressionMatch match = regExp.match(arguments);

    if(match.hasMatch()){
        key = match.captured("key");
        value = match.captured("value");
    }
    else {
        key = "";
        value = "";
    }

    switch (getArgumentsType(key)) {
    case ArgumentsType::FMU:{
        QRegularExpression regExp("/(?<uuid>[^/]*)/[^/]*$");
        QString uuid = regExp.match(value).captured("uuid");
        QString name = ConditionCommon::getInstance()->getFumInfo().value(uuid).name;
        value.replace(uuid,name);
        break;
    }
    default:
        break;
    }
}

void ConditionConfigWidget::on_radioButton_constant_toggled(bool checked)
{
    if(checked){
        m_dataValueSubCondition.comparison_value = m_constant;
        if(m_condition){
            m_condition->comparison_value = m_constant;
        }

        QString valueKey;
        QString value;
        getArgumentsValue(m_dataValueSubCondition.comparison_value,valueKey,value);

        ui->lineEdit_arg2->setText(value);
        ui->lineEdit_arg2->setReadOnly(false);
        ui->pushButton_selectArg2->setHidden(checked);
        ui->lineEdit_index2->setVisible(false);
        ui->lineEdit_index2->setText("0");
    }

}

void ConditionConfigWidget::on_radioButton_variable_toggled(bool checked)
{
    if(checked){
        m_dataValueSubCondition.comparison_value = m_variable;
        if(m_condition){
            m_condition->comparison_value = m_variable;
        }

        QString valueKey;
        QString value;
        getArgumentsValue(m_dataValueSubCondition.comparison_value,valueKey,value);

        ui->lineEdit_arg2->setText(value);
        ui->lineEdit_arg2->setReadOnly(true);
        ui->pushButton_selectArg2->setVisible(checked);
        ui->lineEdit_index2->setVisible(true);
    }
}

void ConditionConfigWidget::on_pushButton_selectArg1_clicked()
{
    SelectSignalDlg selectSignalDlg(_icdInfoList);
    int ret = selectSignalDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
        ICDSignal icdSignal = selectSignalDlg.getInfo();
        slot_arguments1_changed(icdSignal);
        break;
    }


}

void ConditionConfigWidget::on_comboBox_comparisonOperators_currentTextChanged(const QString &arg1)
{
    m_dataValueSubCondition.comparison_operator = arg1;
    if(m_condition){
        m_condition->comparison_operator = arg1;
    }
}

void ConditionConfigWidget::on_pushButton_selectArg2_clicked()
{
    SelectSignalDlg selectSignalDlg(_icdInfoList);
    int ret = selectSignalDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
        ICDSignal icdSignal = selectSignalDlg.getInfo();
        slot_arguments2_changed(icdSignal);
        break;
    }
}

void ConditionConfigWidget::slot_arguments1_changed(ICDSignal icdSignal)
{
    QString arguments = "{\"ICD\":\""+icdSignal.messageName_+"/"+icdSignal.signalInfo_.signalID_+"\"}";
    m_dataValueSubCondition.parameter = arguments;
    if(m_condition){
        m_condition->parameter = arguments;
    }

    QString valueKey;
    QString value;
    getArgumentsValue(m_dataValueSubCondition.parameter,valueKey,value);
    ui->lineEdit_arg1->setText(value);
}

void ConditionConfigWidget::slot_arguments2_changed(ICDSignal icdSignal)
{
    QString arguments = "{\"ICD\":\""+icdSignal.messageName_+"/"+icdSignal.signalInfo_.signalID_+"\"}";
    m_dataValueSubCondition.comparison_value = arguments;
    m_variable = m_dataValueSubCondition.comparison_value;
    if(m_condition){
        m_condition->comparison_value = m_dataValueSubCondition.comparison_value;
    }

    QString valueKey;
    QString value;
    getArgumentsValue(m_dataValueSubCondition.comparison_value,valueKey,value);
    ui->lineEdit_arg2->setText(value);

}

void ConditionConfigWidget::on_lineEdit_arg2_textEdited(const QString &arg1)
{
    m_dataValueSubCondition.comparison_value = "{\"Value\":\""+arg1+"\"}";
    m_constant = m_dataValueSubCondition.comparison_value;
    if(m_condition){
        m_condition->comparison_value = m_dataValueSubCondition.comparison_value;
    }
}

void ConditionConfigWidget::on_lineEdit_index1_editingFinished()
{
    m_dataValueSubCondition.index1 = ui->lineEdit_index1->text();
    if(m_condition)
    {
        m_condition->index1 = ui->lineEdit_index1->text();
    }
}

void ConditionConfigWidget::on_lineEdit_index2_editingFinished()
{
    m_dataValueSubCondition.index2 = ui->lineEdit_index2->text();
    if(m_condition)
    {
        m_condition->index2 = ui->lineEdit_index2->text();
    }
}
