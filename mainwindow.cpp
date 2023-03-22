#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTreeView>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QScrollBar>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QProgressDialog>
#include <QIcon>

const int VerticalInterval   = 50;

#include "vv_tree_item.h"
#include "vv_tree_model.h"
#include "add_beat_dlg.h"
#include "add_command_dlg.h"
#include "icd_data.h"
#include "graphics_scene.h"
#include "graphics_view.h"
#include "data_factory.h"
#include "tooltip.h"
#include "treeview.h"
#include "right_delegate.h"
#include "button_delegate.h"
#include "formula_config_dlg.h"
#include "w_headerview.h"

const QString CONF_FILENAME = "vveditor_conf.ini";
const QString WINDOW_TITLE  = QStringLiteral("测试脚本编辑器V2.0.23");

MainWindow::MainWindow(QString workspace, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _vvTreeModelRight(nullptr),
    _vvTreeModel(nullptr),
    _graphicsScene(nullptr),
    _copyVVTreeItem(nullptr),
    _copyDirAndFile(nullptr),
    _vvFile(nullptr),
    _workspace(workspace),
    _basedType(0),
    _hasModified(false),
    _whichFocus(-1)
{
    ui->setupUi(this);

    setWindowTitle(WINDOW_TITLE);
    setWindowIcon(QIcon(":/img/icon.ico"));

    _icd_data = QSharedPointer<ICDData>(new ICDData(_workspace));
    initConnections();
    initUi();
    ui->mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);//禁用右键的勾选显示与否
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _vvTreeModelRight;
    delete _vvTreeModel;
    delete _graphicsScene;
    delete _vvFile;
}

void MainWindow::initConnections()
{
    connect(ui->action_newDir,           &QAction::triggered, this, &MainWindow::slot_newDir);                  //新建文件夹
    connect(ui->action_newFile,          &QAction::triggered, this, &MainWindow::slot_newFile);                 //新建文件
    connect(ui->action_newBeat,          &QAction::triggered, this, &MainWindow::slot_newBeat);                 //新建节拍
    connect(ui->action_newCommand,       &QAction::triggered, this, &MainWindow::slot_newCommand);              //新建命令
    connect(ui->action_copy,             &QAction::triggered, this, &MainWindow::slot_copy);                    //复制（包括左侧树 & 右侧树）
    connect(ui->action_paste,            &QAction::triggered, this, &MainWindow::slot_paste);                   //粘贴（包括左侧树 & 右侧树）
    connect(ui->action_delete,           &QAction::triggered, this, &MainWindow::slot_delete);                  //删除（包括左侧树 & 右侧树）


    connect(ui->action_modify,           &QAction::triggered, this, &MainWindow::slot_modify);                  //左侧-编辑
    connect(ui->action_modifyBeat,       &QAction::triggered, this, &MainWindow::slot_modifyBeat);              //右侧-编辑节拍
    connect(ui->action_modifyCommand,    &QAction::triggered, this, &MainWindow::slot_modifyCommand);           //右侧-编辑命令

    connect(ui->action_save,             &QAction::triggered, this, &MainWindow::slot_save);                    //保存

    connect(ui->action_beatBased,        &QAction::triggered, this, &MainWindow::slot_beatBasedTriggered);      //切换基于节拍
    connect(ui->action_timeBased,        &QAction::triggered, this, &MainWindow::slot_timeBasedTriggered);      //切换基于时间
    connect(ui->action_based,            &QAction::triggered, this, &MainWindow::slot_basedTriggered);          //切换基于节拍 / 基于时间 / 基于标准时间
    connect(ui->action_arrow,            &QAction::triggered, this, &MainWindow::slot_arrowTriggered);          //切换成：时间轴,视口-展示模式
    connect(ui->action_table,            &QAction::triggered, this, &MainWindow::slot_tableTriggered);          //切换成：节拍命令树--展示模式

    connect(ui->beatArrow,               &BeatArrow::AddOneCommand, this, &MainWindow::slot_addOneCommand);     //节拍轴-添加命令
    connect(ui->beatArrow,               &BeatArrow::addBeatAfter,  this, &MainWindow::slot_addBeatAfter);      //在当前节拍之后添加节拍
    connect(ui->beatArrow,               &BeatArrow::addBeatBefore, this, &MainWindow::slot_addBeatBefore);     //在当前节拍之前添加节拍
    connect(ui->beatArrow,               &BeatArrow::activeClicked, this, &MainWindow::slot_activeClicked);     //点击了当前的节拍

    connect(ui->treeView,                &QTreeView::clicked,       this, &MainWindow::slot_treeViewClicked);   //左侧文件夹文件树，点击事件
    connect(ui->treeView,                &TreeView::modify,         this, &MainWindow::slot_modify);            //左侧文件夹文件树，修改事件
    connect(ui->treeView,                &TreeView::newDir,         this, &MainWindow::slot_newDir);            //左侧文件夹文件树，新建文件夹事件
    connect(ui->treeView,                &TreeView::newFile,        this, &MainWindow::slot_newFile);           //左侧文件夹文件树，新建文件事件
    connect(ui->treeView,                &TreeView::copyDirOrFile,  this, &MainWindow::slot_copyDirOrFile);
    connect(ui->treeView,                &TreeView::pasteDirOrFile, this, &MainWindow::slot_pasteDirOrFile);
    connect(ui->treeView,                &TreeView::currentIndexChanged, this, &MainWindow::slot_currentIndexChanged);

    connect(ui->treeView_beatAndCommand, &TreeViewR::clicked,       this, [this](){ _whichFocus = 1; });
    connect(ui->treeView_beatAndCommand, &TreeViewR::newBeat,       this, &MainWindow::slot_newBeat);           //右侧树，新建节拍
    connect(ui->treeView_beatAndCommand, &TreeViewR::newCommand,    this, &MainWindow::slot_newCommand);        //右侧树，新建命令
    connect(ui->treeView_beatAndCommand, &TreeViewR::addBeatBefore, this, &MainWindow::slot_addBeatBefore);     //右侧树，在之前插入节拍
    connect(ui->treeView_beatAndCommand, &TreeViewR::addBeatAfter,  this, &MainWindow::slot_addBeatAfter);      //右侧树，在之后插入节拍
    connect(ui->treeView_beatAndCommand, &TreeViewR::editCommand,   this, &MainWindow::slot_editCommand);       //右侧树，双击-编辑命令
    connect(ui->treeView_beatAndCommand, &TreeViewR::editFormula,   this, &MainWindow::slot_editFormula);       //右侧树，双击-编辑公式
    connect(ui->treeView_beatAndCommand, &TreeViewR::edit,          this, &MainWindow::slot_editR);             //右侧树，右键-编辑

    connect(ui->graphicsView->horizontalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::slot_graphicsViewHValueChanged);
    connect(ui->graphicsView->verticalScrollBar(),   &QScrollBar::valueChanged, this, &MainWindow::slot_graphicsViewVValueChanged);

    connect(ui->splitter,                &QSplitter::splitterMoved, this, &MainWindow::slot_splitterMoved);

    connect(ui->treeView_beatAndCommand, &TreeViewR::sigChanged,   this, &MainWindow::slot_changed); // 右侧树双击消息名称事件
}

void MainWindow::initUi()
{
    //Init QGraphicsView
    _graphicsScene = new GraphicsScene();
    _graphicsScene->setICDData(_icd_data);
    ui->graphicsView->setScene(_graphicsScene);
    connect(ui->graphicsView, &GraphicsView::changeSize, this, [&](QSize curSize)
    {   //设置临界值不显示水平/垂直滚动条
        int xMinWidth  = _graphicsScene->getXMinWidth()  > curSize.width()  ? _graphicsScene->getXMinWidth()  : curSize.width()-2;
        int yMinHeight = _graphicsScene->getYMinHeight() > curSize.height() ? _graphicsScene->getYMinHeight() : curSize.height()-2;
        _graphicsScene->setXMinWidth(xMinWidth);
        _graphicsScene->setYMinHeight(yMinHeight);
        _graphicsScene->setSceneRect(QRect(0, 0, xMinWidth, yMinHeight));
    });

    //Init TreeView
    ui->treeView->setFocusPolicy(Qt::NoFocus);                         //去掉鼠标移到单元格上时的虚线框
    ui->treeView->header()->setStretchLastSection(true);               //最后一列自适应宽度
    ui->treeView->addAction(ui->action_delete);
    ui->treeView->addAction(ui->action_copy);
    ui->treeView->addAction(ui->action_paste);

    QVector<VVDir *> dirList = initData();
    setModel(dirList);

    ui->stackedWidget->setCurrentIndex(1);
    ui->treeView_beatAndCommand->setEnabled(false);

    QSettings settings(CONF_FILENAME, QSettings::IniFormat);
    settings.beginGroup("Layout");
    QByteArray ba = settings.value("MainSplitter").toByteArray();
    settings.endGroup();
    ui->splitter->restoreState(ba);
    if(ba.isEmpty())
        ui->splitter->setSizes(QList<int>() << 240 << 1600);
    ui->menuBar->setVisible(false);

    settings.beginGroup("Layout");
    QByteArray ba_center = settings.value("MainWindow").toByteArray();
    settings.endGroup();
    restoreState(ba_center);
}

