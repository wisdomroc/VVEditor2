#ifndef CONDITIONRELATIONSHIPWIDGET_H
#define CONDITIONRELATIONSHIPWIDGET_H

#include <QWidget>

#include "DataValueJsonExpression/datavaluejsonexpression.h"

namespace Ui {
class ConditionRelationShipWidget;
}

class ConditionRelationShipWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionRelationShipWidget(QWidget *parent = nullptr);
    ~ConditionRelationShipWidget();

    /**
     * @brief setCondition 配置条件
     * @param data 条件信息
     */
    void setCondition(DataValueSubConditionItem *data);
    void setCondition(DataValueConditionItemInfo *data);

    /**
     * @brief setConditionName 配置条件名称
     * @param conditionName 条件名称
     */
    void setConditionName(const QString &conditionName);

    void setLogicalOperatorVisiable(bool visiable);

private slots:
    void on_comboBox_logicalOperator_currentTextChanged(const QString &arg1);

private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据
private:
    Ui::ConditionRelationShipWidget *ui;
    DataValueSubConditionItem *m_subCondition;
    DataValueConditionItemInfo *m_condition;
};

#endif // CONDITIONRELATIONSHIPWIDGET_H
