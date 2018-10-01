#include "winner_app.h"

#include <qmessagebox.h>
#include <qdebug.h>
#include <QtWidgets/QApplication>
#include <qtimer.h>

#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <array>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

#include "common.h"
#include "db_moudle.h"

#include "breakdown_task.h"
#include "stock_ticker.h"
#include "task_factory.h"

#include "login_win.h"
#include "broker_cfg_win.h"
#include "message_win.h"

#include "index_task.h"
#include "back_tester.h"

#define   DEBUG_TRADE do{local_logger().LogLocal(utility::FormatStr("%s line: %d", __FUNCTION__, __LINE__));}while(0);
static bool SetCurrentEnvPath();

//static void AjustTickFlag(bool & enable_flag);

static const int cst_ticker_update_interval = 2000;  //ms :notice value have to be bigger than 1000
static const int cst_normal_timer_interval = 2000;

//static const unsigned int cst_result_len = 1024 * 1024;

WinnerApp::WinnerApp(int argc, char* argv[])
	: QApplication(argc, argv)
	, ServerClientAppBase("client", "trade_mock", "0.1")
	, tick_strand_(task_pool())
    , index_tick_strand_(task_pool())
	, stock_ticker_(nullptr)
	, index_ticker_(std::make_shared<IndexTicker>(this->local_logger())) 
	, stock_ticker_life_count_(0)
    , index_ticker_life_count_(0)
	, ticker_enable_flag_(false)
	, trade_strand_(task_pool())
	, task_infos_(256)
	, msg_win_(new MessageWin(5000))
    , msgwin_longshow_(new MessageWin(60*1000))
	, winner_win_(this, nullptr)
	, login_win_(this)
	, exit_flag_(false)
	, trade_client_id_(-1)
	, cookie_()
	, db_moudle_(this)
	, user_info_()
	, trade_agent_()
	, strategy_tasks_timer_(std::make_shared<QTimer>())
	, normal_timer_(std::make_shared<QTimer>())
	, stocks_position_(256)
	, stocks_price_info_(256)
	, p_user_account_info_(nullptr)
	, p_user_broker_info_(nullptr)
    , exchange_calendar_()
    , position_mocker_(nullptr)
    , back_tester_(std::make_shared<BackTester>(this))
    , codes_name_(5000)
{   
	connect(strategy_tasks_timer_.get(), SIGNAL(timeout()), this, SLOT(DoStrategyTasksTimeout()));
	connect(normal_timer_.get(), SIGNAL(timeout()), this, SLOT(DoNormalTimer()));
}

WinnerApp::~WinnerApp()
{ 
	if( msg_win_ )
	{
		msg_win_->close();
		delete msg_win_;
	}
}

bool WinnerApp::Init()
{
    //DEBUG_TRADE
	option_dir_type(AppBase::DirType::STAND_ALONE_APP);
	option_validate_app(false);

	std::string cur_dir(".//");
	work_dir(cur_dir);
	local_logger_.SetDir(cur_dir);

	int ret = 0;
	auto val_ret = cookie_.Init();
	if( val_ret != Cookie::TRetCookie::OK )
	{
		switch(Cookie::TRetCookie::ERROR_FILE_OPEN)
		{
		case Cookie::TRetCookie::ERROR_FILE_OPEN: QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("程序已经打开!")); break;
		default: QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("cookie异常!")); break;
		}
		return false;
	} 
    for (int i = 0; i < 2; ++ i)
    {
        task_pool_.AddWorker();
    }

	db_moudle_.Init();
    db_moudle_.LoadTradeDate(&exchange_calendar_);

    db_moudle_.LoadCodesName(codes_name_);
	//-----------------------------login window show --------------------  
	login_win_.Init(); 
	ret = login_win_.exec(); 
	if( ret != QDialog::Accepted )
	{
		Stop();
		return false;
	}
    //-------------------------------end---------------------------------
#if 0
	db_moudle_.LoadAllUserBrokerInfo();

	p_user_account_info_ = db_moudle_.FindUserAccountInfo(user_info_.id);
	p_user_broker_info_ = db_moudle_.FindUserBrokerByUser(user_info_.id);
	assert(p_user_account_info_ && p_user_broker_info_);

#endif
   
#ifndef  USE_MOCK_FLAG
    //---------------broker config window show --------------------
	BrokerCfgWin  bcf_win(this);
	bcf_win.Init();
	ret = bcf_win.exec();
	if( ret != QDialog::Accepted )
	{
		Stop();
		return false;
	}
     
	//=========================trade account login =================
	 
	Buffer result(1024);
	char error[1024] = {0};

	trade_agent_.QueryData(trade_client_id_, (int)TypeQueryCategory::SHARED_HOLDER_CODE, result.data(), error);
	if( strlen(error) != 0 )
	{ 
		QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("查询股权代码失败!"));
	}
	qDebug() << QString::fromLocal8Bit(result.data()) << "\n";
	local_logger().LogLocal(result.data());

	trade_agent_.SetupAccountInfo(result.data());