void MainWindow::initBeatAndCommandView(VVFile *vvFile)
{
    QStringList headers;
    headers << QStringLiteral("节拍")
            << QStringLiteral("描述")
            << QStringLiteral("命令")
            << QStringLiteral("消息名称")
            << QStringLiteral("消息ID")
            << QStringLiteral("信号名称")
            << QStringLiteral("信号ID")
            << QStringLiteral("长度")
            << QStringLiteral("类型")
            << QStringLiteral("值")
            << QStringLiteral("下标")
            << QStringLiteral("操作");

    _vvTreeModelRight = new VVTreeModel(headers, ui->treeView_beatAndCommand);
    VVTreeItem* root = _vvTreeModelRight->root();
    root->setType(VVTreeItem::FILE);
    root->setPtr(vvFile);
    root->setBasedType(_basedType);
    if(vvFile == nullptr)
    {
        ui->treeView_beatAndCommand->setModel(_vvTreeModelRight);
        ui->treeView_beatAndCommand->setEnabled(false);
        setWindowTitle(WINDOW_TITLE);
        return;
    }


    foreach (auto beat, vvFile->_beats)
    {
        VVTreeItem* beatItem = new VVTreeItem(root);
        beatItem->setPtr(beat);               // 保存数据指针
        beatItem->setType(VVTreeItem::BEAT);  // 设置节点类型为DIR
        beatItem->setBasedType(_basedType);

        foreach (auto command, beat->_commands)
        {
            VVTreeItem* commandItem = new VVTreeItem(beatItem);
            commandItem->setPtr(command);                // 保存数据指针
            commandItem->setType(VVTreeItem::COMMAND);   // 设置节点类型为FILE
            commandItem->setBasedType(_basedType);
            beatItem->addChild(commandItem);
        }

        root->addChild(beatItem);
    }

    ui->treeView_beatAndCommand->addAction(ui->action_delete);
    ui->treeView_beatAndCommand->addAction(ui->action_copy);
    ui->treeView_beatAndCommand->addAction(ui->action_paste);
    ui->treeView_beatAndCommand->setModel(_vvTreeModelRight);
    connect(_vvTreeModelRight,  &VVTreeModel::dataChanged,          this, &MainWindow::slot_dataChanged);
    RightDelegate*   rightMsgName     = new RightDelegate(ui->treeView);
    RightDelegate*   rightMsgID       = new RightDelegate(ui->treeView);
    RightDelegate*   rightSigName     = new RightDelegate(ui->treeView);
    RightDelegate*   rightSigID       = new RightDelegate(ui->treeView);
    RightDelegate*   rightValue       = new RightDelegate(ui->treeView);
    RightDelegate*   rightLength      = new RightDelegate(ui->treeView);
    RightDelegate*   rightType        = new RightDelegate(ui->treeView);
    RightDelegate*   rightIndex       = new RightDelegate(ui->treeView);//nisz add 2022-6-17
    ButtonDelegate*  buttonDelegate   = new ButtonDelegate();
    connect(buttonDelegate, &ButtonDelegate::clicked, this, &MainWindow::slot_deleteCommandR);   //连接槽函数
    rightMsgName->setICDData(_icd_data);
    rightMsgName->setItems(_icd_data->getMsgNamesHash());
    rightMsgID->setICDData(_icd_data);
    rightMsgID->setItems(_icd_data->getMsgIDsHash());
    rightSigName->setICDData(_icd_data);
    rightSigID->setICDData(_icd_data);
    rightValue->setICDData(_icd_data);

    int width = ui->treeView_beatAndCommand->width();

    ui->treeView_beatAndCommand->setColumnWidth(1, 0.10*width);
    ui->treeView_beatAndCommand->setColumnWidth(2, 0.07*width);
    ui->treeView_beatAndCommand->setColumnWidth(3, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(4, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(5, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(6, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(7, 0.05*width);
    ui->treeView_beatAndCommand->setColumnWidth(8, 0.05*width);
    ui->treeView_beatAndCommand->setColumnWidth(9, 0.06*width);
    ui->treeView_beatAndCommand->setColumnWidth(10, 0.05*width);

    /** TODO.此处现在有少许性能问题，添加代理太多；但功能如此暂无法避免，将来看看怎么优化 **/
    ui->treeView_beatAndCommand->setItemDelegateForColumn(3, rightMsgName);     //MsgName（ICD名称）
    ui->treeView_beatAndCommand->setItemDelegateForColumn(4, rightMsgID);       //MsgID  （ICD  ID）
    ui->treeView_beatAndCommand->setItemDelegateForColumn(5, rightSigName);     //SigName（信号名称）
    ui->treeView_beatAndCommand->setItemDelegateForColumn(6, rightSigID);       //SigID  （信号  ID）
    ui->treeView_beatAndCommand->setItemDelegateForColumn(7, rightLength);      //长度
    ui->treeView_beatAndCommand->setItemDelegateForColumn(8, rightType);		//类型
    ui->treeView_beatAndCommand->setItemDelegateForColumn(9, rightValue);       //值
    ui->treeView_beatAndCommand->setItemDelegateForColumn(10, rightIndex);//nisz add 2022-6-17       //下标
    ui->treeView_beatAndCommand->setItemDelegateForColumn(11, buttonDelegate);   //删除按钮
    dynamic_cast<WHeaderView*>(ui->treeView_beatAndCommand->header())->init();
}

void MainWindow::updateRuleBar()
{
    if ( ui->graphicsView->scene() == nullptr) return;
    QRectF viewbox = ui->graphicsView->rect();
    QPointF offset = ui->graphicsView->mapFromScene(ui->graphicsView->scene()->sceneRect().topLeft());
    double factor =  1./ui->graphicsView->transform().m11();    //缩放因子
    double lower_x = factor * ( viewbox.left()  - offset.x() );
    double upper_x = factor * ( viewbox.right() - 16 - offset.x()  );
    double lower_y = factor * ( viewbox.top() - offset.y()) * -1;
    double upper_y = factor * ( viewbox.bottom() - 16 - offset.y() ) * -1;
    Q_UNUSED(lower_x)
    Q_UNUSED(upper_x)
    Q_UNUSED(lower_y)
    Q_UNUSED(upper_y)

    ui->beatArrow->setRange(lower_y);
    ui->beatArrow->update();
}

QVector<VVDir*> MainWindow::initData()
{
    DataFactory::get_instance()->setWorkspace(_workspace);
    return DataFactory::get_instance()->init();
}

void MainWindow::setModel(const QVector<VVDir *> &dirList)
{
    QStringList headers;
    headers << QStringLiteral("名称");

    _vvTreeModel = new VVTreeModel(headers, ui->treeView);
    VVTreeItem* root = _vvTreeModel->root();
    foreach (auto dir, dirList)
    {
        VVTreeItem* dirItem = new VVTreeItem(root);
        dirItem->setPtr(dir);               // 保存数据指针
        dirItem->setType(VVTreeItem::DIR);  // 设置节点类型为DIR

        foreach (auto file, dir->_files)
        {
            VVTreeItem* fileItem = new VVTreeItem(dirItem);
            fileItem->setPtr(file);                // 保存数据指针
            fileItem->setType(VVTreeItem::FILE);   // 设置节点类型为FILE
            dirItem->addChild(fileItem);
        }
        root->addChild(dirItem);
    }

    ui->treeView->setModel(_vvTreeModel);
    connect(_vvTreeModel,       &QAbstractItemModel::dataChanged,   this, &MainWindow::slot_dataChanged);
}

void MainWindow::slot_showToolTip(QDialog &dlg, QLineEdit* lineEdit, const QString& text)
{
    if(text.contains("\\") ||
            text.contains("/") ||
            text.contains(":") ||
            text.contains("*") ||
            text.contains("?") ||
            text.contains("\"") ||
            text.contains("<") ||
            text.contains(">") ||
            text.contains("|"))
    {
        ToolTip *toolTip = new ToolTip(&dlg);
        toolTip->setTipText(QStringLiteral("文件名不能包含下列任何字符：\n\\ / : * ? \" < > |"));
        QPoint point(lineEdit->geometry().left(),lineEdit->geometry().bottom() + 5);
        toolTip->move(point);
        toolTip->show();
        QString processedText = text;
        processedText.remove("\\");
        processedText.remove("/");
        processedText.remove(":");
        processedText.remove("*");
        processedText.remove("?");
        processedText.remove("\"");
        processedText.remove("<");
        processedText.remove(">");
        processedText.remove("|");
        lineEdit->setText(processedText);
    }
}

void MainWindow::slot_editBeat(const QModelIndex &index)
{
    Q_UNUSED(index)
    slot_modifyBeat();
}

void MainWindow::adjustDirFileSelection(VVFile* vvFile)
{
    Q_UNUSED(vvFile)
    //! 找到当前打开的文件到底是左侧哪一个，然后进行选中
    for(int i = 0; i < _vvTreeModel->root()->childCount(); ++i)
    {
        for(int j = 0; j < _vvTreeModel->root()->child(i)->childCount(); ++j)
        {
            VVFile* vvFile = reinterpret_cast<VVFile*>(_vvTreeModel->root()->child(i)->child(j)->ptr());
            if(vvFile == _vvFile)
            {
                QModelIndex index = ui->treeView->currentIndex();
                while (index.parent().isValid()) {
                    index = index.parent();
                }

                index = index.sibling(i, 0).child(j, 0);

                ui->treeView->setCurrentIndex(index);
            }
        }
    }
}

void MainWindow::slot_editCommand(const QModelIndex &index)
{
    adjustDirFileSelection(_vvFile);

    VVCommand* vvCommand = reinterpret_cast<VVCommand*>(_vvTreeModelRight->itemFromIndex(index)->ptr());
    AddCommandDlg addCommandDlg(_icd_data, _vvFile, vvCommand);
    addCommandDlg.setVVCommand(vvCommand);
    int ret = addCommandDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
    {
        int expandedRow = index.parent().row();
        VVCommand *oldCommand = vvCommand;
        VVCommand *vvCommandNew = addCommandDlg.getInfo();
        _vvTreeModelRight->modifyCommandRight(index, oldCommand, vvCommandNew);
        QModelIndex index_file = ui->treeView->currentIndex();
        if(_vvTreeModel->itemFromIndex(index_file)->getType() == VVTreeItem::DIR)
        {   //! 找到当前打开的文件到底是左侧哪一个，然后进行选中
            for(int i = 0; i < _vvTreeModel->root()->childCount(); ++i)
            {
                for(int j = 0; j < _vvTreeModel->root()->child(i)->childCount(); ++j)
                {
                    VVFile* vvFile = reinterpret_cast<VVFile*>(_vvTreeModel->root()->child(i)->child(j)->ptr());
                    if(vvFile == _vvFile)
                    {
                        index_file = ui->treeView->rootIndex().child(i, 0).child(j, 0);
                    }
                }
            }
        }
        _vvTreeModel->modifyCommand(index_file, expandedRow, vvCommand, vvCommandNew);


        QModelIndex expandedIndex = _vvTreeModelRight->index(expandedRow, 0, ui->treeView_beatAndCommand->rootIndex());
        ui->treeView_beatAndCommand->setExpanded(expandedIndex, true);
    }
        break;
    case QDialog::Rejected:

        break;
    }

    _whichFocus = 1;
}

void MainWindow::slot_editFormula(const QModelIndex &index)
{
    adjustDirFileSelection(_vvFile);

    VVCommand* vvCommand = reinterpret_cast<VVCommand*>(_vvTreeModelRight->itemFromIndex(index)->ptr());

    FormulaConfigDlg *formulaConfigDlg = new FormulaConfigDlg();
    formulaConfigDlg->setFormulaExpression(vvCommand->_data_value);
    int ret = formulaConfigDlg->exec();
    switch (ret) {
    case QDialog::Accepted:
    {
        vvCommand->_data_value = formulaConfigDlg->getFormulaExpression();
    }
        break;
    case QDialog::Rejected:
        break;
    }

    _whichFocus = 1;
}

void MainWindow::slot_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)
    QByteArray ba = ui->splitter->saveState();
    QSettings settings(CONF_FILENAME, QSettings::IniFormat);
    settings.beginGroup("Layout");
    settings.setValue("MainSplitter", ba);
    settings.endGroup();
}

void MainWindow::slot_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    _hasModified = true;
}

