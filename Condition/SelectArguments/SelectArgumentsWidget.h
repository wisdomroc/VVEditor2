#ifndef SELECTARGUMENTSWIDGET_H
#define SELECTARGUMENTSWIDGET_H

#include <QWidget>

namespace Ui {
class SelectArgumentsWidget;
}

class QStandardItemModel;

class SelectArgumentsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectArgumentsWidget(QWidget *parent = nullptr);
    ~SelectArgumentsWidget();

    void setModel(const QStandardItemModel *model);
signals:
    void signal_selection_confirm(QString arguments);
private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_confirm_clicked();

    void slot_selection_changed(bool haveSelection);

private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据
private:

    Ui::SelectArgumentsWidget *ui;
};

#endif // SELECTARGUMENTSWIDGET_H