#else
    ///DEBUG_TRADE
    position_mocker_ = std::make_shared<PositionMocker>(user_info_.id, &db_moudle_, &exchange_calendar_);

    db_moudle_.LoadPositionMock(*position_mocker_);

    UpdatePositionMock();

#endif
   
    db_moudle_.LoadAllTaskInfo(task_infos_);
    trade_agent_.Init(user_info_.id, &db_moudle_, position_mocker_);

    back_tester_->Init();
     
	//------------------------ winner window ------------------
      
	winner_win_.Init();  
	winner_win_.show();
    //------------------------end------------------------------

	bool ret1 = QObject::connect(this, SIGNAL(SigTaskStatChange(StrategyTask*, int)), &winner_win_, SLOT(DoTaskStatChangeSignal(StrategyTask*, int)));

	ret1 = QObject::connect(this, SIGNAL(SigRemoveTask(int)), &winner_win_, SLOT(RemoveByTaskId(int)));
	//ret1 = QObject::connect(this, SIGNAL(SigShowUi(std::shared_ptr<std::string>)), this, SLOT(DoShowUi(std::shared_ptr<std::string>)));
	//ret1 = QObject::connect(this, SIGNAL(SigShowUi(std::string *)), this, SLOT(DoShowUi(std::string *)));
	ret1 = QObject::connect(this, SIGNAL(SigShowUi(std::string *, bool)), this, SLOT(DoShowUi(std::string *, bool)));
    ret1 = QObject::connect(this, SIGNAL(SigShowLongUi(std::string *, bool)), this, SLOT(DoShowLongUi(std::string *, bool)));
 
#if 1 
	stock_ticker_ = std::make_shared<StockTicker>(this->local_logger());
	stock_ticker_->Init();

	if( !index_ticker_->Init() )
		return false;
    //------------------------ create tasks ------------------
	TaskFactory::CreateAllTasks(task_infos_, strategy_tasks_, this);

#ifndef USE_MOCK_FLAG
	QueryPosition();
#endif

    ticker_enable_flag_ = IsNowTradeTime(); 
	//-----------ticker main loop----------
	task_pool().PostTask([this]()
	{
		while(!this->exit_flag_)
		{
			Delay(cst_ticker_update_interval);

			if( !this->ticker_enable_flag_ )
				continue;

			tick_strand_.PostTask([this]()
			{
				this->stock_ticker_->Procedure();
				this->stock_ticker_life_count_ = 0;
			});

            index_tick_strand_.PostTask([this]()
            {
                this->index_ticker_->Procedure();
				this->index_ticker_life_count_ = 0;
            });
		}
		qDebug() << "out loop \n";
	});
	//----------------
#endif

	strategy_tasks_timer_->start(1000); //msec invoke DoStrategyTasksTimeout
	normal_timer_->start(2000); // invoke DoNormalTimer
	return true;
}

void WinnerApp::Stop()
{
	exit_flag_ = true;
	//FireShutdown();
    // because back tester use winner_api which use task_pool, so let it release first
    back_tester_.reset();
	Shutdown();
    this->quit();
}

void WinnerApp::RemoveTask(unsigned int task_id, TypeTask task_type)
{
	DelTaskById(task_id, task_type);
	EmitSigRemoveTask(task_id);
}

int WinnerApp::Cookie_NextTaskId()
{
	std::lock_guard<std::mutex> locker(cookie_mutex_);
	return ++ cookie_.data_->max_task_id;
}

int WinnerApp::Cookie_MaxTaskId()
{
	std::lock_guard<std::mutex> locker(cookie_mutex_);
	return cookie_.data_->max_task_id;
}

void WinnerApp::Cookie_MaxTaskId(int task_id)
{
	std::lock_guard<std::mutex> locker(cookie_mutex_);
	cookie_.data_->max_task_id = task_id;
}

__int64 WinnerApp::Cookie_NextFillId()
{
    std::lock_guard<std::mutex> locker(cookie_fill_mutex_);
    return ++ cookie_.data_->max_fill_id;
}

__int64 WinnerApp::Cookie_MaxFillId()
{
    std::lock_guard<std::mutex> locker(cookie_fill_mutex_);
    return cookie_.data_->max_fill_id;
}

void WinnerApp::Cookie_MaxFillId(__int64 fill_id)
{
    std::lock_guard<std::mutex> locker(cookie_fill_mutex_);
    cookie_.data_->max_fill_id = fill_id;
}

