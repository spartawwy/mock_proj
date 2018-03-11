#ifndef  TICKER_H_3SDSDFS4FS_
#define  TICKER_H_3SDSDFS4FS_

#include <mutex>
#include <memory>
#include <unordered_map>

//#include <TLib/core/tsystem_local_logger.h>

//#include "strategy_task.h"
//#include "handler.h"

#include <windows.h>
#include "fo_common.h"
//#include "stk_quoter_api.h"

//class StrategyTask; 

// <task id, task>
//typedef std::unordered_map<unsigned int, std::shared_ptr<StrategyTask> > TTaskIdMapStrategyTask;
// <stock code, task_id>
typedef std::unordered_map<std::string, std::list<unsigned int> > TCodeMapTasks;

static const unsigned int cst_result_len = 1024 * 1024;
static const unsigned int cst_error_len = 1024;
static const unsigned int cst_max_stock_code_count = 128;

typedef std::tuple<std::string, std::shared_ptr<QuotesData> > T_codeQuoteDateTuple;

class Ticker  
{
public:

    Ticker();
    virtual ~Ticker();

    void Procedure();

    bool Init();
    //void Register(const std::shared_ptr<StrategyTask> & task);
    //void UnRegister(unsigned int task_id);
    //void ClearAllTask();

    bool GetSecurityBars(int Category, int Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo);

    bool GetQuotes(char* stock_code, QuotesData& quote_data);

    /*bool DecodeStkQuoteResult(Buffer &Result, std::list<T_codeQuoteDateTuple> *ret_quotes_data=nullptr
            , std::function<void(const std::list<unsigned int>& id_list, std::shared_ptr<QuotesData> &data)> tell_all_rel_task=nullptr);

    void TellAllRelTasks(const std::list<unsigned int>& id_list, std::shared_ptr<QuotesData> &data);*/

protected:
      
    /*TTaskIdMapStrategyTask  registered_tasks_;
     
    TCodeMapTasks  codes_taskids_;*/

    std::mutex  tasks_list_mutex_;
    //TSystem::LocalLogger  &logger_;

};
 
#endif