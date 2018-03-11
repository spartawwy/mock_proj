#ifndef INFLECTION_BUY_TASK_DSFS9DFSC4_H_
#define INFLECTION_BUY_TASK_DSFS9DFSC4_H_

#include "strategy_task.h"

class InflectionBuyTask: public StrategyTask
{
public:

    InflectionBuyTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~InflectionBuyTask(){}
    virtual void HandleQuoteData() override;

    __int64 time_point_open_warning_; // if val is 0, means not in warning
    __int64 time_point_bottom_price_;  
	__int64 time_point_half_ready_;
    double bottom_price_;
};
#endif