bool WinnerApp::LoginBroker(int broker_id, int depart_id, const std::string& account, const std::string& password)
{
	assert(trade_agent_.IsInited());

	char error_info[256] = {0};

	auto p_broker_info = db_moudle_.FindUserBrokerByBroker(broker_id);
	if( !p_broker_info) 
		return false;
	auto p_user_account_info = db_moudle_.FindUserAccountInfo(user_info_.id); 
	//assert(p_user_account_info && p_user_broker_info);

	/*trade_client_id_ = trade_agent_.Logon("122.224.113.121"
	, 7708, "2.20", 1, "32506627"
	, "32506627", "626261", "", error_info);*/
	//p_user_account_info->comm_pwd_;
#if 1
	trade_client_id_ = trade_agent_.Logon(const_cast<char*>(p_broker_info->ip.c_str())
		, p_broker_info->port
		, const_cast<char*>(p_broker_info->com_ver.c_str())
		, 1
		, const_cast<char*>(account.c_str())
		, const_cast<char*>(account.c_str())  // default trade no is account no  
		, const_cast<char*>(password.c_str())
		, p_broker_info->type == TypeBroker::ZHONGY_GJ ? password.c_str() : ""// communication password 
		, error_info);
#elif 0
	trade_client_id_ = trade_agent_.Logon("218.205.84.239" //"115.238.180.23"
		, 80
		, "4.02" //"2.43" //"8.19"  // "2.24" 
		, 152
		, const_cast<char*>(account.c_str())
		, const_cast<char*>(account.c_str())  // default trade no is account no  
		, const_cast<char*>(password.c_str())
		, p_broker_info->type == TypeBroker::ZHONGYGJ ? password.c_str() : ""// communication password 
		//, const_cast<char*>(password.c_str())
		, error_info);
#elif 0
	trade_client_id_ = trade_agent_.Logon("218.205.84.239" // tdx yhzq  
		, 80
		, "4.02" //"2.43" //"8.19"  // "2.24" 
		, 152
		, const_cast<char*>(account.c_str())
		, const_cast<char*>(account.c_str())  // default trade no is account no  
		, const_cast<char*>(password.c_str())
		, p_broker_info->type == TypeBroker::ZHONGYGJ ? password.c_str() : ""// communication password 
		//, const_cast<char*>(password.c_str())
		, error_info);
#elif 1
	trade_client_id_ = trade_agent_.Logon("113.108.128.105" //"218.75.75.28" // 
		, 443
		, "2.50" // "4.02" //"2.43" //"8.19"  // "2.24" 
		, 1
		, const_cast<char*>(account.c_str())
		, ""//const_cast<char*>(account.c_str())  // default trade no is account no  
		, const_cast<char*>(password.c_str())
		, p_broker_info->type == TypeBroker::ZHONGYGJ ? password.c_str() : ""// communication password 
		//, const_cast<char*>(password.c_str())
		, error_info);

#endif
	if( trade_client_id_ == -1 ) 
	{
		// QMessageBox::information(nullptr, "alert", "login fail!");
		return false;
	} 
	p_user_broker_info_ = p_broker_info;
	p_user_account_info_  = p_user_account_info;
	return true;
}

void WinnerApp::AppendTaskInfo(int id, std::shared_ptr<T_TaskInformation>& info)
{
	WriteLock  locker(task_infos_mutex_);
	if( task_infos_.find(id) != task_infos_.end() )
	{
		assert(false); 
		// log error
		return;
	}

	task_infos_.insert(std::make_pair(id, info));
}

void WinnerApp::AppendStrategyTask(std::shared_ptr<StrategyTask> &task)
{ 
	WriteLock  locker(strategy_tasks_mutex_);
	strategy_tasks_.push_back(task);
}

std::shared_ptr<T_TaskInformation> WinnerApp::FindTaskInfo(int task_id)
{
	std::shared_ptr<T_TaskInformation> p_info = nullptr;

	ReadLock locker(task_infos_mutex_);
	auto iter = task_infos_.find(task_id);
	if( iter != task_infos_.end() )
		return iter->second;
	return p_info;
}

bool WinnerApp::DelTaskById(int task_id, TypeTask task_type)
{ 
    if( task_type != TypeTask::INDEX_RISKMAN )
    {
	    ticker_strand().PostTask([task_id, this]()
	    {
          this->stock_ticker_->UnRegister(task_id);
	    });
    }else
	{
        index_tick_strand().PostTask([task_id, this]()
	    {
          this->index_ticker_->UnRegister(task_id);
	    }); 
    }

	{
		ReadLock  locker(task_infos_mutex_);
		auto iter = task_infos_.find(task_id);
		assert(iter != task_infos_.end());
		assert( task_type == iter->second->type );
		db_moudle().AddHisTask(iter->second);
	}

	// del database related records
	db_moudle().DelTaskInfo(task_id, task_type);

	WriteLock  locker(strategy_tasks_mutex_);
	for( auto iter = strategy_tasks_.begin(); iter != strategy_tasks_.end(); ++iter )
	{
		if( (*iter)->task_id() == task_id )
		{
			strategy_tasks_.erase(iter);
			return true;
		}
	}
	return false;
}

std::shared_ptr<StrategyTask> WinnerApp::FindStrategyTask(int task_id)
{
	ReadLock  locker(strategy_tasks_mutex_);
	for( auto iter = strategy_tasks_.begin(); iter != strategy_tasks_.end(); ++iter )
	{
		if( (*iter)->task_id() == task_id )
		{ 
			return *iter;
		}
	}
	return nullptr;
}

