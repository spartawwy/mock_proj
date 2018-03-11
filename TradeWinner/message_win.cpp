#include "message_win.h"

#include <cassert>

#include <Windows.h>

#include <QTimer>
#include <qdebug.h>

MessageWin::MessageWin(int time_out_ms)
    : QWidget()
    , time_out_ms_(time_out_ms)
{
    assert(time_out_ms >= 0);
    ui.setupUi(this);
    
    myTimer_ = new QTimer();
    //myTimer_->start(time_out_ms);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    this->setFixedSize(this->width(), this->height());
    bool ret = connect(myTimer_,SIGNAL(timeout()),this,SLOT(SlotTimeout()));
 
}

void MessageWin::SetTitle(const QString &str)
{
    setWindowIconText(str);
}

void MessageWin::SetContent(const QString &str)
{
    ui.label_content->setText(str);
}

void MessageWin::ShowUI(const QString &title_str, const QString &str)
{
    setWindowIconText(title_str);
    ui.label_content->setText(str);
    ::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    //::SetWindowPos(HWND(pMainForm->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); 
    this->show();
     
    myTimer_->start(time_out_ms_);
}

void MessageWin::ShowUI(const std::string &title_str, const std::string &str)
{
    ShowUI(QString::fromLocal8Bit(title_str.c_str()), QString::fromLocal8Bit(str.c_str()));
}

void MessageWin::SlotTimeout()
{
    qDebug() << "SlotTimeout " << this->winId() << "\n";
    
    if( this->isVisible() )
    {
        ::SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        this->hide();
    }
    myTimer_->stop();
}
