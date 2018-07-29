#ifndef RECORDS_WIN_H_DS456454FS
#define RECORDS_WIN_H_DS456454FS


#include <QWidget>
#include <QString>
//#include <QTimer>

#include "ui_records.h"

class WinnerApp;

class RecordsWin : public QWidget
{
    Q_OBJECT

public:

    RecordsWin(WinnerApp *app);

    void ShowUI(const QString &title_str, const QString &str);
    //void ShowUI(const std::string &title_str, const std::string &str);

    //void SetTitle(const QString &str);
    //void SetContent(const QString &str);

public slots:
	 
	void UpdateTblviewFills();
    //void SlotTimeout(); 

private:

    Ui::RecordsForm  ui;
	 
	WinnerApp *app_;
};


#endif // 