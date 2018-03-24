#include "winner_win.h"

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"
#include "equal_section_task.h"
#include "advance_section_task.h"

#include "winner_hq_api.h"

typedef std::tuple<std::shared_ptr<StrategyTask>, std::shared_ptr<T_TaskInformation> > T_Task_Inf_Pair;

static void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para);

static HMODULE st_api_handle = nullptr;
static WinnerHisHq_ConnectDelegate WinnerHisHq_Connect = nullptr;
static WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect = nullptr;
static WinnerHisHq_GetHisFenbiDataDelegate WinnerHisHq_GetHisFenbiData  = nullptr;

static const std::string cst_back_test_tag = "bktest";

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
    char server_ip[] = "192.168.1.5";
    int port = 50010;

    app_->local_logger().LogLocal(utility::FormatStr("InitBacktestWin WinnerHisHq_Connect %s : %d waiting", server_ip, port));
#if 1
    int ret_val = WinnerHisHq_Connect("192.168.1.5", 50010, result, error);
#else
    int ret_val = WinnerHisHq_Connect("128.1.1.3", 50010, result, error);
#endif 
    app_->local_logger().LogLocal("InitBacktestWin WinnerHisHq_Connect ret");

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
    static std::vector<std::shared_ptr<T_FenbiCallBack> > callback_vector;
    static std::vector<std::shared_ptr<T_MockStrategyPara> > mock_strategy_para_vector;
#endif 
    if( !WinnerHisHq_GetHisFenbiData )
    {
        app_->winner_win().DoStatusBar("回测接口未安装!");
        return;
    }

    task_vector.clear();
    taskinfo_vector.clear();
    callback_vector.clear();
    mock_strategy_para_vector.clear();
 
    const double capital = 200000.00;
     
    auto task_info = std::make_shared<T_TaskInformation>();

#if 1 // test equal section task 
    task_info->id = 123;
    task_info->type = TypeTask::EQUAL_SECTION;
    //task_info->stock = "600123";
    task_info->stock = "000789";
    task_info->back_alert_trigger = false;
    //task_in->o.rebounce = 0.3;
    task_info->continue_second = 0;
    //task_info->quantity = 400;
    task_info->quantity = 5000;
    const double alert_price = 8.2;
    task_info->alert_price = alert_price;
    task_info->assistant_field = "";

    task_info->secton_task.fall_percent = 0.7;
    task_info->secton_task.raise_percent = 0.7;
    task_info->secton_task.fall_infection = 0.2;
    task_info->secton_task.raise_percent = 0.2;
    task_info->secton_task.max_position = 3000*10;
    task_info->secton_task.max_trig_price = 9.1;
    task_info->secton_task.min_trig_price = 7.95; 
    taskinfo_vector.push_back( std::move(task_info));

    auto mock_para = std::make_shared<T_MockStrategyPara>();
    mock_para->avaliable_position = (capital / alert_price ) / 2 / 100 * 100;
    mock_para->capital = capital - mock_para->avaliable_position * alert_price;
    mock_strategy_para_vector.push_back(std::move(mock_para));

    auto equal_sec_task = std::make_shared<EqualSectionTask>(*taskinfo_vector[0], app_, mock_strategy_para_vector[0].get()); 
    task_vector.push_back( std::move(equal_sec_task) );

#else
    task_info->id = 888;
    task_info->type = TypeTask::ADVANCE_SECTION;
    task_info->stock = "000789";
    task_info->rebounce = 1;   
    task_info->back_alert_trigger = false;
    task_info->continue_second = 0; 
    task_info->assistant_field = ""; 
    task_info->advance_section_task.is_original = true;
    
    double top_price = 9.0;
    double bottom_price = 8.0;
    double mid_price = (top_price + bottom_price) / 2;
    const int section_num = 5;
    task_info->quantity = int(capital / mid_price / section_num);
    assert(top_price > bottom_price && section_num > 1);

    int i = 0;
    for( ; i < section_num; ++i )
    {
        std::string temp_str = std::to_string(bottom_price + ((top_price - bottom_price) / section_num) * i);
        task_info->advance_section_task.portion_sections += temp_str + ";";

    }
    if( i == section_num )
        task_info->advance_section_task.portion_sections += std::to_string(bottom_price + ((top_price - bottom_price) / section_num) * i);

    task_info->advance_section_task.portion_states = "";
     
    taskinfo_vector.push_back( std::move(task_info));
    auto equal_sec_task = std::make_shared<AdvanceSectionTask>(*taskinfo_vector[0], app_, mock_strategy_para_vector[0].get()); 
    task_vector.push_back( std::move(equal_sec_task) );
