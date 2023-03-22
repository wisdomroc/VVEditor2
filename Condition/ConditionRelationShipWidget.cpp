#include "ConditionRelationShipWidget.h"
#include "ui_ConditionRelationShipWidget.h"

ConditionRelationShipWidget::ConditionRelationShipWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConditionRelationShipWidget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_StyledBackground);

    initData();//初始化数据
    initView();//初始化界面
    initViewData();//初始化界面数据
}

ConditionRelationShipWidget::~ConditionRelationShipWidget()
{
    delete ui;
}

void ConditionRelationShipWidget::setCondition(DataValueSubConditionItem *data)
{
    initData();
    m_subCondition = data;
    initViewData();
}

void ConditionRelationShipWidget::setCondition(DataValueConditionItemInfo *data)
{
    initData();
    m_condition = data;
    initViewData();
}

void ConditionRelationShipWidget::setConditionName(const QString &conditionName)
{
    ui->label_conditionName->setText(conditionName);
}

void ConditionRelationShipWidget::setLogicalOperatorVisiable(bool visiable)
{
    ui->comboBox_logicalOperator->setVisible(visiable);
    if(!visiable){
        if(m_condition){
            m_condition->logical_operator = "";
        }
        if(m_subCondition){
            m_subCondition->logical_operator = "";
        }
    }
}

void ConditionRelationShipWidget::initData()
{
    m_subCondition = nullptr;
    m_condition = nullptr;
}

void ConditionRelationShipWidget::initView()
{

}

void ConditionRelationShipWidget::initViewData()
{
    if(m_condition){
        ui->comboBox_logicalOperator->setCurrentText(m_condition->logical_operator);
    }
    if(m_subCondition){
        ui->comboBox_logicalOperator->setCurrentText(m_subCondition->logical_operator);
    }

    on_comboBox_logicalOperator_currentTextChanged(ui->comboBox_logicalOperator->currentText());
}

void ConditionRelationShipWidget::on_comboBox_logicalOperator_currentTextChanged(const QString &arg1)
{
    if(m_condition){
        m_condition->logical_operator = arg1;
    }
    if(m_subCondition){
        m_subCondition->logical_operator = arg1;
    }
}
