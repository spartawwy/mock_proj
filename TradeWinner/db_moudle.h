#ifndef DBMOULDE_H_DSFWDSF_
#define DBMOULDE_H_DSFWDSF_

#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

#include <string>
#include <atomic>
#include <unordered_map>

#include "common.h"

#include "position_mocker.h"

namespace SQLite
{
    class SQLiteConnection;
}

namespace TSystem
{
    class TaskStrand;
}

//class PositionMocker;
class WinnerApp;

class DBMoudle
{
public:

    DBMoudle(WinnerApp *app);
    ~DBMoudle();

    void Init();

    void LoadAllUserBrokerInfo();
    void LoadAllTaskInfo(std::unordered_map<int, std::shared_ptr<T_TaskInformation> > &taskinfos);
    void LoadTradeDate(void *exchange_calendar);

    T_UserAccountInfo * FindUserAccountInfo(int user_id);
    T_BrokerInfo * FindUserBrokerByUser(int user_id);
    T_BrokerInfo * FindUserBrokerByBroker(int id);
    std::shared_ptr<T_UserAccountInfo> FindAccountInfoByAccNoAndBrokerId(const std::string& accno, int broker_id);
    std::vector<T_BrokerInfo> GetAllBrokerInfo();
     

    bool SaveUserinformation(T_UserInformation &info);
    bool AddAccountInfo(T_AccountInformation &info);
    bool UpdateAccountInfo(T_AccountInformation &info);
    int FindBorkerIdByAccountID(int account_id);

    bool AddTaskInfo(std::shared_ptr<T_TaskInformation> &info);
    bool DelTaskInfo(int task_id, TypeTask type);
    bool UpdateTaskInfo(T_TaskInformation &info);

	void UpdateEqualSection(int taskid, bool is_original, double start_price);
	void UpdateAdvanceSection(T_TaskInformation &info);

    bool AddHisTask(std::shared_ptr<T_TaskInformation>& info);

    bool IsTaskExists(int user_id, TypeTask type, const std::string& stock);

    // ret: > 0 -- userid, else fail
    int CheckLogin(const std::string& name, const std::string& pwd, T_UserInformation *user_info=nullptr);
     
    void GetStockCode(const std::string &code, std::vector<T_StockCodeName>& ret);
    std::string GetStockName(const std::string &code);
      

    //void LoadExchangeCalendar(ExchangeCalendar * calendar);
    void LoadPositionMock(PositionMocker &position_mock);
    void UpdatePositionMock(PositionMocker &position_mock, int date, int user_id);
    void UpdateOnePositionMock(PositionMocker &position_mock, const std::string &code, int date, int user_id);
    void AddOnePositionMock(PositionMocker &position_mock, const std::string &code, int date, int user_id);
    void ResetPositionMock(int user_id);

    void AddFillRecord(T_FillItem& fill_item);
    void DelAllFillRecord();
	std::list<std::shared_ptr<T_FillItem> > LoadAllFillRecord(int user_id);

private:

    DBMoudle(DBMoudle&);
    DBMoudle& operator = (DBMoudle&);
     
    void Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn, const std::string db_file);

    //TSystem::LocalLogger *local_logger_;
    WinnerApp *app_;
    std::shared_ptr<SQLite::SQLiteConnection>  db_conn_;
    std::shared_ptr<SQLite::SQLiteConnection>  exchange_db_conn_;

    std::shared_ptr<TSystem::TaskStrand> strand_;

    typedef boost::shared_mutex            WRMutex;  
	typedef boost::unique_lock<WRMutex>    WriteLock;  
	typedef boost::shared_lock<WRMutex>    ReadLock;  
	WRMutex  taskinfo_table_mutex_;
	WRMutex  equalsection_table_mutex_;
	WRMutex  index_rel_table_mutex_;
    // (broker id, broker info)
    std::unordered_map<int, T_BrokerInfo>  broker_info_map_;
    // (user id, account info) 
    std::unordered_map<int, T_UserAccountInfo> user_account_info_map_;

    int max_accoun_id_;
	//std::atomic_uint64_t max_task_id_;

    
};

#endif