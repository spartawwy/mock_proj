#ifndef DETAIL_WIN_SDFSA4KD_H_
#define DETAIL_WIN_SDFSA4KD_H_


#include <QWidget>
#include <QString>

#include <Windows.h>
//#include <QTimer>

#include "ui_detail.h"

class DetailWin : public QWidget
{
    Q_OBJECT

public:

    DetailWin();

    void ShowUI(const QString &title_str, const QString &str);
    void ShowUI(const std::string &title_str, const std::string &str);

    void SetTitle(const QString &str);
    void SetContent(const QString &str);

private:

    Ui_DetailForm  ui;
      
};

#endif // DETAIL_WIN_SDFSA4KD_H_