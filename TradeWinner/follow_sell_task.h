#ifndef FOLLOW_SELL_TASK_H_SDF23SDS
#define FOLLOW_SELL_TASK_H_SDF23SDS
 

#include "strategy_task.h"
 
class FollowSellTask : public StrategyTask
{
public: 

    FollowSellTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~FollowSellTask(){}
    virtual void HandleQuoteData() override;

    double  window_bottom_price_;
    double  window_up_price_;

    __int64 time_point_dropdown_bottom_; // if val is 0, means not in warning

    bool is_out_warn_;
   
};
#endif