T_CodeMapPosition WinnerApp::QueryPosition()
{ 
    //db_moudle_.GetStockCode(); // todo:

#ifndef USE_MOCK_FLAG
	auto result = std::make_shared<Buffer>(5*1024);

	char error[1024] = {0};
#ifdef USE_TRADE_FLAG
	trade_agent_.QueryData(trade_client_id_, (int)TypeQueryCategory::STOCK, result->data(), error);
	if( strlen(error) != 0 )
	{ 
		qDebug() << "query  fail! " << "\n";
		return T_CodeMapPosition();
	}
	qDebug() << QString::fromLocal8Bit( result->data() ) << "\n";
#endif
	std::string str_result = result->c_data();
      
	TSystem::utility::replace_all_distinct(str_result, "\n", "\t");
	/*qDebug() << " line 382" << "\n";
	qDebug() << str_result.c_str() << " ----\n";*/
	auto result_array = TSystem::utility::split(str_result, "\t");

	std::lock_guard<std::mutex>  locker(stocks_position_mutex_);


	int start = 14;
	int content_col = 13;
	if( p_user_broker_info_->type == TypeBroker::ZHONGY_GJ )
		start = 15;
	else if ( p_user_broker_info_->type == TypeBroker::PING_AN )
	{
		start = 23;
		content_col = 21;
	} 
	{ 
		for( int n = 0; n < (result_array.size() - start) / content_col; ++n )
		{
			T_PositionData  pos_data;
			auto str_code = result_array.at( start + n * content_col);
			TSystem::utility::replace_all_distinct(str_code, "\t", "");
            strcpy_s(pos_data.code, str_code.c_str());
			double qty_can_sell = 0;
			try
			{
				strcpy_s(pos_data.pinyin, result_array.at( start + n * content_col + 1).c_str());
				pos_data.total = boost::lexical_cast<double>(result_array.at( start + n * content_col + 2 ));
				pos_data.avaliable = boost::lexical_cast<double>(result_array.at(start + n * content_col + 3));
				pos_data.cost = boost::lexical_cast<double>(result_array.at(start + n * content_col + 4));
				pos_data.value = boost::lexical_cast<double>(result_array.at(start + n * content_col + 6));
				pos_data.profit = boost::lexical_cast<double>(result_array.at(start + n * content_col + 7));
				pos_data.profit_percent = boost::lexical_cast<double>(result_array.at(start + n * content_col + 8));

			}catch(boost::exception &e)
			{ 
				continue;
			} 

			auto iter = stocks_position_.find(pos_data.code);
			if( iter == stocks_position_.end() )
			{
				stocks_position_.insert(std::make_pair(pos_data.code, std::move(pos_data)));
			}else
				iter->second = pos_data;
		}
	} 
    return stocks_position_;
#else
    return position_mocker_->ReadAllStockPosition(TSystem::Today());
#endif 
	
}

T_PositionData* WinnerApp::QueryPosition(const std::string& code)
{
#ifndef USE_MOCK_FLAG
	QueryPosition();
	std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
	auto iter = stocks_position_.find(code);
	if( iter == stocks_position_.end() )
	{ 
		return nullptr;
	}
	return std::addressof(iter->second);
#else
    return position_mocker_->ReadPosition(TSystem::Today(), code);
#endif
}

int WinnerApp::QueryPosAvaliable_LazyMode(const std::string& code) 
{
#ifndef USE_MOCK_FLAG
	std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
	auto iter = stocks_position_.find(code);
	if( iter == stocks_position_.end() )
		return 0;

	return (int)iter->second.avaliable;
#else
    assert(position_mocker_);
    auto pos =  position_mocker_->ReadPosition(TSystem::Today(), code);
    if( pos )
        return pos->avaliable;
    else
        return 0;
#endif
}

T_PositionData* WinnerApp::QueryPosition_LazyMode(const std::string& code)
{
#ifndef USE_MOCK_FLAG
	std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
	auto iter = stocks_position_.find(code);
	if( iter == stocks_position_.end() )
		return nullptr;
    return std::addressof(iter->second);
#else
    assert(position_mocker_);
    return position_mocker_->ReadPosition(TSystem::Today(), code);
#endif
	
}

void WinnerApp::AddPosition(const std::string& code, int pos)
{
#ifndef USE_MOCK_FLAG
	std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
	auto iter = stocks_position_.find(code);
	if( iter == stocks_position_.end() )
	{
		T_PositionData  pos_data;
		strcpy_s(pos_data.code,  code.c_str());
		pos_data.total = pos; 
		stocks_position_.insert(std::make_pair(code, std::move(pos_data)));
	}else
	{
		iter->second.total += pos;
	}
#else
    assert(position_mocker_);
    position_mocker_->AddTotalPosition(TSystem::Today(), code, (double)pos, true);
#endif
}

