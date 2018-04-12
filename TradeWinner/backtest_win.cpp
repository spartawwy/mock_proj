#include "winner_win.h"

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"
#include "equal_section_task.h"
#include "advance_section_task.h"
#include "batches_buy_task.h"

#include "winner_hq_api.h"

typedef std::tuple<std::shared_ptr<StrategyTask>, std::shared_ptr<T_TaskInformation> > T_Task_Inf_Pair;

static void  FenbiCallBackFunc(T_QuoteAtomData *quote_data, bool is_end, void *para);

static HMODULE st_api_handle = nullptr;
static WinnerHisHq_ConnectDelegate WinnerHisHq_Connect = nullptr;
static WinnerHisHq_DisconnectDelegate WinnerHisHq_DisConnect = nullptr;
static WinnerHisHq_GetHisFenbiDataDelegate WinnerHisHq_GetHisFenbiData  = nullptr;
static WinnerHisHq_GetHisFenbiDataBatchDelegate WinnerHisHq_GetHisFenbiDataBatch  = nullptr;

static const std::string cst_back_test_tag = "bktest";

static const char cst_str_eqsec_bktest[] = {"区间交易回测"};
static const char cst_str_advancesec_bktest[] = {"贝塔交易回测"};
static const char cst_str_batchbuy_bktest[] = {"分批买入回测"};

