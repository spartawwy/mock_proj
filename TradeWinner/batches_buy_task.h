#ifndef BATCHES_BUY_TASK_SDF5FDG45FGH_H_
#define BATCHES_BUY_TASK_SDF5FDG45FGH_H_

#include "strategy_task.h"


class BatchesBuyTask : public StrategyTask
{
public:
     
    BatchesBuyTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~BatchesBuyTask(){}
    virtual void HandleQuoteData() override;

    __int64 time_point_open_warning_; // if val is 0, means not in warning
    int times_has_buy_; 

    struct T_StepItem
    {
        bool has_buy;
        double up_price;
        double bottom_price;
        T_StepItem() : has_buy(false), up_price(0.0), bottom_price(0.0){}
    };
    std::vector<T_StepItem> step_items_;
};

#endif