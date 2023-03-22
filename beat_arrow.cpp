#include "beat_arrow.h"
#include "vv_typedef.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <qmath.h>

const int LeftSpace     = 25;   //左侧空余
const int TopSpace      = 0;    //上面空余
const int TextYOffset   = -4;   //刻度数字的偏移
const int BottomSpace   = 50;   //下方空余
const int ArrowOffset   = 30;   //箭头的偏移
const int UnitScale     = 50;   //刻度最小单位

const int ButtonLeftOffset      = 30;
const int ButtonWidth           = 16;
const int ButtonCenterCrossInch = 6;

const QColor ColorAxis  = QColor("black");
const QColor ColorBg    = QColor(0xFF, 0xFF, 0xFF);

BeatArrow::BeatArrow(QWidget *parent) : QWidget(parent),
    _beatOffset(0),
    _beatCount(0),
    _pixelOffset(0),
    _activeIndex(-1),
    _floatingIndex(-1),
    _centerCrossInch(6),//按钮中央的十字星
    _buttonWidth(16),   //按钮的宽度 & 高度
    _basedType(0)
{

    _actionAddBeatBefore = _menu.addAction(QIcon(":/img/addBefore.png"), QStringLiteral("在之前插入节拍"));
    _actionAddBeatAfter  = _menu.addAction(QIcon(":/img/addAfter.png"),  QStringLiteral("在之后插入节拍"));
    connect(_actionAddBeatBefore, &QAction::triggered, this, &BeatArrow::slot_AddBeatBefore);
    connect(_actionAddBeatAfter,  &QAction::triggered, this, &BeatArrow::slot_AddBeatAfter);

    setMouseTracking(true);
}

//TODO.滚动后选中的节拍会改变，按道理不应该改变

BeatArrow::~BeatArrow()
{

}

void BeatArrow::setRange(double yOffset)
{
    _floatingIndex = -1;
    float beatOffset = yOffset / UnitScale;


    _beatOffset = qFloor(-beatOffset);          //轴体最上面的是第几个节拍
    _pixelOffset = (int)yOffset % UnitScale;   //轴体最上面节拍的偏移量

    // 获取起始坐标 & 结束坐标
    _startPos = rect().topLeft() + QPoint(LeftSpace, TopSpace);//轴的开始点
    _endPos = QPoint(_startPos.x(), (_beatList.size() - _beatOffset - 1) * UnitScale + ArrowOffset + _pixelOffset);//轴的结束点

}

void BeatArrow::setBasedType(int basedType)
{
    _basedType = basedType;
    update();
}

void BeatArrow::setCycleStr(const QString &cycleStr)
{
    _cycleStr = cycleStr;
}

void BeatArrow::initFromFile(VVFile *vvFile)
{
    _beatCount = vvFile->_beats_count;
    _beatList.clear();
    _activeIndex = -1;
    _floatingIndex = -1;
    for(auto beat: vvFile->_beats)
    {
        _beatList.append(QString::number(beat->_beat_number));
    }

    setRange(0);
    repaint();
}

int BeatArrow::getLastBeat()
{
    if(_beatList.size() == 0)
        return -1;
    return _beatList.last().toInt();
}

void BeatArrow::refresh()
{
    bool flag = needAdjust();
    if(flag)
    {
        _beatCount --;
        _beatOffset ++;
    }

    // 获取起始坐标 & 结束坐标
    _startPos = rect().topLeft() + QPoint(LeftSpace, TopSpace);
    _endPos = QPoint(_startPos.x(), (_beatList.size() - _beatOffset - 1) * UnitScale + ArrowOffset + _pixelOffset);
    repaint();
}

bool BeatArrow::addBeat(int beat_number, int type, const QString& des)
{
    Q_UNUSED(type)
    //先判断是否有该节拍
    if(_beatList.contains(QString::number(beat_number)))
        return false;

    _beatList.append(QString::number(beat_number));
    _beatCount ++;

    refresh();
    return true;
}

void BeatArrow::insertBeat(int pos, int beat_number)
{
    Q_ASSERT(pos >= 1);
    int addNumber = _beatList.at(pos).toInt() - _beatList.at(pos - 1).toInt();
    _beatList.insert(pos, QString::number(beat_number));
    _beatCount ++;
    for(int i = pos+1; i < _beatList.size(); ++i)
    {
        int oldBeatNumber = _beatList.at(i).toInt();
        _beatList.replace(i, QString::number(oldBeatNumber + addNumber));
    }

    refresh();
}

bool BeatArrow::deleteCurrentBeat()
{
    if(_activeIndex == -1)
        return false;

    _beatList.removeAt(_activeIndex);
    _beatCount = _beatList.size();
    _activeIndex = -1;
    update();
    return true;
}

