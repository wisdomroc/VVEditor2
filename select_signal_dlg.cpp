#include "select_signal_dlg.h"
#include "ui_select_signal_dlg.h"
#include <QDebug>
#include <QMessageBox>

SelectSignalDlg::SelectSignalDlg(QList<ICDInfo> infoList, bool isFIFO, bool isClearFIFO, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectSignalDlg),
    _infoList(infoList),
    _isFIFO(isFIFO),
    _isClearFIFO(isClearFIFO)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowTitle(QStringLiteral("选择命令目标"));

    initUi();
}

SelectSignalDlg::~SelectSignalDlg()
{
    delete ui;
}

ICDSignal SelectSignalDlg::getInfo()
{
    ICDSignal icdSignal;
    int rowNumber = ui->tableWidget_icd->currentRow();
    if(rowNumber == -1)
    {
        int rowNumberFIFO = ui->tableWidget_FIFO->currentRow();
        if(rowNumberFIFO == -1)
        {
            return icdSignal;
        }
        else
        {
            SingalInfo signalInfo;
            signalInfo.signalName_ = _infoListFIFO.at(rowNumberFIFO).messageName_;
            signalInfo.signalID_   = _infoListFIFO.at(rowNumberFIFO).messageID_;
            icdSignal.signalInfo_ = signalInfo;
            return icdSignal;
        }
    }

    icdSignal.messageID_ = _infoList.at(rowNumber).messageID_;
    icdSignal.messageName_ = _infoList.at(rowNumber).messageName_;

    rowNumber = ui->tableWidget_pin->currentRow();


    if(_isFIFO)
    {
        rowNumber = ui->tableWidget_FIFO->currentRow();
        SingalInfo signalInfo;
        if(rowNumber != -1)
        {
            signalInfo.signalName_ = _infoListFIFO.at(rowNumber).messageName_;
            signalInfo.signalID_   = _infoListFIFO.at(rowNumber).messageID_;
        }
        icdSignal.signalInfo_ = signalInfo;
    }
    else
    {
        if(rowNumber == -1)
        {
            return icdSignal;
        }
        icdSignal.signalInfo_ = _curICD.signals_.at(rowNumber);
    }

    return icdSignal;
}

void SelectSignalDlg::setInfo(const QString &msgID, const QString &sigID)
{
    ui->tableWidget_icd->clearSelection();
    ui->tableWidget_pin->clearSelection();

    for(int i = 0; i < ui->tableWidget_icd->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->tableWidget_icd->item(i, 0);//! 0为name，1为id。由于执行端代码原因，这里使用ICDName
        if(item->text() == msgID)
        {
            ui->tableWidget_icd->setCurrentItem(item);
            slot_ICDItemClicked(item);
        }
    }
    for(int i = 0; i < ui->tableWidget_pin->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->tableWidget_pin->item(i, 1);//! 0为name，1为id。由于执行端代码原因，这里使用sigID
        if(item->text() == sigID)
        {
            ui->tableWidget_pin->setCurrentItem(item);
        }
    }
    for(int i = 0; i < ui->tableWidget_FIFO->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->tableWidget_FIFO->item(i, 0);
        if(item->text() == sigID)
        {
            ui->tableWidget_FIFO->setCurrentItem(item);
        }
    }
}

