/***********************************************************************
* @filename tooltip.h
* @brief    提供自定义的浮动提示框
* @author   wanghp
* @date     2022-04-26
************************************************************************/

#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QWidget>
#include <QLabel>

class ToolTip : public QWidget
{
    Q_OBJECT
public:
    explicit ToolTip(QWidget *parent = nullptr);

    void setTipText(const QString& txt);    //设置提示内容
signals:

public slots:

private:
    QLabel *tip_label;
    QTimer *timer;

protected:
    void paintEvent(QPaintEvent *);
};

#endif // TOOLTIP_H