// sub avaliable position
void WinnerApp::SubAvaliablePosition(const std::string& code, int pos)
{
#ifndef USE_MOCK_FLAG
	assert( pos > 0 );
	std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
	auto iter = stocks_position_.find(code);
	if( iter == stocks_position_.end() )
	{
		local_logger().LogLocal("error: WinnerApp::SubPosition can't find " + code);
	}else
	{
		if( iter->second.avaliable < pos )
		{
			local_logger().LogLocal( utility::FormatStr("error: WinnerApp::SubPosition %s avaliable < %d ", code.c_str(), pos) );
		}else
		{
			iter->second.avaliable -= pos; 
			iter->second.total -= pos;
		}
	}
#else 
    assert(position_mocker_);
    position_mocker_->SubAvaliablePosition(TSystem::Today(), code, (double)pos);
#endif
}

T_Capital WinnerApp::QueryCapital()
{
	T_Capital capital;

#ifndef USE_MOCK_FLAG
	auto result = std::make_shared<Buffer>(5*1024);

	char error[1024] = {0};
#ifdef USE_TRADE_FLAG
	trade_agent_.QueryData(trade_client_id_, (int)TypeQueryCategory::CAPITAL, result->data(), error);
	if( strlen(error) != 0 )
	{ 
		qDebug() << "query  fail! " << "\n";
	}
	qDebug() << QString::fromLocal8Bit( result->data() ) << "\n";
#endif
	std::string str_result = result->c_data();
	TSystem::utility::replace_all_distinct(str_result, "\n", "\t");

	auto result_array = TSystem::utility::split(str_result, "\t");
	if( result_array.size() < 13 )
		return capital;
	try
	{
		if( p_user_broker_info_->type == TypeBroker::PING_AN )
		{
			capital.remain = boost::lexical_cast<double>(result_array.at(19));
			capital.available = boost::lexical_cast<double>(result_array.at(20));
			capital.total = boost::lexical_cast<double>(result_array.at(25));
		}else
		{
			capital.remain = boost::lexical_cast<double>(result_array.at(11));
			capital.available = boost::lexical_cast<double>(result_array.at(12));
			capital.total = boost::lexical_cast<double>(result_array.at(15));
		}
	}catch(boost::exception &e)
	{ 
	}
#else
    assert(position_mocker_);
    auto p_capital_pos = position_mocker_->ReadPosition(TSystem::Today(), CAPITAL_SYMBOL);
    if( p_capital_pos )
    {
        capital.total = p_capital_pos->total;
        capital.available = p_capital_pos->avaliable;
        capital.remain = capital.available; // need check
    }
#endif
	return capital;
}

T_StockPriceInfo * WinnerApp::GetStockPriceInfo(const std::string& code, bool is_lazy)
{
	assert(index_ticker_->StkQuoteGetQuote_);
	char stocks[1][16];
   
	auto iter = stocks_price_info_.find(code);
	if( iter != stocks_price_info_.end() )
    {
        if( is_lazy )
        {
		    return std::addressof(iter->second);
        }
    }
	strcpy_s(stocks[0], code.c_str());
 
	T_StockPriceInfo price_info[1];
	auto num = index_ticker_->StkQuoteGetQuote_(stocks, 1, price_info);
	if( num < 1 )
		return nullptr;
 
    if( iter != stocks_price_info_.end() )
    {
        iter->second = price_info[0];
        return std::addressof(iter->second);
    }
    else
    {
	    T_StockPriceInfo& info = stocks_price_info_.insert(std::make_pair(code, price_info[0])).first->second;
	    return &info;
    }
}

void WinnerApp::SlotStopAllTasks(bool)
{ 
    StopAllStockTasks();
	StopAllIndexRelTypeTasks(TindexTaskType::ALERT); 
    StopAllIndexRelTypeTasks(TindexTaskType::CLEAR); 
    StopAllIndexRelTypeTasks(TindexTaskType::RELSTOCK); 

}