bool BeatArrow::modifyBeat(int old_beat_number, int new_beat_number)
{
    int index = _beatList.indexOf(QString::number(old_beat_number));
    if(index == -1) return false;

    int min; int max;
    if(index == 0) min = 0;
    else min = _beatList.at(index - 1).toInt();

    if(index == _beatList.size() - 1) max = INT_MAX;
    else max = _beatList.at(index + 1).toInt();

    if(new_beat_number >= max || new_beat_number <= min) return false;

    _beatList.replace(index, QString::number(new_beat_number));
    return true;
}

int BeatArrow::getActiveIndex()
{
    return _activeIndex/* + _beatOffset*/;
}

int BeatArrow::getActiveBeatNumber()
{
    if(_activeIndex == -1) return -1;
    return _beatList.at(_activeIndex/* + _beatOffset*/).toInt();
}

void BeatArrow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QBrush(ColorAxis), 1));

    //draw backgroud
    painter.fillRect(rect(), QBrush(ColorBg));

    drawArrow(painter, _startPos, _endPos);
    drawScale(painter);
    drawScaleText(painter);
    drawScaleButton(painter);


}

void BeatArrow::mouseMoveEvent(QMouseEvent *event)
{
    for(int i = 0; i < _addBeatRectList.size(); ++i)
    {
        if(_addBeatRectList.at(i).contains(event->pos()))
        {
            _floatingIndex = i;
            repaint();
            return;
        }
    }
    for(int i = 0; i < _beatNumberRectList.size(); ++i)
    {
        if(_beatNumberRectList.at(i).contains(event->pos()))
        {
            _floatingIndex = i;
            repaint();
            return;
        }
    }
}

void BeatArrow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton) return;


    for(int i = 0; i < _addBeatRectList.size(); ++i)
    {
        if(_addBeatRectList.at(i).contains(event->pos()))
        {   // 添加命令
            emit AddOneCommand(_beatOffset + i);
            break;
        }
    }

    for(int i = 0; i < _beatNumberRectList.size(); ++i)
    {
        if(_beatNumberRectList.at(i).contains(event->pos()))
        {
            _activeIndex = i + _beatOffset;
            emit activeClicked(_activeIndex);
            repaint();
            break;
        }
    }
}

void BeatArrow::contextMenuEvent(QContextMenuEvent *event)
{
    bool upShow   = true;
    bool menuShow = false;
    QPoint point = event->pos();
    for(int i = 0; i < _beatNumberRectList.size(); ++i)
    {
        if(_beatNumberRectList.at(i).contains(point))
        {
            if((i == 0) &&(_beatList.at(i).toInt() == 0))
            {
                upShow = false;
            }

            menuShow = true;
            _activeIndex = i + _beatOffset;
            break;
        }
    }

    if(upShow)
    {
        _actionAddBeatBefore->setEnabled(true);
    }
    else
    {
        _actionAddBeatBefore->setEnabled(false);
    }

    if(menuShow) _menu.popup(event->globalPos());
}

//TODO.to be optimize
void BeatArrow::ConvertTimeStr2ValueAndUnit(const QString &timeStr, int &value, QString &unit)
{
    if(timeStr.endsWith("ms"))
    {
        value = timeStr.mid(0, timeStr.size()-2).toInt();
        unit = "ms";
    }
    else if(timeStr.endsWith("us"))
    {
        value = timeStr.mid(0, timeStr.size()-2).toInt();
        unit = "us";
    }
    else if(timeStr.endsWith("ns"))
    {
        value = timeStr.mid(0, timeStr.size()-2).toInt();
        unit = "ns";
    }
    else if(timeStr.endsWith("s"))
    {
        value = timeStr.mid(0, timeStr.size()-1).toInt();
        unit = "s";
    }
}

void BeatArrow::drawScale(QPainter &painter)
{
    for(int i = 0; i < _beatList.size() - _beatOffset; i ++)
    {
        QPoint scalePos = QPoint(rect().topLeft().x() + LeftSpace, i * UnitScale + TopSpace + _pixelOffset);
        QPoint scaleOffsetPos = QPoint(scalePos.x() - 5, scalePos.y());
        painter.drawLine(scaleOffsetPos, scalePos);
    }
}