#endif 
      
    auto fenbi_callback_obj = std::make_shared<T_FenbiCallBack>();
    fenbi_callback_obj->call_back_func = FenbiCallBackFunc;
    fenbi_callback_obj->para = std::addressof(task_vector[0]);
    callback_vector.push_back(std::move(fenbi_callback_obj));

    char error[1024] = {0};
    //int date[] = { 20180212, 20180213, 20180214, 20180215, 20180216, 20180222 };
    /*int date[] = { 20171020, 20171021, 20171022, 20171023, 20171024, 20171025, 20171026, 20171027, 20171028, 20171029, 20171030,20171031
    , 20171101, 20171102,  20171103,  20171104,  20171105,  20171106,  20171107,  20171108,  20171109, 20171110 
    , 20171111, 20171112,  20171113,  20171114,  20171115,  20171116,  20171117,  20171118,  20171119, 20171120 
    , 20171121, 20171122,  20171123,  20171124,  20171125,  20171126,  20171127,  20171128,  20171129, 20171130 
    };*/
    int date[] = { 20170914, 20170915, 20170916, 20170917, 20170918, 20170919, 20170920, 20170923, 20170924, 20170925
                  , 20170926, 20170927, 20170928, 20170929, 20171009, 20171010, 20171011,  20171012, 20171013, 20171014
                  , 20171015,  20171016,  20171017, 20171018, 20171019, 20171020, 20171021, 20171022, 20171023, 20171024
                  , 20171025, 20171026, 20171027, 20171028, 20171029, 20171030};
    for(int i = 0; i < sizeof(date)/sizeof(date[0]); ++i )
    {
        WinnerHisHq_GetHisFenbiData(const_cast<char*>(taskinfo_vector[0]->stock.c_str())
            , date[i]
        , callback_vector[0].get()
            , error);
    }

}

void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para)
{
    //static unsigned int num = 0; 
    static auto show_result = [](std::shared_ptr<StrategyTask>& strategy_task, int date, double price)
    {
        double ori_assets = strategy_task->GetOriMockAssets();
        double assets = strategy_task->GetMockAssets(price);

        auto p_str = new std::string(TSystem::utility::FormatStr("back_test %s original assets:%.2f | %d ret assets:%.2f", strategy_task->stock_code(), ori_assets, date, assets));
        strategy_task->app()->local_logger().LogLocal(cst_back_test_tag, *p_str);
        strategy_task->app()->AppendLog2Ui(p_str->c_str());
        strategy_task->app()->EmitSigShowUi(p_str);
        qDebug() << "FenbiCallBackFunc assets: " << assets << "\n";
    };

    T_FenbiCallBack *p_callback_obj = (T_FenbiCallBack*)para; 
     
    std::shared_ptr<StrategyTask>& strategy_task = *((std::shared_ptr<StrategyTask>*)(p_callback_obj->para));
    if( strategy_task->has_bktest_result_fetched() )
        return;

    if( quote_data->date != p_callback_obj->date )
    { 
        p_callback_obj->date = quote_data->date; 
        strategy_task->do_mock_date_change(quote_data->date);
    }

    auto quotes_data = std::make_shared<QuotesData>();
    quotes_data->cur_price = quote_data->price;
    qDebug() << p_callback_obj->serial++ << " " << quote_data->price << "\n"; 

    strategy_task->ObtainData(quotes_data);

    if( strategy_task->is_waitting_removed() )
    {
        strategy_task->has_bktest_result_fetched(true);
        show_result(strategy_task, p_callback_obj->date, quotes_data->cur_price);  

    }else if( is_end ) 
    {
        show_result(strategy_task, p_callback_obj->date, quotes_data->cur_price);  
    }
}