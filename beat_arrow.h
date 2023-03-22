/***********************************************************************
* @filename beat_arrow.h
* @brief    自定义的一个节拍轴 / 时间轴
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef BEAT_ARROW_H
#define BEAT_ARROW_H

#include <QMenu>
#include <QWidget>
struct VVFile;

class BeatArrow : public QWidget
{
    Q_OBJECT
public:
    explicit BeatArrow(QWidget *parent = nullptr);
    ~BeatArrow();

    void setRange(double yOffset);              //设置上方偏移，然后初始化数据，然后进行paintEvent
    void setBasedType(int basedType);           //设置基于那种方式：0-基于节拍；1-基于时间
    void setCycleStr(const QString& cycleStr);  //设置调度周期
    void initFromFile(VVFile *vvFile);          //初始化
    bool addBeat(int beat_number, int type, const QString &des);    //添加一个节拍
    void insertBeat(int pos, int beat_number);
    bool deleteCurrentBeat();                   //删除一个节拍
    bool modifyBeat(int old_beat_number, int new_beat_number);

    int  getActiveIndex();                      //获得当前实际节拍是第几个index
    int  getActiveBeatNumber();                       //获取当前实际的节拍名称（也就是第X拍）
    int  getLastBeat();                         //获取最后一个节拍的值




    inline int getFirstIndex(){ return _beatOffset; }   //获取刻度尺最上面的节拍的index
    inline int getBeatCount(){ return _beatCount; }     //获取节拍总数

    static void ConvertTimeStr2ValueAndUnit(const QString &timeStr, int &value, QString &unit);
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void drawArrow(QPainter &painter, QPoint startPos, QPoint endPos); //画箭头
    void drawScale(QPainter &painter);
    void drawScaleText(QPainter &painter);
    void drawScaleButton(QPainter &painter);

    void refresh();
    bool needAdjust();  //坐标轴起始点需要上移

private:
    QPoint                  _startPos;
    QPoint                  _endPos;

    double                  _pixelOffset;       //最上面的偏移量 （对应graphicsView而言）

    int                     _beatOffset;        //刻度尺最上面的节拍的index
    int                     _activeIndex;       //当前节拍，是哪个节拍，即所有节拍里面是第几个
    int                     _floatingIndex;     //当前floating节拍

    int                     _beatCount;         //节拍数组大小
    QStringList             _beatList;          //节拍list，保存的是0,1,2,4,7等数字
    QList<QRect>            _addBeatRectList;   //节拍上添加 命令按钮 区域list
    QList<QRect>            _beatNumberRectList;//节拍文字所在的区域list

    int _centerCrossInch;    //按钮中央的十字星
    int _buttonWidth;        //按钮的宽度 & 高度
    int _basedType;          //基于什么类型：0-基于节拍；1-基于时间
    QString _cycleStr;       //节拍间隔（调度周期）

    QMenu    _menu;              //菜单
    QAction* _actionAddBeatBefore;   //在之前添加
    QAction* _actionAddBeatAfter;    //在之后添加

private slots:
    void slot_AddBeatBefore();
    void slot_AddBeatAfter();

signals:
    void addBeatBefore(int, int);       //在当前节拍 之前添加节拍
    void addBeatAfter(int);             //在当前节拍 之后添加节拍

    void AddOneCommand(int beatNumber); //点击添加命令
    void activeClicked(int);
};

#endif // BEAT_ARROW_H