void BeatArrow::drawScaleText(QPainter &painter)
{
    _beatNumberRectList.clear();
    painter.save();
    painter.setFont(QFont(QStringLiteral("微软雅黑"), 14));
    if(_basedType == 1) painter.setFont(QFont(QStringLiteral("微软雅黑"), 6));

    for(int i = 0; i < _beatList.size() - _beatOffset; i ++)
    {
        QPoint textPos = QPoint(rect().topLeft().x() + 2, i * UnitScale + TopSpace + TextYOffset + _pixelOffset);
        QString scaleVal = _beatList.at(i + _beatOffset);

        int cycle; QString unit;
        ConvertTimeStr2ValueAndUnit(_cycleStr, cycle, unit);
        if(_basedType == 1) scaleVal = QString::number(scaleVal.toInt() * cycle) + "\n" + unit;

        QRect textRect = QRect(textPos.x(), textPos.y(), 20, 20);
        _beatNumberRectList.append(textRect);
        // 画刻度
        painter.drawText(textRect, Qt::AlignCenter, scaleVal);

        // 画标记
        if(i == _floatingIndex)
        {
            painter.save();
            painter.setPen(QPen(QColor("lightBlue"), 2));
            painter.drawLine(textRect.bottomLeft() + QPoint(0, 6), textRect.bottomRight() + QPoint(0, 6)); // 6为数字下方横线与数字之间的距离
            painter.drawLine(textRect.bottomLeft() + QPoint(0, 6), textRect.bottomLeft() + QPoint(0, 0));
            painter.drawLine(textRect.bottomRight() + QPoint(0, 6), textRect.bottomRight() + QPoint(0, 0));
            painter.restore();
        }
        if(i == _activeIndex - _beatOffset)
        {
            painter.save();
            painter.setPen(QPen(QColor("blue"), 2));
            painter.drawLine(textRect.bottomLeft() + QPoint(0, 6), textRect.bottomRight() + QPoint(0, 6)); // 6为数字下方横线与数字之间的距离
            painter.drawLine(textRect.bottomLeft() + QPoint(0, 6), textRect.bottomLeft() + QPoint(0, 0));
            painter.drawLine(textRect.bottomRight() + QPoint(0, 6), textRect.bottomRight() + QPoint(0, 0));
            painter.restore();
        }
    }
    painter.restore();
}

void BeatArrow::drawScaleButton(QPainter &painter)
{


    _addBeatRectList.clear();
    for(int i = 0; i < _beatList.size() - _beatOffset; i ++)
    {
        QPoint startPos = QPoint(rect().topLeft().x() + ButtonLeftOffset, i * UnitScale + TopSpace + _pixelOffset);
        QRect rect = QRect(startPos.x(), startPos.y(), ButtonWidth, ButtonWidth);
        _addBeatRectList.append(rect);
        painter.save();
        if(i == _floatingIndex)
            painter.setPen(QPen(QBrush(QColor("gray")), 1));
        else
            painter.setPen(QPen(QBrush(QColor("lightGray")), 1));
        painter.drawRoundRect(rect);
        QPoint centerPos = QPoint(startPos.x() + ButtonWidth/2, startPos.y() + ButtonWidth/2);
        painter.drawLine(QPoint(centerPos.x() - ButtonCenterCrossInch, centerPos.y()), QPoint(centerPos.x() + ButtonCenterCrossInch, centerPos.y()));
        painter.drawLine(QPoint(centerPos.x(), centerPos.y() - ButtonCenterCrossInch), QPoint(centerPos.x(), centerPos.y() + ButtonCenterCrossInch));
        painter.restore();
    }
}

bool BeatArrow::needAdjust()
{
    // 获取起始坐标 & 结束坐标
    _startPos = rect().topLeft() + QPoint(LeftSpace, TopSpace);
    _endPos = QPoint(_startPos.x(), (_beatList.size() - _beatOffset - 1) * UnitScale + ArrowOffset + _pixelOffset);

    if(_endPos.y() > rect().height())
    {
        return true;
    }
    return false;
}

void BeatArrow::slot_AddBeatBefore()
{
    int lastBeatNumber = _beatList.at(_activeIndex - 1).toInt();
    int curBeatNumber  = _beatList.at(_activeIndex).toInt();
    emit addBeatBefore(lastBeatNumber, curBeatNumber);
}

void BeatArrow::slot_AddBeatAfter()
{
    int curBeatNumber = _beatList.at(_activeIndex).toInt();
    emit addBeatAfter(curBeatNumber);
}

void BeatArrow::drawArrow(QPainter &painter, QPoint startPos, QPoint endPos)
{
    painter.drawLine(startPos, endPos);
    endPos = QPoint(endPos.x(), endPos.y() + 2);//这里进行偏移2px，使箭头更完整，即轴的结束点

    QPainterPath arrowPath;
    arrowPath.moveTo(endPos);
    arrowPath.lineTo(QPoint(endPos.x()-ArrowOffset/4, endPos.y()-ArrowOffset/2));
    arrowPath.lineTo(QPoint(endPos.x()+ArrowOffset/4, endPos.y()-ArrowOffset/2));
    arrowPath.lineTo(endPos);
    painter.fillPath(arrowPath, QBrush(ColorAxis));
}
