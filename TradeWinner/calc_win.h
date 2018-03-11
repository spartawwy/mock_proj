#ifndef CALC_WIN_H_DS3SDFS
#define CALC_WIN_H_DS3SDFS


#include <QWidget>
#include <QString>
//#include <QTimer>

#include "ui_calc.h"

class CalcWin : public QWidget
{
    Q_OBJECT

public:

    CalcWin();

    void ShowUI(const QString &title_str, const QString &str);
    //void ShowUI(const std::string &title_str, const std::string &str);

    //void SetTitle(const QString &str);
    //void SetContent(const QString &str);

private slots:

    void DoCalc(bool);
    //void SlotTimeout();

private:

    Ui::CalcForm  ui;

    //QTimer *myTimer_;
};


#endif //CALC_WIN_H_DS3SDFS 