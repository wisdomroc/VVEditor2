#ifndef SUBCONDITIONITEMWIDGET_H
#define SUBCONDITIONITEMWIDGET_H

#include <QWidget>

#include "ConditionConfigWidget.h"
namespace Ui {
class SubConditionItemWidget;
}

class SubConditionItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubConditionItemWidget(QWidget *parent = nullptr);
    ~SubConditionItemWidget();

    /**
     * @brief setCondition 设置条件信息
     * @param condition 条件信息
     */
    void setCondition(DataValueSubCondition *condition);

    void setConditionName(const QString &name);
signals:
    void signal_condition_remove(DataValueSubCondition *m_condition);
private slots:
    void on_pushButton_remove_clicked();

private:
    Ui::SubConditionItemWidget *ui;
    DataValueSubCondition *m_condition;
};

#endif // SUBCONDITIONITEMWIDGET_H
