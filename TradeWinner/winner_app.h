/* write by wwy.
   for one trader
*/
#ifndef WINNER_APP_H_SD3SDFDS
#define WINNER_APP_H_SD3SDFDS
 
#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

#include <TLib/tool/tsystem_server_client_appbase.h>
 
#include <QApplication> 
#include "login_win.h"
#include "winner_win.h"
#include "trade_agent.h"
#include "db_moudle.h"
#include "message_win.h"

#include "stk_quoter_api.h"
  
#include "cookie.h"

using namespace TSystem;
 
typedef std::unordered_map<std::string, T_PositionData> T_CodeMapPosition;

class StockTicker;
class IndexTicker;
class StrategyTask; 
class IndexTask;

class WinnerApp : public QApplication, public TSystem::ServerClientAppBase
{
    Q_OBJECT

public:

    // (task id , task info)
    typedef std::unordered_map<int, std::shared_ptr<T_TaskInformation> >  T_IdMapTaskInfo;
     
    WinnerApp(int argc, char* argv[]); 
    ~WinnerApp();

    MessageWin& msg_win() { assert(msg_win_); return *msg_win_; }
    MessageWin& msgwin_longshow() { assert(msgwin_longshow_); return *msgwin_longshow_; }

    bool Init();

    virtual void HandleNodeHandShake(communication::Connection* p, const Message& msg) override {};
	virtual void HandleNodeDisconnect(std::shared_ptr<communication::Connection>& pconn
			, const TError& te) override {};

    void Stop();

    TaskStrand& ticker_strand() { return tick_strand_;} 
    TaskStrand& index_tick_strand() { return index_tick_strand_; }

    TaskStrand& trade_strand() { return trade_strand_; }

    TradeAgent& trade_agent() { return trade_agent_; }

    int trade_client_id() { return trade_client_id_; }
    void RemoveTask(unsigned int task_id, TypeTask task_type);

    int Cookie_NextTaskId();
    int Cookie_MaxTaskId();
    void Cookie_MaxTaskId(int task_id);

    DBMoudle& db_moudle() { return db_moudle_; }
    void user_info(const T_UserInformation& val) { user_info_ = val; }
    T_UserInformation& user_info() { return user_info_; }
	 
    bool LoginBroker(int broker_id, int depart_id, const std::string& account, const std::string& password);
    
    StockTicker& stock_ticker() { return *stock_ticker_; }
    IndexTicker& index_ticker() { return *index_ticker_; }

    T_UserAccountInfo *user_account_info() { return p_user_account_info_; }
    T_BrokerInfo *user_broker_info() { return p_user_broker_info_; }
    void user_broker_info(T_BrokerInfo* p_val) { p_user_broker_info_ = p_val; }

    void AppendTaskInfo(int, std::shared_ptr<T_TaskInformation>& info);
    void AppendStrategyTask(std::shared_ptr<StrategyTask> &task);

    std::shared_ptr<T_TaskInformation> FindTaskInfo(int task_id);

    T_IdMapTaskInfo & task_infos() { return task_infos_; }

    bool DelTaskById(int task_id, TypeTask task_type);
    std::shared_ptr<StrategyTask> FindStrategyTask(int task_id);

    void Emit(StrategyTask* p, int type) { emit SigTaskStatChange(p, type); }
    void EmitSigRemoveTask(int id) { emit SigRemoveTask(id);}
    // memory will be delete by this function 
    void EmitSigShowUi(std::string *str, bool flash_task_bar=false) { emit SigShowUi(str, flash_task_bar); }
    void EmitSigShowLongUi(std::string *str, bool flash_task_bar=false) { emit SigShowLongUi(str, flash_task_bar); }

    T_PositionData* QueryPosition(const std::string& code);
    T_Capital QueryCapital();
    //std::unordered_map<std::string, int>& stocks_position() { return stocks_position_; }
    T_CodeMapPosition QueryPosition();
    int QueryPosAvaliable_LazyMode(const std::string& code);
    T_PositionData* QueryPosition_LazyMode(const std::string& code);

    void AddPosition(const std::string& code, int pos);
    void SubPosition(const std::string& code, int pos);

	T_StockPriceInfo * GetStockPriceInfo(const std::string& code, bool is_lazy=true);
     
    void AppendLog2Ui(const char *fmt, ...);
 
    bool SellAllPosition(IndexTask * task);   

signals:

    //(StrategyTask*, change kind)
    void SigTaskStatChange(StrategyTask*, int);
    void SigAppendLog(char*);
    void SigRemoveTask(int);
    //void SigShowUi(std::shared_ptr<std::string>); //cause can't invoke so use raw point
    void SigShowUi(std::string*, bool);
    void SigShowLongUi(std::string*, bool);

public slots:

    void SlotStopAllTasks(bool);    

private slots:

    void DoStrategyTasksTimeout();
    void DoNormalTimer();
    //void DoShowUi(std::shared_ptr<std::string>);
    void DoShowUi(std::string*, bool flash_taskbar = false);
    void DoShowLongUi(std::string*, bool flash_taskbar = false);
      
private:
	 
    void StopAllStockTasks(); 
    void StopAllIndexRelTypeTasks(TindexTaskType type); 

    //QApplication  *qt_app_;
    TaskStrand  tick_strand_;
    TaskStrand  index_tick_strand_;
   
    TaskStrand  trade_strand_;
   
    std::shared_ptr<StockTicker>  stock_ticker_;
    std::shared_ptr<IndexTicker>  index_ticker_;

    int stock_ticker_life_count_;
    int index_ticker_life_count_;

    bool ticker_enable_flag_;

    TradeAgent  trade_agent_;

    // StrategyTask is create base on task info
    std::list<std::shared_ptr<StrategyTask> >  strategy_tasks_;
    typedef boost::shared_mutex            WRMutex;  
	typedef boost::unique_lock<WRMutex>    WriteLock;  
	typedef boost::shared_lock<WRMutex>    ReadLock;  
	WRMutex  strategy_tasks_mutex_;  

    WRMutex  task_infos_mutex_;  
    // (task_id, task_info)   // just insert but never erase
    T_IdMapTaskInfo task_infos_;
	 
    MessageWin  *msg_win_;
    MessageWin  *msgwin_longshow_;
    LoginWin  login_win_;
    WinnerWin  winner_win_;
    bool  exit_flag_;

    T_UserInformation  user_info_;

    int trade_client_id_;

    std::mutex cookie_mutex_;
    Cookie   cookie_;
    DBMoudle db_moudle_;

    std::shared_ptr<QTimer>  strategy_tasks_timer_;
    std::shared_ptr<QTimer>  normal_timer_;

    T_CodeMapPosition stocks_position_;
    std::mutex  stocks_position_mutex_;
	  
	std::unordered_map<std::string, T_StockPriceInfo> stocks_price_info_;
     
    T_UserAccountInfo *p_user_account_info_;
    T_BrokerInfo *p_user_broker_info_;
      

    friend class IndexTask;
};
#endif