#ifndef SUBCONDITIONWIDGET_H
#define SUBCONDITIONWIDGET_H

#include <QWidget>
#include "DataValueJsonExpression/datavaluejsonexpression.h"

namespace Ui {
class SubConditionWidget;
}

class SubConditionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubConditionWidget(QWidget *parent = nullptr);
    ~SubConditionWidget();

    /**
     * @brief setSubConditionList 配置子条件列表
     * @param subConditonList 子条件列表
     */
    void setSubConditionList(QList<DataValueSubConditionItem> *subConditionList);

    QList<DataValueSubConditionItem> *getSubConditionList() const;
    /**
     * @brief addSubCondition 添加子条件
     * @param subCondition 子条件
     */
    void addSubCondition(DataValueSubConditionItem subCondition);

    /**
     * @brief subConditionKey 获取子条件的关键字
     * @return 子条件关键字
     */
    QString getSubConditionKey() const;


signals:
    void signal_subConditionList_changed();
    void signal_subConditionList_remove(QList<DataValueSubConditionItem> *subConditionList);
private slots:
    void slot_removeSubCondition(DataValueSubCondition *m_condition);
private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据

    /**
     * @brief clearLayout 清空Layout的widget
     * @param layout 需要清空的Layout
     */
    void clearLayout(QLayout *layout);
private:
    Ui::SubConditionWidget *ui;
    QList<DataValueSubConditionItem> *m_subConditionList;
    QString m_subConditionKey;//子条件关键字
};

#endif // SUBCONDITIONWIDGET_H
