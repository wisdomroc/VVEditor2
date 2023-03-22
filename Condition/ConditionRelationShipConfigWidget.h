#ifndef CONDITIONRELATIONSHIPCONFIGWIDGET_H
#define CONDITIONRELATIONSHIPCONFIGWIDGET_H

#include <QLayout>
#include <QWidget>

#include "DataValueJsonExpression/datavaluejsonexpression.h"


namespace Ui {
class ConditionRelationShipConfigWidget;
}

class ConditionRelationShipConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionRelationShipConfigWidget(QWidget *parent = nullptr);
    ~ConditionRelationShipConfigWidget();

    /**
     * @brief setConditionList 配置条件列表
     * @param conditionList 条件列表
     */
    void setConditionList(QList<DataValueConditionItemInfo> *conditionList);
    void setConditionList(QList<DataValueSubConditionItem> *conditionList);
public slots:
    /**
     * @brief slot_upDataView 更新界面
     */
    void slot_upDataView();
private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据
private:
    Ui::ConditionRelationShipConfigWidget *ui;
    QList<DataValueConditionItemInfo> *m_conditionList;
    QList<DataValueSubConditionItem> *m_subConditionList;
private:
    /**
     * @brief clearLayout 清空Layout的widget
     * @param layout 需要清空的Layout
     */
    void clearLayout(QLayout *layout);
};

#endif // CONDITIONRELATIONSHIPCONFIGWIDGET_H
