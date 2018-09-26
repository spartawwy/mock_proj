#include "back_tester.h"

#include <qt_windows.h>
#include <qdebug.h>

#include <io.h>

#include <algorithm>

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_hq_api.h"
#include "winner_app.h"

static const std::string cst_back_test_tag = "bktest";

/*
WinnerHisHq_ConnectDelegate WinnerHisHq_Connect;
WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect;
WinnerHisHq_GetHisFenbiDataDelegate WinnerHisHq_GetHisFenbiData;
WinnerHisHq_GetHisFenbiDataBatchDelegate WinnerHisHq_GetHisFenbiDataBatch;
*/

void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para)
{
    static auto show_result = [](std::shared_ptr<StrategyTask>& strategy_task, int date, double price, bool is_end)
    {
        double ori_assets = strategy_task->GetOriMockAssets();
        double assets = strategy_task->GetMockAssets(price);

        
        if( is_end )
        {
            auto profit = (assets - ori_assets) / ori_assets * 100;
            auto p_str = new std::string(TSystem::utility::FormatStr("back_test %s original assets:%.2f | %d ret assets:%.2f\n | PROFIT PERCENT:%.2f", strategy_task->stock_code(), ori_assets, date, assets, profit));

            strategy_task->app()->local_logger().LogLocal(cst_back_test_tag, *p_str);
            strategy_task->app()->AppendLog2Ui(p_str->c_str());
            strategy_task->app()->EmitSigShowUi(p_str);
        }else
        {
            auto p_str = new std::string(TSystem::utility::FormatStr("back_test %s original assets:%.2f | %d ret assets:%.2f", strategy_task->stock_code(), ori_assets, date, assets));

            strategy_task->app()->local_logger().LogLocal(cst_back_test_tag, *p_str);
            strategy_task->app()->AppendLog2Ui(p_str->c_str());
            delete p_str; p_str = nullptr;
        }
        //qDebug() << "FenbiCallBackFunc assets: " << assets << "\n";
    }; 

    if( !para )
        return;

    BackTester &back_tester = *(BackTester*)(((T_FenbiCallBack*)para)->para);

    auto data = std::make_shared<QuotesData>(); 
    data->time_stamp= quote_data->time;
    data->cur_price = quote_data->price;
    //qDebug() << p_callback_obj->serial++ 
     
    struct tm * timeinfo = localtime(&quote_data->time);
    int long_date = (timeinfo->tm_year + 1900) * 10000 + (timeinfo->tm_mon + 1) * 100 + timeinfo->tm_mday;
     
    std::for_each(back_tester.id_backtest_items().begin(), back_tester.id_backtest_items().end(), [&data, &back_tester, is_end, long_date](TTaskIdMapBackTestItem::reference entry)
    {
        auto task_id = entry.first;
        std::shared_ptr<StrategyTask> strategy_task = std::get<0>(entry.second);
        //std::shared_ptr<T_TaskInformation> task_info = std::get<1>(entry.second);
        //std::shared_ptr<T_MockStrategyPara> mock_para = std::get<2>(entry.second);

        T_FenbiCallBack *p_callback_obj = ((T_FenbiCallBack*)(back_tester.p_fenbi_callback_obj()));
        p_callback_obj->serial++ ;

        if( strategy_task->has_bktest_result_fetched() )
            return;
        if( long_date != p_callback_obj->date )
        { 
            p_callback_obj->date = long_date; 
            strategy_task->do_mock_date_change(long_date);
        }
        strategy_task->ObtainData(data);

        if( strategy_task->is_waitting_removed() )
        {
            strategy_task->has_bktest_result_fetched(true);
            show_result(strategy_task, p_callback_obj->date, data->cur_price, is_end);  

        }else if( is_end ) 
        {
            show_result(strategy_task, p_callback_obj->date, data->cur_price, is_end);  
            strategy_task->app()->Emit_SigEnableBtnBackTest();
        }
    });
     
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
    , detail_file_dir_()
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
    detail_file_dir_ = TSystem::AppDir(*app_) + "BackTestDetail";
    if( _access(detail_file_dir_.c_str(), 0) != 0 )
    {
        TSystem::utility::CreateDir(detail_file_dir_);
    }

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

void BackTester::ResetItemResult(int task_id)
{
    auto item = id_backtest_items_.find(task_id);
    if( item == id_backtest_items_.end() )
        return;
    auto tuple_item = item->second;
    std::shared_ptr<T_MockStrategyPara>  &mock_para = std::get<2>(tuple_item);
    mock_para->avaliable_position = 0;
    mock_para->frozon_position = 0;
    mock_para->capital = mock_para->ori_capital;
}

void BackTester::ResetAllitemResult()
{
    std::for_each( std::begin(id_backtest_items_), std::end(id_backtest_items_), [this](TTaskIdMapBackTestItem::reference entry)
    {
        this->ResetItemResult(entry.first);
    });
}

void BackTester::StartTest(int start_date, int end_date)
{
    if( !ConnectHisHqServer() )
        return;
    assert(p_fenbi_callback_obj_);
    char error[1024] = {0};

    auto iter = id_backtest_items_.begin();
    if( iter == id_backtest_items_.end() )
        return;
    ResetItemResult(iter->first);
    auto strategy_task = std::get<0>(iter->second);
    strategy_task->ResetBktestResult();
    
    auto get_his_fenbi_data_batch = ((WinnerHisHq_GetHisFenbiDataBatchDelegate)WinnerHisHq_GetHisFenbiDataBatch);

    get_his_fenbi_data_batch( const_cast<char*>(strategy_task->stock_code()), start_date, end_date, (T_FenbiCallBack*)p_fenbi_callback_obj_, error);


}
