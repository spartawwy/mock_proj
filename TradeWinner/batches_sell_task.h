#ifndef BATCHES_SELL_TASK_SDF5FDGFGH_H_
#define BATCHES_SELL_TASK_SDF5FDGFGH_H_

#include "strategy_task.h"


class BatchesSellTask : public StrategyTask
{
public:
     
    BatchesSellTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~BatchesSellTask(){}
    virtual void HandleQuoteData() override;

    __int64 time_point_open_warning_; // if val is 0, means not in warning
    //unsigned int continue_seconds_;  // minite keep low price seconds
    //unsigned int quantity_;

    struct T_StepItem
    {
        bool has_selled;
        double up_price;
        double bottom_price;
        T_StepItem() : has_selled(false), up_price(0.0), bottom_price(0.0){}
    };
    std::vector<T_StepItem> step_items_;

private:

    bool is_waitting_removed_;
    bool is_wait_trade_result_;
};

#endif