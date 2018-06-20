#ifndef DEMO_H
#define DEMO_H

#include <memory>

#include <QTextCodec>

#include <QtWidgets/QDialog>
#include "ui_demo.h"

class QTimerContainner;
class demo : public QDialog
{
    Q_OBJECT

public:
    demo(QWidget *parent = 0);
    ~demo();

public slots:

    void  DoMyTimer()
    {

    }

    void DoTest();

private:
    Ui::demoClass ui;
     
    std::shared_ptr<QTimerContainner>  one_shot_timers_;
    
    std::shared_ptr<QTimerContainner>  serial_shot_timers_; 
};

void utf8ToGbk(std::string& strUtf8);

void gbkToUtf8(std::string& strGbk);

#endif // DEMO_H
