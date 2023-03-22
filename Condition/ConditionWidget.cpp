#include "ConditionWidget.h"
#include "ui_ConditionWidget.h"

#include "SubConditionWidget.h"

#include "DataValueJsonExpression/datavaluejsonexpression.h"

#include "ConditionCommon.h"

#include "ConditionRelationShipConfigWidget.h"

#include <QMessageBox>


ConditionWidget::ConditionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConditionWidget)
{
    ui->setupUi(this);

    initData();//初始化数据
    initView();//初始化界面
    initViewData();//初始化界面数据
}

ConditionWidget::~ConditionWidget()
{
    delete ui;
}

void ConditionWidget::setConditionExpression(QString &conditionExpression)
{
    DataValueJsonExpression dataValueJsonExpression;
    dataValueJsonExpression.toConditionStruct(conditionExpression,m_conditionList);
    initViewData();
}

void ConditionWidget::setModel(const QStandardItemModel *model)
{
    ConditionCommon::getInstance()->setModel(model);
}

void ConditionWidget::setIcdsInfo(const QList<ICDInfo> &icdsInfo)
{
    _icdInfoList = icdsInfo;
    ui->widget_conditionConfig->setIcdsInfo(_icdInfoList);
}

void ConditionWidget::initData()
{

}

void ConditionWidget::initView()
{
    ui->tabWidget_condition->setTabsClosable(true);
}

void ConditionWidget::initViewData()
{
    ui->widget_conditionRelationShipConfig->setConditionList(&m_conditionList);

    ui->tabWidget_condition->clear();
    for(int i=0;i<m_conditionList.size();i++){
        SubConditionWidget *subConditionWidget = new SubConditionWidget(this);
        subConditionWidget->setSubConditionList(&m_conditionList[i].sub_item);
        connect(subConditionWidget,&SubConditionWidget::signal_subConditionList_remove,this,&ConditionWidget::slot_subConditionList_remove);
        ui->tabWidget_condition->addTab(subConditionWidget,QStringLiteral("条件")+QString::number(i+1));
    }
}

void ConditionWidget::on_pushButton_addCondition_clicked()
{
    DataValueSubConditionItem dataValueSubConditionItem;
    dataValueSubConditionItem.json_expression = ui->widget_conditionConfig->getCondition();
    if(dataValueSubConditionItem.json_expression.parameter.isEmpty() ||
            dataValueSubConditionItem.json_expression.comparison_value.isEmpty()){
        QMessageBox::critical(this,QStringLiteral("无法添加"),QStringLiteral("条件无效"), QStringLiteral("确定"));
        return;
    }

    bool existing = false;
    int tabCount = ui->tabWidget_condition->count();
    for(int index=0;index<tabCount;index++){
        SubConditionWidget *subConditionWidget = dynamic_cast<SubConditionWidget *>(ui->tabWidget_condition->widget(index));
        connect(subConditionWidget,&SubConditionWidget::signal_subConditionList_remove,this,&ConditionWidget::slot_subConditionList_remove);
        if(subConditionWidget && subConditionWidget->getSubConditionKey().compare(dataValueSubConditionItem.json_expression.parameter) == 0){
            existing = true;
            subConditionWidget->addSubCondition(dataValueSubConditionItem);
        }
    }

    if(!existing){
        DataValueConditionItemInfo dataValueConditionItemInfo;
        dataValueConditionItemInfo.sub_item.append(dataValueSubConditionItem);
        m_conditionList.append(dataValueConditionItemInfo);

        SubConditionWidget *subConditionWidget = new SubConditionWidget(this);

        subConditionWidget->setSubConditionList(&m_conditionList.last().sub_item);
        connect(subConditionWidget,&SubConditionWidget::signal_subConditionList_remove,this,&ConditionWidget::slot_subConditionList_remove);
        ui->tabWidget_condition->addTab(subConditionWidget,QStringLiteral("条件")+QString::number(ui->tabWidget_condition->count()+1));

        ui->widget_conditionRelationShipConfig->slot_upDataView();
    }
}

void ConditionWidget::on_pushButton_confirm_clicked()
{
    QString conditionExpression;
    DataValueJsonExpression dataValueJsonExpression;
    dataValueJsonExpression.toComditionExpression(m_conditionList,conditionExpression);
    signal_condition_confirm(conditionExpression);
    this->close();
}

void ConditionWidget::on_pushButton_cancel_clicked()
{
    this->close();
}

void ConditionWidget::on_tabWidget_condition_tabCloseRequested(int index)
{
    SubConditionWidget *subConditionWidget = dynamic_cast<SubConditionWidget *>(ui->tabWidget_condition->widget(index));
    if(subConditionWidget){
        for(int i=0;i<m_conditionList.size();i++){
            if(&m_conditionList[i].sub_item == subConditionWidget->getSubConditionList())
            {
                m_conditionList.removeAt(i);
                break;
            }
        }
    }
    ui->tabWidget_condition->removeTab(index);
    subConditionWidget->deleteLater();

    for(int i=0;i<ui->tabWidget_condition->count();i++){
        ui->tabWidget_condition->setTabText(i,QStringLiteral("条件")+QString::number(i+1));
    }

    ui->widget_conditionRelationShipConfig->slot_upDataView();
}

void ConditionWidget::slot_subConditionList_remove(QList<DataValueSubConditionItem> *subConditionList)
{
    on_tabWidget_condition_tabCloseRequested(ui->tabWidget_condition->currentIndex());
}
