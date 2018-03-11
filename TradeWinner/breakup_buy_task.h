#ifndef BREAKUP_BUY_TASK_DSFS9W34_H_
#define BREAKUP_BUY_TASK_DSFS9W34_H_

#include "strategy_task.h"

class BreakUpBuyTask: public StrategyTask
{
public:

    BreakUpBuyTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~BreakUpBuyTask(){}
    virtual void HandleQuoteData() override;
private:

	__int64 time_point_bt_open_warning_;
};
#endif