bool WinnerWin::InitBacktestWin()
{
    bool ret = true;
    ui.cb_bktest_type->addItem(QString::fromLocal8Bit(cst_str_eqsec_bktest), QVariant(static_cast<int>(TypeTask::EQUAL_SECTION)));
    ui.cb_bktest_type->addItem(QString::fromLocal8Bit(cst_str_advancesec_bktest), QVariant(static_cast<int>(TypeTask::ADVANCE_SECTION)));
    ui.cb_bktest_type->addItem(QString::fromLocal8Bit(cst_str_batchbuy_bktest), QVariant(static_cast<int>(TypeTask::BATCHES_BUY)));
    //ui.wid_bktest_eqsec->show();
    ui.wid_bktest_adv_sec->setGeometry(ui.wid_bktest_eqsec->geometry().x(), ui.wid_bktest_eqsec->geometry().y()
                                       , ui.wid_bktest_adv_sec->geometry().width(), ui.wid_bktest_adv_sec->geometry().height());
    ui.wid_bktest_adv_sec->hide();
    ret = QObject::connect(ui.cb_bktest_type, SIGNAL(currentTextChanged(const QString&)), this, SLOT(DoBktestTypeChanged(const QString&)));

    m_backtest_list_hint_ = new HintList(this, ui.le_bktest_stock);
    m_backtest_list_hint_->hide();
    ret = QObject::connect(ui.le_bktest_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    ret = QObject::connect(m_backtest_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = QObject::connect(m_backtest_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));
     

    ret = connect(ui.pbtn_start_backtest, SIGNAL(clicked(bool)), this, SLOT(DoStartBacktest(bool)));
    ret = connect(this->app_, SIGNAL(SigEnableBtnBackTest()), this, SLOT(DoEnableBtnBackTest()));

    /*HMODULE*/ st_api_handle = LoadLibrary("winner_api.dll");
    if( !st_api_handle )
    {
        std::cout << "LoadLibrary winner_api.dll fail" << std::endl;
        return false;
    }
    //ui.pbtn_start_backtest->setDisabled(true);
   
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
    WinnerHisHq_GetHisFenbiDataBatch 
        = (WinnerHisHq_GetHisFenbiDataBatchDelegate)GetProcAddress(st_api_handle, "WinnerHisHq_GetHisFenbiDataBatch"); 
    if( !WinnerHisHq_GetHisFenbiDataBatch )
    {
        std::cout << " GetProcAddress WinnerHisHq_GetHisFenbiDataBatch fail " << std::endl;
        return false;
    }
    
    return true;
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

void WinnerWin::DoBktestTypeChanged(const QString&)
{
    int val = ui.cb_bktest_type->currentData().toInt();
    switch (val)
    {
    case (int)TypeTask::EQUAL_SECTION: 
        ui.wid_bktest_eqsec->show();
        ui.wid_bktest_adv_sec->hide();
        break;
    case (int)TypeTask::ADVANCE_SECTION: 
        ui.wid_bktest_eqsec->hide();
        ui.wid_bktest_adv_sec->show();
        break;
    default: break;
    }

}

void WinnerWin::DoStartBacktest(bool)
{
    static auto check_le_stock = [this]() ->bool
    {
       // check stock codes
		QString::SectionFlag flag = QString::SectionSkipEmpty;
		QString text_str = ui.le_bktest_stock->text().trimmed();
		QString stock_str = text_str.section('/', 0, 0, flag);
        if( stock_str.length() != 6 )
        {
			// todo: show erro info
            ui.le_bktest_stock->setFocus();
            this->DoStatusBar("股票代码有误!");
            return false;
        } 
        
        if( ui.dbspbox_bktest_start_price->value() < 0.01 )
        {
            ui.dbspbox_bktest_start_price->setFocus();
            this->DoStatusBar("起始价格不能为0!");
            return false;
        }
        if( ui.dbspbox_bktest_start_capital->value() < 0.01 )
        {
            ui.dbspbox_bktest_start_capital->setFocus();
            this->DoStatusBar("起始资金不能为0!");
            return false;
        }

        if( ui.dbspbox_bktest_raise_percent->value() < 0.1 )
        {
            ui.dbspbox_bktest_raise_percent->setFocus();
            this->DoStatusBar("上升百分比不能为0!");
            return false;
        }
        if( ui.dbspbox_bktest_fall_percent->value() < 0.1 )
        {
            ui.dbspbox_bktest_fall_percent->setFocus();
            this->DoStatusBar("下降百分比不能为0!");
            return false;
        }
        if( ui.cb_bktest_rebounce->isChecked() && ui.spinBox_bktest_rebounce->value() < 0.1 )
        { 
            ui.spinBox_bktest_rebounce->setFocus();
            this->DoStatusBar("拐点值不能为0!");
            return false;
        }  
        if( ui.spinBox_bktest_quantity->value() < 100 )
        {
            ui.spinBox_bktest_quantity->setFocus();
            this->DoStatusBar("买卖数量不能为0!");
            return false;
        }
        
        if( ui.cb_bktest_max_qty->isChecked() && ui.spinBox_bktest_max_qty->value() < 100 )
        {
            ui.cb_bktest_max_qty->setFocus();
            this->DoStatusBar("最大仓位不能为0!");
            return false;
        } 

        auto start_date = ui.de_bktest_begin->date().toString("yyyyMMdd").toInt();
        auto end_date = ui.de_bktest_end->date().toString("yyyyMMdd").toInt();

        if( end_date < start_date )
        {
            ui.de_bktest_begin->setFocus();
            this->DoStatusBar("开始日期不能大于结束日期!");
            return false;
        }
        return true;
    };
  
    if( !WinnerHisHq_GetHisFenbiData )
    {
        app_->winner_win().DoStatusBar("回测接口未安装!");
        return;
    }

    int ret_val = -1;
    //ret_val = WinnerHisHq_Connect("192.168.11.5", 50010, result, error);
    char result[1024] = {0};
    char error[1024] = {0};
    if( !stricmp(TSystem::utility::host().c_str(), "hzdev103") )
        ret_val = WinnerHisHq_Connect("128.1.1.3", 50010, result, error);
    else
        ret_val = WinnerHisHq_Connect("192.168.1.5", 50010, result, error);

    if( ret_val != 0 ) 
    {
        //ui.pbtn_start_backtest->setEnabled(true);
        this->DoStatusBar("服务器未连接!");
        return;
    }
     
    //-----------------
    static std::vector<std::shared_ptr<StrategyTask> > task_vector;
    static std::vector<std::shared_ptr<T_TaskInformation> > taskinfo_vector;
    static std::vector<std::shared_ptr<T_FenbiCallBack> > callback_vector;
    static std::vector<std::shared_ptr<T_MockStrategyPara> > mock_strategy_para_vector;
   
    task_vector.clear();
    taskinfo_vector.clear();
    callback_vector.clear();
    mock_strategy_para_vector.clear();
    //--------------------

    auto task_info = std::make_shared<T_TaskInformation>();

    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString stock_str = ui.le_bktest_stock->text().trimmed();
    QString stock_pinyin = stock_str.section('/', 1, 1, flag);
    task_info->stock = stock_str.section('/', 0, 0, flag).toLocal8Bit();
 
    task_info->id = 123; // ndedt:
    task_info->type = (TypeTask)ui.cb_bktest_type->currentData().toInt();
     
    task_info->back_alert_trigger = false;
    task_info->assistant_field = ""; 
    task_info->continue_second = 0; 

    auto mock_para = std::make_shared<T_MockStrategyPara>();
     
    if( task_info->type == TypeTask::BATCHES_BUY || task_info->type == TypeTask::EQUAL_SECTION )
    {
        if( !check_le_stock() ) 
            return;
        task_info->rebounce = ui.cb_bktest_rebounce->isChecked() ? ui.spinBox_bktest_rebounce->value() : 0.0;
        task_info->quantity = ui.spinBox_bktest_quantity->value();
        //const double alert_price = 20.3;
        task_info->alert_price = ui.dbspbox_bktest_start_price->value();

        task_info->secton_task.fall_percent = 1;
        //task_info->secton_task.fall_infection = 0.2;
        task_info->secton_task.raise_percent = 1;
        //task_info->secton_task.raise_infection = 0.2;
        task_info->secton_task.max_position = ui.cb_bktest_max_qty->isChecked() ? ui.spinBox_bktest_max_qty->value() : EQSEC_MAX_POSITION;
        task_info->secton_task.min_position = ui.cb_bktest_min_qty->isChecked() ? ui.spinBox_bktest_min_qty->value() : EQSEC_MIN_POSITION;
        task_info->secton_task.max_trig_price = ui.cb_bktest_max_stop_trigger->isChecked() ? ui.dbspbox_bktest_max_price->value() : MAX_STOCK_PRICE;
        task_info->secton_task.min_trig_price = ui.cb_bktest_min_clear_trigger->isChecked() ? ui.dbspbox_bktest_min_price->value() : MIN_STOCK_PRICE;
        if( task_info->type == TypeTask::BATCHES_BUY )
        {
            task_info->step = 1;
            task_info->bs_times = 1;
        }
        auto tmp_price = app_->GetStockPriceInfo(task_info->stock.c_str(), false);
        double cur_stock_price = tmp_price ? tmp_price->cur_price : 2.0;
        mock_para->avaliable_position = ui.spinBox_bktest_start_pos->value();
        mock_para->capital = ui.dbspbox_bktest_start_capital->value() + cur_stock_price * mock_para->avaliable_position;
    }else if( task_info->type == TypeTask::ADVANCE_SECTION )
    {
        
        double top_price = ui.dbspb_bktest_adv_max_price->value();
        double bottom_price = ui.dbspb_bktest_adv_min_price->value();
        if( top_price < bottom_price + 0.05 )
        {
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("顶部价格必须大于底部价格一定值!"));
            return;
        }
        int section_count = ui.spb_bktest_adv_section_count->value();
        if( section_count < 2 )
        {
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("区间数必须大于1!"));
            return;
        }
        task_info->quantity = ui.spinBox_bktest_adv_qty->value();
        double atom_h = (top_price - bottom_price ) / section_count;
        if( atom_h < 0.1 )
        {
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("单区间价格过小,请调整顶底价格或区间数目!"));
            return;
        }
        char buf[16] = {0}; 
        for( int i = 0; i < section_count; ++i )
        {
            sprintf_s(buf, sizeof(buf), "%.2f\0", bottom_price + atom_h * i);
            task_info->advance_section_task.portion_sections.append(buf);
            task_info->advance_section_task.portion_sections.append(";");
            task_info->advance_section_task.portion_states.append(std::to_string(int(AdvanceSectionTask::PortionState::UNKNOW)));
            task_info->advance_section_task.portion_states.append(";");
        }
        sprintf_s(buf, "%.2f\0", top_price);
        task_info->advance_section_task.portion_sections.append(buf);
        task_info->advance_section_task.portion_states.append(std::to_string(int(AdvanceSectionTask::PortionState::UNKNOW)));

        mock_para->avaliable_position = 0;
        mock_para->capital = (top_price + bottom_price) * task_info->quantity * ui.spb_bktest_adv_section_count->value() / 2;
    }
    
     
    mock_strategy_para_vector.push_back(std::move(mock_para));
    taskinfo_vector.push_back( task_info );
      
    switch (task_info->type)
    {
    case TypeTask::EQUAL_SECTION:
        {
            auto task = std::make_shared<EqualSectionTask>(*taskinfo_vector[0], app_, mock_strategy_para_vector[0].get()); 
            task_vector.push_back( std::move(task) );
            break;
        }
    case TypeTask::ADVANCE_SECTION:
        {
            auto task = std::make_shared<AdvanceSectionTask>(*taskinfo_vector[0], app_, mock_strategy_para_vector[0].get()); 
            task_vector.push_back( std::move(task) );
            break;
        }
    case TypeTask::BATCHES_BUY:
        {
            auto task = std::make_shared<BatchesBuyTask>(*taskinfo_vector[0], app_, mock_strategy_para_vector[0].get()); 
            task_vector.push_back( std::move(task) );
            break;
        }
    }
    
     