bool MainWindow::isCopyR()
{
    if(_copyVVTreeItem != nullptr && _copyVVTreeItem->getType() == VVTreeItem::BEAT)
    {
        return true;
    }

    return false;
}

bool MainWindow::isCopy()
{
    if(_copyDirAndFile != nullptr && _copyDirAndFile->getType() == VVTreeItem::DIR)
    {
        return true;
    }

    return false;
}

void MainWindow::setStatusBarInfo(const QString &workspace, const QString &iwrp)
{
    QLabel *label_workspace = new QLabel(ui->statusBar);
    label_workspace->setText(tr("%1%2").arg(QStringLiteral("工作空间：")).arg(QDir::toNativeSeparators(workspace)));
    QLabel *spacer = new QLabel(ui->statusBar);
    spacer->setFixedWidth(20);
    QLabel *label_iwrp = new QLabel(ui->statusBar);
    label_iwrp->setText(tr("%1%2").arg(QStringLiteral("项目文件：")).arg(QDir::toNativeSeparators(iwrp)));
    ui->statusBar->addPermanentWidget(label_workspace);
    ui->statusBar->addPermanentWidget(spacer);
    ui->statusBar->addPermanentWidget(label_iwrp);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(CONF_FILENAME, QSettings::IniFormat);
    settings.beginGroup("Layout");
    settings.setValue("MainWindow", this->saveState());
    settings.endGroup();

    if(_hasModified)
    {
        QMessageBox msgBox;
        msgBox.setText(QStringLiteral("数据已修改，是否保存？"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::No | QMessageBox::Discard);
        msgBox.setButtonText(QMessageBox::Save, QStringLiteral("保存"));
        msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
        msgBox.setButtonText(QMessageBox::Discard, QStringLiteral("不保存"));
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:
            slot_save();
            break;
        case QMessageBox::No:
            event->ignore();
            return;
        case QMessageBox::Discard:
            return;
        default:
            return;
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    int width = ui->treeView_beatAndCommand->width();

    ui->treeView_beatAndCommand->setColumnWidth(1, 0.10*width);
    ui->treeView_beatAndCommand->setColumnWidth(2, 0.07*width);
    ui->treeView_beatAndCommand->setColumnWidth(3, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(4, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(5, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(6, 0.12*width);
    ui->treeView_beatAndCommand->setColumnWidth(7, 0.05*width);
    ui->treeView_beatAndCommand->setColumnWidth(8, 0.05*width);
    ui->treeView_beatAndCommand->setColumnWidth(9, 0.06*width);
    ui->treeView_beatAndCommand->setColumnWidth(10, 0.05*width);
}

void MainWindow::slot_newDir()
{
    //! Init Ui Dlg
    QDialog newFileDlg;
    newFileDlg.setFixedSize(400, 260);
    newFileDlg.setWindowTitle(QStringLiteral("添加文件夹"));
    newFileDlg.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    QLabel *label_name = new QLabel(QStringLiteral("名    称："), &newFileDlg);
    QLineEdit *lineEdit_name = new QLineEdit(&newFileDlg);
    connect(lineEdit_name, &QLineEdit::textEdited, this, [lineEdit_name, &newFileDlg, this](const QString& newText){ this->slot_showToolTip(newFileDlg, lineEdit_name, newText); });
    QPushButton *pushButtonOK = new QPushButton(QStringLiteral("确定"), &newFileDlg);
    QPushButton *pushButtonCancel = new QPushButton(QStringLiteral("取消"), &newFileDlg);
    connect(pushButtonOK, &QPushButton::clicked, this, [lineEdit_name, &newFileDlg](){
        if(lineEdit_name->text().isEmpty())
        {
            QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("名称不能为空！"), QStringLiteral("确定"));
            return;
        }
        newFileDlg.accept();
    });
    connect(pushButtonCancel, &QPushButton::clicked, &newFileDlg, &QDialog::reject);
    QVBoxLayout *vBoxLayout = new QVBoxLayout(&newFileDlg);
    QHBoxLayout *hBoxLayout = new QHBoxLayout(&newFileDlg);
    hBoxLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hBoxLayout->addWidget(label_name);
    hBoxLayout->addWidget(lineEdit_name);
    hBoxLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QHBoxLayout *hBoxLayout1 = new QHBoxLayout(&newFileDlg);
    hBoxLayout1->addItem(new QSpacerItem(100,5,QSizePolicy::Expanding, QSizePolicy::Minimum));
    hBoxLayout1->addWidget(pushButtonOK);
    hBoxLayout1->addWidget(pushButtonCancel);
    vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vBoxLayout->addLayout(hBoxLayout1);
    vBoxLayout->setContentsMargins(20, 20, 20, 20);
    newFileDlg.setLayout(vBoxLayout);
    int ret = newFileDlg.exec();
    QString dirname = "";
    switch (ret) {
    case QDialog::Accepted:
        dirname = lineEdit_name->text();
        break;
    case QDialog::Rejected:
        return;
    }


    if(dirname.isEmpty())
        return;

    bool ok = _vvTreeModel->addDir(dirname);
    if(!ok)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("存在同名文件夹！"), QStringLiteral("确定"));
    }

    _hasModified = true;
}

void MainWindow::slot_newFile()
{
    QModelIndex curIndex = ui->treeView->currentIndex();
    if(!curIndex.isValid())
        return;

    if(_vvTreeModel->dataType(ui->treeView->currentIndex()) != VVTreeItem::DIR)
    {
        ui->treeView->setCurrentIndex(ui->treeView->currentIndex().parent());
        curIndex = ui->treeView->currentIndex();
    }

    //! Init Ui Dlg
    QDialog newFileDlg;
    newFileDlg.setFixedSize(400, 260);
    newFileDlg.setWindowTitle(QStringLiteral("添加文件"));
    newFileDlg.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    QLabel *label_name = new QLabel(QStringLiteral("名    称："), &newFileDlg);
    QLabel *label_executionNumber = new QLabel(QStringLiteral("执行次数："), &newFileDlg);
    QLabel *label_cycle = new QLabel(QStringLiteral("调度周期："), &newFileDlg);
    QLineEdit *lineEdit_name = new QLineEdit(&newFileDlg);
    connect(lineEdit_name, &QLineEdit::textEdited, this, [lineEdit_name, &newFileDlg, this](const QString& newText){ this->slot_showToolTip(newFileDlg, lineEdit_name, newText); });
    QSpinBox *spinBox_executionNumber = new QSpinBox(&newFileDlg);
    QSpinBox *spinBox_cycle = new QSpinBox(&newFileDlg);
    QComboBox *comboBox_unit = new QComboBox(&newFileDlg);
    spinBox_executionNumber->setRange(-1, MAX_BEAT);
    spinBox_executionNumber->setValue(-1);
    spinBox_executionNumber->setToolTip(QStringLiteral("默认-1：代表执行次数是无穷\n0~99999：代表执行次数即为相应数值！"));
    spinBox_cycle->setRange(1,MAX_BEAT);
    spinBox_cycle->setValue(1);
    comboBox_unit->addItems(QStringList() << "s" << "ms" << "us" << "ns");
    comboBox_unit->setCurrentText("ms");
    comboBox_unit->setFixedWidth(40);
    QPushButton *pushButtonOK = new QPushButton(QStringLiteral("确定"), &newFileDlg);
    QPushButton *pushButtonCancel = new QPushButton(QStringLiteral("取消"), &newFileDlg);
    connect(pushButtonOK, &QPushButton::clicked, this, [&](){
        if(lineEdit_name->text().isEmpty())
        {
            QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("名称不能为空！"), QStringLiteral("确定"));
            return;
        }
        if(spinBox_executionNumber->value() == 0)
        {
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("执行次数不能为零！"));
            return;
        }

        newFileDlg.accept();
    });
    connect(pushButtonCancel, &QPushButton::clicked, &newFileDlg, &QDialog::reject);
    QVBoxLayout *vBoxLayout = new QVBoxLayout(&newFileDlg);
    QGridLayout *gridLayout = new QGridLayout(&newFileDlg);
    gridLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0);
    gridLayout->addWidget(label_name, 0, 1);
    gridLayout->addWidget(lineEdit_name, 0, 2);
    gridLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 4);
    gridLayout->addWidget(label_executionNumber, 1, 1);
    gridLayout->addWidget(spinBox_executionNumber, 1, 2);
    gridLayout->addWidget(label_cycle, 2, 1);
    QHBoxLayout *hBoxLayout_child = new QHBoxLayout(&newFileDlg);
    hBoxLayout_child->addWidget(spinBox_cycle);
    hBoxLayout_child->addWidget(comboBox_unit);
    gridLayout->addLayout(hBoxLayout_child, 2, 2);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(&newFileDlg);
    hBoxLayout->addItem(new QSpacerItem(100,5,QSizePolicy::Expanding, QSizePolicy::Minimum));
    hBoxLayout->addWidget(pushButtonOK);
    hBoxLayout->addWidget(pushButtonCancel);
    vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vBoxLayout->addLayout(gridLayout);
    vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->setContentsMargins(20, 20, 20, 20);
    newFileDlg.setLayout(vBoxLayout);
    int ret = newFileDlg.exec();
    QString filename = "";
    int executionNumber = -1;
    QString cycleStr = "";
    switch (ret) {
    case QDialog::Accepted:
        filename = lineEdit_name->text();
        executionNumber = spinBox_executionNumber->value();
        cycleStr = QString::number(spinBox_cycle->value()) + comboBox_unit->currentText();
        break;
    case QDialog::Rejected:
        return;
    }

    if(!filename.isEmpty())
    {
        bool ok = _vvTreeModel->addFile(filename, executionNumber, cycleStr, curIndex);
        if(!ok)
        {
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("文件夹内存在同名文件，请修改！"), QStringLiteral("确定"));
            return;
        }
        ui->treeView->setExpanded(curIndex, true);
    }

    _hasModified = true;
}

