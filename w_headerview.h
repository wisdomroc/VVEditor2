/***********************************************************************
* @filename w_headerview.h
* @brief    在treeView的header上添加 展开 / 收起 按钮，O(∩_∩)O~
* @author   wanghp
* @date     2022-04-27
************************************************************************/

#ifndef W_HEADERVIEW_H
#define W_HEADERVIEW_H

#include <QHeaderView>
#include <QPushButton>

class WHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    WHeaderView(Qt::Orientation ori = Qt::Horizontal, QWidget*parent = nullptr);
    ~WHeaderView();
    void init();

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

signals:
    void expanded(bool expanded);

public slots:
    void slot_pushButtonClicked();

private:
    bool         _expanded;
    QPushButton* _pushButton;
};

#endif // W_HEADERVIEW_H