void WinnerApp::SlotResetMockSys(bool)
{ 
    assert(position_mocker_);
    auto ret_button = QMessageBox::question(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("重置系统将删除所有模拟交易数据.确定要重置?"),
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if( ret_button == QMessageBox::Cancel )
        return;
    SlotStopAllTasks(true);
    // reset mock system
    this->trade_strand().PostTask([this]()
    {
        position_mocker_->Reset();
        db_moudle_.DelAllFillRecord();
    });
    winner_win_.ClearLog(); 
}

void WinnerApp::DoStrategyTasksTimeout()
{
    static auto is_in_task_time = [](const QTime &current, const QTime &start, const QTime &end) ->bool
    {
        return current >= start && current <= end;
    };
	
    auto cur_time = QTime::currentTime();
    //qDebug() << "DoStrategyTasksTimeout: " << cur_time.toString() << "\n";
	// register --------------
	ReadLock locker(strategy_tasks_mutex_);
	std::for_each( std::begin(strategy_tasks_), std::end(strategy_tasks_), [&cur_time, this](std::shared_ptr<StrategyTask>& entry)
	{
        if( !entry->is_to_run()  )
            return;
        if( entry->cur_state() == TaskCurrentState::WAITTING ) // state: unregistered
        {
            if( is_in_task_time(cur_time, entry->tp_start(), entry->tp_end()) )
            {
                if( entry->task_info().type == TypeTask::INDEX_RISKMAN )
                {
                    // index ticker register 
					this->index_tick_strand_.PostTask([entry, this]()
			        {
						this->index_ticker_->Register(entry);
			        });
                }else
                {
                    // stock ticker register
                    this->tick_strand_.PostTask([entry, this]()
			        {
				        this->stock_ticker_->Register(entry);
			        });
                }
                if( IsNowTradeTime() )
                    entry->cur_state(TaskCurrentState::STARTING);
                else
                    entry->cur_state(TaskCurrentState::REST);
				this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
            }
        }else if( entry->cur_state() > TaskCurrentState::WAITTING ) // state:  registered
        {
            if( !is_in_task_time(cur_time, entry->tp_start(), entry->tp_end()) ) // in task time
            {
                if( entry->task_info().type == TypeTask::INDEX_RISKMAN )
                {
                    // index ticker unregister 
					this->index_ticker_->UnRegister(entry->task_id());
                }else
                {
                    this->stock_ticker_->UnRegister(entry->task_id());
                }
                entry->cur_state(TaskCurrentState::WAITTING); 
				this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));

            }else if( entry->cur_state() != TaskCurrentState::REST )
            {
                if( !IsNowTradeTime() ) // not in rest time
                {
                    entry->cur_state(TaskCurrentState::REST); 
				    this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
                }
            }

            if( entry->cur_state() == TaskCurrentState::RUNNING )
            {
                if( entry->life_count_++ > 60 )
                {
                    this->local_logger().LogLocal(utility::FormatStr("error: task %d not in running", entry->task_id()));
                    entry->cur_state(TaskCurrentState::EXCEPT);
                    this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
                }
            }
        } 
         
	});

}
 
void WinnerApp::DoShowUi(std::string* str, bool flash_taskbar)
{
	assert(str);
	msg_win().ShowUI("提示", *str);
	delete str; str = nullptr;

    if( flash_taskbar )
        winner_win_.TriggerFlashWinTimer(true);
}
 
void WinnerApp::DoShowLongUi(std::string* str, bool flash_taskbar)
{
	assert(str);
	msgwin_longshow().ShowUI("提示", *str);
	delete str; str = nullptr;

    if( flash_taskbar )
        winner_win_.TriggerFlashWinTimer(true);
}

void WinnerApp::DoNormalTimer()
{ 
    assert(position_mocker_);
    //bool is_date_change = false;
	ticker_enable_flag_ = IsNowTradeTime();

	if( IsDateChange() )
	{
		UpdateLatestDateTag(TSystem::Today());
		  
		UpdatePositionMock();
	}

	if( ticker_enable_flag_ )
	{
        bool is_stopped = false;
		if( ++stock_ticker_life_count_ > 10 )
		{
			local_logger().LogLocal("thread stock_ticker procedure stoped!");
			winner_win_.DoStatusBar("异常: 股票内部报价停止!");
            is_stopped = true;
		}
        if(++index_ticker_life_count_ > 10 )
		{
			local_logger().LogLocal("thread stock_ticker procedure stoped!");
			winner_win_.DoStatusBar("异常: 指数内部报价停止!");
            is_stopped = true;
		}
        if( !is_stopped )
			winner_win_.DoStatusBar("正常");
	}

	static int count_query = 0;
	// 30 second do a position query
	assert( 30000 / cst_normal_timer_interval > 0 );
	if( ++count_query % (30000 / cst_normal_timer_interval) == 0 )
	{
		trade_strand().PostTask([this]()
		{ 
			this->QueryPosition();
#ifdef USE_MOCK_FLAG
            // query to keep api online 
            char *stock_codes[1] = {"000001"}; 

            TCodeMapQuotesData ret_quotes_data; 
            this->stock_ticker().GetQuoteDatas(stock_codes, 1, ret_quotes_data);
#endif
            return;
		});
	}
}


void WinnerApp::AppendLog2Ui(const char *fmt, ...)
{
	va_list ap;

	const int cst_buf_len = 1024;
	char szContent[cst_buf_len] = {0};
	char *p_buf = new char[cst_buf_len]; 
	memset(p_buf, 0, cst_buf_len);

	va_start(ap, fmt);
	vsprintf_s(szContent, sizeof(szContent), fmt, ap);
	va_end(ap);

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	sprintf_s( p_buf, cst_buf_len, "[%d %02d:%02d:%02d] %s \n"
		, (timeinfo->tm_year+1900)*10000 + (timeinfo->tm_mon+1)*100 + timeinfo->tm_mday
		, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, szContent ); 

	emit SigAppendLog(p_buf); //will invoke SlotAppendLog
}

