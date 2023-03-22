#include "SubConditionItemWidget.h"
#include "ui_SubConditionItemWidget.h"


SubConditionItemWidget::SubConditionItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubConditionItemWidget)
{
    ui->setupUi(this);

    m_condition = nullptr;

    ui->widget_conditionConfigWidget->setArg1_configurable(false);
}

SubConditionItemWidget::~SubConditionItemWidget()
{
    delete ui;
}

void SubConditionItemWidget::setCondition(DataValueSubCondition *condition)
{
    m_condition = condition;
    ui->widget_conditionConfigWidget->setCondition(condition);
}

void SubConditionItemWidget::setConditionName(const QString &name)
{
    ui->groupBox_name->setTitle(name);
}

void SubConditionItemWidget::on_pushButton_remove_clicked()
{
    emit signal_condition_remove(m_condition);
    this->deleteLater();
}
