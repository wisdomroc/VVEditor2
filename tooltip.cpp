#include "tooltip.h"
#include <QLinearGradient>
#include <QPainter>
#include <QTimer>

ToolTip::ToolTip(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(180,70);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    tip_label = new QLabel(this);
    tip_label->move(1,16);
    tip_label->setFixedSize(this->width() - 2, this->height() - 17);
    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, [this](){ this->deleteLater(); });
    timer->start();
}

//设置提示内容
void ToolTip::setTipText(const QString &txt)
{
    tip_label->setText(txt);
}

//重绘窗口
void ToolTip::paintEvent(QPaintEvent *)
{
    QPainter painter2(this);
    painter2.setPen(Qt::red);   //窗口框色

    {//设置色调，渐变，可有可无
        QLinearGradient linear2(rect().topLeft(), rect().bottomLeft());
        linear2.setColorAt(0, QColor(247, 247, 250));
        linear2.setColorAt(0.5, QColor(240, 242, 247));
        linear2.setColorAt(1, QColor(233, 233, 242));
        painter2.setBrush(linear2);
    }

    //自绘窗口
    static const QPointF points[8] = {QPointF(15, 0), QPointF(15, 15), QPointF(0, 15), QPointF(0, this->height()-1), QPointF(this->width()-1, this->height()-1), QPointF(this->width()-1, 15),   QPointF(30, 15), QPointF(15, 0)};
    painter2.drawPolygon(points, 8);
}
