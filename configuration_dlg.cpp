#include "configuration_dlg.h"
#include "ui_configuration_dlg.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"

ConfigurationDlg::ConfigurationDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDlg)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("系统配置"));
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

ConfigurationDlg::~ConfigurationDlg()
{
    delete ui;
}

void ConfigurationDlg::getWorkspaceAndIwrpInfo(QString &workspace, QString &iwrp)
{
    workspace = _workspace;
    iwrp      = _iwrp;
}

void ConfigurationDlg::on_pushButton_confirm_clicked()
{
    _workspace = ui->lineEdit_workSpace->text();
    _iwrp      = ui->lineEdit_project->text();

    if(_workspace.isEmpty() || _iwrp.isEmpty())
    {
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("工作空间和项目文件均不能为空！"));
        return;
    }

    this->accept();
}

void ConfigurationDlg::on_pushButton_cancel_clicked()
{
    this->reject();
}

void ConfigurationDlg::on_pushButton_selectProject_clicked()
{
    QString file = QFileDialog::getOpenFileName(nullptr,QStringLiteral("选择项目文件"),QString());
    if(!file.isEmpty())
    {
        ui->lineEdit_project->setText(QDir::toNativeSeparators(file));
    }
}

void ConfigurationDlg::on_pushButton_selectWorkSpace_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr,QStringLiteral("选择工作空间"));
    if(!dir.isEmpty())
    {
        ui->lineEdit_workSpace->setText(QDir::toNativeSeparators(dir));
    }
}
