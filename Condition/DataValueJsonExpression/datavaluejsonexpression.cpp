#include "datavaluejsonexpression.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


DataValueJsonExpression::DataValueJsonExpression()
{

}

//将expression转为ConditionItemInfo
bool DataValueJsonExpression::toConditionStruct(const QString &expression, QList<DataValueConditionItemInfo>& condition_items)
{
    QString tempExpression = expression;
    tempExpression = tempExpression.remove("dataValue::");

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(tempExpression.toLocal8Bit(), &error); //传化为Json文档
    if (document.isNull() || (error.error != QJsonParseError::NoError))
    {
        return false;
    }

    if(document.isArray() == false) return false;
    QJsonArray expression_array = document.array();

    QList<DataValueConditionItemInfo> tempCondition;

    int expression_array_size = expression_array.size();
    for (int i = 0; i < expression_array_size; ++i) //整个表达式大数组
    {
        QJsonValue json_value = expression_array.at(i);
        if(i % 2 == 0) //i为偶数时
        {
            DataValueConditionItemInfo cond_item;
            if(json_value.isArray() == false) return false; //暂时不支持数组意外的数据
            QJsonArray condition_item_j = json_value.toArray();
            int condition_item_size = condition_item_j.size();
            for (int j = 0; j < condition_item_size; ++j) //每一个条件项
            {
                QJsonValue json_value = condition_item_j.at(j);
                DataValueSubConditionItem sub_condition_item;
                if(j%2==0)//j为偶数时
                {
                    if(json_value.isArray() == false) return false;
                    QJsonArray sub_condition_item_j = json_value.toArray(); //子条件
//                    if(sub_condition_item_j.size() != 3) return false;
                    DataValueSubCondition sub_cond;
                    if(sub_condition_item_j.size() ==3)
                    {
                        sub_cond.parameter = QString(QJsonDocument(sub_condition_item_j.at(0).toObject()).toJson(QJsonDocument::Compact));
                        sub_cond.comparison_operator = sub_condition_item_j.at(1).toString();
                        sub_cond.comparison_value = QString(QJsonDocument(sub_condition_item_j.at(2).toObject()).toJson(QJsonDocument::Compact));
                    }
                    else if(sub_condition_item_j.size() == 5)
                    {
                        sub_cond.parameter = QString(QJsonDocument(sub_condition_item_j.at(0).toObject()).toJson(QJsonDocument::Compact));
                        sub_cond.index1 = sub_condition_item_j.at(1).toString();
                        sub_cond.comparison_operator = sub_condition_item_j.at(2).toString();
                        sub_cond.comparison_value = QString(QJsonDocument(sub_condition_item_j.at(3).toObject()).toJson(QJsonDocument::Compact));
                        sub_cond.index2 = sub_condition_item_j.at(4).toString();
                    }
					//! not_zero
                    else if(sub_condition_item_j.size() == 7)
                    {
                        sub_cond.parameter = QString(QJsonDocument(sub_condition_item_j.at(0).toObject()).toJson(QJsonDocument::Compact));
                        sub_cond.index1 = sub_condition_item_j.at(1).toString();
                        sub_cond.blockUuid1 = sub_condition_item_j.at(2).toString();
                        sub_cond.comparison_operator = sub_condition_item_j.at(3).toString();

                        sub_cond.comparison_value = QString(QJsonDocument(sub_condition_item_j.at(4).toObject()).toJson(QJsonDocument::Compact));
                        sub_cond.index2 = sub_condition_item_j.at(5).toString();
                        sub_cond.blockUuid2 = sub_condition_item_j.at(6).toString();
                    }
					//! end
                    else
                    {
                        return false;
                    }

                    sub_condition_item.json_expression = sub_cond;
                    cond_item.sub_item.append(sub_condition_item);
                }
                else
                {
                    if(json_value.isString() == false) return false;
                    //sub_condition_item.logical_operator = json_value.toString();
                    cond_item.sub_item[cond_item.sub_item.size()-1].logical_operator = json_value.toString();
                }

            }
            tempCondition.append(cond_item);
        }
        else
        {
            if(json_value.isString() == false) return false;
            tempCondition[tempCondition.size()-1].logical_operator = json_value.toString();
        }

    }

    condition_items = tempCondition;
    return true;
}

QJsonObject StringToJson(const QString& str)
{
    QJsonObject l_ret;

    QJsonParseError l_err;
    QJsonDocument l_doc = QJsonDocument::fromJson(str.toUtf8(), &l_err);
    if (l_err.error == QJsonParseError::NoError)
    {
        if (l_doc.isObject())
        {
            l_ret = l_doc.object();
        }
    }
    return l_ret;
}

//将ConditionItemInfo转为expression
bool DataValueJsonExpression::toComditionExpression(const QList<DataValueConditionItemInfo> &condition_item, QString &expression)
{
    QJsonArray expression_array; //整个json数组表达式

    if(condition_item.isEmpty())
        return false;

    for (int i = 0; i < condition_item.size(); ++i)
    {
        QJsonArray cond_array;
        DataValueConditionItemInfo cond_item = condition_item.at(i);
        QList<DataValueSubConditionItem> sub_cond_items = cond_item.sub_item;

        QJsonArray sub_json_array;
        for (int j = 0; j < sub_cond_items.size(); ++j)
        {
            DataValueSubConditionItem sub_cond_item = sub_cond_items.at(j);
            DataValueSubCondition sub_cond = sub_cond_item.json_expression;
            QJsonArray json_array;


            json_array.append(QJsonValue(StringToJson(sub_cond.parameter)));
            json_array.append(QJsonValue(sub_cond.index1));
            json_array.append(QJsonValue(sub_cond.blockUuid1)); //! not_zero
            json_array.append(QJsonValue(sub_cond.comparison_operator));
            json_array.append(QJsonValue(StringToJson(sub_cond.comparison_value)));
            json_array.append(QJsonValue(sub_cond.index2));
            json_array.append(QJsonValue(sub_cond.blockUuid2)); //! not_zero


            sub_json_array.append(json_array);
            if(j != sub_cond_items.size()-1){
                sub_json_array.append(QJsonValue(sub_cond_item.logical_operator));
            }

        }
        expression_array.append(sub_json_array);
        if(i != condition_item.size()-1){
            expression_array.append(QJsonValue(cond_item.logical_operator));
        }

    }
    QJsonDocument doc;
    doc.setArray(expression_array);
    expression.clear();
    expression.append((doc.toJson(QJsonDocument::Compact)));

    expression = "dataValue::" + expression;

    return true;
}
