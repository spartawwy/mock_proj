#include "detail_win.h"

DetailWin::DetailWin()
{
    ui.setupUi(this);
}

void DetailWin::SetTitle(const QString &str)
{
    setWindowIconText(str);
}

void DetailWin::SetContent(const QString &str)
{
    ui.pte_bktest_detail->setPlainText(str);
}

void DetailWin::ShowUI(const QString &title_str, const QString &str)
{
    setWindowIconText(title_str);
    SetContent(str);
    ::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    //::SetWindowPos(HWND(pMainForm->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); 
    this->show(); 
}

void DetailWin::ShowUI(const std::string &title_str, const std::string &str)
{
    ShowUI(QString::fromLocal8Bit(title_str.c_str()), QString::fromLocal8Bit(str.c_str()));
}