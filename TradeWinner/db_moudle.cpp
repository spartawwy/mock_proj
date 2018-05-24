#include "db_moudle.h"

#include <boost/lexical_cast.hpp>
#include <cctype>
#include <memory>

#include <SQLite/sqlite_connection.h>
#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_sqlite_functions.h>
 
#include "exchange_calendar.h"
#include "position_mocker.h"
#include "winner_app.h"


/*CREATE TABLE BrokerInfo (id INTEGER, ip TEXT, port INTEGER, type INTEGER, remark TEXT, com_ver text, PRIMARY KEY(id));
CREATE TABLE AccountInfo(id INTEGER, account_no TEXT, trade_account_no TEXT, trade_pwd  TEXT, comm_pwd TEXT, broker_id INTEGER, department_id text, remark TEXT, PRIMARY KEY(id));
CREATE TABLE UserInformation(  id INTEGER
    , level INTEGER
    , name TEXT UNIQUE
    , nick_name TEXT
    , password TEXT
    , account_id INTEGER
    , remark TEXT, PRIMARY KEY(id) );
CREATE TABLE Department(no integer, broker_type integer, name text, primary key(no, broker_type));
CREATE TABLE stock(code text not null, name text, pinyin text, temp_code text, remark text, primary key(code));
CREATE TABLE TaskInfo( id INTEGER,
                 type INTEGER,
                 user_id INTEGER,
                stock  TEXT not null,
                alert_price  DOUBLE,
                back_alert_trigger  BOOL,
                rebounce  DOUBLE,
                continue_second   INTEGER,
                step  DOUBLE,
                quantity  INTEGER,
                target_price_level INTEGER,
                start_time   INTEGER,
                end_time     INTEGER,
                is_loop      BOOL,
                state        INTEGER,
                stock_pinyin TEXT,
                bs_times     INTEGER,
                assistant_field  TEXT,
                PRIMARY KEY(id));
CREATE TABLE OrderMessage( longdate INTEGER, timestamp TEXT, msg_type TEXT, msg_id INTEGER, user_id INTEGER,
                stock  TEXT not null,
                alert_price  DOUBLE,
                back_alert_trigger  BOOL,
                rebounce  DOUBLE,
                top_or_buttom_price DOUBLE,
                continue_second   INTEGER,
                step  DOUBLE,
                quantity  INTEGER,
                target_price_level INTEGER);
CREATE TABLE HisTask( id INTEGER,
                longdate INTEGER,
                timestamp TEXT,
                type INTEGER,
                user_id INTEGER,
                stock  TEXT not null,
                alert_price  DOUBLE,
                back_alert_trigger  BOOL,
                rebounce  DOUBLE,
                continue_second   INTEGER,
                step  DOUBLE,
                quantity  INTEGER,
                target_price_level INTEGER,
                start_time   INTEGER,
                end_time     INTEGER,
                is_loop      BOOL,
                state        INTEGER,
                stock_pinyin TEXT,
                bs_times     INTEGER,
                PRIMARY KEY(id));
				
CREATE TABLE EqualSectionTask(id INTEGER, 
								raise_percent DOUBLE, fall_percent DOUBLE, 
								raise_infection DOUBLE, fall_infection DOUBLE,  
								multi_qty INTEGER, 
								max_trig_price DOUBLE, min_trig_price DOUBLE,
								is_original BOOL,
                                max_position INTEGER, min_position INTEGER,
								PRIMARY KEY(id));

CREATE TABLE IndexRelateTask(id INTEGER, index_task_type INTEGER, relate_stock TEXT, is_down_trigger bool, is_buy bool,  PRIMARY KEY(id));

// portion_sections seperated by ';' , portion_states also 
CREATE TABLE AdvanceSectionTask(id INTEGER,
                                portion_sections TEXT,
                                portion_states TEXT,
                                pre_trade_price DOUBLE,
                                is_original BOOL,
                                PRIMARY KEY(id));

CREATE TABLE CapitalMock(id INTEGER, avaliable DOUBLE);
CREATE TABLE PositionMock(stock TEXT not null, avaiable INTEGER, frozen INTEGER, updata_date INTEGER, PRIMARY KEY(stock));
 */

static std::string db_file_path = "./pzwj.kd";
static std::string exchane_db_file_path = "./exchbase.kd";

using namespace  TSystem;
DBMoudle::DBMoudle(WinnerApp *app)
    : app_(app)
    , db_conn_(nullptr)
    , exchange_db_conn_(nullptr)
    , strand_(std::make_shared<TSystem::TaskStrand>(app->task_pool()))
    , max_accoun_id_(1) 
{

}

DBMoudle::~DBMoudle()
{

}