void SelectSignalDlg::initUi()
{
    //!-------------------------------------- ICD init ------------------------------------------------
    QStringList icdHeaders;
    icdHeaders << QStringLiteral("消息名称") << QStringLiteral("消息ID");// << QStringLiteral("信号数量") << QStringLiteral("摘要");
    ui->tableWidget_icd->setColumnCount(icdHeaders.size());
    ui->tableWidget_icd->setHorizontalHeaderLabels(icdHeaders);
    ui->tableWidget_icd->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_icd->setRowCount(_infoList.size());
    for (int i = 0; i < _infoList.size(); i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(_infoList.at(i).messageName_);
        QTableWidgetItem *item1 = new QTableWidgetItem(_infoList.at(i).messageID_);
//        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(_infoList.at(i).singals_.count()));
//        QTableWidgetItem *item3 = new QTableWidgetItem(_infoList.at(i).dataPkgSummary_);
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
//        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
//        item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_icd->setItem(i, 0, item0);
        ui->tableWidget_icd->setItem(i, 1, item1);
//        ui->tableWidget_icd->setItem(i, 2, item2);
//        ui->tableWidget_icd->setItem(i, 3, item3);
    }
    ui->tableWidget_icd->setColumnWidth(0, 130);
    ui->tableWidget_icd->setColumnWidth(1, 140);
//    ui->tableWidget_icd->setColumnWidth(2, 60);
    ui->tableWidget_icd->horizontalHeader()->setStretchLastSection(true);

    //!------------------------------------------- Signal init --------------------------------------------
    QStringList pinHeaders;
    pinHeaders << QStringLiteral("信号名称") << QStringLiteral("信号ID") << QStringLiteral("类型") << QStringLiteral("长度") << QStringLiteral("默认值");
    ui->tableWidget_pin->setColumnCount(pinHeaders.size());
    ui->tableWidget_pin->setHorizontalHeaderLabels(pinHeaders);
    ui->tableWidget_pin->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_pin->setColumnWidth(0, 100);
    ui->tableWidget_pin->setColumnWidth(1, 140);
    ui->tableWidget_pin->setColumnWidth(2, 60);
    ui->tableWidget_pin->setColumnWidth(3, 40);
    ui->tableWidget_pin->horizontalHeader()->setStretchLastSection(true);

    //!-------------------------------------------- FIFO init ------------------------------------------------
    QStringList icdHeadersFIFO;
    icdHeadersFIFO << QStringLiteral("消息名称") << QStringLiteral("消息ID");// << QStringLiteral("信号数量") << QStringLiteral("摘要");
    ui->tableWidget_FIFO->setColumnCount(icdHeadersFIFO.size());
    ui->tableWidget_FIFO->setHorizontalHeaderLabels(icdHeadersFIFO);
    ui->tableWidget_FIFO->setSelectionBehavior(QAbstractItemView::SelectRows);
    _infoListFIFO.clear();
    for(auto i: _infoList)
    {
        if(i.messageID_.contains("FIFO") || i.messageName_.contains("FIFO"))
        {
            _infoListFIFO.append(i);
        }
    }
    ui->tableWidget_FIFO->setRowCount(_infoListFIFO.size());
    for (int i = 0; i < _infoListFIFO.size(); i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(_infoListFIFO.at(i).messageName_);
        QTableWidgetItem *item1 = new QTableWidgetItem(_infoListFIFO.at(i).messageID_);
//        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(_infoList.at(i).singals_.count()));
//        QTableWidgetItem *item3 = new QTableWidgetItem(_infoList.at(i).dataPkgSummary_);
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
//        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
//        item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_FIFO->setItem(i, 0, item0);
        ui->tableWidget_FIFO->setItem(i, 1, item1);
//        ui->tableWidget_icd->setItem(i, 2, item2);
//        ui->tableWidget_icd->setItem(i, 3, item3);
    }
    ui->tableWidget_FIFO->setColumnWidth(0, 130);
    ui->tableWidget_FIFO->setColumnWidth(1, 140);
//    ui->tableWidget_icd->setColumnWidth(2, 60);
    ui->tableWidget_FIFO->horizontalHeader()->setStretchLastSection(true);

    if(_isFIFO)
    {
        ui->frame_FIFO->setVisible(true);
        ui->frame_pin->setVisible(false);
    }
    else
    {
        ui->frame_FIFO->setVisible(false);
        ui->frame_pin->setVisible(true);
    }
    if(_isClearFIFO)
    {
        ui->frame_icd->setVisible(false);
    }
    else
    {
        ui->frame_icd->setVisible(true);
    }



    connect(ui->tableWidget_icd, &QTableWidget::itemClicked, this, &SelectSignalDlg::slot_ICDItemClicked);
}

