#include "bars_graph.h"

#include <QPainter>
#include <qevent.h> 


BarsGraph::BarsGraph(QWidget *parent)
    : head_h_(10)
    , bottom_h_(10)
    , left_w_(10)
    , right_w_(10)
{

}

void BarsGraph::paintEvent(QPaintEvent *) 
{
    QPainter painter(this);

    const int mm_h = this->height() - head_h_ - bottom_h_;
    const int mm_w = this->width();

    QPen pen; 
    pen.setColor(Qt::white);
    painter.setPen(pen);
    auto old_font = painter.font();

    QFont font;  
    font.setPointSize(old_font.pointSize() * 2); 

    painter.setFont(font);
    //painter.drawText(mm_w - right_w - 70, -1 *(this->height() - 100), cur_stock_code_.c_str());
    //painter.setFont(old_font); 
    pen.setColor(Qt::red);
    painter.setPen(pen);
    QBrush brush(Qt::red);
    painter.setBrush(brush);
    painter.drawRect(10, 10, 100, 100);
}