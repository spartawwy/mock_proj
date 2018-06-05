#ifndef BARS_GRAPH_SDF23KDF_H_
#define BARS_GRAPH_SDF23KDF_H_

#include <QtWidgets/QWidget>

class BarsGraph : public QWidget
{
    Q_OBJECT

public:

    BarsGraph(QWidget *parent = 0);
    ~BarsGraph(){}

private:

    virtual void paintEvent(QPaintEvent *) override;

    int head_h_;
    int bottom_h_;
    int left_w_;
    int right_w_;
}; 
#endif // BARS_GRAPH_SDF23KDF_H_