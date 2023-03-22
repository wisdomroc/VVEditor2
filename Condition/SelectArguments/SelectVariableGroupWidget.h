#ifndef SELECTVARIABLEGROUPWIDGET_H
#define SELECTVARIABLEGROUPWIDGET_H

#include <QWidget>

#include "SelectArgumentsCommon.h"

class QStandardItem;

namespace Ui {
class SelectVariableGroupWidget;
}

class QStandardItemModel;

class SelectVariableGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectVariableGroupWidget(QWidget *parent = nullptr);
    ~SelectVariableGroupWidget();

    void setVariableGroupModel(const QStandardItemModel *model);
signals:
    void signal_selectedVariableGroup(VariableGroup variableGroup);
private slots:
    void on_treeView_variableGroup_doubleClicked(const QModelIndex &index);

    void on_treeView_variableGroup_clicked(const QModelIndex &index);

private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据

    QString getItemPath(QStandardItem *item);
private:
    Ui::SelectVariableGroupWidget *ui;

};

#endif // SELECTVARIABLEGROUPWIDGET_H
