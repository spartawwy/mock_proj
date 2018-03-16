 
#include "winner_win.h"

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"
#include "equal_section_task.h"

#include "winner_hq_api.h"

typedef std::tuple<std::shared_ptr<StrategyTask>, std::shared_ptr<T_TaskInformation> > T_Task_Inf_Pair;

static void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para);

static HMODULE st_api_handle = nullptr;
static WinnerHisHq_ConnectDelegate WinnerHisHq_Connect = nullptr;
static WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect = nullptr;
static WinnerHisHq_GetHisFenbiDataDelegate WinnerHisHq_GetHisFenbiData  = nullptr;

bool WinnerWin::InitBacktestWin()
{
    bool ret = connect(ui.pbtn_start_backtest, SIGNAL(clicked(bool)), this, SLOT(DoStartBacktest(bool)));

    /*HMODULE*/ st_api_handle = LoadLibrary("winner_api.dll");
    if( !st_api_handle )
    {
        std::cout << "LoadLibrary winner_api.dll fail" << std::endl;
        return false;
    }
    //void *p_tchk = GetProcAddress(st_api_handle, "WinnerHisHq_Connect");
    WinnerHisHq_Connect = (WinnerHisHq_ConnectDelegate)GetProcAddress(st_api_handle, "WinnerHisHq_Connect"); 
    if ( !WinnerHisHq_Connect )
    {
        std::cout << " GetProcAddress WinnerHisHq_Connect fail " << std::endl;
        return false;
    }

    WinnerHisHq_DisConnect =  (WinnerHisHq_DisconnectDelegate)GetProcAddress(st_api_handle, "WinnerHisHq_Disconnect"); 

    WinnerHisHq_GetHisFenbiData 
        = (WinnerHisHq_GetHisFenbiDataDelegate)GetProcAddress(st_api_handle, "WinnerHisHq_GetHisFenbiData"); 
    if( !WinnerHisHq_GetHisFenbiData )
    {
        std::cout << " GetProcAddress WinnerHisHq_GetHisFenbiData fail " << std::endl;
        return false;
    }
    char result[1024] = {0};
    char error[1024] = {0};
#if 1
    int ret_val = WinnerHisHq_Connect("192.168.1.5", 50010, result, error);
#else
    int ret_val = WinnerHisHq_Connect("128.1.1.3", 50010, result, error);
#endif 
    return ret_val == 0;

}

void WinnerWin::UnInstallBacktest()
{
    if( WinnerHisHq_DisConnect )
        WinnerHisHq_DisConnect();
    if( !st_api_handle )
    {
        FreeLibrary(st_api_handle);
    }
}

void WinnerWin::DoStartBacktest(bool)
{
    //BreakUpBuyTask>(*iter->second, app)
#if 0
    static std::vector<T_Task_Inf_Pair> task_taskinfo_vector;
#else
    static std::vector<std::shared_ptr<StrategyTask> > task_vector;
    static std::vector<std::shared_ptr<T_TaskInformation> > taskinfo_vector;
#endif 
    if( !WinnerHisHq_GetHisFenbiData )
    {
       app_->winner_win().DoStatusBar("回测接口未安装!");
       return;
    }
     
#if 0
    task_taskinfo_vector.clear();
#else
    task_vector.clear();
    taskinfo_vector.clear();
#endif 
    auto task_info = std::make_shared<T_TaskInformation>();

    int date = 20180315;
    task_info->id = 123;
    task_info->type = TypeTask::EQUAL_SECTION;
    task_info->stock = "000063";
    task_info->back_alert_trigger = false;
    //task_in->o.rebounce = 0.3;
    task_info->continue_second = 0;
    task_info->quantity = 400;
    task_info->alert_price = 12.2;
    task_info->assistant_field = "";

    task_info->secton_task.fall_percent = 0.7;
    task_info->secton_task.raise_percent = 0.7;
    task_info->secton_task.fall_infection = 0.2;
    task_info->secton_task.raise_percent = 0.2;
    task_info->secton_task.max_position = 1000;
    task_info->secton_task.max_trig_price = 35.0;
    task_info->secton_task.min_trig_price = 26.0;
      
    taskinfo_vector.push_back( std::move(task_info));

    auto equal_sec_task = std::make_shared<EqualSectionTask>(*taskinfo_vector[0], app_);
#if 0
    task_taskinfo_vector.push_back( std::make_tuple(std::move(equal_sec_task), std::move(task_info)) );
#else
    task_vector.push_back( std::move(equal_sec_task) );
#endif

    char error[1024] = {0};
    WinnerHisHq_GetHisFenbiData(const_cast<char*>(taskinfo_vector[0]->stock.c_str())
        , date
        , FenbiCallBackFunc
        , std::addressof(task_vector[0])
        , error);
}

void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para)
{
    static unsigned int num = 0;
    qDebug() << ++num << " " << quote_data->price << "\n"; 
    auto quotes_data = std::make_shared<QuotesData>();
    quotes_data->cur_price = quote_data->price;
    std::shared_ptr<StrategyTask>& strategy_task = *(std::shared_ptr<StrategyTask>*)para;
    strategy_task->ObtainData(quotes_data);

}