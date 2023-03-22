#ifndef CONDITIONWIDGET_H
#define CONDITIONWIDGET_H


#include <QWidget>
#include "DataValueJsonExpression/datavaluejsonexpression.h"
//#include "vv_common.h"
#include "vv_typedef.h"
class QStandardItemModel;
namespace Ui {
class ConditionWidget;
}

class  ConditionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionWidget(QWidget *parent = nullptr);
    ~ConditionWidget();

    void setConditionExpression(QString &conditionExpression);
    void setModel(const QStandardItemModel *model);

    /**
     * @brief setIcdsInfo icd信息
     * @param icdsInfo
     */
    void setIcdsInfo(const QList<ICDInfo> &icdsInfo);
    QList<ICDInfo> _icdInfoList;//icd信息
signals:
    void signal_condition_confirm(QString conditionExpression);
private slots:
    void on_pushButton_addCondition_clicked();

    void on_pushButton_confirm_clicked();

    void on_pushButton_cancel_clicked();

    void on_tabWidget_condition_tabCloseRequested(int index);

    void slot_subConditionList_remove(QList<DataValueSubConditionItem> *subConditionList);
private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据
private:
    Ui::ConditionWidget *ui;
    QList<DataValueConditionItemInfo> m_conditionList;
};

#endif // CONDITIONWIDGET_H