void DBMoudle::Init()
{
    if( !db_conn_ )
		Open(db_conn_, db_file_path);
    if( !exchange_db_conn_ )
        Open(exchange_db_conn_, exchane_db_file_path);

    if( !utility::ExistTable("accountInfo", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::Init"
                , "can't find table accountInfo");

     std::string sql = "SELECT max(id) FROM AccountInfo ";
     db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
     {
         try
         {
           this->max_accoun_id_ = boost::lexical_cast<int>(*(vals));
         }catch(boost::exception& )
         {
            return 0;
         }  
         return 0;
     });

     sql = "SELECT max(id) FROM TaskInfo ";
     db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
     {
		 if( !(*vals) )
			 return 0;
         try
         {
             //this->max_task_id_ = boost::lexical_cast<int>(*(vals));
             int max_tsk_id = boost::lexical_cast<int>(*(vals));
             if( max_tsk_id > app_->Cookie_MaxTaskId() )
                 app_->Cookie_MaxTaskId(max_tsk_id);

         }catch(boost::exception& )
         {
            return 0;
         }  
         return 0;
     });
     
    if( !utility::ExistTable("ExchangeDate", *exchange_db_conn_) )
         ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
         , "DBMoudle::Init"
         , "can't find table ExchangeDate");
}

void DBMoudle::LoadAllUserBrokerInfo()
{
    assert( user_account_info_map_.size() <= 0 );
     
    std::shared_ptr<SQLite::SQLiteConnection> db_conn = nullptr;
    Open(db_conn, db_file_path);

    if( !utility::ExistTable("UserInformation", *db_conn) || !utility::ExistTable("accountInfo", *db_conn) || !utility::ExistTable("brokerInfo", *db_conn) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::LoadInformation"
                , "can't find table UserInformation or accountInfo or brokerInfo");

     std::string sql = "SELECT id, type, ip, port, com_ver, remark FROM brokerInfo ORDER BY id ";
     db_conn->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
     {
         T_BrokerInfo  broker_info;
         try
         {
         broker_info.id = boost::lexical_cast<int>(*(vals));
         broker_info.type = static_cast<TypeBroker>(boost::lexical_cast<int>(*(vals + 1)));
         broker_info.ip = *(vals + 2);
         broker_info.port = boost::lexical_cast<int>(*(vals + 3));
         broker_info.com_ver = *(vals + 4);
         broker_info.remark = *(vals + 5);
         }catch(boost::exception& )
         {
            return 0;
         } 
         broker_info_map_.insert(std::make_pair(broker_info.id, std::move(broker_info)));
         return 0;
     });
      
     // SELECT every user 's current account information and broker information ----------
     sql = "SELECT u.level, a.broker_id, a.account_no, a.trade_account_no, a.trade_pwd, a.comm_pwd, a.department_id, a.id, u.id "
     " FROM UserInformation u INNER JOIN accountInfo a INNER JOIN brokerInfo b ON u.account_id=a.id AND a.broker_id=b.id ORDER BY u.id";
    
    db_conn->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
    {
        T_UserAccountInfo  user_account_info;
        int user_id = 0;
        try
        {
            user_account_info.level_ = boost::lexical_cast<int>(*vals);
            user_account_info.broker_id_ = boost::lexical_cast<int>(*(vals + 1));
            user_account_info.account_id = boost::lexical_cast<int>(*(vals + 7));
            user_id = boost::lexical_cast<int>(*(vals + 8));
        }catch(boost::exception& )
        {
            return 0;
        } 
        user_account_info.account_no_in_broker_ = *(vals + 2);
        user_account_info.trade_no_in_broker_ = *(vals + 3);
        user_account_info.trade_pwd_ = *(vals + 4);
        user_account_info.comm_pwd_ = *(vals + 5);
        user_account_info.department_id_ = *(vals + 6);
        
        
        this->user_account_info_map_.insert( std::make_pair(user_id, std::move(user_account_info)) );
        return 0;
    });
}