bool WinnerApp::SellAllPosition(IndexTask * task)
{ 
    T_CodeMapPosition stocks_pos = QueryPosition();
       
    auto p_stocks_pos = std::make_shared<T_CodeMapPosition>(std::move(stocks_pos));
    if( p_stocks_pos->size() < 1 && stocks_position_.size() > 0)
    {
        p_stocks_pos->clear();
        {
        std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
        *p_stocks_pos = stocks_position_;
        }
    }
          
    char* stock_codes[cst_max_stock_code_count];
    memset(stock_codes, 0, sizeof(char*)*cst_max_stock_code_count);
     
    //byte markets[cst_max_stock_code_count];
    short stock_count = 0;
    std::for_each( std::begin(*p_stocks_pos), std::end(*p_stocks_pos), [&, this](T_CodeMapPosition::reference entry)
    {
        if( entry.second.avaliable <= 0 )
                return; 
        stock_codes[stock_count++] = const_cast<char*>(entry.first.c_str());
    });

    if( stock_count < 1 )
    {
        std::string str = utility::FormatStr("warning: %d 清仓任务触发, 并无可卖出仓位!", task->task_id());
        this->local_logger().LogLocal(TagOfOrderLog(), str);
        this->AppendLog2Ui(str.c_str());
        return false;
    }

    // get quotes --------------------------
    Buffer Result(cst_result_len);
    Buffer ErrInfo(cst_error_len);
        
    if( !stock_ticker_->GetQuotes(stock_codes, stock_count, Result) )
    {
        auto ret_str = new std::string(utility::FormatStr("error %d 清仓任务失败:获得报价失败!", task->task_id()));
        this->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
        this->AppendLog2Ui(ret_str->c_str());
        this->EmitSigShowUi(ret_str, true);
        return false;
    }
    // decode  
    auto quotes_data_list = std::make_shared<std::list<T_codeQuoteDateTuple> >();
    stock_ticker_->DecodeStkQuoteResult(Result, quotes_data_list.get(), nullptr);
     
    // send orders -----
    trade_strand().PostTask([ quotes_data_list, p_stocks_pos, task, this]()
    {
        std::for_each( std::begin(*quotes_data_list), std::end(*quotes_data_list), [p_stocks_pos, task, this](T_codeQuoteDateTuple &entry)
        {
            auto iter = p_stocks_pos->find( std::get<0>(entry) );
            if( iter == p_stocks_pos->end() || iter->second.avaliable == 0 )
                return;

            char result[1024] = {0};
            char error_info[1024] = {0};
            // get quote of buy
            double price = task->GetQuoteTargetPrice(*(std::get<1>(entry)), false);
            price -= 0.01;
#ifdef USE_TRADE_FLAG
            // send order 
            this->trade_agent().SendOrder(this->trade_client_id()
                    , (int)TypeOrderCategory::SELL
					, 0
                    , const_cast<T_AccountData *>(this->trade_agent().account_data(GetStockMarketType( std::get<0>(entry) )))->shared_holder_code
					, const_cast<char*>(std::get<0>(entry).c_str())
                    , price
					, iter->second.avaliable
                    , result
					, error_info); 
         
            // judge result 
            if( strlen(error_info) > 0 )
            {
                auto ret_str = new std::string(utility::FormatStr("error %d 清仓卖出 %s %.2f %d 失败:%s"
                    , task->task_id(), std::get<0>(entry).c_str(), price, iter->second.avaliable, error_info));

                this->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
                this->AppendLog2Ui(ret_str->c_str());
                this->EmitSigShowUi(ret_str, true);
                  
            }else
            {
                auto ret_str = new std::string(utility::FormatStr("%d 清仓卖出 %s %.2f %d 成功:%s"
                    , task->task_id(), std::get<0>(entry).c_str(), price, iter->second.avaliable, error_info));
                this->EmitSigShowUi(ret_str, true);
            } 
#endif  
        }); // for each

    }); // post task

    
    return true;
}

