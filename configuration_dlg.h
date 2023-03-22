/***********************************************************************
* @filename configuration_dlg.h
* @brief    先于主界面的一个配置窗口
*           如果不是从WRP中点击启动的VVEditor，那么该窗口会先弹出
*           以便配置工作空间 和 项目文件
*           如果不配置工作空间，则ICD无法初始化
*           如果不配置项目文件，则RESET命令下方的项目树中则没有内容
*           之前遗留，已做优化
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef CONFIGURATIONDLG_H
#define CONFIGURATIONDLG_H

#include <QDialog>

namespace Ui {
class ConfigurationDlg;
}

class ConfigurationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDlg(QWidget *parent = nullptr);
    ~ConfigurationDlg();

    void getWorkspaceAndIwrpInfo(QString& workspace, QString& iwrp);

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_selectProject_clicked();
    void on_pushButton_selectWorkSpace_clicked();

private:
    Ui::ConfigurationDlg *ui;
    QString _workspace;
    QString _iwrp;
};

#endif // CONFIGURATIONDLG_H