void DBMoudle::LoadAllTaskInfo(std::unordered_map<int, std::shared_ptr<T_TaskInformation> > &taskinfos)
{
    if( !db_conn_ )
       Open(db_conn_, db_file_path);

    if( !utility::ExistTable("TaskInfo", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::LoadAllTaskInfo"
                , "can't find table TaskInfo: ");

    std::string sql  = utility::FormatStr("SELECT id, type, stock, alert_price, back_alert_trigger, rebounce, continue_second, step, quantity, target_price_level, start_time, end_time, is_loop, state, user_id, stock_pinyin, bs_times, assistant_field"
        " FROM TaskInfo WHERE user_id=%d AND type NOT IN (%d, %d) order by id ", app_->user_info().id, (int)TypeTask::EQUAL_SECTION, (int)TypeTask::INDEX_RISKMAN);
    
    //std::make_shared<std::string>();
    db_conn_->ExecuteSQL(sql.c_str(),[&taskinfos, this](int num_cols, char** vals, char** names)->int
    {
        auto task_info = std::make_shared<T_TaskInformation>();
        
        try
        {
            task_info->id = boost::lexical_cast<int>(*(vals));
           	task_info->type = static_cast<TypeTask>(boost::lexical_cast<int>(*(vals + 1)));
            task_info->stock = *(vals + 2);
            if( task_info->stock.length() < 6 )
            {
                app_->local_logger().LogLocal("error", utility::FormatStr("task %d stock %s", task_info->id, task_info->stock.c_str()));
                return 0;
            }
            task_info->alert_price = boost::lexical_cast<double>(*(vals + 3));
            task_info->back_alert_trigger = boost::lexical_cast<bool>(*(vals + 4));
			task_info->rebounce = boost::lexical_cast<double>(*(vals + 5));
            task_info->continue_second = boost::lexical_cast<int>(*(vals + 6));
			task_info->step = boost::lexical_cast<double>(*(vals + 7));
            task_info->quantity = boost::lexical_cast<int>(*(vals + 8));
            if( task_info->quantity <= 0 || task_info->quantity % 100 != 0 )
            {
                app_->local_logger().LogLocal("error", utility::FormatStr("task %d quantity %d", task_info->id, task_info->quantity));
                return 0;
            }
            task_info->target_price_level = boost::lexical_cast<int>(*(vals + 9));
            task_info->start_time = boost::lexical_cast<int>(*(vals + 10));
            task_info->end_time = boost::lexical_cast<int>(*(vals + 11));
            task_info->is_loop = boost::lexical_cast<int>(*(vals + 12));
            task_info->state = boost::lexical_cast<int>(*(vals + 13)); 
			task_info->stock_pinyin = *(vals + 15); 
            utf8ToGbk(task_info->stock_pinyin);
			task_info->bs_times = boost::lexical_cast<int>(*(vals + 16));
			//if( task_info->type == TypeTask::BATCHES_BUY || task_info->type == TypeTask::BATCHES_SELL )
            task_info->assistant_field = *(vals + 17);

        }catch(boost::exception& )
        {
            return 0;
        }
        taskinfos.insert( std::make_pair(task_info->id, std::move(task_info)) );

        return 0;
    });

	// equal section task
	sql = utility::FormatStr("SELECT t.id, t.type, t.stock, t.stock_pinyin, t.alert_price, t.back_alert_trigger, t.rebounce, t.continue_second, "
		" t.quantity, t.target_price_level, t.start_time, t.end_time, t.state, t.user_id, "
		" e.raise_percent, e.fall_percent, e.raise_infection, e.fall_infection, e.multi_qty, e.max_trig_price, e.min_trig_price, e.is_original, t.assistant_field, e.max_position, e.min_position "
        " FROM TaskInfo t INNER JOIN EqualSectionTask e ON t.id=e.id WHERE t.user_id=%d order by t.id ", app_->user_info().id);
    db_conn_->ExecuteSQL(sql.c_str(),[&taskinfos, this](int num_cols, char** vals, char** names)->int
    {
        auto task_info = std::make_shared<T_TaskInformation>();
        
        try
        {
            task_info->id = boost::lexical_cast<int>(*(vals));
           	task_info->type = static_cast<TypeTask>(boost::lexical_cast<int>(*(vals + 1)));
            task_info->stock = *(vals + 2);
            if( task_info->stock.length() < 6 )
            {
                app_->local_logger().LogLocal("error", utility::FormatStr("task %d stock %s", task_info->id, task_info->stock.c_str()));
                return 0;
            }
            
			task_info->stock_pinyin = *(vals + 3);
            utf8ToGbk(task_info->stock_pinyin);
            task_info->alert_price = boost::lexical_cast<double>(*(vals + 4));
			task_info->back_alert_trigger = boost::lexical_cast<bool>(*(vals + 5)); 
			task_info->rebounce = boost::lexical_cast<double>(*(vals + 6));
            task_info->continue_second = boost::lexical_cast<int>(*(vals + 7)); 
            task_info->quantity = boost::lexical_cast<int>(*(vals + 8));
            if( task_info->quantity <= 0 || task_info->quantity % 100 != 0 )
            {
                app_->local_logger().LogLocal("error", utility::FormatStr("task %d quantity %d", task_info->id, task_info->quantity));
                return 0;
            }
            task_info->target_price_level = boost::lexical_cast<int>(*(vals + 9));
            task_info->start_time = boost::lexical_cast<int>(*(vals + 10));
            task_info->end_time = boost::lexical_cast<int>(*(vals + 11)); 
            task_info->state = boost::lexical_cast<int>(*(vals + 12));
            //auto userid = boost::lexical_cast<int>(*(vals + 13));
			  
			task_info->secton_task.raise_percent = boost::lexical_cast<double>(*(vals + 14));
			task_info->secton_task.fall_percent = boost::lexical_cast<double>(*(vals + 15));
			task_info->secton_task.raise_infection = boost::lexical_cast<double>(*(vals + 16));
			task_info->secton_task.fall_infection = boost::lexical_cast<double>(*(vals + 17));
			task_info->secton_task.multi_qty = boost::lexical_cast<int>(*(vals + 18));
			task_info->secton_task.max_trig_price = boost::lexical_cast<double>(*(vals + 19));
			task_info->secton_task.min_trig_price = boost::lexical_cast<double>(*(vals + 20));
			task_info->secton_task.is_original = boost::lexical_cast<bool>(*(vals + 21));
			
            task_info->assistant_field = *(vals + 22);
            task_info->secton_task.max_position = boost::lexical_cast<int>(*(vals + 23));
            task_info->secton_task.min_position = boost::lexical_cast<int>(*(vals + 24));
        }catch(boost::exception& )
        {
            return 0;
        }
        taskinfos.insert( std::make_pair(task_info->id, std::move(task_info)) );
        return 0;
    });

    //IndexRelateTask task --------------- 
        
    sql = utility::FormatStr("SELECT t.id, t.type, t.stock, t.stock_pinyin, t.alert_price, t.continue_second, "
		    " t.quantity, t.target_price_level, t.start_time, t.end_time, t.state, t.user_id, "
		    " i.index_task_type, i.relate_stock, i.is_down_trigger, i.is_buy"
            " FROM TaskInfo t INNER JOIN IndexRelateTask i ON t.id=i.id WHERE t.user_id=%d order by i.id ", app_->user_info().id);
    db_conn_->ExecuteSQL(sql.c_str(),[&taskinfos, this](int num_cols, char** vals, char** names)->int
    {
        auto task_info = std::make_shared<T_TaskInformation>();
		
		task_info->id = boost::lexical_cast<int>(*(vals));
        task_info->type = static_cast<TypeTask>(boost::lexical_cast<int>(*(vals + 1)));
        task_info->stock = *(vals + 2);
        if( task_info->stock.length() < 6 )
        {
            app_->local_logger().LogLocal("error", utility::FormatStr("task %d stock %s", task_info->id, task_info->stock.c_str()));
            return 0;
        }
        task_info->stock_pinyin = *(vals + 3);
        utf8ToGbk(task_info->stock_pinyin);
        task_info->alert_price = boost::lexical_cast<double>(*(vals + 4));  
        task_info->continue_second = boost::lexical_cast<int>(*(vals + 5)); 
        task_info->quantity = boost::lexical_cast<int>(*(vals + 6));
        task_info->target_price_level = boost::lexical_cast<int>(*(vals + 7));
        task_info->start_time = boost::lexical_cast<int>(*(vals + 8));
        task_info->end_time = boost::lexical_cast<int>(*(vals + 9)); 
        task_info->state = boost::lexical_cast<int>(*(vals + 10)); 

        task_info->index_rel_task.rel_type = (TindexTaskType)boost::lexical_cast<int>(*(vals + 12)); 
        task_info->index_rel_task.stock_code = *(vals + 13);
        task_info->index_rel_task.is_down_trigger =  boost::lexical_cast<int>(*(vals + 14)); 
        task_info->index_rel_task.is_buy =  boost::lexical_cast<int>(*(vals + 15)); 
        taskinfos.insert( std::make_pair(task_info->id, std::move(task_info)) );
        return 0;
    });
        
}

void DBMoudle::LoadTradeDate(void *exchange_calendar)
{
    assert(exchange_calendar);
    if( !exchange_db_conn_ )
       Open(exchange_db_conn_, exchane_db_file_path);

    if( !utility::ExistTable("ExchangeDate", *exchange_db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::LoadTradeDate"
                , "can't find table ExchangeDate ");
     
    std::string sql = "SELECT date FROM ExchangeDate WHERE is_tradeday = 1 ORDER BY date";
    int num = 0;
    exchange_db_conn_->ExecuteSQL(sql.c_str(),[&num, &exchange_calendar, this](int num_cols, char** vals, char** names)->int
    { 
        try
        { 
            ++num;
            int date =  boost::lexical_cast<int>(*(vals)); 
            ((ExchangeCalendar*)exchange_calendar)->trade_dates_->insert(std::make_pair(date, true));
         }catch(boost::exception& )
        {
            return 0;
        } 
        return 0;
    }); 
}

T_UserAccountInfo * DBMoudle::FindUserAccountInfo(int user_id)
{
    auto iter = user_account_info_map_.find(user_id);
    if( iter == user_account_info_map_.end() )
        return nullptr;
    return &iter->second;
}
 
T_BrokerInfo * DBMoudle::FindUserBrokerByUser(int user_id)
{
    auto p_account_info = FindUserAccountInfo(user_id);
    if( !p_account_info )
        return nullptr;
    auto iter = broker_info_map_.find(p_account_info->broker_id_);
    if( iter == broker_info_map_.end() )
        return nullptr;
    return std::addressof(iter->second);
}

T_BrokerInfo * DBMoudle::FindUserBrokerByBroker(int id)
{
    auto iter = broker_info_map_.find(id);
    if( iter == broker_info_map_.end() )
        return nullptr;
    return std::addressof(iter->second);
}

std::shared_ptr<T_UserAccountInfo> DBMoudle::FindAccountInfoByAccNoAndBrokerId(const std::string& accno, int broker_id)
{
    std::shared_ptr<T_UserAccountInfo>  p_info = nullptr;
   if( !db_conn_ )
       Open(db_conn_, db_file_path);
    if( !utility::ExistTable("UserInformation", *db_conn_) || !utility::ExistTable("accountinfo", *db_conn_))
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::FindUserAccountInfoByAccNoAndBrokerId"
                , "can't find table UserInformation or accountinfo ");

    /*std::string sql = utility::FormatStr("SELECT u.id, u.level, u.name, u.nick_name, u.password, u.account_id, u.remark "
        " FROM UserInformation u LEFT JOIN accountinfo a ON u.account_id = a.id WHERE a.account_no='%s' and a.broker_id = %d"
        , accno.c_str(), broker_id);*/
    std::string sql = utility::FormatStr("SELECT broker_id, account_no, trade_account_no, trade_pwd, comm_pwd, department_id, id, remark "
        " FROM AccountInfo WHERE account_no='%s' and broker_id = %d"
        , accno.c_str(), broker_id);
    db_conn_->ExecuteSQL(sql.c_str(),[&p_info, this](int num_cols, char** vals, char** names)->int
    {
         p_info = std::make_shared<T_UserAccountInfo>();
         try
         { 
             p_info->broker_id_  = boost::lexical_cast<int>(*(vals));
             p_info->account_no_in_broker_ = *(vals + 1);
             p_info->trade_no_in_broker_ = *(vals + 2);
             p_info->trade_pwd_ = *(vals + 3);
             p_info->comm_pwd_ = *(vals + 4);
             p_info->department_id_  = *(vals + 5);
             p_info->account_id = boost::lexical_cast<int>(*(vals + 6));
         }catch(boost::exception& )
        {
            return 0;
        } 
        return 0;
    });
    return p_info;
}

std::vector<T_BrokerInfo> DBMoudle::GetAllBrokerInfo()
{
    std::vector<T_BrokerInfo> broker_infos;

    std::for_each(std::begin(broker_info_map_), std::end(broker_info_map_), [&broker_infos](std::unordered_map<int, T_BrokerInfo>::reference entry)
    {
        broker_infos.push_back(entry.second);
    });
    std::sort( std::begin(broker_infos), std::end(broker_infos), compare);
    return broker_infos;
}

bool DBMoudle::SaveUserinformation(T_UserInformation &info)
{
    std::string sql = utility::FormatStr("INSERT OR REPLACE INTO UserInformation VALUES( %d, %d, '%s', '%s', '%s', %d, '%s' ) "
        , info.id
        , info.level
        , info.name.c_str()
        , info.nick_name.c_str()
        , info.password.c_str()
        , info.account_id
        , info.remark.c_str());
    std::shared_ptr<SQLite::SQLiteConnection> db_conn = nullptr;
    Open(db_conn, db_file_path);
    if( !utility::ExistTable("UserInformation", *db_conn) )
    {
        // throw exception
        return false; 
    }

    return db_conn->ExecuteSQL(sql.c_str());
}

bool DBMoudle::AddAccountInfo(T_AccountInformation &info)
{
    std::string sql = utility::FormatStr("INSERT OR REPLACE INTO AccountInfo VALUES( %d, '%s', '%s', '%s', '%s', %d, '%s', '%s' ) "
        , this->max_accoun_id_ + 1
        , info.account_no.c_str()
        , info.trade_acc_no.c_str()
        , info.trade_pwd.c_str()
        , info.comm_pwd.c_str()
        , info.broker_id
        , info.department_id.c_str()
        , info.remark.c_str());
    if( !db_conn_ )
	     Open(db_conn_, db_file_path);
    if( !utility::ExistTable("AccountInfo", *db_conn_) )
    {
        // throw exception
        return false; 
    }

    auto ret = db_conn_->ExecuteSQL(sql.c_str());
    if( ret ) 
    {
        ++ this->max_accoun_id_;
        info.id = this->max_accoun_id_;
    }
    return ret;
}

bool DBMoudle::UpdateAccountInfo(T_AccountInformation &info)
{
    std::string sql = 
        utility::FormatStr("UPDATE AccountInfo SET account_no='%s', trade_account_no='%s', trade_pwd='%s'"
                           ", comm_pwd='%s', broker_id=%d, department_id='%s', remark='%s' "
                           "WHERE id = %d"
                            , info.account_no.c_str()
                            , info.trade_acc_no.c_str()
                            , info.trade_pwd.c_str()
                            , info.comm_pwd.c_str()
                            , info.broker_id
                            , info.department_id.c_str()
                            , info.remark.c_str()
                            , info.id);
    if( !db_conn_ )
	    Open(db_conn_, db_file_path);
    if( !utility::ExistTable("AccountInfo", *db_conn_) )
    {
        // throw exception
        return false; 
    }
     
    return db_conn_->ExecuteSQL(sql.c_str());
}

int DBMoudle::FindBorkerIdByAccountID(int account_id)
{
    // assert all broker_id > 0
   if( !db_conn_ )
		Open(db_conn_, db_file_path);
    if( !utility::ExistTable("AccountInfo", *db_conn_) )
    {
        // throw exception
        return 0; 
    }
    std::string sql = utility::FormatStr("SELECT broker_id FROM AccountInfo WHERE id = %d", account_id);
    int broker_id  = 0;
    db_conn_->ExecuteSQL(sql.c_str(),[&broker_id , this](int num_cols, char** vals, char** names)->int
    {
        broker_id = boost::lexical_cast<int>(*vals);
        return 0;
    });
    return broker_id;
}

// info->id will set if saved ok
bool DBMoudle::AddTaskInfo(std::shared_ptr<T_TaskInformation> &info)
{
    auto str_stock_py = info->stock_pinyin;
    gbkToUtf8(str_stock_py);
    std::string sql = utility::FormatStr("INSERT INTO TaskInfo VALUES( %d, %d, %d, '%s', %.2f,  %d, %.2f, %d, %.2f, %d,  %d, %d, %d, %d, %d, '%s', %d, '%s') "
        , app_->Cookie_MaxTaskId() + 1
        , info->type
        , app_->user_info().id
        , info->stock.c_str()
        , info->alert_price
        , info->back_alert_trigger
        , info->rebounce
        , info->continue_second
        , info->step
        , info->quantity
        , info->target_price_level
        , info->start_time
        , info->end_time
        , info->is_loop
        , info->state
		, str_stock_py.c_str()
        , info->bs_times
        , info->assistant_field.c_str());
   if( !db_conn_ )
		Open(db_conn_, db_file_path);
    if( !utility::ExistTable("TaskInfo", *db_conn_) )
    {  // throw exception
        return false; 
    }
    if( info->type == TypeTask::EQUAL_SECTION && !utility::ExistTable("EqualSectionTask", *db_conn_) )
    {  // throw exception
        return false; 
    }else if( info->type == TypeTask::INDEX_RISKMAN && !utility::ExistTable("IndexRelateTask", *db_conn_) )
    {   // throw exception
        return false; 
    }
    bool ret = true;
     
    {
        WriteLock locker(taskinfo_table_mutex_);
        ret = db_conn_->ExecuteSQL(sql.c_str());
    }
    if( ret )
    {
        if( info->type == TypeTask::EQUAL_SECTION )
        {
            sql = utility::FormatStr("INSERT INTO EqualSectionTask VALUES( %d, %.2f, %.2f, %.2f, %.2f, %d, %.2f, %.2f, %d, %d, %d) "
                , app_->Cookie_MaxTaskId() + 1
                , info->secton_task.raise_percent 
                , info->secton_task.fall_percent 
                , info->secton_task.raise_infection 
                , info->secton_task.fall_infection 
                , info->secton_task.multi_qty 
                , info->secton_task.max_trig_price 
                , info->secton_task.min_trig_price 
                , (int)info->secton_task.is_original
                , info->secton_task.max_position
                , info->secton_task.min_position);
             WriteLock locker(equalsection_table_mutex_);
             ret = db_conn_->ExecuteSQL(sql.c_str()); 
        }else if( info->type == TypeTask::INDEX_RISKMAN )
        {
            sql = utility::FormatStr("INSERT INTO IndexRelateTask VALUES( %d, %d, '%s', %d, %d )"
                , app_->Cookie_MaxTaskId() + 1
                , info->index_rel_task.rel_type
                , info->index_rel_task.stock_code.c_str()
                , (int)info->index_rel_task.is_down_trigger
                , (int)info->index_rel_task.is_buy);
            WriteLock locker(index_rel_table_mutex_);
            ret = db_conn_->ExecuteSQL(sql.c_str());  
        }
         
        if( !ret )
        {  
            sql = utility::FormatStr("DELETE FROM TaskInfo WHERE id=%d", app_->Cookie_MaxTaskId() + 1);
            WriteLock locker(taskinfo_table_mutex_);
            db_conn_->ExecuteSQL(sql.c_str());
        }else
            info->id = app_->Cookie_NextTaskId();
    }
    return ret;
}

bool DBMoudle::DelTaskInfo(int task_id, TypeTask type)
{
    if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }

    if( !utility::ExistTable("TaskInfo", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::DelTaskInfo"
                , "can't find table TaskInfo");
     
    std::string sql = utility::FormatStr("DELETE FROM TaskInfo WHERE id=%d ", task_id);
     
    {
        WriteLock locker(taskinfo_table_mutex_);
        db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
        { 
            return 0;
        });
    }
	if( type == TypeTask::EQUAL_SECTION )
	{
		// del related recorde in table EqualSectionTask
		std::string sql = utility::FormatStr("DELETE FROM EqualSectionTask WHERE id=%d ", task_id);
		WriteLock locker(equalsection_table_mutex_);
		db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
        { 
            return 0;
        });
    }else if( type == TypeTask::INDEX_RISKMAN )
	{
		// del related recorde in table IndexRelateTask
		std::string sql = utility::FormatStr("DELETE FROM IndexRelateTask WHERE id=%d ", task_id);
		WriteLock locker(index_rel_table_mutex_);
		db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
        { 
            return 0;
        });
	}
    return true;
}

