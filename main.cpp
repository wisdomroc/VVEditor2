#include "mainwindow.h"
#include "configuration_dlg.h"
#include <QFontDatabase>
#include <QApplication>
#include <QFileInfo>
#include <QTranslator>
#include <QMessageBox>
#include <QSharedMemory>
#include <QDebug>
#include "WRP_Project/SingletonProject.h"
#include <QResource>
#include <QMessageBox>
int initResource(QString file){
    if(!QResource::registerResource(file)){
        QMessageBox::warning(nullptr,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("资源加载失败：%1").arg(file));
        return -1;
    }
    return 0;
}
int initResources(){
    int initCheck=0;
    initCheck+=initResource("resources\\VVEditor2\\src.rcc");
    return initCheck;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(initResources()<0){
        return 0;
    }
    //!限制不能重复启动
    static QSharedMemory *shareMem = new QSharedMemory("VVEditor");
    if (!shareMem->create(1))
    {
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("本机已启动VVEditor，不能重复启动！"), QStringLiteral("确定"));
        qApp->quit();
        return -1;
    }


    //! 设置样式表
    QFile qss(":/pure.qss");
    if( qss.open(QFile::ReadOnly))
    {
        QString style = QString(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }


    //! 获取程序启动的命令参数
    QStringList arguments = QCoreApplication::arguments();
    QString workspace;
    QString project;
    if (arguments.count() == 2)
    {
        workspace = arguments.at(1);
    }
    else if(arguments.count() == 3)
    {
        workspace = arguments.at(1);
        project = arguments.at(2);
    }

    //! 根据命令参数进行初始化UI
    QFileInfo fileInfo(workspace);
    if(!fileInfo.isDir())
    {
        ConfigurationDlg w;
        int ret = w.exec();
        switch (ret) {
        case QDialog::Accepted:
            w.getWorkspaceAndIwrpInfo(workspace, project);
            break;
        case QDialog::Rejected:
            return -1;
        }
    }

    if(!SingletonProject::project_.setFilePath(project))
    {
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("项目文件打开失败，RESET命令将无法添加!"));
        //LogWidget::getInstance().addLog( QStringLiteral("项目文件打开失败，RESET命令将无法添加!"), 1 );
    }

    //! 添加苹方字体

    int loadedFontID = QFontDatabase::addApplicationFont(":/font/PingFang ExtraLight.ttf");
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty())
    {
        QString sansCNFamily = loadedFontFamilies.at(0);
        QFont defaultFont = a.font();
        defaultFont.setFamily(QString::fromLocal8Bit("微软雅黑"));
        defaultFont.setPixelSize(12);
        defaultFont.setBold(false);
        a.setFont(defaultFont);
    }


    //! 添加翻译文件
    static QTranslator translator;
    if (translator.load(":/font/VVEditor_zh.qm"))
    {
        qApp->installTranslator(&translator);
    }

    MainWindow w(workspace);
    w.setStatusBarInfo(workspace, project);
    w.showMaximized();

    return a.exec();
}
