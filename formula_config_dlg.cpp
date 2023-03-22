#include "formula_config_dlg.h"
#include "ui_formula_config_dlg.h"
#include "vv_typedef.h"
#include <QMessageBox>

FormulaConfigDlg::FormulaConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormulaConfigDlg)
{
    ui->setupUi(this);

    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowTitle(QStringLiteral("选择公式"));
    setFixedSize(380, 280);

    initUi();
    initSineWave();
    initPhaseStep();
    initSlope();
    initSquareWave();
    initFx();
    initWhiteNoise();
    initRandomNum();
}

FormulaConfigDlg::~FormulaConfigDlg()
{
    delete ui;
}

QStringList FormulaConfigDlg::getAllFormulaStr()
{
    QStringList strList;
    strList.append("SineWave");
    strList.append("StepOffset");
    strList.append("Ramp");
    strList.append("SquareWave");
    strList.append("Linear");
    strList.append("WhiteNoise");
    strList.append("RandomNumber");
    return strList;
}

bool FormulaConfigDlg::setFormulaExpression(const QString &expression)
{
    // 正弦波   阶跃        斜坡    方波     一元一次方程   白噪声    随机数
    // SineWave、StepOffset、Ramp、SquareWave、Linear、WhiteNoise、RandomNumber------------这里保留旧版本的标志

    if(expression.isEmpty()) return true;
    _curFormula = expression.split(";").first();

    int index = getAllFormulaStr().indexOf(_curFormula);
    ui->comboBox_type->setCurrentIndex(index);
    ui->stackedWidget->setCurrentIndex(index);

    if(_curFormula == "SineWave")
        return setFormulaExpressionSineWave(expression);
    else if(_curFormula == "StepOffset")
        return setFormulaExpressionPhaseStep(expression);
    else if(_curFormula == "Ramp")
        return setFormulaExpressionSlope(expression);
    else if(_curFormula == "SquareWave")
        return setFormulaExpressionSquareWave(expression);
    else if(_curFormula == "Linear")
        return setFormulaExpressionFx(expression);
    else if(_curFormula == "WhiteNoise")
        return setFormulaExpressionWhiteNoise(expression);
    else if(_curFormula == "RandomNumber")
        return setFormulaExpressionRandomNum(expression);
}

QString FormulaConfigDlg::getFormulaExpression()
{
    if(_curFormula == "SineWave")
        return getFormulaExpressionSineWave();
    else if(_curFormula == "StepOffset")
        return getFormulaExpressionPhaseStep();
    else if(_curFormula == "Ramp")
        return getFormulaExpressionSlope();
    else if(_curFormula == "SquareWave")
        return getFormulaExpressionSquareWave();
    else if(_curFormula == "Linear")
        return getFormulaExpressionFx();
    else if(_curFormula == "WhiteNoise")
        return getFormulaExpressionWhiteNoise();
    else if(_curFormula == "RandomNumber")
        return getFormulaExpressionRandomNum();
}

void FormulaConfigDlg::initUi()
{
    QStringList headerList;
    headerList << QStringLiteral("正弦波") << QStringLiteral("阶跃")
               << QStringLiteral("斜坡") << QStringLiteral("方波")
               << QStringLiteral("一元一次方程")<< QStringLiteral("白噪声")
               << QStringLiteral("随机数");
    ui->comboBox_type->addItems(headerList);
    _curFormula = "SineWave";
}

void FormulaConfigDlg::initSineWave()
{
    ui->doubleSpinBox_amplitude->setRange(0, MAX_DOUBLE);
    ui->doubleSpinBox_frequency->setRange(0, MAX_DOUBLE);
    ui->spinBox_durationSineWave->setRange(1, MAX_BEAT);
    ui->radioButton_keepSineWave->setChecked(true);
}

void FormulaConfigDlg::initPhaseStep()
{
    ui->doubleSpinBox_stepValue->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->spinBox_durationPhaseStep->setRange(1, MAX_BEAT);
    ui->radioButton_keepPhaseStep->setChecked(true);
}

void FormulaConfigDlg::initSlope()
{
    ui->doubleSpinBox_initalValue->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->doubleSpinBox_targetValue->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->spinBox_durationSlope->setRange(1, MAX_BEAT);
    ui->radioButton_keepSlope->setChecked(true);
}

void FormulaConfigDlg::initSquareWave()
{
    ui->doubleSpinBox_positiveValue->setRange(0, MAX_DOUBLE);
    ui->spinBox_positiveDuration->setRange(0, MAX_BEAT);
    ui->doubleSpinBox_negativeValue->setRange(0, MAX_DOUBLE);
    ui->spinBox_negativeDuration->setRange(0, MAX_BEAT);
    ui->spinBox_durationSquareWave->setRange(1, MAX_BEAT);
    ui->radioButton_keepSquareWave->setChecked(true);
}

