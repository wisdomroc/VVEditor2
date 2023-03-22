#include "ConditionRelationShipConfigWidget.h"
#include "ui_ConditionRelationShipConfigWidget.h"

#include "ConditionRelationShipWidget.h"

ConditionRelationShipConfigWidget::ConditionRelationShipConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConditionRelationShipConfigWidget)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_StyledBackground);

    initData();//初始化数据
    initView();//初始化界面
    initViewData();//初始化界面数据
}

ConditionRelationShipConfigWidget::~ConditionRelationShipConfigWidget()
{
    delete ui;
}

void ConditionRelationShipConfigWidget::setConditionList(QList<DataValueConditionItemInfo> *conditionList)
{
    initData();
    m_conditionList = conditionList;
    initViewData();
}

void ConditionRelationShipConfigWidget::setConditionList(QList<DataValueSubConditionItem> *conditionList)
{
    initData();
    m_subConditionList = conditionList;
    initViewData();
}

void ConditionRelationShipConfigWidget::slot_upDataView()
{
    initViewData();
}

void ConditionRelationShipConfigWidget::initData()
{
    m_conditionList = nullptr;
    m_subConditionList = nullptr;
}

void ConditionRelationShipConfigWidget::initView()
{

}

void ConditionRelationShipConfigWidget::initViewData()
{
    clearLayout(ui->gridLayout);

    int row = 0;
    int column = 0;
    if(m_conditionList){
        for(int index=0;index<m_conditionList->size();index++){
            ConditionRelationShipWidget *conditionRelationShipWidget = new ConditionRelationShipWidget(this);
            conditionRelationShipWidget->setContentsMargins(0,0,0,0);
            conditionRelationShipWidget->setConditionName(QStringLiteral("条件")+QString::number(index+1));
            conditionRelationShipWidget->setCondition(&((*m_conditionList)[index]));

            if(index == m_conditionList->size()-1)
                conditionRelationShipWidget->setLogicalOperatorVisiable(false);

            row = (index-index%10)/10;
            column = index%10;

            ui->gridLayout->addWidget(conditionRelationShipWidget,row,column);
        }
    }
    if(m_subConditionList){
        for(int index=0;index<m_subConditionList->size();index++){
            ConditionRelationShipWidget *conditionRelationShipWidget = new ConditionRelationShipWidget(this);
            conditionRelationShipWidget->setContentsMargins(0,0,0,0);
            conditionRelationShipWidget->setConditionName(QStringLiteral("子条件")+QString::number(index+1));
            conditionRelationShipWidget->setCondition(&((*m_subConditionList)[index]));

            if(index == m_subConditionList->size()-1)
                conditionRelationShipWidget->setLogicalOperatorVisiable(false);

            row = (index-index%10)/10;
            column = index%10;

            ui->gridLayout->addWidget(conditionRelationShipWidget,row,column);
        }
    }
    QHBoxLayout *hBox = new QHBoxLayout(this);
    hBox->addStretch();
    ui->gridLayout->addLayout(hBox,0,10);
    this->update();
}

void ConditionRelationShipConfigWidget::clearLayout(QLayout *layout)
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
