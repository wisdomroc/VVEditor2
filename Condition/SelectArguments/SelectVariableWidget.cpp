#include "SelectVariableWidget.h"
#include "ui_SelectVariableWidget.h"

#include "icdloadershuck.h"
#include "ICDController.h"

#include <QJsonObject>
#include <QRegularExpression>

SelectVariableWidget::SelectVariableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectVariableWidget)
{
    ui->setupUi(this);
}

SelectVariableWidget::~SelectVariableWidget()
{
    delete ui;
}

QString SelectVariableWidget::getSelectedVariable()
{
    if(!ui->listWidget_variable->currentItem())
        return QString();
    QString variableType;
    QString variableGroup;
    QString type = ui->comboBox_variableGroup->currentData(Qt::UserRole+3).toString();

    if(type.compare("fmu")==0){
        variableType = "FMU";
        variableGroup = ui->comboBox_variableGroup->currentData(Qt::UserRole+5).toString();

        QString uuid = ui->comboBox_variableGroup->currentData().toString();
        QRegularExpression regExp("[^/]+$");
        variableGroup.replace(regExp,ui->comboBox_variableGroup->currentData().toString());
    }else if(type.compare("dataSource")==0 || type.compare("dataOutPuter")==0){
        variableType = "ICD";
        variableGroup = ui->comboBox_variableGroup->currentData(Qt::UserRole+2).toString();
    }
    QString variableName = ui->listWidget_variable->currentItem()->text();

    return "{\""+variableType+"\":\""+variableGroup+"/"+variableName+"\"}";
}

void SelectVariableWidget::slot_selectedVariableGroup_changed(VariableGroup variableGroup)
{
    ui->comboBox_variableGroup->clear();
    ui->listWidget_variable->clear();

    if(variableGroup.type.compare("fmu")==0){
        //comboBox
        ui->comboBox_variableGroup->addItem(variableGroup.name,variableGroup.uuid);

        int index = ui->comboBox_variableGroup->count() - 1;
        ui->comboBox_variableGroup->setItemData(index,variableGroup.name, Qt::UserRole+1);
        ui->comboBox_variableGroup->setItemData(index,variableGroup.type, Qt::UserRole+3);
        ui->comboBox_variableGroup->setItemData(index,variableGroup.blockUuid, Qt::UserRole+4);
        ui->comboBox_variableGroup->setItemData(index,variableGroup.path, Qt::UserRole+5);

        qSort(variableGroup.nameList.begin(),variableGroup.nameList.end());

        //listWidget
        for(auto name:variableGroup.nameList){
            QListWidgetItem *item = new QListWidgetItem(name,ui->listWidget_variable);
            ui->listWidget_variable->addItem(item);
        }

    }else{
        for(auto name:variableGroup.nameList){
            QString text = variableGroup.name +"/"+ name;
            ui->comboBox_variableGroup->addItem(text,variableGroup.uuid);

            int index = ui->comboBox_variableGroup->count() - 1;
            ui->comboBox_variableGroup->setItemData(index,variableGroup.name, Qt::UserRole+1);
            ui->comboBox_variableGroup->setItemData(index,name, Qt::UserRole+2);
            ui->comboBox_variableGroup->setItemData(index,variableGroup.type, Qt::UserRole+3);
            ui->comboBox_variableGroup->setItemData(index,variableGroup.blockUuid, Qt::UserRole+4);
            ui->comboBox_variableGroup->setItemData(index,variableGroup.path, Qt::UserRole+5);
        }
    }
}

void SelectVariableWidget::on_comboBox_variableGroup_currentTextChanged(const QString &arg1)
{  
    ui->listWidget_variable->clear();
    QString type = ui->comboBox_variableGroup->currentData(Qt::UserRole+3).toString();

    if(type.compare("dataSource")==0 ||type.compare("dataOutPuter")==0)
    {
        QString name = ui->comboBox_variableGroup->currentData(Qt::UserRole+2).toString();//ÏûÏ¢Ãû³Æ

        PackageDef * packageDef = WRPEngine::tj::ICDLoaderShuck::instance().getICDController()->findICDDefinationByID(name);
        if(!packageDef)
            return;

        QList<QString> keyList = packageDef->signalCatalog.keys();
        qSort(keyList.begin(),keyList.end());

        for(auto signal:keyList){
            QString name = signal;
            QListWidgetItem *item = new QListWidgetItem(name,ui->listWidget_variable);
            ui->listWidget_variable->addItem(item);
        }
    }
}

void SelectVariableWidget::on_listWidget_variable_itemSelectionChanged()
{
    emit signal_selection_changed(!ui->listWidget_variable->selectedItems().isEmpty());
}

void SelectVariableWidget::on_listWidget_variable_doubleClicked(const QModelIndex &index)
{
    emit signal_selection_confirm();
}