void MainWindow::slot_delete()
{
    if(_whichFocus == 1 && ui->treeView_beatAndCommand->hasFocus())
    {// 右侧树的删除
        slot_remove();
    }
    else
    {// 左侧树的删除
        QModelIndexList indexs = ui->treeView->getSelectedIndexs();

        QStringList infoListFile;
        QModelIndexList indexsDir;
        QModelIndexList indexsFile;
        for(QModelIndex index: indexs)
        {
            if(!index.isValid())
                continue;
            if(!index.parent().isValid())
            {
                indexsDir.append(index);
            }
            else
            {
                indexsFile.append(index);
                VVDir*  vvDir  = reinterpret_cast<VVDir*>(_vvTreeModel->itemFromIndex(index.parent())->ptr());
                VVFile* vvFile = reinterpret_cast<VVFile*>(_vvTreeModel->itemFromIndex(index)->ptr());
                infoListFile.append("[" + vvDir->_dir_name + "\\" + vvFile->_file_name + "]");
            }
        }

        QMessageBox msgBox;
        if(infoListFile.size() != 0)
        {
            msgBox.setText(tr("%1\n%2%3").arg(QStringLiteral("是否确定删除")).arg(infoListFile.join(QStringLiteral("，"))).arg(QStringLiteral("？")));
            msgBox.setInformativeText(QStringLiteral("注意：文件夹下的所有文件删除后，文件夹也会被删除！\n选中的空文件夹也会被删除！"));
        }
        else
        {
            msgBox.setText(tr("%1").arg(QStringLiteral("是否确定删除所选文件夹？\n只有空文件夹才会被删除！")));
        }
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, QStringLiteral("确定"));
        msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Yes:
            break;
        case QMessageBox::No:
        default:
            return;
        }


        qSort(indexsDir.begin(),    indexsDir.end(),    [](QModelIndex indexA, QModelIndex indexB){ return indexA.row() > indexB.row(); });
        qSort(indexsFile.begin(),   indexsFile.end(),   [](QModelIndex indexA, QModelIndex indexB){ return indexA.row() == indexB.row() ? indexA.parent().row() > indexB.parent().row() : indexA.row() > indexB.row(); });

        for(auto index: indexsFile)
        {
            VVFile* vvFile = reinterpret_cast<VVFile*>(_vvTreeModel->itemFromIndex(index)->ptr());
            _vvTreeModel->deleteDirOrFile(index);

            if(vvFile == _vvFile)
            {
                _vvFile = nullptr;
                initBeatAndCommandView(_vvFile);
            }
        }

        //! 如果文件夹下的所有文件均被选中，则该文件夹可以进行删除
        QModelIndexList indexFileList;
        for(auto index: indexsDir)
        {
            bool canRemove = true;
            QModelIndex index_file;
            VVTreeItem* vvTreeItem = _vvTreeModel->itemFromIndex(index);
            VVTreeItem::Type type = vvTreeItem->getType();
            VVBeat* tmpVVBeat = reinterpret_cast<VVBeat*>(vvTreeItem->ptr());
            Q_UNUSED(type)
            Q_UNUSED(tmpVVBeat)
            for(int i = 0; i < vvTreeItem->childCount(); ++i)
            {
                index_file = index.child(vvTreeItem->row(), 0);
                indexFileList.append(index_file);
                if(!indexsFile.contains(index_file))
                    canRemove = false;
            }

            if(canRemove)
                _vvTreeModel->deleteDirOrFile(index);
        }
    }


    _hasModified = true;
}

bool MainWindow::getFileIntervalAndUnit(int &interval, QString &unit)
{
    if(!_vvFile)
        return false;

    BeatArrow::ConvertTimeStr2ValueAndUnit(_vvFile->_time_interval, interval, unit);
    return true;
}

void MainWindow::checkIcdExist(VVCommand* vvCommand,QStringList &icdList)
{
    QString var_package_id;
    if(vvCommand-> _command_type_str!="RESET"){
        if(_icd_data->findICDInstanceByID(vvCommand-> _msg_name)== nullptr){
            icdList<<vvCommand->_msg_name;
        }
        if(vvCommand->_data_value.indexOf(",") != -1)
        {   //! value是变量
            if(vvCommand->_data_value.indexOf(";") != -1)
            {   //! value是变量并且有范围
                var_package_id = vvCommand->_data_value.split(";").at(0).split(",").at(0);
            }
            else
            {   //! value是变量，但没有范围
                var_package_id = vvCommand->_data_value.split(",").at(0);
            }
            if( (_icd_data->findICDInstanceByID(var_package_id))== nullptr){
                icdList<<var_package_id;
            }
        }
    }
    else {
        if(vvCommand->_target_list.size()>0){
            for(auto target_list:vvCommand->_target_list){
                if(target_list->type=="icd"){
                    if( (_icd_data->findICDInstanceByID(target_list->name))== nullptr){
                        icdList<<target_list->name;
                    }
                }
            }
        }
}
}

void MainWindow::slot_newBeat()
{
    adjustDirFileSelection(_vvFile);

    int value; QString unit;
    bool ok = getFileIntervalAndUnit(value, unit);
    if(!ok)
        return;

    int lastBeat = -1;
    int rowCount = _vvTreeModelRight->root()->childCount();
    if(rowCount > 0)
        lastBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->root()->child(rowCount-1)->ptr())->_beat_number;
    AddBeatDlg addBeatDlg(lastBeat, value, unit);
    int ret = addBeatDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
    {
        int beat;
        int type;
        QString des;
        addBeatDlg.getInfo(beat, type, des);

        //! 节拍轴添加节拍
        //        bool ok = ui->beatArrow->addBeat(beat, type, des);
        //        if(!ok)
        //        {
        //            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("已存在该节拍！"), QStringLiteral("确定"));
        //            return;
        //        }

        //! 创建新的节拍
        VVBeat *vvBeat = new VVBeat(beat, type, des);
        vvBeat->_time_interval = _vvFile->_time_interval;

        //! 左侧树添加节拍
        _vvTreeModel->addBeat(ui->treeView->currentIndex(), vvBeat);

        //! 视口添加节拍
        //        _graphicsScene->addBeat();
        //        if(ui->beatArrow->getFirstIndex() == 0)
        //        {   //QGraphicsView没有滚动条
        //            qDebug() << "getFirstIndex == 0" << endl;
        //        }
        //        else
        //        {   //QGraphicsView存在滚动条，纵向增大QGraphicsScene高度
        //            _graphicsScene->adjustSceneRect();
        //            ui->graphicsView->verticalScrollBar()->setValue(1000000);//这里设置滚动到最下面
        //        }

        //! 右侧树添加节拍
        QModelIndex root = ui->treeView_beatAndCommand->rootIndex();
        if(root.isValid())
            qDebug() << "root is valid" << endl;
        int childCount = _vvTreeModelRight->root()->childCount();
        _vvTreeModelRight->addBeatRight(root, childCount, vvBeat);







    }
        break;
    case QDialog::Rejected:
        return;
    }

    _hasModified = true;
    _whichFocus = 1;
}