bool DBMoudle::UpdateTaskInfo(T_TaskInformation &info)
{
    if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }

    if( !utility::ExistTable("TaskInfo", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::UpdateTaskInfo"
                , "can't find table TaskInfo");

    auto str_stock_py = info.stock_pinyin;
    gbkToUtf8(str_stock_py);
    std::string sql = utility::FormatStr("UPDATE TaskInfo SET type=%d, stock='%s', alert_price=%.2f"
        ", back_alert_trigger=%d, rebounce=%.2f, continue_second=%d, step=%.2f, quantity=%d, target_price_level=%d"
        ", start_time=%d, end_time=%d, is_loop=%d, state=%d, stock_pinyin='%s', bs_times=%d, assistant_field='%s' WHERE id=%d "
        , info.type
        , info.stock.c_str()
        , info.alert_price
        , info.back_alert_trigger
        , info.rebounce
        , info.continue_second
        , info.step
        , info.quantity
        , info.target_price_level
        , info.start_time
        , info.end_time
        , info.is_loop
        , info.state
        , str_stock_py.c_str()
        , info.bs_times
        , info.assistant_field.c_str()
         , info.id);
    bool ret = true; 
    {
        WriteLock locker(taskinfo_table_mutex_);
        ret = db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
        { 
            return 0;
        });
    }
    return ret;
}

