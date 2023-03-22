#include "SelectVariableGroupWidget.h"
#include "ui_SelectVariableGroupWidget.h"

#include <QStandardItemModel>

#include "common_md.h"

#include "icdloadershuck.h"

SelectVariableGroupWidget::SelectVariableGroupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectVariableGroupWidget)
{
    ui->setupUi(this);

    initData();
    initView();
    initViewData();
}

SelectVariableGroupWidget::~SelectVariableGroupWidget()
{
    delete ui;
}

void SelectVariableGroupWidget::setVariableGroupModel(const QStandardItemModel *model)
{
    QStandardItemModel *variableGroupModel = const_cast<QStandardItemModel*>(model);
    ui->treeView_variableGroup->setModel(variableGroupModel);
    ui->treeView_variableGroup->expandAll();//展开
}

void SelectVariableGroupWidget::on_treeView_variableGroup_doubleClicked(const QModelIndex &index)
{
    //判断model有效性
    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(ui->treeView_variableGroup->model());
    if(!model)
        return;

    //判断TreeNode有效性
    QStandardItem *item = model->itemFromIndex(index);
    TreeNode *node = dynamic_cast<TreeNode *>(item);
    TreeNode *parentNode = dynamic_cast<TreeNode *>(node->parent());
    if(!node || !parentNode)
        return;

    //判断parentType类型
    ModuleType parentType = parentNode->moduleType;
    ModuleType type = node->moduleType;
    if(parentType != ModuleType::Block)
        return;

    VariableGroup variableGroup;
    variableGroup.blockUuid = parentNode->uuid;//blockUuid
    variableGroup.uuid = parentNode->uuid;//blockUuid
    variableGroup.name = node->text();//名称
    variableGroup.path = getItemPath(item);
    bool emitSignal = true;

    //处理不同的ModuleType
    switch (type) {
    case ModuleType::FMU:{       
        QStringList paramNameList;

        QStandardItemModel * inputParams = node->modelReference->inputParams;
        if(inputParams){
            for(int i=0;i<inputParams->rowCount();i++){
                QString paramName = inputParams->item(i)->text();
                paramNameList.append(paramName);
            }
        }

        QStandardItemModel * outputParams = node->modelReference->outputParams;
        if(outputParams){
            for(int i=0;i<outputParams->rowCount();i++){
                QString paramName = outputParams->item(i)->text();
                paramNameList.append(paramName);
            }
        }

        variableGroup.uuid = node->uuid;//fmuUuid
        variableGroup.type = "fmu";
        variableGroup.nameList = paramNameList;
        break;
    }
    case ModuleType::DATASOURCE:{
        QStringList icdNameList;
        QStandardItemModel& icd_input = parentNode->icdInput;
        for(int i=0;i<icd_input.rowCount();i++){
            QString icdUuid = icd_input.item(i)->text();
            QString icdName = QString::fromStdWString(WRPEngine::tj::ICDLoaderShuck::instance().getICDName(icdUuid.toStdWString()));
            icdNameList.append(icdName);
        }
        variableGroup.type = "dataSource";
        variableGroup.nameList = icdNameList;
        break;
    }
    case ModuleType::DATAOUTPUTER:{
        QStringList icdNameList;
        QStandardItemModel& icd_output = parentNode->icdOutput;
        for(int i=0;i<icd_output.rowCount();i++){
            QString icdUuid = icd_output.item(i)->text();
            QString icdName = QString::fromStdWString(WRPEngine::tj::ICDLoaderShuck::instance().getICDName(icdUuid.toStdWString()));
            icdNameList.append(icdName);
        }
        variableGroup.type = "dataOutPuter";
        variableGroup.nameList = icdNameList;
        break;
    }
    default:
        emitSignal = false;
        break;
    }

    if(emitSignal)
    {
        emit signal_selectedVariableGroup(variableGroup);
    }

}

void SelectVariableGroupWidget::on_treeView_variableGroup_clicked(const QModelIndex &index)
{
    on_treeView_variableGroup_doubleClicked(index);
}

void SelectVariableGroupWidget::initData()
{
}

void SelectVariableGroupWidget::initView()
{
    ui->treeView_variableGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView_variableGroup->setHeaderHidden(true);//隐藏列标题

}

void SelectVariableGroupWidget::initViewData()
{

}

QString SelectVariableGroupWidget::getItemPath(QStandardItem *item)
{
    if(!item)
        return QString();
    QString path = item->text();

    QStandardItem *parent = item->parent();
    while(parent)
    {
        path = parent->text() + "/" + path;
        parent = parent->parent();
    }
    return path;
}

