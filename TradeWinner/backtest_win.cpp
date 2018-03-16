 
#include "winner_win.h"

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"

void WinnerWin::InitBacktestWin()
{
    bool ret = connect(ui.pbtn_start_backtest, SIGNAL(clicked(bool)), this, SLOT(DoStartBacktest(bool)));
}

void WinnerWin::DoStartBacktest(bool)
{

}