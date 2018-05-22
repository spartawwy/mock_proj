#ifndef POSITION_MOCKER_SDFD_H_
#define POSITION_MOCKER_SDFD_H_

#include <memory>
#include <unordered_map>

#include "common_base.h"
//#include "db_moudle.h"
//typedef std::unordered_map<std::string, T_PositionData> T_CodeMapPosition;
//typedef std::unordered_map<std::string, int> T_CodeMapPosition;
typedef std::unordered_map<int, T_CodeMapPosition> T_DayMapPosition;

//class DBMoudle;
class PositionMocker
{
public: 
    friend class DBMoudle;

    PositionMocker(DBMoudle *db_moudle
        , const T_DateMapIsopen &trd_dates);
    void Update();
    void UpdatePosition(const std::string &code, double avaliable, double frozon);

private:

    DBMoudle *db_moudle_;
    const T_DateMapIsopen &trade_dates_;
    T_DayMapPosition  days_positions_;

    
};
#endif // POSITION_MOCKER_SDFD_H_