void MainWindow::slot_newCommand()
{
    adjustDirFileSelection(_vvFile);

    if(!_vvTreeModelRight)
        return;

    VVBeat* vvBeat = nullptr;
    VVTreeItem* rightVVTreeItem = _vvTreeModelRight->itemFromIndex(ui->treeView_beatAndCommand->currentIndex());
    if(rightVVTreeItem->getType() == VVTreeItem::BEAT)
    {
        vvBeat = reinterpret_cast<VVBeat*>(rightVVTreeItem->ptr());
    }
    else if(rightVVTreeItem->getType() == VVTreeItem::COMMAND)
    {
        vvBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->itemFromIndex(ui->treeView_beatAndCommand->currentIndex().parent())->ptr());
    }

    if(!vvBeat)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("未选择任何一个节拍！"), QStringLiteral("确定"));
        return;
    }


    AddCommandDlg addCommandDlg(_icd_data, _vvFile);
    int ret = addCommandDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
    {
        VVCommand *vvCommand = addCommandDlg.getInfo();


        VVFile* vvFile = reinterpret_cast<VVFile*>(_vvTreeModel->itemFromIndex(ui->treeView->currentIndex())->ptr());
        VVTreeItem* rightVVTreeItem = _vvTreeModelRight->itemFromIndex(ui->treeView_beatAndCommand->currentIndex());

        if(rightVVTreeItem->getType() == VVTreeItem::BEAT)
        {
            int childCount = rightVVTreeItem->childCount();
            QModelIndex curIndex = ui->treeView_beatAndCommand->currentIndex();
            _vvTreeModelRight->addCommandRight(curIndex.sibling(curIndex.row(), 0), childCount, vvCommand);
        }
        else if(rightVVTreeItem->getType() == VVTreeItem::COMMAND)
        {
            int childCount = rightVVTreeItem->parent()->childCount();
            _vvTreeModelRight->addCommandRight(ui->treeView_beatAndCommand->currentIndex().parent(), childCount, vvCommand);
        }
        int index = vvFile->_beats.indexOf(vvBeat);
        Q_ASSERT(index != -1);
        _vvTreeModel->addCommand(ui->treeView->currentIndex(), index,vvCommand);
    }
        break;
    case QDialog::Rejected:

        return;
    }

    _hasModified = true;
    _whichFocus = 1;
}

void MainWindow::slot_copy()
{
    if(_whichFocus == 1 && ui->treeView_beatAndCommand->hasFocus())
    {// 右侧树的复制、粘贴
        adjustDirFileSelection(_vvFile);
        if(ui->treeView_beatAndCommand->getSelectedIndexsSize() > 1)
        {
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("复制粘贴不支持多选！"), QStringLiteral("确定"));
            return;
        }
        QModelIndex curIndex = ui->treeView_beatAndCommand->currentIndex();
        _copyVVTreeItem = _vvTreeModelRight->itemFromIndex(curIndex);

    }
    else if(ui->treeView->hasFocus())
    {// 左侧树的复制、粘贴
        if(ui->treeView->getSelectedIndexsSize() > 1)
        {
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("复制粘贴不支持多选！"), QStringLiteral("确定"));
            return;
        }
        slot_copyDirOrFile();
    }
}

void MainWindow::slot_paste()
{
    if(_whichFocus == 1 && ui->treeView_beatAndCommand->hasFocus())
    {
        adjustDirFileSelection(_vvFile);

        QModelIndex curIndex = ui->treeView_beatAndCommand->currentIndex();
        curIndex = curIndex.sibling(curIndex.row(), 0);
        VVTreeItem* vvTreeItem = _vvTreeModelRight->itemFromIndex(curIndex);

        if(!_copyVVTreeItem)
            return;

        if(vvTreeItem->getType() == VVTreeItem::COMMAND && _copyVVTreeItem->getType() == VVTreeItem::COMMAND)
        {
            VVCommand* command = reinterpret_cast<VVCommand*>(_copyVVTreeItem->ptr());
            VVCommand* newVVCommand = new VVCommand();
            *newVVCommand = *command;

            int childCount = _vvTreeModelRight->itemFromIndex(curIndex.parent())->childCount();
            _vvTreeModelRight->pasteCommand(curIndex.parent(), childCount, newVVCommand);
            QModelIndex index_ = _vvTreeModelRight->index(curIndex.parent().row(), 0, curIndex.parent().parent());

            _vvTreeModel->addCommand(ui->treeView->currentIndex(), index_.row(), newVVCommand);
        }
        else if(vvTreeItem->getType() == VVTreeItem::BEAT && _copyVVTreeItem->getType() == VVTreeItem::COMMAND)
        {
            VVCommand* command = reinterpret_cast<VVCommand*>(_copyVVTreeItem->ptr());
            VVCommand* newVVCommand = new VVCommand();
            *newVVCommand = *command;
            int childCount = _vvTreeModelRight->itemFromIndex(curIndex)->childCount();
            _vvTreeModelRight->pasteCommand(curIndex, childCount, newVVCommand);
            QModelIndex index_ = _vvTreeModelRight->index(curIndex.row(), 0, curIndex.parent());

            _vvTreeModel->addCommand(ui->treeView->currentIndex(), index_.row(), newVVCommand);
        }
        else if(vvTreeItem->getType() == VVTreeItem::BEAT && _copyVVTreeItem->getType() == VVTreeItem::BEAT)
        {
            VVBeat* beat = reinterpret_cast<VVBeat*>(_copyVVTreeItem->ptr());
            VVBeat* newVVBeat = new VVBeat();
            *newVVBeat = *beat;

            VVBeat* lastVVBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->root()->child(_vvTreeModelRight->root()->childCount() - 1)->ptr());
            newVVBeat->_beat_number = lastVVBeat->_beat_number + 1;

            int childCount = _vvTreeModelRight->itemFromIndex(curIndex.parent())->childCount();
            _vvTreeModelRight->pasteBeat(curIndex.parent(), childCount, newVVBeat);
            _vvTreeModel->addBeat(ui->treeView->currentIndex(), newVVBeat);

            //选中粘贴出来的条目
            QItemSelectionModel *_model = new QItemSelectionModel(_vvTreeModelRight);
            int rowCount = vvTreeItem->parent()->childCount();
            int lastRow = _vvTreeModelRight->root()->child(rowCount - 1)->row();
            QModelIndex headModelIndex = _vvTreeModelRight->index(lastRow, 0, curIndex.parent());
            QItemSelection selection(headModelIndex, headModelIndex);
            _model->select(selection, QItemSelectionModel::SelectCurrent);
            ui->treeView_beatAndCommand->setSelectionModel(_model);
            ui->treeView_beatAndCommand->setCurrentIndex(headModelIndex);
        }
    }
    else if(ui->treeView->hasFocus())
    {
        slot_pasteDirOrFile();
    }

    _hasModified = true;
}

void MainWindow::slot_copyDirOrFile()
{
    _copyDirAndFile = _vvTreeModel->itemFromIndex(ui->treeView->currentIndex());
    ui->action_paste->setVisible(true);
}

void MainWindow::slot_pasteDirOrFile()
{
    if(!_copyDirAndFile)
        return;

    QModelIndex curIndex = ui->treeView->currentIndex();
    VVTreeItem* vvTreeItem = _vvTreeModelRight->itemFromIndex(curIndex);

    if(vvTreeItem->getType() == VVTreeItem::DIR && _copyDirAndFile->getType() == VVTreeItem::DIR)
    {
        VVDir* dir = reinterpret_cast<VVDir*>(_copyDirAndFile->ptr());
        VVDir* newVVDir = new VVDir();
        *newVVDir = *dir;
        _vvTreeModel->addDir(newVVDir);
    }
    else if(vvTreeItem->getType() == VVTreeItem::DIR && _copyDirAndFile->getType() == VVTreeItem::FILE)
    {
        VVFile* file = reinterpret_cast<VVFile*>(_copyDirAndFile->ptr());
        VVFile* newVVFile = new VVFile();
        *newVVFile = *file;
        _vvTreeModel->addFile(newVVFile, curIndex);
    }
    else if(vvTreeItem->getType() == VVTreeItem::FILE && _copyDirAndFile->getType() == VVTreeItem::FILE)
    {
        VVFile* file = reinterpret_cast<VVFile*>(_copyDirAndFile->ptr());
        VVFile* newVVFile = new VVFile();
        *newVVFile = *file;
        _vvTreeModel->addFile(newVVFile, curIndex.parent());
    }
}

void MainWindow::slot_currentIndexChanged(const QModelIndex & current, const QModelIndex & previous)
{
    _whichFocus = 0;
}

void MainWindow::slot_remove()
{
    adjustDirFileSelection(_vvFile);

    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("是否确定删除？"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, QStringLiteral("确定"));
    msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
        return;
    default:
        break;
    }


    //! 表格--的删除
    QModelIndexList indexs = ui->treeView_beatAndCommand->selectionModel()->selectedRows();
    QModelIndexList indexsBeat;
    QModelIndexList indexsCommand;
    for(QModelIndex index: indexs)
    {
        if(!index.isValid())
            continue;
        if(!index.parent().isValid())
            indexsBeat.append(index);
        else
            indexsCommand.append(index);
    }

    qSort(indexsBeat.begin(),    indexsBeat.end(),    [](QModelIndex indexA, QModelIndex indexB){ return indexA.row() > indexB.row(); });
    qSort(indexsCommand.begin(), indexsCommand.end(), [](QModelIndex indexA, QModelIndex indexB){ return indexA.row() == indexB.row() ? indexA.parent().row() > indexB.parent().row() : indexA.row() > indexB.row(); });
    for(auto index: indexsCommand)
    {
        VVCommand* vvCommand = reinterpret_cast<VVCommand*>(_vvTreeModelRight->itemFromIndex(index)->ptr());
        VVBeat* vvBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->itemFromIndex(index.parent())->ptr());
        _vvTreeModelRight->deleteCommandRight(index.parent(), index.row(), index.row());
        _vvTreeModel->deleteCommand(ui->treeView->currentIndex(), vvBeat, vvCommand);
    }
    for(auto index: indexsBeat)
    {
        VVBeat* vvBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->itemFromIndex(index)->ptr());
        _vvTreeModelRight->deleteBeatRight(index.parent(), index.row(), index.row());
        _vvTreeModel->deleteBeat(ui->treeView->currentIndex(), vvBeat);
    }


    _hasModified = true;
    _whichFocus = 1;
    return;




    QMap<int, int> aboutToDeleteCommands = _graphicsScene->deleteCurrentCommandsPre();
    if(aboutToDeleteCommands.size() == 0) return;

    QStringList warningStrList;
    QMap<int, int>::iterator it = aboutToDeleteCommands.begin();
    for(; it != aboutToDeleteCommands.end(); ++it)
    {
        warningStrList.append(tr("%1%2%3%4%5").arg(QStringLiteral("第")).arg(QString::number(it.key())).arg(QStringLiteral("拍的")).arg(it.value()).arg(QStringLiteral("个命令")));
    }


    //! 轴线--的删除
    int activeBeat  = ui->beatArrow->getActiveBeatNumber();
    if(activeBeat == -1) return;

    /*
    QMessageBox msgBox;
    msgBox.setText(tr("%1%2%3").arg(QStringLiteral("是否确定删除")).arg(activeBeat).arg(QStringLiteral("节拍？")));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, QStringLiteral("确定"));
    msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
    default:
        return;
    }
    */

    ui->beatArrow->deleteCurrentBeat();
    //TODO.删除节拍后，删除场景中的相关item并且后续adjust操作
}

