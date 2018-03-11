#ifndef MSG_WIN_WSDFDF3GFDGFD_H_
#define MSG_WIN_WSDFDF3GFDGFD_H_

#include <QWidget>
#include <QString>
#include <QTimer>

#include "ui_message.h"

class MsgWin : public QWidget
{
    Q_OBJECT

public:

    MsgWin(int time_out_seconds);

    void ShowUI(const QString &title_str, const QString &str);
    void ShowUI(const std::string &title_str, const std::string &str);

    void SetTitle(const QString &str);
    void SetContent(const QString &str);

private slots:

    void SlotTimeout();

private:

    Ui::messageForm  ui;

    QTimer *myTimer_;

    int time_out_ms_;
};

#endif