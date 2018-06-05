#include "graphic_componet.h"

#include "bars_graph.h"

GraphicComponet::GraphicComponet(QWidget *parent)
    : QWidget(parent)
{
    //ui.setupUi(this);

    BarsGraph *bar_obj = new BarsGraph(this);
    bar_obj->show();
}

GraphicComponet::~GraphicComponet()
{

}
