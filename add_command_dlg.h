/***********************************************************************
* @filename add_command_dlg.h
* @brief    本类是添加命令对话框类
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef ADD_COMMAND_DLG_H
#define ADD_COMMAND_DLG_H

#include <QDialog>
#include "select_signal_dlg.h"

namespace Ui {
class AddCommandDlg;
}

class ICDData;
class QStandardItem;
class QStandardItemModel;

class AddCommandDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddCommandDlg(QSharedPointer<ICDData> icd_data, VVFile* vvFile, VVCommand *vvCommand = nullptr, QWidget *parent = nullptr);
    ~AddCommandDlg();

    VVCommand *getInfo();
    void setVVCommand(VVCommand *vvCommand);
    void initUiVisible();
    void setICDData(QSharedPointer<ICDData> icd_data);  //! 设置ICD相关信息

private slots:
    void on_pushButton_selectTarget_clicked();
    void on_comboBox_commandType_currentIndexChanged(int index);
    void on_pushButton_conditionConfig_clicked();

    void slot_architecture_itemChanged(QStandardItem *item);
private:
    void initData();//初始化数据
    void initView();//初始化界面
    void initViewData();//初始化界面数据
    void cloneModel(QStandardItemModel *model, QStandardItemModel *newModel);
    void cloneItem(QStandardItem *item, QStandardItem *&newItem);
    void addMsgInfoToView(QStandardItem *item);
    void updateParentCheckState(QStandardItem *parent);
    void updateChildCheckState(QStandardItem *item);
    void updateTargetInfoToView(VVCommand *command_);
    void updateTargetInfoToItem(QStandardItem *item, VVCommand *command_);
    QString getItemPath(QStandardItem *item);
    int getCommandType(const QString &commandType);
    void getTargetInfoFromView(VVCommand *vvCommand);
    void getTargetInfoFromItem(QStandardItem *item, VVCommand *vvCommand);

public:
    bool    setFormulaExpression(const QString &expression);
    QString getFormulaExpression();

private:
    void initUi();          //
    void initSineWave();    //正弦波
    void initPhaseStep();   //阶跃
    void initSlope();       //斜坡
    void initSquareWave();  //方波
    void initFx();          //一元一次方程
    void initWhiteNoise();  //白噪声
    void initRandomNum();   //随机数

    bool setFormulaExpressionSineWave(const QString &expression);   //正弦波
    bool setFormulaExpressionPhaseStep(const QString &expression);  //阶跃
    bool setFormulaExpressionSlope(const QString &expression);      //斜坡
    bool setFormulaExpressionSquareWave(const QString &expression); //方波
    bool setFormulaExpressionFx(const QString &expression);         //一元一次方程
    bool setFormulaExpressionWhiteNoise(const QString &expression); //白噪声
    bool setFormulaExpressionRandomNum(const QString &expression);  //随机数

    QString getFormulaExpressionSineWave();     //正弦波
    QString getFormulaExpressionPhaseStep();    //阶跃
    QString getFormulaExpressionSlope();        //斜坡
    QString getFormulaExpressionSquareWave();   //方波
    QString getFormulaExpressionFx();           //一元一次方程
    QString getFormulaExpressionWhiteNoise();   //白噪声
    QString getFormulaExpressionRandomNum();    //随机数

    QStringList getAllFormulaStr();

private slots:
    void on_comboBox_type_currentIndexChanged(int index);

    void on_pushButton_chooseDest_clicked();

    void on_spinBox_continuousBeat_valueChanged(int arg1);

    void on_spinBox_startOffset_valueChanged(int arg1);

    void on_spinBox_endOffset_valueChanged(int arg1);

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

    void on_comboBox_operator_2_currentIndexChanged(int index);

    void on_radioButton_const_toggled(bool checked);

    void on_radioButton_variable_toggled(bool checked);

    void on_pushButton_chooseVariable_clicked();

private:
    Ui::AddCommandDlg *ui;
    QList<ICDInfo> _infoList;
    VVFile *_vvFile;
    QStandardItemModel *architectureModel_;
    QSharedPointer<ICDData> _icd_data;

    QString _curFormula;
    VVCommand* _vvCommand;
};

#endif // ADD_COMMAND_DLG_H
