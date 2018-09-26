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
    void ClearTestItems(){ id_backtest_items_.clear(); cur_max_task_id_ = 0; }
    int AllocTaskId() { return ++ cur_max_task_id_; }
    void ResetItemResult(int task_id);
    void ResetAllitemResult();
    void StartTest(int start_date, int end_date);

    TTaskIdMapBackTestItem & id_backtest_items() { return id_backtest_items_; }

    void * p_fenbi_callback_obj() { return p_fenbi_callback_obj_; } //FuncFenbiCallback_;

    std::string detail_file_dir() { return detail_file_dir_; }
    void detail_file_dir(const std::string &val) { detail_file_dir_= val; }

private:
     
    WinnerApp * app_;

    void *  st_api_handle;
    void *  WinnerHisHq_Connect;
    void *  WinnerHisHq_DisConnect;
    void *  WinnerHisHq_GetHisFenbiData;
    void *  WinnerHisHq_GetHisFenbiDataBatch;
    void *  p_fenbi_callback_obj_; // ;

    TTaskIdMapBackTestItem  id_backtest_items_;
    std::atomic_int  cur_max_task_id_;
    /*TTaskIdMapStrategyTask  id_tasks_; 
    TTaskIdMapTaskInfo  id_task_infos_; 
    TTaskIdMapMockPara  id_mock_strategy_para_;*/
    //static std::vector<std::shared_ptr<T_FenbiCallBack> > callback_vector;
    std::string detail_file_dir_;
};

#endif // BACK_TESTER_SDF7ERETR_H_