#ifdef TMP_TEST     
    const double capital = 200000.00;

    auto task_info = std::make_shared<T_TaskInformation>();
#if 1
    task_info->id = 123;
    task_info->type = TypeTask::EQUAL_SECTION; 
    task_info->stock = "601069";
    task_info->back_alert_trigger = false;
    task_info->rebounce = 0.3;
    //task_info->rebounce = 0;
    task_info->continue_second = 0; 
    task_info->quantity = 500;
    const double alert_price = 20.3;
    task_info->alert_price = alert_price;
    task_info->assistant_field = "";

    task_info->secton_task.fall_percent = 1;
    //task_info->secton_task.fall_infection = 0.2;
    task_info->secton_task.raise_percent = 1;
    //task_info->secton_task.raise_infection = 0.2;
    task_info->secton_task.max_position = 3000*10;
    task_info->secton_task.max_trig_price = 23.0;
    task_info->secton_task.min_trig_price = 15.0; 
    taskinfo_vector.push_back( std::move(task_info));

    auto mock_para = std::make_shared<T_MockStrategyPara>();
    mock_para->avaliable_position = (capital / alert_price ) / 2 / 100 * 100;
    mock_para->capital = capital - mock_para->avaliable_position * alert_price;
    mock_strategy_para_vector.push_back(std::move(mock_para));

    auto equal_sec_task = std::make_shared<EqualSectionTask>(*taskinfo_vector[0], app_, mock_strategy_para_vector[0].get()); 
    task_vector.push_back( std::move(equal_sec_task) );

