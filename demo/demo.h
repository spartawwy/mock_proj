#ifndef DEMO_H
#define DEMO_H

#include <QtWidgets/QDialog>
#include "ui_demo.h"

class demo : public QDialog
{
    Q_OBJECT

public:
    demo(QWidget *parent = 0);
    ~demo();

private:
    Ui::demoClass ui;
};

#endif // DEMO_H
