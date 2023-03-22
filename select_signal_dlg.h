/***********************************************************************
* @filename select_signal_dlg.h
* @brief    提供一个选择目标ICD的窗口
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef SELECTSIGNALDLG_H
#define SELECTSIGNALDLG_H

#include <QDialog>
#include <QTableWidgetItem>
#include "vv_typedef.h"

namespace Ui {
class SelectSignalDlg;
}

class SelectSignalDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectSignalDlg(QList<ICDInfo> infoList, bool isFIFO = false, bool isClearFIFO = false, QWidget *parent = nullptr);
    ~SelectSignalDlg();
    ICDSignal getInfo();
    void setInfo(const QString &msgID, const QString& sigID);

private:
    void initUi();

private slots:
    void slot_ICDItemClicked(QTableWidgetItem *item);

    void on_lineEdit_icdFilter_textChanged(const QString &arg1);

    void on_lineEdit_pinFilter_textChanged(const QString &arg1);

    void on_pushButton_confirm_clicked();

    void on_pushButton_cancel_clicked();

    void on_lineEdit_FIFOFilter_textChanged(const QString &arg1);

    void on_radioButton_name_toggled(bool checked);

    void on_radioButton_id_toggled(bool checked);

private:
    Ui::SelectSignalDlg *ui;
    QList<ICDInfo> _infoList;
    QList<ICDInfo> _infoListFIFO;
    ICDInfo _curICD;
    bool _isFIFO;
    bool _isClearFIFO;
};

#endif // SELECTSIGNALDLG_H
