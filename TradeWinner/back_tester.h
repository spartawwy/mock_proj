#ifndef BACK_TESTER_SDF7ERETR_H_
#define BACK_TESTER_SDF7ERETR_H_

#include <memory>
#include <unordered_map>

#include "strategy_task.h"
 
typedef std::unordered_map<int, std::shared_ptr<T_MockStrategyPara> >  TTaskIdMapMockPara;

class BackTester
{
public:

    BackTester();
    ~BackTester(){}

private:
      
    //std::shared_ptr<T_TaskInformation>
    TTaskIdMapStrategyTask id_tasks_;
    //static std::vector<std::shared_ptr<T_FenbiCallBack> > callback_vector;
    TTaskIdMapMockPara  id_mock_strategy_para_;
};

#endif // BACK_TESTER_SDF7ERETR_H_