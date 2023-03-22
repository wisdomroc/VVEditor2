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
    void initData();//��ʼ������
    void initView();//��ʼ������
    void initViewData();//��ʼ����������

    QString getItemPath(QStandardItem *item);
private:
    Ui::SelectVariableGroupWidget *ui;

};

#endif // SELECTVARIABLEGROUPWIDGET_H
