#include "SelectArgumentsWidget.h"
#include "ui_SelectArgumentsWidget.h"

#include "SelectVariableGroupWidget.h"
#include "SelectVariableWidget.h"

#include <QDebug>

SelectArgumentsWidget::SelectArgumentsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectArgumentsWidget)
{
    ui->setupUi(this);

    initData();
    initView();
    initViewData();
}

SelectArgumentsWidget::~SelectArgumentsWidget()
{
    delete ui;
}

void SelectArgumentsWidget::setModel(const QStandardItemModel *model)
{
    ui->widget_selectVariableGroup->setVariableGroupModel(model);
}

void SelectArgumentsWidget::initData()
{

}

void SelectArgumentsWidget::initView()
{
    ui->pushButton_confirm->setEnabled(false);

    connect(ui->widget_selectVariableGroup,&SelectVariableGroupWidget::signal_selectedVariableGroup,
            ui->widget_selectVariable,&SelectVariableWidget::slot_selectedVariableGroup_changed);

    connect(ui->widget_selectVariable,&SelectVariableWidget::signal_selection_changed,this,&SelectArgumentsWidget::slot_selection_changed);
    connect(ui->widget_selectVariable,&SelectVariableWidget::signal_selection_confirm,
            this,&SelectArgumentsWidget::on_pushButton_confirm_clicked);

}

void SelectArgumentsWidget::initViewData()
{

}

void SelectArgumentsWidget::on_pushButton_cancel_clicked()
{
    this->close();
}

void SelectArgumentsWidget::on_pushButton_confirm_clicked()
{
    QString arguments = ui->widget_selectVariable->getSelectedVariable();
    if(arguments.isEmpty())
    {
        qDebug()<<__FUNCTION__<<"arguments.isEmpty()"<<QStringLiteral("²ÎÊýÎª¿Õ");
    }else{
        emit signal_selection_confirm(arguments);
        this->close();
    }
}

void SelectArgumentsWidget::slot_selection_changed(bool haveSelection)
{
    ui->pushButton_confirm->setEnabled(haveSelection);
}
