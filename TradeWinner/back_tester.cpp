#include "back_tester.h"

#include <qt_windows.h>

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_hq_api.h"
#include "winner_app.h"

/*
WinnerHisHq_ConnectDelegate WinnerHisHq_Connect;
WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect;
WinnerHisHq_GetHisFenbiDataDelegate WinnerHisHq_GetHisFenbiData;
WinnerHisHq_GetHisFenbiDataBatchDelegate WinnerHisHq_GetHisFenbiDataBatch;
*/

static void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para)
{

}

BackTester::BackTester(WinnerApp *app)
    : app_(app)
    , st_api_handle(nullptr)
    , WinnerHisHq_Connect(nullptr)
    , WinnerHisHq_DisConnect(nullptr)
    , WinnerHisHq_GetHisFenbiData(nullptr)
    , WinnerHisHq_GetHisFenbiDataBatch(nullptr)
    , p_fenbi_callback_obj_(nullptr)
    , id_backtest_items_(64)
{
    cur_max_task_id_ = 0;
}

BackTester::~BackTester()
{
    if( p_fenbi_callback_obj_ )
        delete p_fenbi_callback_obj_;
}

bool BackTester::Init()
{
    p_fenbi_callback_obj_ = new T_FenbiCallBack;
    ((T_FenbiCallBack*)p_fenbi_callback_obj_)->call_back_func = FenbiCallBackFunc;
    ((T_FenbiCallBack*)p_fenbi_callback_obj_)->para = this;

    /*HMODULE*/ st_api_handle = LoadLibrary("winner_api.dll");
    if( !st_api_handle )
    {
        //std::cout << "LoadLibrary winner_api.dll fail" << std::endl;
        return false;
    }
    //ui.pbtn_start_backtest->setDisabled(true);

    WinnerHisHq_Connect = (WinnerHisHq_ConnectDelegate)GetProcAddress((HMODULE)st_api_handle, "WinnerHisHq_Connect"); 
    if ( !WinnerHisHq_Connect )
    {
        //std::cout << " GetProcAddress WinnerHisHq_Connect fail " << std::endl;
        return false;
    }

    WinnerHisHq_DisConnect =  (WinnerHisHq_DisconnectDelegate)GetProcAddress((HMODULE)st_api_handle, "WinnerHisHq_Disconnect"); 

    WinnerHisHq_GetHisFenbiData 
        = (WinnerHisHq_GetHisFenbiDataDelegate)GetProcAddress((HMODULE)st_api_handle, "WinnerHisHq_GetHisFenbiData"); 
    if( !WinnerHisHq_GetHisFenbiData )
    {
        //std::cout << " GetProcAddress WinnerHisHq_GetHisFenbiData fail " << std::endl;
        return false;
    }
    WinnerHisHq_GetHisFenbiDataBatch 
        = (WinnerHisHq_GetHisFenbiDataBatchDelegate)GetProcAddress((HMODULE)st_api_handle, "WinnerHisHq_GetHisFenbiDataBatch"); 
    if( !WinnerHisHq_GetHisFenbiDataBatch )
    {
        //std::cout << " GetProcAddress WinnerHisHq_GetHisFenbiDataBatch fail " << std::endl;
        return false;
    }

    return true;
}

bool BackTester::ConnectHisHqServer()
{
    if( !WinnerHisHq_GetHisFenbiData )
    {
        app_->winner_win().DoStatusBar("回测接口未安装!");
        return false;
    }

    int ret_val = -1;
    //ret_val = WinnerHisHq_Connect("192.168.11.5", 50010, result, error);
    char result[1024] = {0};
    char error[1024] = {0};
    if( !stricmp(TSystem::utility::host().c_str(), "hzdev103") )
        ret_val = ((WinnerHisHq_ConnectDelegate)WinnerHisHq_Connect)("128.1.1.3", 50010, result, error);
    else
        ret_val = ((WinnerHisHq_ConnectDelegate)WinnerHisHq_Connect)("192.168.1.5", 50010, result, error);

    if( ret_val != 0 ) 
    {
        //ui.pbtn_start_backtest->setEnabled(true);
        app_->winner_win().DoStatusBar("服务器未连接!");
        return false;
    }
    return true;
}

void BackTester::AddBackTestItem(std::shared_ptr<StrategyTask> &task, std::shared_ptr<T_TaskInformation> &task_info, std::shared_ptr<T_MockStrategyPara> &para)
{
    assert( id_backtest_items_.find(task->task_id()) == id_backtest_items_.end() );

    id_backtest_items_.insert(std::make_pair(task->task_id(), std::make_tuple(task, task_info, para)));
}

void BackTester::StartTest(int start_date, int end_date)
{
    assert(p_fenbi_callback_obj_);
    char error[1024] = {0};

    auto iter = id_backtest_items_.begin();
    if( iter == id_backtest_items_.end() )
        return;

    auto strategy_task = std::get<0>(iter->second);
    auto get_his_fenbi_data_batch = ((WinnerHisHq_GetHisFenbiDataBatchDelegate)WinnerHisHq_GetHisFenbiDataBatch);

    get_his_fenbi_data_batch( const_cast<char*>(strategy_task->stock_code()), start_date, end_date, (T_FenbiCallBack*)p_fenbi_callback_obj_, error);
    //FuncFenbiCallback_
}
