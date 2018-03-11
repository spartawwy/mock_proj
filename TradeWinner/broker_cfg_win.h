#ifndef BROKER_CFG_23SDFS_H_
#define BROKER_CFG_23SDFS_H_

#include <QtWidgets/QDialog>

#include "ui_broker_cfg.h"

class WinnerApp;
class BrokerCfgWin : public QDialog
{
    Q_OBJECT

public:

    BrokerCfgWin(WinnerApp* app);
    bool Init();

 
public slots:
   void  DocurrentIndexChanged(int);
   void  DoOkBtnClicked();

private:

    Ui::broker_cfg_dlg  ui_;
    WinnerApp  *app_;

};
#endif