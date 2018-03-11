#ifndef BREAK_DOWN_TASK_SDFS3_H_
#define BREAK_DOWN_TASK_SDFS3_H_

#include "strategy_task.h"

class BreakDownTask : public StrategyTask
{
public:

    //BreakDownTask(unsigned int task_id, const std::string& code, TypeMarket  market_type, WinnerApp *app);
    BreakDownTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~BreakDownTask(){}
    virtual void HandleQuoteData() override;

    __int64 time_point_open_warning_; // if val is 0, means not in warning
    //unsigned int continue_seconds_;  // minite keep low price seconds
    //unsigned int quantity_;
};

#endif