void MainWindow::slot_deleteCommandR(const QModelIndex& index)
{
    QString warningStr;
    if(_vvTreeModelRight->itemFromIndex(index)->getType() == VVTreeItem::BEAT)
        warningStr = QStringLiteral("节拍");
    else
        warningStr = QStringLiteral("命令");
    QMessageBox msgBox;
    msgBox.setText(tr("%1%2%3").arg(QStringLiteral("是否确定删除该")).arg(warningStr).arg(QStringLiteral("？")));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, QStringLiteral("确定"));
    msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
    default:
        return;
    }

    VVTreeItem::Type type = _vvTreeModelRight->itemFromIndex(index)->getType();
    if(type == VVTreeItem::BEAT)
    {
        VVBeat* vvBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->itemFromIndex(index)->ptr());
        _vvTreeModelRight->deleteBeatRight(index.parent(), index.row(), index.row());
        _vvTreeModel->deleteBeat(ui->treeView->currentIndex(), vvBeat);
    }
    else if(type == VVTreeItem::COMMAND)
    {
        VVCommand* vvCommand = reinterpret_cast<VVCommand*>(_vvTreeModelRight->itemFromIndex(index)->ptr());
        _vvTreeModelRight->deleteCommandRight(index.parent(), index.row(), index.row());
        VVBeat* vvBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->itemFromIndex(index.parent())->ptr());
        _vvTreeModel->deleteCommand(ui->treeView->currentIndex(), vvBeat, vvCommand);
    }

    _hasModified = true;
}

void MainWindow::slot_modify()
{
    if(!ui->treeView->currentIndex().isValid()) return;
    if(_vvTreeModel->dataType(ui->treeView->currentIndex()) == VVTreeItem::DIR)
    {   //! 当前点击的是文件夹
        //! Init Ui Dlg
        QDialog newFileDlg;
        newFileDlg.setFixedSize(400, 260);
        newFileDlg.setWindowTitle(QStringLiteral("修改文件夹"));
        newFileDlg.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        QLabel *label_name = new QLabel(QStringLiteral("名    称："), &newFileDlg);
        QLineEdit *lineEdit_name = new QLineEdit(&newFileDlg);
        connect(lineEdit_name, &QLineEdit::textEdited, this, [lineEdit_name, &newFileDlg, this](const QString& newText){ this->slot_showToolTip(newFileDlg, lineEdit_name, newText); });
        QPushButton *pushButtonOK = new QPushButton(QStringLiteral("确定"), &newFileDlg);
        QPushButton *pushButtonCancel = new QPushButton(QStringLiteral("取消"), &newFileDlg);
        connect(pushButtonOK, &QPushButton::clicked, &newFileDlg, &QDialog::accept);
        connect(pushButtonCancel, &QPushButton::clicked, &newFileDlg, &QDialog::reject);
        QVBoxLayout *vBoxLayout = new QVBoxLayout(&newFileDlg);
        QHBoxLayout *hBoxLayout = new QHBoxLayout(&newFileDlg);
        hBoxLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum));
        hBoxLayout->addWidget(label_name);
        hBoxLayout->addWidget(lineEdit_name);
        hBoxLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum));
        QHBoxLayout *hBoxLayout1 = new QHBoxLayout(&newFileDlg);
        hBoxLayout1->addItem(new QSpacerItem(100,5,QSizePolicy::Expanding, QSizePolicy::Minimum));
        hBoxLayout1->addWidget(pushButtonOK);
        hBoxLayout1->addWidget(pushButtonCancel);
        vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
        vBoxLayout->addLayout(hBoxLayout);
        vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
        vBoxLayout->addLayout(hBoxLayout1);
        vBoxLayout->setContentsMargins(20, 20, 20, 20);
        newFileDlg.setLayout(vBoxLayout);

        QString oldDir = _vvTreeModel->data(ui->treeView->currentIndex(), Qt::DisplayRole).toString();
        lineEdit_name->setText(oldDir);
        int ret = newFileDlg.exec();
        QString dirname = "";
        switch (ret) {
        case QDialog::Accepted:
            dirname = lineEdit_name->text();
            break;
        case QDialog::Rejected:
            break;
        }


        if(dirname.isEmpty())
            return;

        bool ok = _vvTreeModel->modifyDir(oldDir, dirname, ui->treeView->currentIndex());
        if(!ok)
        {
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("修改文件夹失败，有同名文件夹！"), QStringLiteral("确定"));
        }
    }
    else
    {   //! 当前点击的是文件
        //! Init Ui Dlg
        QDialog newFileDlg;
        newFileDlg.setFixedSize(400, 260);
        newFileDlg.setWindowTitle(QStringLiteral("修改文件"));
        newFileDlg.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        QLabel *label_name = new QLabel(QStringLiteral("名    称："), &newFileDlg);
        QLabel *label_executionNumber = new QLabel(QStringLiteral("执行次数："), &newFileDlg);
        QLabel *label_cycle = new QLabel(QStringLiteral("调度周期："), &newFileDlg);
        QLineEdit *lineEdit_name = new QLineEdit(&newFileDlg);
        connect(lineEdit_name, &QLineEdit::textEdited, this, [lineEdit_name, &newFileDlg, this](const QString& newText){ this->slot_showToolTip(newFileDlg, lineEdit_name, newText); });
        QSpinBox *spinBox_executionNumber = new QSpinBox(&newFileDlg);
        QSpinBox *spinBox_cycle = new QSpinBox(&newFileDlg);
        QComboBox *comboBox_unit = new QComboBox(&newFileDlg);
        spinBox_executionNumber->setRange(-1, MAX_BEAT);
        spinBox_executionNumber->setValue(-1);
        spinBox_executionNumber->setToolTip(QStringLiteral("默认-1：代表执行次数是无穷\n0~99999：代表执行次数即为相应数值！"));
        spinBox_cycle->setRange(1,MAX_BEAT);
        spinBox_cycle->setValue(1);
        comboBox_unit->addItems(QStringList() << "s" << "ms" << "us" << "ns");
        comboBox_unit->setCurrentText("ms");
        comboBox_unit->setFixedWidth(40);
        QPushButton *pushButtonOK = new QPushButton(QStringLiteral("确定"), &newFileDlg);
        QPushButton *pushButtonCancel = new QPushButton(QStringLiteral("取消"), &newFileDlg);
        connect(pushButtonOK, &QPushButton::clicked, &newFileDlg, [&](){
            if(spinBox_executionNumber->value() == 0)
            {
                QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("执行次数不能为零！"));
                return;
            }
            newFileDlg.accept();
        });
        connect(pushButtonCancel, &QPushButton::clicked, &newFileDlg, &QDialog::reject);
        QVBoxLayout *vBoxLayout = new QVBoxLayout(&newFileDlg);
        QGridLayout *gridLayout = new QGridLayout(&newFileDlg);
        gridLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0);
        gridLayout->addWidget(label_name, 0, 1);
        gridLayout->addWidget(lineEdit_name, 0, 2);
        gridLayout->addItem(new QSpacerItem(100, 5, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 4);
        gridLayout->addWidget(label_executionNumber, 1, 1);
        gridLayout->addWidget(spinBox_executionNumber, 1, 2);
        gridLayout->addWidget(label_cycle, 2, 1);
        QHBoxLayout *hBoxLayout_child = new QHBoxLayout(&newFileDlg);
        hBoxLayout_child->addWidget(spinBox_cycle);
        hBoxLayout_child->addWidget(comboBox_unit);
        gridLayout->addLayout(hBoxLayout_child, 2, 2);

        QHBoxLayout *hBoxLayout = new QHBoxLayout(&newFileDlg);
        hBoxLayout->addItem(new QSpacerItem(100,5,QSizePolicy::Expanding, QSizePolicy::Minimum));
        hBoxLayout->addWidget(pushButtonOK);
        hBoxLayout->addWidget(pushButtonCancel);
        vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
        vBoxLayout->addLayout(gridLayout);
        vBoxLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
        vBoxLayout->addLayout(hBoxLayout);
        vBoxLayout->setContentsMargins(20, 20, 20, 20);
        newFileDlg.setLayout(vBoxLayout);

        //! 设置原有的旧值
        QString     oldFile     = _vvTreeModel->data(ui->treeView->currentIndex(), Qt::DisplayRole).toString();
        VVTreeItem* item        = _vvTreeModel->itemFromIndex(ui->treeView->currentIndex());
        VVFile*     vvFile      = reinterpret_cast<VVFile*>(item->ptr());
        QString     oldCycleStr = vvFile->_time_interval;
        int         oldNumOfExe = vvFile->_num_of_exe;
        int         oldCycle;
        QString     oldUnit;
        BeatArrow::ConvertTimeStr2ValueAndUnit(oldCycleStr, oldCycle, oldUnit);
        lineEdit_name->setText(oldFile);
        spinBox_executionNumber->setValue(oldNumOfExe);
        spinBox_cycle->setValue(oldCycle);
        comboBox_unit->setCurrentText(oldUnit);
        int     ret             = newFileDlg.exec();
        QString filename        = "";
        int     executionNumber = -1;
        QString cycleStr        = "";
        int     cycle           = spinBox_cycle->value();
        QString unit            = comboBox_unit->currentText();
        switch (ret) {
        case QDialog::Accepted:
            //! 获取现在的新值
            filename        = lineEdit_name->text();
            executionNumber = spinBox_executionNumber->value();
            cycleStr        = QString::number(cycle) + unit;
            break;
        case QDialog::Rejected:
            break;
        }

        //! [0]添加对新的调度周期 & 旧的调度周期，作出改变后的相关操作
        qDebug() << "oldCycleStr:" << oldCycleStr << ", newCycleStr:" << cycleStr << endl;
        if(cycle > oldCycle)
        {   // 调度周期变大-->节拍不变，只变调度周期！
            QMessageBox msgBox;
            msgBox.setText(QStringLiteral("是否确定将调度周期调大？\n结果：\n"
                                          "脚本中的所有节拍不变（即0节拍还是0节拍，1节拍还是1节拍），\n"
                                          "但节拍间隔增大（即0节拍和1节拍的实际间隔时间变大）！"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setButtonText(QMessageBox::Yes, QStringLiteral("确定"));
            msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();
            switch (ret) {
            case QMessageBox::Yes:
                break;
            case QMessageBox::No:
            default:
                return;
            }
        }
        else if(cycle < oldCycle)
        {   // 调度周期变小-->节拍不变，只变调度周期！
            //            -->能够被整除，则节拍自动变大，调度周期当然也变！
            QMessageBox msgBox;
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(QStringLiteral("是否确定将调度周期调小？<br>结果：<br>"
                                          "若新的调度周期<span style=\" color:#ff0000;\">不能被</span>旧的调度周期<span style=\" color:#ff0000;\">整除</span>，则：<span style=\" text-decoration: underline;\">调度周期减小但节拍不变</span><br>"
                                          "（即：原来<b>0</b>拍、<b>1</b>拍、<b>2</b>拍[调度周期为10ms]--><b>0</b>拍、<b>1</b>拍、<b>2</b>拍[调度周期为8ms]）<br>"
                                          "若新的调度周期<span style=\" color:#ff0000;\">可以被</span>旧的调度周期<span style=\" color:#ff0000;\">整除</span>，则：<span style=\" text-decoration: underline;\">调度周期减小且节拍变大</span>！<br>"
                                          "（即：原来<b>0</b>拍、<b>1</b>拍、<b>2</b>拍[调度周期为10ms]--><b>0</b>拍、<b>2</b>拍、<b>4</b>拍[调度周期为5ms]）"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setButtonText(QMessageBox::Yes, QStringLiteral("确定"));
            msgBox.setButtonText(QMessageBox::No, QStringLiteral("取消"));
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();
            switch (ret) {
            case QMessageBox::Yes:
                break;
            case QMessageBox::No:
            default:
                return;
            }
        }
        //! [0]

        int multiplier = oldCycle / cycle;
        int remainder  = oldCycle % cycle;
        if(remainder != 0)
        {
            multiplier = 0;
        }

        ui->beatArrow->setCycleStr(vvFile->_time_interval);

        QModelIndex curIndex = ui->treeView->currentIndex();
        if(!filename.isEmpty())
        {
            bool ok = _vvTreeModel->modifyFile(vvFile, filename, executionNumber, cycleStr, curIndex, multiplier);
            if(!ok)
            {
                QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("修改文件失败，有同名文件！"), QStringLiteral("确定"));
                return;
            }
        }
    }

    _hasModified = true;
}

void MainWindow::slot_editR()
{
    adjustDirFileSelection(_vvFile);

    VVTreeItem::Type type = _vvTreeModelRight->itemFromIndex(ui->treeView_beatAndCommand->currentIndex())->getType();
    if(type == VVTreeItem::BEAT)
    {
        slot_editBeat(ui->treeView_beatAndCommand->currentIndex());
    }
    else if(type == VVTreeItem::COMMAND)
    {
        slot_editCommand(ui->treeView_beatAndCommand->currentIndex());
    }

    _whichFocus = 1;
}

// 右侧-编辑节拍
void MainWindow::slot_modifyBeat()
{
    //    int activeBeat = ui->beatArrow->getActiveBeatNumber();
    //    if(activeBeat == -1) return;

    VVTreeItem *vvTreeItem = _vvTreeModel->itemFromIndex(ui->treeView->currentIndex());
    if(vvTreeItem->getType() != VVTreeItem::FILE)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请在树上选中一个文件！"), QStringLiteral("确定"));
        return;
    }

    VVFile *vvFile = reinterpret_cast<VVFile*>(vvTreeItem->ptr());
    VVBeat *vvBeat = reinterpret_cast<VVBeat*>(_vvTreeModelRight->itemFromIndex(ui->treeView_beatAndCommand->currentIndex())->ptr());
    int activeBeat = vvBeat->_beat_number;
    int index = vvFile->_beats.indexOf(vvBeat);
    int lastBeat = -1;
    if(index > 0)
        lastBeat = vvFile->_beats.at(index - 1)->_beat_number;


    int value; QString unit;
    BeatArrow::ConvertTimeStr2ValueAndUnit(vvFile->_time_interval, value, unit);
    //!                                              这里是修改, 真正的上一拍
    AddBeatDlg addBeatDlg(activeBeat - 1, value, unit,  true, lastBeat);
    addBeatDlg.setInfo(activeBeat, vvBeat->_type, vvBeat->_description);
    int ret = addBeatDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
    {
        int beat;
        int type;
        QString des;
        addBeatDlg.getInfo(beat, type, des);
        //! 修改节拍
        //! step1. BeatArrow
        //        bool ok = ui->beatArrow->modifyBeat(oldBeatNumber, beat);
        //        if(!ok)
        //        {
        //            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("已存在该节拍！"), QStringLiteral("确定"));
        //            return;
        //        }

        //! step2. treeModel
        _vvTreeModel->modifyBeat(vvBeat, beat, type, des, ui->treeView->currentIndex());

        //! step3. 场景
        _graphicsScene->addBeat();

        if(ui->beatArrow->getFirstIndex() == 0)
        {   //QGraphicsView没有滚动条
            qDebug() << "getFirstIndex == 0" << endl;
        }
        else
        {   //QGraphicsView存在滚动条，纵向增大QGraphicsScene高度
            _graphicsScene->adjustSceneRect();
            ui->graphicsView->verticalScrollBar()->setValue(1000000);//这里设置滚动到最下面
        }
    }
        break;
    case QDialog::Rejected:
        return;
    }

    _hasModified = true;
}

