#ifndef GRAPHIC_COMPONET_H
#define GRAPHIC_COMPONET_H

#include <QtWidgets/QWidget>
#include "ui_graphic_componet.h"

class GraphicComponet : public QWidget
{
    Q_OBJECT

public:
    GraphicComponet(QWidget *parent = 0);
    ~GraphicComponet();

private:
    Ui::GraphicComponetClass ui;
};

#endif // GRAPHIC_COMPONET_H