void FormulaConfigDlg::initFx()
{
    ui->doubleSpinBox_slope->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->doubleSpinBox_intercept->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->spinBox_durationFx->setRange(1, MAX_BEAT);
    ui->radioButton_keepFx->setChecked(true);
}

void FormulaConfigDlg::initWhiteNoise()
{
    ui->doubleSpinBox_meanValue->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->doubleSpinBox_standardDeviation->setRange(0, MAX_DOUBLE);
    ui->spinBox_durationWhiteNoise->setRange(1, MAX_BEAT);
    ui->radioButton_keepWhiteNoise->setChecked(true);
}

void FormulaConfigDlg::initRandomNum()
{
    ui->doubleSpinBox_max->setRange(-MAX_DOUBLE, MAX_DOUBLE);
    ui->doubleSpinBox_min->setRange(-MAX_DOUBLE, MAX_DOUBLE);

    connect(ui->doubleSpinBox_max, &QDoubleSpinBox::editingFinished, this, [this](){
        double maxValue = ui->doubleSpinBox_max->text().toDouble();
        double minValue = ui->doubleSpinBox_min->text().toDouble();
        if(maxValue < minValue)
        {
            ui->doubleSpinBox_max->setValue(ui->doubleSpinBox_min->text().toDouble());
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("最大值不能小于最小值！"), QStringLiteral("确定"));
            return;
        }
    });
    connect(ui->doubleSpinBox_min, &QDoubleSpinBox::editingFinished, this, [this](){
        double maxValue = ui->doubleSpinBox_max->text().toDouble();
        double minValue = ui->doubleSpinBox_min->text().toDouble();
        if(maxValue < minValue)
        {
            ui->doubleSpinBox_min->setValue(ui->doubleSpinBox_max->text().toDouble());
            QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("最小值不能大于最大值！"), QStringLiteral("确定"));
            return;
        }
    });

    ui->spinBox_durationRandomNum->setRange(1,MAX_BEAT);//whp change param1 from -1 to 0
    ui->radioButton_keepRandomNum->setChecked(true);
}