void MainWindow::slot_modifyCommand()
{
    _graphicsScene->modifyCurrentCommand();
}

void MainWindow::slot_save()
{
//    QProgressDialog dlg(this);
//    dlg.setWindowFlag(Qt::FramelessWindowHint,true);
//    dlg.setStyleSheet("QDialog{border:1px solid darkBlue} QProgressBar{border:1px solid grey}");
//    dlg.setModal(true);
//    dlg.setValue(0);
//    dlg.setCancelButton(nullptr);

    bool ok = DataFactory::get_instance()->save(_vvTreeModel->root());
    if(!ok)
    {
        QString debugInfo = DataFactory::get_instance()->getDebugInfo();
        QMessageBox::information(this, QStringLiteral("提示"), QString("%1%2").arg(QStringLiteral("保存失败！\n当前打开脚本中有空白项，请检查！\n文件路径如下：\n")).arg(debugInfo), QStringLiteral("确定"));
        return;
    }
    QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("保存成功！"), QStringLiteral("确定"));
    _hasModified = false;
}

void MainWindow::slot_beatBasedTriggered(bool checked)
{
    if(checked)
        ui->action_timeBased->setChecked(false);
    else
        ui->action_beatBased->setChecked(true);

    _basedType = 0;
    updateBeatArrowBasedType(_basedType);

}

void MainWindow::slot_timeBasedTriggered(bool checked)
{
    if(checked)
        ui->action_beatBased->setChecked(false);
    else
        ui->action_timeBased->setChecked(true);

    _basedType = 1;
    updateBeatArrowBasedType(_basedType);
}

void MainWindow::slot_basedTriggered(bool checked)
{
    Q_UNUSED(checked)
    QAction* action = qobject_cast<QAction*>(sender());
    Q_UNUSED(action)
    if(_basedType == 0)
    {
        _basedType = 1;
    }
    else if(_basedType == 1)
    {
        _basedType = 2;
    }
    else if(_basedType == 2)
    {
        _basedType = 0;
    }
    updateBeatArrowBasedType(_basedType);
}

void MainWindow::slot_arrowTriggered(bool checked)
{
    if(checked)
        ui->action_table->setChecked(false);
    else
        ui->action_arrow->setChecked(true);

    _showType = 0;
    updateShowType();
}

void MainWindow::slot_tableTriggered(bool checked)
{
    if(checked)
        ui->action_arrow->setChecked(false);
    else
        ui->action_table->setChecked(true);

    _showType = 1;
    updateShowType();
}

void MainWindow::updateBeatArrowBasedType(int basedType)
{
    ui->beatArrow->setBasedType(basedType);
    if(_vvTreeModelRight)
    {
        _vvTreeModelRight->setBasedType(basedType);
    }
    _whichFocus = 0;
}

void MainWindow::updateShowType()
{
    ui->stackedWidget->setCurrentIndex(_showType);
}



void MainWindow::slot_addOneCommand(int beatIndex) //这里的beatIndex是第几个节拍
{
    if(!ui->treeView->currentIndex().parent().isValid())
        return;

    AddCommandDlg addCommandDlg(_icd_data, _vvFile);
    int ret = addCommandDlg.exec();
    switch (ret) {
    case QDialog::Accepted:
    {
        VVCommand *vvCommand = addCommandDlg.getInfo();
        //结构树上添加 新的信息
        _vvTreeModel->addCommand(ui->treeView->currentIndex(), beatIndex, vvCommand);
        //场景里面添加 新的信息
        _graphicsScene->addCommand(beatIndex, vvCommand);
    }
        break;
    case QDialog::Rejected:

        break;
    }
}

