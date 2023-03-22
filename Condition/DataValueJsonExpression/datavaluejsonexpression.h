#ifndef DATAVALUEJSONEXPRESSION_H
#define DATAVALUEJSONEXPRESSION_H

/***********************************************************************
* @filename datavaluejsonexpression.h
* @brief    这是一个工具类，用于转换：ConditionItemInfo & expression
* @author   wanghp
* @date     2022-04-06
************************************************************************/

#include <QString>
#include <QList>


// [[[{ICD:msg1/sig1},<,{Value:0}],&&,[{ICD:msg1/sig1},<,{Value:0}]]|| ......]

struct DataValueSubCondition
{
    QString parameter; //fmu的端口或是icd的信号 （FMU:uuid/port 、 ICD:msgid/signal）
    QString index1;  //! 等号左边的index
    QString blockUuid1;
//    QString parameter_key; //是变量parameter中的键
//    QString parameter_value; //是变量parameter中的值
    QString comparison_operator ; //比较运算符
    QString comparison_value; //比较的值（可能为其他端口或信号  FMU:uuid/port 、 ICD:msgid/signal）
    QString index2; //! 等号右边的index
    QString blockUuid2;
};

struct DataValueSubConditionItem
{
    DataValueSubCondition json_expression;
    QString logical_operator; // 逻辑运算符，只包含：||、&&
};

struct DataValueConditionItemInfo
{
//    QString protocol_type; // fmu、icd
    QList<DataValueSubConditionItem> sub_item;
    QString logical_operator; // 逻辑运算符，只包含：||、&&
};


class  DataValueJsonExpression
{
public:
    DataValueJsonExpression();

    bool toConditionStruct(const QString &expression, QList<DataValueConditionItemInfo>& condition_item);

    bool toComditionExpression(const QList<DataValueConditionItemInfo> &condition_item, QString & expression);

};

#endif // DATAVALUEJSONEXPRESSION_H