void SelectSignalDlg::slot_ICDItemClicked(QTableWidgetItem *item)
{
    _curICD.messageID_   = "";
    _curICD.messageName_ = "";
    QTableWidgetItem *item_need = ui->tableWidget_icd->item(item->row(), 1);
    QString needStr = item_need->text();
    for (ICDInfo icd: _infoList)
    {
        if(icd.messageID_ == needStr)
        {
            _curICD = icd;
            break;
        }
    }
    if(_curICD.messageID_ == "" && _curICD.messageName_ == "")
    {
        ui->tableWidget_pin->setRowCount(0);
        return;
    }

    int rowCount = _curICD.signals_.count();
    ui->tableWidget_pin->setRowCount(0);
    ui->tableWidget_pin->setRowCount(rowCount);
    for (int i = 0; i < rowCount; i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(_curICD.signals_.at(i).signalName_);
        QTableWidgetItem *item1 = new QTableWidgetItem(_curICD.signals_.at(i).signalID_);
        QTableWidgetItem *item2 = new QTableWidgetItem(_curICD.signals_.at(i).strType_);
        QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(_curICD.signals_.at(i).lenght_));
        QTableWidgetItem *item4 = new QTableWidgetItem(_curICD.signals_.at(i).strValue_);
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
        item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
        item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_pin->setItem(i, 0, item0);
        ui->tableWidget_pin->setItem(i, 1, item1);
        ui->tableWidget_pin->setItem(i, 2, item2);
        ui->tableWidget_pin->setItem(i, 3, item3);
        ui->tableWidget_pin->setItem(i, 4, item4);
    }
}

void SelectSignalDlg::on_lineEdit_icdFilter_textChanged(const QString &arg1)
{
    int rowCount = ui->tableWidget_icd->rowCount();
    int columnNumber = 0;
    if(ui->radioButton_id->isChecked())
    {
        columnNumber = 1;
    }

    for (int i=0; i<rowCount; i++)
    {
        ui->tableWidget_icd->item(i, columnNumber)->text().contains(arg1, Qt::CaseInsensitive) ? ui->tableWidget_icd->setRowHidden(i, false) : ui->tableWidget_icd->setRowHidden(i, true);
    }
}

void SelectSignalDlg::on_lineEdit_pinFilter_textChanged(const QString &arg1)
{
    int rowCount = ui->tableWidget_pin->rowCount();
    int columnNumber = 0;
    if(ui->radioButton_id->isChecked())
    {
        columnNumber = 1;
    }

    for (int i=0; i<rowCount; i++)
    {
        ui->tableWidget_pin->item(i, columnNumber)->text().contains(arg1, Qt::CaseInsensitive) ? ui->tableWidget_pin->setRowHidden(i, false) : ui->tableWidget_pin->setRowHidden(i, true);
    }
}

void SelectSignalDlg::on_lineEdit_FIFOFilter_textChanged(const QString &arg1)
{
    int rowCount = ui->tableWidget_FIFO->rowCount();
    int columnNumber = 0;
    if(ui->radioButton_id->isChecked())
    {
        columnNumber = 1;
    }

    for (int i=0; i<rowCount; i++)
    {
        ui->tableWidget_FIFO->item(i, columnNumber)->text().contains(arg1, Qt::CaseInsensitive) ? ui->tableWidget_FIFO->setRowHidden(i, false) : ui->tableWidget_FIFO->setRowHidden(i, true);
    }
}

void SelectSignalDlg::on_pushButton_confirm_clicked()
{
    if(!ui->tableWidget_pin->currentIndex().isValid() && !_isFIFO)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择一个信号！"), QStringLiteral("确定"));
        return;
    }
    if(!ui->tableWidget_FIFO->currentIndex().isValid() && _isFIFO)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择一个FIFO！"), QStringLiteral("确定"));
        return;
    }
    this->accept();
}

void SelectSignalDlg::on_pushButton_cancel_clicked()
{
    this->reject();
}

void SelectSignalDlg::on_radioButton_name_toggled(bool checked)
{
    if(checked)
    {
        on_lineEdit_icdFilter_textChanged(ui->lineEdit_icdFilter->text());
        on_lineEdit_pinFilter_textChanged(ui->lineEdit_pinFilter->text());
        on_lineEdit_FIFOFilter_textChanged(ui->lineEdit_FIFOFilter->text());
    }
}

void SelectSignalDlg::on_radioButton_id_toggled(bool checked)
{
    if(checked)
    {
        on_lineEdit_icdFilter_textChanged(ui->lineEdit_icdFilter->text());
        on_lineEdit_pinFilter_textChanged(ui->lineEdit_pinFilter->text());
        on_lineEdit_FIFOFilter_textChanged(ui->lineEdit_FIFOFilter->text());
    }
}
