/***********************************************************************
* @filename add_beat_dlg.h
* @brief    本类是添加节拍对话框类
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef ADD_BEAT_DLG_H
#define ADD_BEAT_DLG_H

#include <QDialog>

namespace Ui {
class AddBeatDlg;
}

class AddBeatDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddBeatDlg(int lastBeat, int interval, QString unit, bool isModify = false, int realLastBeat = -1, QWidget *parent = nullptr);
    ~AddBeatDlg();

    void setInfo(int beat, int type, QString desc);
    void getInfo(int &beat, int &type, QString &des);

    void setRange(int min, int max);

private slots:

    void on_comboBox_type_currentIndexChanged(int index);
    void on_lineEdit_time_editingFinished();
    void slot_textChanged();

    void on_lineEdit_timeBetween_editingFinished();

    void on_spinBox_beat_valueChanged(const QString &arg1);

    void on_spinBox_beat_editingFinished();

private:
    Ui::AddBeatDlg *ui;
    bool _isModify;     //是新增，还是修改

    int _interval;      //时间间隔（调度周期）
    int _lastBeat;      //其值为：当前拍-1
    int _realLastBeat;  //上一节拍
    int _min;
    int _max;
};

#endif // ADD_BEAT_DLG_H
