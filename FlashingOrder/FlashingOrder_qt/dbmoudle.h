#ifndef DB_MOUDLE_H_SDF32DSFD_
#define DB_MOUDLE_H_SDF32DSFD_
 
#include <memory>

class QSqlDatabase;
class FlashingOrder;

class DBMoudle
{
public:
    DBMoudle(FlashingOrder *app);
    ~DBMoudle(){}

    bool Init(int broker_id, int account_id);
	 
	bool LoadBrokerByBrokerId(int broker_id);
	bool LoadAccountInfoByAccountId(int account_id);

private:
    std::shared_ptr<QSqlDatabase> p_database_;
	FlashingOrder *app_;
};

#endif //DB_MOUDLE_H_SDF32DSFD_

