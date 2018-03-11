#ifndef INFLECTION_SELL_TASK_H_SDF3DSFS_
#define INFLECTION_SELL_TASK_H_SDF3DSFS_

#include "strategy_task.h"
 
class InflectionSellTask : public StrategyTask
{
public: 

    InflectionSellTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~InflectionSellTask(){}
    virtual void HandleQuoteData() override;

    __int64 time_point_open_warning_; // if val is 0, means not in warning
    __int64 time_point_top_price_;  
    __int64 time_point_half_ready_;
    double top_price_;
    //double cur_price_;
   
};

#endif