T_CodeMapProfit  WinnerApp::CalcProfit()
{
	T_CodeMapProfit  code_profit;
	auto records_for_calprofit = db_moudle().LoadFillRecordsForCalProfit(user_info().id);
	char *stock[64] = {0};
	int i = 0;
	std::for_each( std::begin(records_for_calprofit), std::end(records_for_calprofit), [&i, &stock](T_CodeMapFills::reference entry)
	{
		stock[i++] = const_cast<char*>(entry.first.c_str());
	});
	TCodeMapQuotesData  stock_quotes;
	stock_ticker().GetQuoteDatas(stock, i, stock_quotes);

	auto positions = QueryPosition();
     
    std::for_each( std::begin(records_for_calprofit), std::end(records_for_calprofit), [&code_profit, &positions, &stock_quotes, this](T_CodeMapFills::reference entry)
    {
		auto iter = stock_quotes.find(entry.first);
		if( iter == stock_quotes.end() )
		{
			// log_error:
			return; 
		}
		double cur_price = iter->second->cur_price;
		auto pos_iter  = positions.find(entry.first);
        if( pos_iter == positions.end() )
		{ // log_error:
			return; 
		}
		 
        double input_amount = 0.0;
        double mid_get_amount = 0.0;
        std::for_each( std::begin(entry.second), std::end(entry.second), [&input_amount, &mid_get_amount, &entry, this](std::shared_ptr<T_FillItem>& in)
        {
            if( in->is_buy )
            {
                input_amount += in->amount + in->fee;
            }else
            {
                mid_get_amount += in->amount - in->fee;
            }
        });
		double market_value = cur_price * pos_iter->second.total;
        double profit = input_amount < 0.000001 ? 0.0 : (market_value + mid_get_amount - input_amount);
		double profit_percent = input_amount < 0.000001 ? 0.0 : (profit * 100 / input_amount);

		// 计算公式：成本价=（买入金额-盈亏金额）/持股股数
		double cost_price = 0.0;
		if( pos_iter->second.total > 0 )
			cost_price = (input_amount - profit) / pos_iter->second.total;
		
		auto item = code_profit.insert( std::make_pair(entry.first, T_PROFIT()) ).first;  
		item->second.stock = entry.first;
		item->second.market_value = market_value;
        item->second.cur_price = cur_price;
        item->second.cost_price = cost_price;
		item->second.profit = profit;
		item->second.profit_percent = profit_percent;

    });

	return code_profit;
      
}

void WinnerApp::StopAllStockTasks()
{
    ticker_strand().PostTask([this]()
	{
		this->stock_ticker().ClearAllTask(); 

		std::for_each( std::begin(strategy_tasks_), std::end(strategy_tasks_), [this](std::shared_ptr<StrategyTask> entry)
		{
            if( entry->task_info().type != TypeTask::INDEX_RISKMAN )
            {
			    entry->SetOriginalState(TaskCurrentState::STOP);
			    db_moudle().UpdateTaskInfo(entry->task_info());
			    this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
            } 
		});
	}); 
}

void WinnerApp::StopAllIndexRelTypeTasks(TindexTaskType type_task)
{
    ticker_strand().PostTask([this, &type_task]()
	{
        this->index_ticker().ClearAllTask(); 

		std::for_each( std::begin(strategy_tasks_), std::end(strategy_tasks_), [this, &type_task](std::shared_ptr<StrategyTask> entry)
		{
            if( entry->task_info().type == TypeTask::INDEX_RISKMAN && entry->task_info().index_rel_task.rel_type == type_task )
            {
                //entry->task_info().index_rel_task.rel_type == TindexTaskType::RELSTOCK;

			    entry->SetOriginalState(TaskCurrentState::STOP);
			    db_moudle().UpdateTaskInfo(entry->task_info());
			    this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
            } 
		});
	}); 
}

bool SetCurrentEnvPath()  
{  
	char chBuf[0x8000]={0};  
	DWORD dwSize = GetEnvironmentVariable("path", chBuf, 0x10000);  
	std::string strEnvPaths(chBuf);  

	if(!::GetModuleFileName(NULL, chBuf, MAX_PATH))  
		return false;   
	std::string strAppPath(chBuf);  

	auto nPos = strAppPath.rfind('\\');
	if( nPos > 0 )
	{   
		strAppPath = strAppPath.substr(0, nPos+1);  
	}  

	strEnvPaths += ";" + strAppPath +";";  
	bool bRet = SetEnvironmentVariable("path",strEnvPaths.c_str());  

	return bRet;  
}  

// ps: make sure date_end > date_begin
std::vector<int> WinnerApp::GetSpanTradeDates(int date_begin, int date_end)
{
    std::vector<int> ret_days;

    const int year_begin = date_begin / 10000;
    int mon_begin = (date_begin % 10000) / 100;
    int day_begin = (date_begin % 100);
    const int year_end = date_end / 10000;
    const int mon_end = (date_end % 10000) / 100;
    const int day_end = (date_end % 100);
     
    std::array<int, 12> legal_mon = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    std::array<int, 31> legal_day = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10
                                     , 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
                                     , 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
 
    for( int y = year_begin; y <= year_end; ++y )
    {
        for(auto m : legal_mon)
        {
            for( auto d : legal_day )
            {
                int local_date = y * 10000 + m * 100 + d;
                if( local_date >= date_begin && local_date <= date_end 
                    && exchange_calendar_.IsTradeDate(local_date) )
                    ret_days.push_back(local_date);
            } 
        }
    }
    return ret_days;
}

void WinnerApp::UpdatePositionMock()
{
    auto today = TSystem::Today();
	//if( IsDateChange() )
	{
		//UpdateLatestDateTag(today);
		if( exchange_calendar_.IsTradeDate(today) )
			position_mocker_->DoEnterNewTradeDate(today);
		else
        {
			position_mocker_->UnFreezePosition();
        }
	}
}

bool WinnerApp::IsDateChange()
{ 
	return TSystem::Today() != cookie_.latest_date_tag();
}

void WinnerApp::UpdateLatestDateTag(int date)
{
	cookie_.latest_date_tag(date);
}