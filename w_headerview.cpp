#include "w_headerview.h"
#include <QDebug>

WHeaderView::WHeaderView(Qt::Orientation ori, QWidget *parent)
    :QHeaderView(ori,parent)
{
    _pushButton = new QPushButton(this);
    _pushButton->setFixedSize(7, 14);
    _pushButton->setStyleSheet("border-image: url(:/img/expanded_true.png);");
    _pushButton->setFlat(true);
    _expanded = false;

    setStretchLastSection(true);
    setSectionResizeMode(QHeaderView::Interactive);
    setCascadingSectionResizes(true);
    connect(_pushButton, &QPushButton::clicked,this, &WHeaderView::slot_pushButtonClicked);
}

WHeaderView::~WHeaderView()
{

}

void WHeaderView::init()
{
    _expanded = false;
    _pushButton->setStyleSheet("border-image: url(:/img/expanded_true.png);");
}

void WHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter,rect,logicalIndex);

    //logicalIndex 当前第几列，也可以自定义显示其他控件；
    if(logicalIndex == 0)
    {
        QRect tmp;
        tmp.setSize(QSize(7, 14));
        tmp.moveCenter(rect.center());
        tmp.moveRight(rect.right() - 3);
        _pushButton->setGeometry(tmp);
    }
}

void WHeaderView::slot_pushButtonClicked()
{
    if(_expanded)
    {
        _expanded = false;
        _pushButton->setStyleSheet("border-image: url(:/img/expanded_true.png);");
        emit expanded(_expanded);
    }
    else
    {
        _expanded = true;
        _pushButton->setStyleSheet("border-image: url(:/img/expanded_false.png);");
        emit expanded(_expanded);
    }
}


