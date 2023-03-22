#ifndef CONDITIONCONFIGWIDGET_H
#define CONDITIONCONFIGWIDGET_H

#include <QWidget>

#include "DataValueJsonExpression/datavaluejsonexpression.h"

#include "vv_typedef.h"

/**
 * @brief The ArgumentsType enum 参数类型
 */
enum ArgumentsType{
    NONE,
    FMU,
    ICD,
    Value,
};

namespace Ui {
class ConditionConfigWidget;
}

class ConditionConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConditionConfigWidget(QWidget *parent = nullptr);
    ~ConditionConfigWidget();

    /**
     * @brief setCondition 设置条件信息
     * @param condition 条件信息
     */
    void setCondition(DataValueSubCondition *condition);
    DataValueSubCondition getCondition();

    /**
     * @brief setArg1_configurable 设置参数1是否可配置
     * @param configurable true可配置,false不可配置
     */
    void setArg1_configurable(bool configurable);


    void setIcdsInfo(const QList<ICDInfo> &icdsInfo);

private slots:
    void on_radioButton_constant_toggled(bool checked);//参数2常量

    void on_radioButton_variable_toggled(bool checked);//参数2变量

    void on_pushButton_selectArg1_clicked();//选择参数1

    void on_comboBox_comparisonOperators_currentTextChanged(const QString &arg1);//修改比较运算符

    void on_pushButton_selectArg2_clicked();//选择参数2

    void slot_arguments1_changed(ICDSignal icdSignal);//参数1确认

    void slot_arguments2_changed(ICDSignal icdSignal);//参数2确认

    void on_lineEdit_arg2_textEdited(const QString &arg1);

    void on_lineEdit_index1_editingFinished();

    void on_lineEdit_index2_editingFinished();

private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据

    /**
     * @brief getArgumentsType 获取参数类型
     * @param arguments 参数字符串
     * @return 参数类型
     */
    ArgumentsType getArgumentsType(const QString &arguments);

    void getArgumentsValue(QString arguments,QString &key,QString &value);
private:
    Ui::ConditionConfigWidget *ui;
    static QList<ICDInfo> _icdInfoList;//icd信息
    DataValueSubCondition *m_condition;
    DataValueSubCondition m_dataValueSubCondition;
    QString m_constant;//常量
    QString m_variable;//变量
};

#endif // CONDITIONCONFIGWIDGET_H
