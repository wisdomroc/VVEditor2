/***********************************************************************
* @filename right_delegate.h
* @brief    本类为VVEditor主界面右侧的treeView（即节拍命令树）中除去最后一列删除列外的其他代理类
*           提供修改msgName / msgID，两者互相呼应，并且会自动更新sigName / sigID列
*           提供修改sigName / sigID,两者互相呼应
*           提供value列，根据不同值类型，弹出不同的编辑控件
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef RIGHT_DELEGATE_H
#define RIGHT_DELEGATE_H
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QPushButton>
#include <QComboBox>
#include <QCompleter>
#include <QSpinBox>
#include <QDebug>
#include <QLineEdit>
#include "icd_data.h"

class RightDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RightDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) { }

    void setItems(const QHash<QString, QString>& hash) {
        _hash = hash;
        _texts = hash.keys();
        qSort(_texts);
    }
    void setICDData(QSharedPointer<ICDData> icdData) { _icdData = icdData; }

public:


    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        Q_UNUSED(option)
        QString command_type = index.model()->data(index.sibling(index.row(), 2)).toString();

        QString text = index.model()->data(index, Qt::DisplayRole).toString();

        QComboBox   *editor     = new QComboBox(parent);
        editor->setEditable(true);
        QSpinBox*       editor_int    = new QSpinBox(parent);
        QDoubleSpinBox* editor_double = new QDoubleSpinBox(parent);
        QLineEdit   *editor_txt = new QLineEdit(parent);
        switch (index.column()) {
        case 3:                         //! 消息名称
        case 4:                         //! 消息ID
        {
            if(command_type == "PUSH_INTO_FIFO" || command_type == "CLEAR_FIFO")
                return nullptr;

            editor->addItems(_texts);
            QCompleter *completer = new QCompleter(_texts, editor);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            editor->setCompleter(completer);
        }
            return editor;
        case 5:                         //! 信号名称
        {
            if(command_type == "PUSH_INTO_FIFO" || command_type == "CLEAR_FIFO")
                return nullptr;

            QStringList list = _icdData->getSigNamesHash(index.sibling(index.row(), 3).data().toString()).keys(); //!TODO. 这里是根据的ICD_ID，但是会有问题，因为历史问题，这里暂使用ICD_NAME
            qSort(list);
            editor->addItems(list);

            QCompleter *completer = new QCompleter(list, editor);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            editor->setCompleter(completer);
        }
            return editor;
        case 6:                         //! 信号ID
        {
            if(command_type == "PUSH_INTO_FIFO" || command_type == "CLEAR_FIFO")
                return nullptr;

            QStringList list = _icdData->getSigIDsHash(index.sibling(index.row(), 3).data().toString()).keys(); //!TODO. 这里是根据的ICD_ID，但是会有问题，因为历史问题，这里暂使用ICD_NAME
            qSort(list);
            editor->addItems(list);

            QCompleter *completer = new QCompleter(list, editor);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            editor->setCompleter(completer);
        }
            return editor;
        case 9:                         //! 值

            if(index.sibling(index.row(), 2).data(0).toString() == "COMBINATION" ||
               index.sibling(index.row(), 2).data(0).toString() == "RESET" ||
               index.sibling(index.row(), 2).data(0).toString() == "SVPC_CHECK" ||
               index.sibling(index.row(), 2).data(0).toString() == "PUSH_INTO_FIFO" ||
               index.sibling(index.row(), 2).data(0).toString() == "CLEAR_FIFO")
                return nullptr;
        case 10:
            return nullptr;//nisz add 2022-6-17
            if(isNum(text.toStdString()))
            {
                if(text.contains("."))
                    return editor_double;
                else
                    return editor_int;
            }

            return editor_txt;

        }
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();

        QComboBox*      comboBox      = dynamic_cast<QComboBox*>(editor);
        QSpinBox*       spinBox       = dynamic_cast<QSpinBox*>(editor);
        QDoubleSpinBox* doubleSpinBox = dynamic_cast<QDoubleSpinBox*>(editor);
        QLineEdit*      lineEdit      = dynamic_cast<QLineEdit*>(editor);
        if(comboBox)
        {
            int tindex = comboBox->findText(text);
            comboBox->setCurrentIndex(tindex);
        }
        else
        {
            if(isNum(text.toStdString()))
            {
                if(spinBox)
                {
                    spinBox->setValue(text.toInt());
                }
                else
                {
                    doubleSpinBox->setValue(text.toDouble());
                }
            }
            else if(lineEdit)
                lineEdit->setText(text);
        }
    }

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        QString beforeEditValue = model->data(index).toString();
        QString currentValue = "";
        bool changeOtherColumnFlag = true;

        QComboBox*      comboBox      = dynamic_cast<QComboBox*>(editor);
        QSpinBox*       spinBox       = dynamic_cast<QSpinBox*>(editor);
        QDoubleSpinBox* doubleSpinBox = dynamic_cast<QDoubleSpinBox*>(editor);
        QLineEdit*      lineEdit      = dynamic_cast<QLineEdit*>(editor);
        if(comboBox)
        {
            QString lineEditStr = comboBox->lineEdit()->text();
            qDebug() << "lineEditStr: " << lineEditStr << endl;
            currentValue = comboBox->currentText();
            qDebug() << "comboBox: " << currentValue << endl;
            if(currentValue == beforeEditValue)
            {
                changeOtherColumnFlag = false;
            }
            if(!currentValue.isEmpty())
                model->setData(index, currentValue, Qt::EditRole);
        }
        else if(spinBox)
        {
            currentValue = QString::number(spinBox->value());
            model->setData(index, currentValue, Qt::EditRole);
        }
        else if(doubleSpinBox)
        {
            currentValue = QString::number(doubleSpinBox->value());
            model->setData(index, currentValue, Qt::EditRole);
        }
        else if(lineEdit)
        {
            currentValue = lineEdit->text();
            model->setData(index, currentValue, Qt::EditRole);
        }

        if(currentValue.isEmpty())
            return;

        //不同的列对应不同的操作
        if(changeOtherColumnFlag)
        {
            if(index.column() == 3)
            {
                // 有对应的msgID就直接设置上
                model->setData(index.sibling(index.row(), 4), _hash.value(currentValue), Qt::EditRole);
                QHash<QString, QString> sigNamesHash = _icdData->getSigNamesHash(_hash.value(currentValue));
                if(!sigNamesHash.contains(model->data(index.sibling(index.row(), 5)).toString()))
                {
                    model->setData(index.sibling(index.row(), 5), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 6), "", Qt::EditRole);
//nisz add 2022-6-15
                    model->setData(index.sibling(index.row(), 7), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 8), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 9), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 10), "", Qt::EditRole);
//add end
                }
            }
            else if(index.column() == 4)
            {   // 有对应的msgName就直接设置上
                model->setData(index.sibling(index.row(), 3), _hash.value(currentValue), Qt::EditRole);
                QHash<QString, QString> sigIDsHash = _icdData->getSigIDsHash(_hash.value(currentValue));
                if(!sigIDsHash.contains(model->data(index.sibling(index.row(), 6)).toString()))
                {
                    model->setData(index.sibling(index.row(), 5), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 6), "", Qt::EditRole);
//nisz add 2022-6-15
                    model->setData(index.sibling(index.row(), 7), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 8), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 9), "", Qt::EditRole);
                    model->setData(index.sibling(index.row(), 10), "", Qt::EditRole);
//add end
                }
            }
            else if(index.column() == 5)
            {   // 有对应的sigID就直接设置上
                QHash<QString, QString> hash = _icdData->getSigNamesHash(model->data(index.sibling(index.row(), 3)).toString());    //!TODO. 这里是根据的ICD_ID，但是会有问题，因为历史问题，这里暂使用ICD_NAME
                QStringList infos = hash.value(currentValue).split(",");
                model->setData(index.sibling(index.row(), 6), infos.at(0), Qt::EditRole);
                model->setData(index.sibling(index.row(), 7), infos.at(1), Qt::EditRole);
                model->setData(index.sibling(index.row(), 8), infos.at(2), Qt::EditRole);
            }
            else if(index.column() == 6)
            {   // 有对应的sigName就直接设置上
                QHash<QString, QString> hash = _icdData->getSigIDsHash(model->data(index.sibling(index.row(), 3)).toString());      //!TODO. 这里是根据的ICD_ID，但是会有问题，因为历史问题，这里暂使用ICD_NAME
                QStringList infos = hash.value(currentValue).split(",");
                model->setData(index.sibling(index.row(), 5), infos.at(0), Qt::EditRole);
                model->setData(index.sibling(index.row(), 7), infos.at(1), Qt::EditRole);
                model->setData(index.sibling(index.row(), 8), infos.at(2), Qt::EditRole);

            }
        }
    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override
    {
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }

private:
    bool isNum(std::string str) const
    {
        std::stringstream sin(str);
        double d;
        char c;
        if(!(sin >> d))
        {
            return false;
        }
        if (sin >> c)
        {
            return false;
        }
        return true;
    }

private:
    QHash<QString, QString> _hash;      //存的是-->msgID:msgName或者msgName:msgID
    QStringList             _texts;     //存的是msgIDs或者msgNames
    QString                 _msgID;     //当前已经选中的msgID
    QSharedPointer<ICDData> _icdData;   //ICD所有信息
};

#endif // RIGHT_DELEGATE_H
