#ifndef MESSAGE_WIN_WSDF32SDFD_H_
#define MESSAGE_WIN_WSDF32SDFD_H_

#include <QWidget>
#include <QString>
#include <QTimer>

#include "ui_msg.h"

class MessageWin : public QWidget
{
    Q_OBJECT

public:

    MessageWin(int time_out_seconds);

    void ShowUI(const QString &title_str, const QString &str);
    void ShowUI(const std::string &title_str, const std::string &str);

    void SetTitle(const QString &str);
    void SetContent(const QString &str);

private slots:

    void SlotTimeout();

private:

    Ui::MsgForm  ui;

    QTimer *myTimer_;

    int time_out_ms_;
};

#endif