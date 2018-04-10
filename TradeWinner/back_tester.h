#ifndef BACK_TESTER_SDF7ERETR_H_
#define BACK_TESTER_SDF7ERETR_H_

#include <memory>
#include "strategy_task.h"

class StrategyTask;

class BackTester
{
public:

    BackTester();
    ~BackTester(){}

private:

    std::shared_ptr<StrategyTask> task_vector;
    std::shared_ptr<T_TaskInformation> taskinfo_vector;
    //static std::vector<std::shared_ptr<T_FenbiCallBack> > callback_vector;
    std::shared_ptr<T_MockStrategyPara>  mock_strategy_para_vector;
};

#endif // BACK_TESTER_SDF7ERETR_H_