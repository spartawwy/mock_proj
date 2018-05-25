#ifndef POSITION_MOCKER_SDFD_H_
#define POSITION_MOCKER_SDFD_H_

#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

#include <memory>
#include <unordered_map>

#include "common_base.h"
//#include "db_moudle.h"
//typedef std::unordered_map<std::string, T_PositionData> T_CodeMapPosition;
//typedef std::unordered_map<std::string, int> T_CodeMapPosition;
typedef std::unordered_map<int, T_CodeMapPosition> T_DayMapPosition;

//class DBMoudle;
class ExchangeCalendar;
class PositionMocker
{
public: 
    friend class DBMoudle;

    PositionMocker(int user_id, DBMoudle *db_moudle, ExchangeCalendar *exchange_calendar);

    void DoEnterNewTradeDate(int date);
    void UpdatePosition(const std::string &code, double avaliable, double frozon);
    void UnFreezePosition();

    T_PositionData * ReadPosition(int date, const std::string& code);
    void AddTotalPosition(int date, const std::string& code, double val, bool is_freeze);
    bool SubAvaliablePosition(int date, const std::string& code, double val);

    
private:

    int user_id_;
    DBMoudle  *db_moudle_;
    ExchangeCalendar  *exchange_calendar_;

    T_DayMapPosition  days_positions_; // ps: make big enough other wise p_cur_capital_ point to wild pointer 
    T_PositionData *p_cur_capital_;

    typedef boost::shared_mutex            WRMutex;  
	typedef boost::unique_lock<WRMutex>    WriteLock;  
	typedef boost::shared_lock<WRMutex>    ReadLock;   
    WRMutex  pos_mock_mutex_;

    int last_position_date_;
     
};
#endif // POSITION_MOCKER_SDFD_H_