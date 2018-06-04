#ifndef BACK_TESTER_SDF7ERETR_H_
#define BACK_TESTER_SDF7ERETR_H_

#include <memory>
#include <unordered_map>

#include "strategy_task.h"
  

//// (taskid , mock strategepara)
//typedef std::unordered_map<int, std::shared_ptr<T_MockStrategyPara> >  TTaskIdMapMockPara;
//// (taskid , taskinfo)
//typedef std::unordered_map<int, std::shared_ptr<T_TaskInformation> >  TTaskIdMapTaskInfo;

typedef std::unordered_map<int, std::tuple<std::shared_ptr<StrategyTask>, std::shared_ptr<T_TaskInformation>, std::shared_ptr<T_MockStrategyPara> > > TTaskIdMapBackTestItem;
 
class BackTester
{
public:

    BackTester(WinnerApp *app);
    ~BackTester();

    bool Init();
    bool ConnectHisHqServer();
    void AddBackTestItem(std::shared_ptr<StrategyTask> &task, std::shared_ptr<T_TaskInformation> &task_info, std::shared_ptr<T_MockStrategyPara> &para);

    int AllocTaskId() { return ++ cur_max_task_id_; }

    void StartTest();

private:
     
    WinnerApp * app_;

    void *  st_api_handle;
    void *  WinnerHisHq_Connect;
    void *  WinnerHisHq_DisConnect;
    void *  WinnerHisHq_GetHisFenbiData;
    void *  WinnerHisHq_GetHisFenbiDataBatch;
    void *  FuncFenbiCallback_;

    TTaskIdMapBackTestItem  id_backtest_items_;
    std::atomic_int cur_max_task_id_;
    /*TTaskIdMapStrategyTask  id_tasks_; 
    TTaskIdMapTaskInfo  id_task_infos_; 
    TTaskIdMapMockPara  id_mock_strategy_para_;*/
    //static std::vector<std::shared_ptr<T_FenbiCallBack> > callback_vector;

};

#endif // BACK_TESTER_SDF7ERETR_H_