void MainWindow::slot_addBeatBefore(int lastBeatNumber, int curBeatNumber)
{
    adjustDirFileSelection(_vvFile);

    int interval; QString unit;
    bool ok = getFileIntervalAndUnit(interval, unit);
    if(!ok)
        return;

    AddBeatDlg addBeatDlg(curBeatNumber - 1, interval, unit,true,lastBeatNumber+1);
    addBeatDlg.setRange(lastBeatNumber+1, curBeatNumber-1);
    int ret = addBeatDlg.exec();
    if(ret == QDialog::Accepted)
    {
        int beat; int type; QString des;
        addBeatDlg.getInfo(beat, type, des);
        /*
        // step1.treeView
        int startIndex = 0;
        for(int i = 0; i < _vvFile->_beats.size(); ++i)
        {
            if(_vvFile->_beats.at(i)->_beat_number == curBeatNumber)
            {
                startIndex = i;
                break;
            }
        }
        VVBeat* vvBeat = new VVBeat();
        vvBeat->_type = type;
        vvBeat->_beat_number = beat;
        vvBeat->_time_interval = _vvFile->_time_interval;
        _vvTreeModel->insertBeat(ui->treeView->currentIndex(), startIndex, vvBeat);

        // step2.beatArrow
        ui->beatArrow->insertBeat(startIndex, beat);

        // step3.viewport
        _graphicsScene->insertBeat(startIndex);
        _graphicsScene->adjustSceneRect();
        */





        int startIndex = 0;
        for(int i = 0; i < _vvFile->_beats.size(); ++i)
        {
            if(_vvFile->_beats.at(i)->_beat_number == curBeatNumber)
            {
                startIndex = i;
                break;
            }
        }
        VVBeat* vvBeat = new VVBeat();
        vvBeat->_type = type;
        vvBeat->_beat_number = beat;
        vvBeat->_time_interval = _vvFile->_time_interval;
        // step4. treeViewRight
        _vvTreeModelRight->insertBeatRight(ui->treeView_beatAndCommand->currentIndex().parent(), startIndex, vvBeat);
        _vvTreeModel->insertBeat(ui->treeView->currentIndex(), startIndex, vvBeat);
    }

    _whichFocus = 1;
}

void MainWindow::slot_addBeatAfter(int curBeatNumber)
{
    adjustDirFileSelection(_vvFile);

    int interval; QString unit;
    bool ok = getFileIntervalAndUnit(interval, unit);
    if(!ok)
        return;

    AddBeatDlg addBeatDlg(curBeatNumber, interval, unit);
    addBeatDlg.setRange(curBeatNumber+1, MAX_BEAT);
    int ret = addBeatDlg.exec();
    if(ret == QDialog::Accepted)
    {
        int beat; int type; QString des;
        addBeatDlg.getInfo(beat, type, des);
        /*
        // step1.treeView
        int startIndex = 0;
        for(int i = 0; i < _vvFile->_beats.size(); ++i)
        {
            if(_vvFile->_beats.at(i)->_beat_number == curBeatNumber)
            {
                startIndex = i + 1;
                break;
            }
        }
        VVBeat* vvBeat = new VVBeat();
        vvBeat->_type = type;
        vvBeat->_beat_number = beat;
        vvBeat->_time_interval = _vvFile->_time_interval;
        _vvTreeModel->insertBeat(ui->treeView->currentIndex(), startIndex, vvBeat);

        // step2.beatArrow
        ui->beatArrow->insertBeat(startIndex, beat);

        // step3.viewport
        _graphicsScene->insertBeat(startIndex);
        _graphicsScene->adjustSceneRect();
        */






        int startIndex = 0;
        for(int i = 0; i < _vvFile->_beats.size(); ++i)
        {
            if(_vvFile->_beats.at(i)->_beat_number == curBeatNumber)
            {
                startIndex = i;
                break;
            }
        }
        VVBeat* vvBeat = new VVBeat();
        vvBeat->_type = type;
        vvBeat->_beat_number = beat;
        vvBeat->_time_interval = _vvFile->_time_interval;

        // step4. treeViewRight
        startIndex ++;
        _vvTreeModelRight->insertBeatRight(ui->treeView_beatAndCommand->currentIndex().parent(), startIndex, vvBeat);
        _vvTreeModel->insertBeat(ui->treeView->currentIndex(), startIndex, vvBeat);
    }
    _whichFocus = 1;
}

void MainWindow::slot_activeClicked(int activeIndex)
{
    _graphicsScene->highLight(activeIndex);
}

void MainWindow::resetSceneHeight()
{
    int yMinHeight = _graphicsScene->getYMinHeight();
    int standardHeight = ui->beatArrow->getBeatCount() * VerticalInterval;
    yMinHeight = yMinHeight < standardHeight ? standardHeight: yMinHeight;
    _graphicsScene->setYMinHeight(yMinHeight);
}

void MainWindow::slot_treeViewClicked(const QModelIndex &index)
{
    VVTreeItem *vvTreeItem = _vvTreeModel->itemFromIndex(index);
    VVTreeItem::Type type = vvTreeItem->getType();
    QStringList icdList;
    if(type == VVTreeItem::DIR)
    {
        _whichFocus = 0;
        ui->treeView->setFocus();
        ui->action_copy->setVisible(true);
        ui->action_delete->setVisible(true);
        return;
    }
    //![0] 先检测当前右侧的treeView中的内容是否符合要求，TODO.左侧树内容较少，这里先这样
    if(_vvFile)
    {
        for(int i = 0; i < _vvFile->_beats_count; ++i)
        {
            VVBeat* vvBeat = _vvFile->_beats.at(i);
            for(int j = 0; j < vvBeat->_commands_count; ++j)
            {
                VVCommand* vvCommand = vvBeat->_commands.at(j);
                if((vvCommand->_msg_name.isEmpty()
                    || vvCommand->_msg_id.isEmpty()
                    || vvCommand->_sig_name.isEmpty()
                    || vvCommand->_sig_id.isEmpty())
                  &&
                  (vvCommand->_command_type_str == "SET"
                    || vvCommand->_command_type_str == "CHECK"
                    || vvCommand->_command_type_str == "SVPC_CHECK"
                    || vvCommand->_command_type_str == "COMBINATION"
                    || vvCommand->_command_type_str == "PUSH_INTO_FIFO"))
                {
                    QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("当前打开脚本中有空白项，请检查！"), QStringLiteral("确定"));
                    for(int k = 0; k < _vvTreeModel->root()->childCount(); ++k)
                    {
                        if(_vvTreeModel->root()->child(k)->data(0).toString() == ui->treeView->currentIndex().parent().data().toString())
                        {
                            for(int m = 0; m < _vvTreeModel->root()->child(k)->childCount(); ++m)
                            {
                                if(_vvTreeModel->root()->child(k)->child(m)->data(0) == _vvFile->_file_name)
                                {
                                    QModelIndex rightIndex = ui->treeView->currentIndex().sibling(m, 0);
                                    _vvTreeModel->root()->child(k)->child(m);
                                    ui->treeView->setCurrentIndex(rightIndex);
                                    break;
                                }
                            }
                        }
                    }

                    _whichFocus = 0;
                    ui->treeView->setFocus();
                    ui->action_copy->setVisible(true);
                    ui->action_delete->setVisible(true);
                    return;
                }
            }
        }
    }
    //![0]



    //! 初始化 beatArrow
    _vvFile = static_cast<VVFile *>(vvTreeItem->ptr());

    for(int i = 0; i < _vvFile->_beats_count; ++i)
    {
        VVBeat* vvBeat = _vvFile->_beats.at(i);
        for(int j = 0; j < vvBeat->_commands_count; ++j)
        {
            VVCommand* vvCommand = vvBeat->_commands.at(j);
            checkIcdExist(vvCommand,icdList);
        }
    }

    VVDir* vvDir = static_cast<VVDir *>(_vvTreeModel->itemFromIndex(index.parent())->ptr());
    this->setWindowTitle(tr("%1------%2/%3").arg(WINDOW_TITLE).arg(vvDir->_dir_name).arg(_vvFile->_file_name));
    ui->beatArrow->initFromFile(_vvFile);
    ui->beatArrow->setCycleStr(_vvFile->_time_interval);

    //! 初始化视口
    resetSceneHeight();
    ui->graphicsView->verticalScrollBar()->setValue(0);
    _graphicsScene->initFromFile(_vvFile);
    _graphicsScene->adjustSceneRect(ui->graphicsView->width(), ui->graphicsView->height());

    //! 初始化中央窗口
    ui->treeView_beatAndCommand->setEnabled(true);
    initBeatAndCommandView(_vvFile);

    updateBeatArrowBasedType(_basedType);

    _whichFocus = 0;
    ui->treeView->setFocus();
    ui->action_copy->setVisible(true);
    ui->action_delete->setVisible(true);
    if(_copyDirAndFile != nullptr && _copyDirAndFile->getType() == VVTreeItem::FILE)
    {
        ui->action_paste->setVisible(true);
    }
    if(icdList.size()>0){
        icdList=icdList.toSet().toList();//去掉重复

        QString str=icdList.join(",");

        QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("警告：变量ICD：【")+str+QStringLiteral("】在工作空间未找到！"));
    }

}

void MainWindow::slot_graphicsViewHValueChanged()
{
    updateRuleBar();
}

void MainWindow::slot_graphicsViewVValueChanged()
{
    updateRuleBar();
}
void MainWindow::slot_changed()
{
   _hasModified=true;
}
