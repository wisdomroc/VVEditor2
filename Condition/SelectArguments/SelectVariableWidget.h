#ifndef SELECTVARIABLEWIDGET_H
#define SELECTVARIABLEWIDGET_H

#include <QWidget>
#include "SelectArgumentsCommon.h"

namespace Ui {
class SelectVariableWidget;
}

class SelectVariableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectVariableWidget(QWidget *parent = nullptr);
    ~SelectVariableWidget();

    /**
     * @brief getSelectedVariable 获取被选择的变量
     * @return 变量信息
     */
    QString getSelectedVariable();
signals:
    void signal_selection_changed(bool haveSelection);
    void signal_selection_confirm();
public slots:
    void slot_selectedVariableGroup_changed(VariableGroup variableGroup);
private slots:

    void on_comboBox_variableGroup_currentTextChanged(const QString &arg1);

    void on_listWidget_variable_itemSelectionChanged();

    void on_listWidget_variable_doubleClicked(const QModelIndex &index);

private:
    Ui::SelectVariableWidget *ui;
};

#endif // SELECTVARIABLEWIDGET_H