#elif 0 // test equal section task 
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
       
#endif

    auto fenbi_callback_obj = std::make_shared<T_FenbiCallBack>();
    fenbi_callback_obj->call_back_func = FenbiCallBackFunc;
    fenbi_callback_obj->para = std::addressof(task_vector[0]);
    callback_vector.push_back(std::move(fenbi_callback_obj));

    assert(callback_vector.size());
    memset(error, 0, sizeof(error)); 
    //auto date_vector = app_->GetSpanTradeDates(20170914, 20171031);
    auto start_date = ui.de_bktest_begin->date().toString("yyyyMMdd").toInt();
    auto end_date = ui.de_bktest_end->date().toString("yyyyMMdd").toInt();
    auto date_vector = app_->GetSpanTradeDates(start_date, end_date);
    if( date_vector.size() < 1 )
    {
        app_->winner_win().DoStatusBar("无可用交易日!");
        return;
    }

#if 1 
    this->ui.pbtn_start_backtest->setDisabled(true);
    oneceshot_timer_contain_->InsertTimer(2 * 2 * 1000, [this]()
    {
        this->ui.pbtn_start_backtest->setEnabled(true);
    }); 
#endif

    WinnerHisHq_GetHisFenbiDataBatch(const_cast<char*>(taskinfo_vector[0]->stock.c_str())
            , start_date
            , end_date
            , callback_vector[0].get()
            , error);
  
}


void WinnerWin::DoEnableBtnBackTest()
{
    ui.pbtn_start_backtest->setEnabled(true);
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

     struct tm * timeinfo = localtime(&quote_data->time);
     int long_date = (timeinfo->tm_year + 1900) * 10000 + (timeinfo->tm_mon + 1) * 100 + timeinfo->tm_mday;

    if( long_date != p_callback_obj->date )
    { 
        p_callback_obj->date = long_date; 
        strategy_task->do_mock_date_change(long_date);
    }

    auto data = std::make_shared<QuotesData>();
   
    data->time_stamp= quote_data->time;
    data->cur_price = quote_data->price;
    qDebug() << p_callback_obj->serial++ << " " << quote_data->price << "\n"; 

    strategy_task->ObtainData(data);

    if( strategy_task->is_waitting_removed() )
    {
        strategy_task->has_bktest_result_fetched(true);
        show_result(strategy_task, p_callback_obj->date, data->cur_price);  

    }else if( is_end ) 
    {
        show_result(strategy_task, p_callback_obj->date, data->cur_price);  
        strategy_task->app()->Emit_SigEnableBtnBackTest();
    }
}