void DBMoudle::UpdateEqualSection(int taskid, bool is_original, double start_price)
{ 
	if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }

	std::string sql = utility::FormatStr("UPDATE TaskInfo SET assistant_field='%.2f' WHERE id=%d ", start_price, taskid); 
	{
		WriteLock locker(taskinfo_table_mutex_);
		db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
		{ 
			return 0;
		});
	}
	sql = utility::FormatStr("UPDATE EqualSectionTask SET is_original=%d WHERE id=%d ", (int)is_original, taskid); 
	{
		WriteLock locker(equalsection_table_mutex_);
		db_conn_->ExecuteSQL(sql.c_str(),[this](int num_cols, char** vals, char** names)->int
		{ 
			return 0;
		});
	}
}

bool DBMoudle::AddHisTask(std::shared_ptr<T_TaskInformation>& info)
{
    std::tuple<int, std::string> date_time = CurrentDateTime();

    auto str_stock_py = info->stock_pinyin;
    gbkToUtf8(str_stock_py);
    std::string sql = utility::FormatStr("INSERT INTO HisTask VALUES( %d, %d, '%s', %d, %d, '%s', %.2f,  %d, %.2f, %d, %.2f, %d,  %d, %d, %d, %d, %d, '%s', %d) "
        , info->id
        , std::get<0>(date_time)
        , std::get<1>(date_time).c_str()
        , info->type
        , app_->user_info().id
        , info->stock.c_str()
        , info->alert_price
        , info->back_alert_trigger
        , info->rebounce
        , info->continue_second
        , info->step
        , info->quantity
        , info->target_price_level
        , info->start_time
        , info->end_time
        , info->is_loop
        , info->state
		, str_stock_py.c_str()
        , info->bs_times);
    std::shared_ptr<SQLite::SQLiteConnection> db_conn = nullptr;
    Open(db_conn, db_file_path);
    if( !utility::ExistTable("HisTask", *db_conn) )
    {
        // throw exception
        return false; 
    }
    bool ret = db_conn->ExecuteSQL(sql.c_str());
    return ret;
}