bool FormulaConfigDlg::setFormulaExpressionSineWave(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("SineWave") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_amplitude->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_frequency->setValue(argList.at(2).toDouble());
            ui->spinBox_durationSineWave->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepSineWave->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepSineWave->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool FormulaConfigDlg::setFormulaExpressionPhaseStep(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("StepOffset") == 0)
    {
        if(argList.size() == 4)
        {
            ui->doubleSpinBox_stepValue->setValue(argList.at(1).toDouble());
            ui->spinBox_durationPhaseStep->setValue(argList.at(2).toInt());

            if(argList.at(3).toInt())
            {
                ui->radioButton_keepPhaseStep->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepPhaseStep->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool FormulaConfigDlg::setFormulaExpressionSlope(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("Ramp") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_initalValue->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_targetValue->setValue(argList.at(2).toDouble());
            ui->spinBox_durationSlope->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepSlope->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepSlope->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool FormulaConfigDlg::setFormulaExpressionSquareWave(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("SquareWave") == 0)
    {
        if(argList.size() == 7)
        {
            ui->doubleSpinBox_positiveValue->setValue(argList.at(1).toDouble());
            ui->spinBox_positiveDuration->setValue(argList.at(2).toInt());
            ui->doubleSpinBox_negativeValue->setValue(qAbs(argList.at(3).toDouble()));
            ui->spinBox_negativeDuration->setValue(argList.at(4).toInt());
            ui->spinBox_durationSquareWave->setValue(argList.at(5).toInt());

            if(argList.at(6).toInt())
            {
                ui->radioButton_keepSquareWave->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepSquareWave->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool FormulaConfigDlg::setFormulaExpressionFx(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("Linear") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_slope->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_intercept->setValue(argList.at(2).toDouble());
            ui->spinBox_durationFx->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepFx->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepFx->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool FormulaConfigDlg::setFormulaExpressionWhiteNoise(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("WhiteNoise") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_meanValue->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_standardDeviation->setValue(argList.at(2).toDouble());
            ui->spinBox_durationWhiteNoise->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepWhiteNoise->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepWhiteNoise->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

bool FormulaConfigDlg::setFormulaExpressionRandomNum(const QString &expression)
{
    QList<QString> argList = expression.split(";");
    if(argList.isEmpty()) return false;

    if(argList.at(0).compare("RandomNumber") == 0)
    {
        if(argList.size() == 5)
        {
            ui->doubleSpinBox_max->setValue(argList.at(1).toDouble());
            ui->doubleSpinBox_min->setValue(argList.at(2).toDouble());
            ui->spinBox_durationRandomNum->setValue(argList.at(3).toInt());

            if(argList.at(4).toInt())
            {
                ui->radioButton_keepRandomNum->setChecked(true);
            }
            else
            {
                ui->radioButton_notKeepRandomNum->setChecked(true);
            }
            return true;
        }
    }
    return false;
}

QString FormulaConfigDlg::getFormulaExpressionSineWave()
{
    QString expression;
    QString amplitude = ui->doubleSpinBox_amplitude->text();
    QString frequency = ui->doubleSpinBox_frequency->text();
    QString duration = ui->spinBox_durationSineWave->text();
    QString keep = ui->radioButton_keepSineWave->isChecked() ? "1" :"0";

    expression = "SineWave;"
                + amplitude + ";"
                + frequency + ";"
                + duration  + ";"
                + keep;
    return expression;
}

QString FormulaConfigDlg::getFormulaExpressionPhaseStep()
{
    QString expression;
    QString stepValue = ui->doubleSpinBox_stepValue->text();
    QString duration = ui->spinBox_durationPhaseStep->text();
    QString keep = ui->radioButton_keepPhaseStep->isChecked() ? "1" :"0";

    expression = "StepOffset;"
                + stepValue + ";"
                + duration  + ";"
                + keep;
    return expression;
}

QString FormulaConfigDlg::getFormulaExpressionSlope()
{
    QString expression;
    QString initalValue = ui->doubleSpinBox_initalValue->text();
    QString targetValue = ui->doubleSpinBox_targetValue->text();
    QString duration = ui->spinBox_durationSlope->text();
    QString keep = ui->radioButton_keepSlope->isChecked() ? "1" :"0";

    expression = "Ramp;"
                + initalValue + ";"
                + targetValue + ";"
                + duration    + ";"
                + keep;
    return expression;
}

QString FormulaConfigDlg::getFormulaExpressionSquareWave()
{
    QString expression;
    QString positiveValue = ui->doubleSpinBox_positiveValue->text();
    QString positiveDuration = ui->spinBox_positiveDuration->text();
    QString negativeValue = "-" + ui->doubleSpinBox_negativeValue->text();
    QString negativeDuration = ui->spinBox_negativeDuration->text();
    QString duration = ui->spinBox_durationSquareWave->text();
    QString keep = ui->radioButton_keepSquareWave->isChecked() ? "1" :"0";

    expression = "SquareWave;"
                + positiveValue    + ";"
                + positiveDuration + ";"
                + negativeValue    + ";"
                + negativeDuration + ";"
                + duration         + ";"
                + keep;
    return expression;
}

QString FormulaConfigDlg::getFormulaExpressionFx()
{
    QString expression;
    QString slope = ui->doubleSpinBox_slope->text();
    QString intercept = ui->doubleSpinBox_intercept->text();
    QString duration = ui->spinBox_durationFx->text();
    QString keep = ui->radioButton_keepFx->isChecked() ? "1" :"0";

    expression = "Linear;"
                + slope     + ";"
                + intercept + ";"
                + duration  + ";"
                + keep;
    return expression;
}

QString FormulaConfigDlg::getFormulaExpressionWhiteNoise()
{
    QString expression;
    QString meanValue = ui->doubleSpinBox_meanValue->text();
    QString standardDeviation = ui->doubleSpinBox_standardDeviation->text();
    QString duration = ui->spinBox_durationWhiteNoise->text();
    QString keep = ui->radioButton_keepWhiteNoise->isChecked() ? "1" :"0";

    expression = "WhiteNoise;"
                + meanValue         + ";"
                + standardDeviation + ";"
                + duration          + ";"
                + keep;
    return expression;
}

QString FormulaConfigDlg::getFormulaExpressionRandomNum()
{
    QString expression;
    QString max = ui->doubleSpinBox_max->text();
    QString min = ui->doubleSpinBox_min->text();
    QString duration = ui->spinBox_durationRandomNum->text();
    QString keep = ui->radioButton_keepRandomNum->isChecked() ? "1" :"0";

    expression = "RandomNumber;"
                + max      + ";"
                + min      + ";"
                + duration + ";"
                + keep;
    return expression;
}

void FormulaConfigDlg::on_comboBox_type_currentIndexChanged(int index)
{
    // SineWave、StepOffset、Ramp、SquareWave、Linear、WhiteNoise、RandomNumber------------这里保留旧版本的标志
    if(index == -1) return;

    _curFormula = getAllFormulaStr().at(index);
    ui->stackedWidget->setCurrentIndex(index);
}
