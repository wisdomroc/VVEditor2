/***********************************************************************
* @filename formula_config_dlg.h
* @brief    提供当前7种公式的配置界面
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef FORMULA_CONFIG_DLG_H
#define FORMULA_CONFIG_DLG_H

#include <QDialog>

namespace Ui {
class FormulaConfigDlg;
}

class AbstractFormulaEditor;

class FormulaConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FormulaConfigDlg(QWidget *parent = nullptr);
    ~FormulaConfigDlg();
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

private:
    Ui::FormulaConfigDlg *ui;
    QString _curFormula;
};

#endif // FORMULA_CONFIG_DLG_H
