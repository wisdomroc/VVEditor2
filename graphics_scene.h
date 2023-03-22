/***********************************************************************
* @filename graphics_scene.h
* @brief    配合graphics_view实现右侧视口的所有功能
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <QGraphicsScene>

class CommandItem;
class ICDData;
struct VVFile;
struct VVBeat;
struct VVCommand;

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsScene();

    inline int getXMinWidth() const { return _xMinWidth; }
    inline int getYMinHeight() const { return _yMinHeight; }
    void setXMinWidth(int width)
    {
        _xMinWidth = width;
        setSceneRect(QRect(0, 0, _xMinWidth, _yMinHeight));
    }
    void setYMinHeight(int height)
    {
        _yMinHeight = height;
        setSceneRect(QRect(0, 0, _xMinWidth, _yMinHeight));
    }

    void highLight(int beatIndex);
    void setICDData(QSharedPointer<ICDData> icd_data);  //! 设置ICD相关信息
    void initFromFile(VVFile *vvFile);                  //! 从文件进行item的恢复

    void modifyBeat(int oldBeat, int newBeat);          //! 修改节拍
    void addBeat();                                     //! 添加节拍
    void insertBeat(int pos);                                  //! 插入节拍
    void addCommand(int beatIndex, VVCommand *command); //! 添加命令（在第几个节拍上）
    void adjustSceneRect(int width = 0, int height = 0);//! 自动调整场景大小

    bool            deleteBeat(int beatIndex);                      //删除节拍
    QMap<int, int>  deleteCurrentCommandsPre();                     //返回即将删除的command的数量
    bool            deleteCurrentCommands();                        //删除当前命令
    bool            deleteOneCommand(int row, VVCommand *vvCommand);//删除某个命令


    void modifyCurrentCommand();//修改当前命令



private:
    bool modifyCommand(int row, VVCommand *vvCommand);

private:
    int _xMinWidth;
    int _yMinHeight;
    QList<QList<CommandItem*>>  _items;             //! 同下结构，只是里面存的是场景中的item
    QList<QList<VVCommand*>>    _beatCommandsList;  //! ├──第一个节拍的：命令1，命令2，命令3
                                                    //! ├──第二个节拍的：命令1，命令2，命令3
                                                    //! ├──第三个节拍的：命令1，命令2，命令3

    QList<int> _beatList;                       //! 每个节拍的index的list
    VVFile *_vvFile;
    QSharedPointer<ICDData> _icd_data;
    QGraphicsLineItem *_highLightItem;
};

#endif // GRAPHICS_SCENE_H