bool DBMoudle::IsTaskExists(int user_id, TypeTask type, const std::string& stock)
{
    if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }

    if( !utility::ExistTable("TaskInfo", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::Init"
                , "can't find table TaskInfo");
    bool exists = false;
    std::string sql = utility::FormatStr("SELECT id FROM TaskInfo WHERE user_id=%d AND type=%d AND stock like '%%%s%%'", user_id, int(type), stock.c_str());
    
    ReadLock locker(taskinfo_table_mutex_);
    db_conn_->ExecuteSQL(sql.c_str(),[&exists, this](int num_cols, char** vals, char** names)->int
    {
        exists = true;
        return 0;
    });

    return exists;
}

// user's last updated record has related account_id
int DBMoudle::CheckLogin(const std::string& name, const std::string& pwd, T_UserInformation *user_info)
{
    int ret = -1;
    if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }
    if( name == "test" )
    {
        if( pwd == "123456" )
        {
            user_info->id = USER_ID_TEST;
            user_info->level = 2;
            user_info->name = "test";
            user_info->password = pwd;
            user_info->account_id = 0;
            return 9999;
        }
    }else
        return -1;

    if( !utility::ExistTable("UserInformation", *db_conn_) )
        return ret;

    std::string sql = utility::FormatStr("SELECT u.id, u.level, u.name, u.nick_name, u.password, u.account_id, u.remark, a.account_no FROM UserInformation u LEFT JOIN accountinfo a ON u.account_id = a.id WHERE u.name='%s' AND u.password='%s'", name.c_str(), pwd.c_str());
     
    db_conn_->ExecuteSQL(sql.c_str(),[&, this](int num_cols, char** vals, char** names)->int
    { 
        ret = boost::lexical_cast<int>(*vals);
        if( user_info )
        {
            user_info->id = ret;
            user_info->level = boost::lexical_cast<int>(*(vals+1));
            user_info->name = *(vals+2);
            if( *(vals+3) )
                user_info->nick_name = *(vals+3);
            user_info->password = *(vals+4);
            user_info->account_id = boost::lexical_cast<int>(*(vals+5));
            if( *(vals+6) )
                user_info->remark = *(vals+6);
            if( *(vals+7) )
                user_info->account_no = *(vals+7);
        }
        return 0;
    });
    return ret;
}

