#include "SubConditionWidget.h"
#include "ui_SubConditionWidget.h"

#include "ConditionRelationShipConfigWidget.h"
#include "SubConditionItemWidget.h"
#include <QPushButton>

SubConditionWidget::SubConditionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubConditionWidget)
{
    ui->setupUi(this);

    initData();//初始化数据
    initView();//初始化界面
    initViewData();//初始化界面数据
}

SubConditionWidget::~SubConditionWidget()
{
    delete ui;
}

void SubConditionWidget::setSubConditionList(QList<DataValueSubConditionItem> *subConditionList)
{
    m_subConditionList = subConditionList;
    if(m_subConditionList && m_subConditionList->size()>=1)
        m_subConditionKey = m_subConditionList->first().json_expression.parameter;//记录参数1

    //条件关系配置
    ui->widget_conditionRelationShipConfig->setConditionList(m_subConditionList);

    initViewData();
}

QList<DataValueSubConditionItem> *SubConditionWidget::getSubConditionList() const
{
    return m_subConditionList;
}

void SubConditionWidget::addSubCondition(DataValueSubConditionItem subCondition)
{
    if(!m_subConditionList)
        return;
    m_subConditionList->append(subCondition);
    SubConditionItemWidget *subConditionItemWidget = new SubConditionItemWidget(this);
    subConditionItemWidget->setCondition(&m_subConditionList->last().json_expression);
    subConditionItemWidget->setConditionName(QStringLiteral("子条件")+QString::number(ui->verticalLayout->count()));
    connect(subConditionItemWidget,&SubConditionItemWidget::signal_condition_remove,this,&SubConditionWidget::slot_removeSubCondition);
    ui->verticalLayout->insertWidget(ui->verticalLayout->count()-1,subConditionItemWidget);

    //更新条件关系
    ui->widget_conditionRelationShipConfig->slot_upDataView();
}

void SubConditionWidget::initData()
{
    m_subConditionList = nullptr;
}

void SubConditionWidget::initView()
{

}

void SubConditionWidget::initViewData()
{
    //更新条件关系
    ui->widget_conditionRelationShipConfig->slot_upDataView();

    //清空
    clearLayout(ui->verticalLayout);

    if(!m_subConditionList)
        return;

    for(int index=0;index<m_subConditionList->size();index++){

        SubConditionItemWidget *subConditionItemWidget = new SubConditionItemWidget(this);
        subConditionItemWidget->setCondition(&((*m_subConditionList)[index].json_expression));
        subConditionItemWidget->setConditionName(QStringLiteral("子条件")+QString::number(index+1));
        connect(subConditionItemWidget,&SubConditionItemWidget::signal_condition_remove,this,&SubConditionWidget::slot_removeSubCondition);

        ui->verticalLayout->addWidget(subConditionItemWidget,index,0);
    }

    ui->verticalLayout->addStretch();
}

void SubConditionWidget::clearLayout(QLayout *layout)
{
    while(layout->count()>0){
        QLayoutItem * item = layout->takeAt(0);
        if(item){
            if(item->widget()){
                item->widget()->setParent(nullptr);
                item->widget()->deleteLater();
            }

            delete item;
            item = nullptr;
        }
    }
}

QString SubConditionWidget::getSubConditionKey() const
{
    return m_subConditionKey;
}

void SubConditionWidget::slot_removeSubCondition(DataValueSubCondition *m_condition)
{
    if(m_subConditionList)
    {
        for(int index=0;index<m_subConditionList->size();index++){
            if(&(*m_subConditionList)[index].json_expression == m_condition){
                m_subConditionList->removeAt(index);
                initViewData();
                break;
            }
        }
        //子条件个数为0
        if(m_subConditionList->size() == 0){
            emit signal_subConditionList_remove(m_subConditionList);
        }
    }
}
