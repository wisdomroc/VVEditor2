#include "add_beat_dlg.h"
#include "ui_add_beat_dlg.h"
#include "vv_typedef.h"

AddBeatDlg::AddBeatDlg(int lastBeat, int interval, QString unit, bool isModify, int realLastBeat, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBeatDlg),
    _isModify(isModify),
    _interval(interval),
    _lastBeat(lastBeat),
    _realLastBeat(realLastBeat),
    _min(-1),
    _max(-1)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowTitle(QStringLiteral("添加节拍"));
    if(isModify) setWindowTitle(QStringLiteral("修改节拍"));

    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &AddBeatDlg::slot_textChanged);

    QIntValidator *intValidator = new QIntValidator();
    QIntValidator *intValidatorBetween = new QIntValidator();
    intValidator->setRange(0, MAX_BEAT);
    intValidatorBetween->setRange(0, MAX_BEAT);
    ui->lineEdit_time->setValidator(intValidator);
    ui->lineEdit_timeBetween->setValidator(intValidatorBetween);

    ui->spinBox_beat->setValue(lastBeat+1);
    if(_isModify)
        ui->spinBox_beat->setRange(realLastBeat+1, MAX_BEAT);
    if(!_isModify)
        ui->spinBox_beat->setRange(lastBeat+1, MAX_BEAT);
    ui->lineEdit_time->setText(QString::number((lastBeat+1)*_interval));
    ui->label_unit->setText(unit);
    ui->lineEdit_timeBetween->setToolTip(QStringLiteral("请输入周期的整数倍"));

    //! 添加间隔时间的支持
    ui->lineEdit_timeBetween->setText(QString::number((1)*_interval));
    if(_lastBeat==-1)
       ui->lineEdit_timeBetween->setText(QString::number(0));
    if(_isModify)
    {
        if( _realLastBeat != -1)
            ui->lineEdit_timeBetween->setText(QString::number((lastBeat+1-_realLastBeat)*_interval));
        else
            ui->lineEdit_timeBetween->setText(QString::number((lastBeat+1)*_interval));
    }
    ui->label_unitBetween->setText(unit);

    ui->label_time->setVisible(false);
    ui->lineEdit_time->setVisible(false);
    ui->label_unit->setVisible(false);
}

AddBeatDlg::~AddBeatDlg()
{
    delete ui;
}

void AddBeatDlg::setInfo(int beat, int type, QString desc)
{
    if(type == -1)
    {
        type = 0;
    }
    ui->spinBox_beat->setValue(beat);
    ui->comboBox_type->setCurrentIndex(type);
    ui->plainTextEdit->setPlainText(desc);
}

void AddBeatDlg::getInfo(int &beat, int &type, QString& des)
{
    beat = ui->spinBox_beat->value();
    type = ui->comboBox_type->currentIndex();
    des  = ui->plainTextEdit->toPlainText();
}

void AddBeatDlg::setRange(int min, int max)
{
    _min = min;
    _max = max;
    ui->spinBox_beat->setRange(min, max);
}

void AddBeatDlg::on_comboBox_type_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->label_beat->setVisible(true);
        ui->spinBox_beat->setVisible(true);
        ui->label_time->setVisible(false);
        ui->lineEdit_time->setVisible(false);
        ui->label_unit->setVisible(false);
    }
    else
    {
        ui->label_time->setVisible(true);
        ui->lineEdit_time->setVisible(true);
        ui->label_unit->setVisible(true);
        ui->label_beat->setVisible(false);
        ui->spinBox_beat->setVisible(false);
    }
}

void AddBeatDlg::on_lineEdit_time_editingFinished()
{
    int value = ui->lineEdit_time->text().toInt();
    int zhengshu = value / _interval;
    int yushu    = value % _interval;
    if(yushu != 0)
    {
        value = (zhengshu + 1) * _interval;
        zhengshu ++;
    }
    ui->lineEdit_time->setText(QString::number(value));
    ui->spinBox_beat->setValue(zhengshu);

    if(_realLastBeat != -1)
        ui->lineEdit_timeBetween->setText(QString::number((zhengshu-_realLastBeat)*_interval));
    else
        ui->lineEdit_timeBetween->setText(QString::number(zhengshu*_interval));
}

void AddBeatDlg::slot_textChanged()
{
    QString textContent = ui->plainTextEdit->toPlainText();
    int length = textContent.size();
    if(length > 50)
    {
        QTextCursor cursor = ui->plainTextEdit->textCursor();
        cursor.movePosition((QTextCursor::End));
        if(cursor.hasSelection())
        {
            cursor.clearSelection();
        }

        cursor.deletePreviousChar();
        ui->plainTextEdit->setTextCursor(cursor);
    }
}

void AddBeatDlg::on_lineEdit_timeBetween_editingFinished()
{
    int value = ui->lineEdit_timeBetween->text().toInt();
    int standardValue;
    if(_realLastBeat != -1)
        standardValue = (_lastBeat+1-_realLastBeat)*_interval;
    else
        standardValue = (_lastBeat+1)*_interval;
//nisz add 2022-6-21
    if(!_isModify)
    {
        if(value==0)
        {
            if(_lastBeat==-1)
            {
                ui->lineEdit_timeBetween->setText(QString::number(0));
                ui->spinBox_beat->setValue(0);
                return;
            }
            else {
                ui->lineEdit_timeBetween->setText(QString::number((ui->spinBox_beat->value()-_lastBeat)*_interval));
                return;
            }
        }
    }
    else {
        if(value==0)
        {
            if(_realLastBeat==-1)
            {
                ui->lineEdit_timeBetween->setText(QString::number(0));
                ui->spinBox_beat->setValue(0);
                return;
            }
            else {
                ui->lineEdit_timeBetween->setText(QString::number(standardValue));
                return;
            }
        }
    }
//nisz add end
    int zhengshu = value / _interval;
    int yushu    = value % _interval;
    if(yushu != 0)
    {
        value = (zhengshu + 1) * _interval;
        zhengshu ++;
    }
    //nisz add 2022-6-17
    ui->lineEdit_timeBetween->setText(QString::number(value));
    if(_isModify)
    {
        if(_realLastBeat != -1)
            ui->spinBox_beat->setValue(zhengshu + _realLastBeat);
        else
            ui->spinBox_beat->setValue(zhengshu);
    }
    else
    {
        if(_lastBeat !=-1)
            ui->spinBox_beat->setValue((zhengshu + _lastBeat));
        else
            ui->spinBox_beat->setValue(zhengshu);
    }
    //nisz add end
}

void AddBeatDlg::on_spinBox_beat_valueChanged(const QString &arg1)
{
    ui->lineEdit_time->setText(QString::number((arg1.toInt())*_interval));
}
//nisz add 2022-6-21

void AddBeatDlg::on_spinBox_beat_editingFinished()
{
    int value=ui->spinBox_beat->text().toInt();
    if(_isModify)
    {
        if(_realLastBeat==-1)
        {
            if(value==0)
                ui->lineEdit_timeBetween->setText(QString::number(0));
            else
                ui->lineEdit_timeBetween->setText(QString::number(value* _interval));
        }
        else
            ui->lineEdit_timeBetween->setText(QString::number((value-_realLastBeat)* _interval));
    }

    else
    {
        if(_lastBeat==-1)
            if(value==0)
                ui->lineEdit_timeBetween->setText(QString::number(0));
            else
                ui->lineEdit_timeBetween->setText(QString::number((value)* _interval));
        else {
            ui->lineEdit_timeBetween->setText(QString::number((value-_lastBeat)* _interval));

        }


    }
}
//nisz add end