void DBMoudle::GetStockCode(const std::string &code, std::vector<T_StockCodeName>& ret)
{
    ret.clear();

    if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }
   
    std::string sql;
    if( IsStrAlpha(code) )
    {
        sql = utility::FormatStr("SELECT code, name from stock WHERE pinyin like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }else if( IsStrNum(code) )
    {
        sql = utility::FormatStr("SELECT code, name from stock WHERE code like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }else
    {
        sql = utility::FormatStr("SELECT code, name from stock WHERE name like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }

    if( !utility::ExistTable("stock", *db_conn_) )
        return;
     
    db_conn_->ExecuteSQL(sql.c_str(),[&ret, this](int num_cols, char** vals, char** names)->int
    { /*
        T_StockCodeName code_name;
        code_name.code = *vals;
        code_name.name = *(vals + 1);*/
        ret.emplace_back(*vals, *(vals + 1));
        return 0;
    });
    return;
}

std::string DBMoudle::GetStockName(const std::string &code_num)
{
    if( !IsStrNum(code_num) )
        return "";

    if( !db_conn_ )
    {
        Open(db_conn_, db_file_path);
    }
   
    std::string name;
    if( !utility::ExistTable("stock", *db_conn_) )
        return "";
    std::string sql = utility::FormatStr("SELECT name from stock WHERE code like '%%%s%%' ", code_num.c_str());

    db_conn_->ExecuteSQL(sql.c_str(),[&name, this](int num_cols, char** vals, char** names)->int
    { 
        name = *vals; 
        return 0;
    });
    return name;

}

void DBMoudle::Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn, const std::string db_file)
{
    db_conn = std::make_shared<SQLite::SQLiteConnection>();

    //std::string db_file = "./pzwj.kd";

    if( db_conn->Open(db_file.c_str(), SQLite::SQLiteConnection::OpenMode::READ_WRITE) != SQLite::SQLiteCode::OK )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::Open"
                , "can't open database: " + db_file);
    
}

 
//
//void DBMoudle::LoadExchangeCalendar(ExchangeCalendar * calendar)
//{
//    assert(calendar); 
//    assert(db_conn_);
//    assert(calendar->calendar_date_.empty());
//
//    if( !utility::ExistTable("ExchangeDate", *db_conn_) )
//        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
//                , "DBMoudle::LoadExchangeCalendar"
//                , "can't find table ExchangeDate: ");
//    std::string sql = utility::FormatStr("SELECT date, is_tradeday, frozen FROM ExchangeDate order by date ");
//
//    db_conn_->ExecuteSQL(sql.c_str(), [calendar, this](int cols, char **vals, char **names)
//    {
//        //T_CalendarDate calendar_date;
//        int date = boost::lexical_cast<int>(*vals);
//        bool is_trd_day = boost::lexical_cast<bool>(*(vals+1));
//        calendar->calendar_date_.emplace_back(date, is_trd_day);
//        return 0;
//    });
//
//}

void DBMoudle::LoadPositionMock(PositionMocker &position_mock)
{
    assert(db_conn_); 
    assert(position_mock.days_positions_.empty()); 
   
    if( !utility::ExistTable("Position", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::LoadPositionMock"
                , "can't find table Position: ");

    auto date_time = CurrentDateTime();
    //// if current position none get pre day position
    std::string sql = utility::FormatStr("SELECT date, code, avaliable, frozen FROM Position WHERE user_id=%d ORDER BY date ", USER_ID_TEST);

    db_conn_->ExecuteSQL(sql.c_str(), [&position_mock, this](int cols, char **vals, char **names)
    {
        position_mock.last_position_date_ = boost::lexical_cast<int>(*vals);
        auto target_iter = position_mock.days_positions_.find(position_mock.last_position_date_);
        if( target_iter == position_mock.days_positions_.end() )
            target_iter = position_mock.days_positions_.insert(std::make_pair(position_mock.last_position_date_, T_CodeMapPosition())).first;
         
        T_PositionData pos_data;
        strcpy_s(pos_data.code, sizeof(pos_data.code), *(vals + 1));
        pos_data.avaliable = boost::lexical_cast<double>(*(vals + 2));
        pos_data.total = pos_data.avaliable + boost::lexical_cast<double>(*(vals + 3));
        target_iter->second.insert(std::make_pair(pos_data.code, pos_data));

        return 0;
    });
}

bool DBMoudle::UpdatePositionMock(PositionMocker &position_mock, int date, int user_id)
{
    assert(db_conn_); 

    auto iter = position_mock.days_positions_.find(date);
    if( iter == position_mock.days_positions_.end() )
        return false;

     if( !utility::ExistTable("Position", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::UpdateTodayPositionMock"
                , "can't find table Position: ");
      
     std::for_each( std::begin(iter->second), std::end(iter->second), [&position_mock, user_id, date, this](T_CodeMapPosition::reference entry)
     { 
         std::string sql = utility::FormatStr("INSERT OR REPLACE INTO Position VALUES(%d, '%s', '%d', %.2f, %.2f)"
             , user_id, entry.second.code, date, (float)entry.second.avaliable, float(entry.second.total-entry.second.avaliable));
         bool ret = this->db_conn_->ExecuteSQL(sql.c_str());
         /*if( ret && date > position_mock.last_position_date_ )
             position_mock.last_position_date_ = date;*/
         return 0; 
     });
     return true;
}

bool DBMoudle::UpdateOneStockInPositionMock(PositionMocker &position_mock, const std::string &code, int date, int user_id)
{
    assert(db_conn_); 

    auto iter = position_mock.days_positions_.find(date);
    if( iter == position_mock.days_positions_.end() )
        return false;

     if( !utility::ExistTable("Position", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
                , "DBMoudle::UpdateOneStockInPositionMock"
                , "can't find table Position: ");
     auto pos_iter = iter->second.find(code);
     if( pos_iter == iter->second.end() )
         return false; 
     std::string sql = utility::FormatStr("INSERT OR REPLACE INTO Position VALUES(%d, '%s', '%d', %.2f, %.2f)"
             , user_id, pos_iter->second.code, date, pos_iter->second.avaliable, pos_iter->second.total - pos_iter->second.avaliable);
     bool ret = this->db_conn_->ExecuteSQL(sql.c_str());
     return ret;
}