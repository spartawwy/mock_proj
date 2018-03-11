#ifndef COMMON_H_SD3DSFSDD
#define COMMON_H_SD3DSFSDD

#include <memory>
#include <vector>
#include <string>
#include <cassert>
#include <tuple>

#include <QObject>
#include <QtCore/qstring.h> 
#include <QTime>

#include "common_base.h"

#ifndef IN
#define  IN
#define  OUT
#define  INOUT
#endif

#define EQSEC_MAX_POSITION 9999900
#define EQSEC_MIN_POSITION 0

#define EQSEC_MAX_STOP_PRICE 999.00
#define EQSEC_MIN_CLEAR_PRICE 0.0

#define MAX_STOCK_PRICE 9999.0
#define MIN_STOCK_PRICE 0.0

#define APP_CODE_TEXT "GBK"

#define DO_LOG(tag, b)  do{ app_->local_logger().LogLocal((tag), b); }while(0);
#define USE_TRADE_FLAG

#if 0
std::vector<std::string> split(const std::string& line, const std::string& seperator = " ", 
		const std::string& quotation = "\"");

std::string&   replace_all(std::string&   str, const   std::string&   old_value, const   std::string&   new_value);
std::string&   replace_all_distinct(std::string&   str, const   std::string&   old_value, const  std::string&   new_value);
#endif

const QString cst_sh_index = "000001";
const char cst_sh_index_name[]  = "上证指数";

const QString cst_sz_compre_index = "399001";
const char cst_sz_compre_index_name[]  = "深圳成指";

const QString cst_entrepren_plate_index = "399006";
const char cst_entrepren_plate_index_name[]  = "创业板指数";

const QString cst_entreplate_compre_index = "399102"; //创业板综合
const char cst_entreplate_compre_index_name[]  = "创业板综指";

 
enum class TypeOrderCategory : char
{
    BUY = 0,
    SELL = 1,
    RQ_BUY,
    RQ_SELL,
    BQ_HQ,
    SQ_HQ,
    CQ_HQ,
};

enum class TypeUserLevel : char
{
    NORMAL = 1,
};

enum class TypeBroker : char
{
    FANG_ZHENG = 1,    //方正
    PING_AN,           //平安
	ZHONG_XIN,          //中信
    ZHONGY_GJ,          //中银国际
	YINGHE_ZQ,         //银河证券
};

enum class TypeTask : char
{
    INFLECTION_BUY = 0, 
    BREAKUP_BUY,
    BATCHES_BUY,

    INFLECTION_SELL,  //3
    BREAK_SELL,
    FOLLOW_SELL,
    BATCHES_SELL,

	EQUAL_SECTION,
    INDEX_RISKMAN, // 8
	ADVANCE_SECTION,
};

enum class TypeQuoteLevel : char
{
    PRICE_CUR = 0,
    PRICE_BUYSELL_1,
    PRICE_BUYSELL_2,
    PRICE_BUYSELL_3,
    PRICE_BUYSELL_4,
    PRICE_BUYSELL_5,
};

enum class TaskStatChangeType : char
{
    CUR_PRICE_CHANGE = 0,
    CUR_STATE_CHANGE,
};

// in database, STOP and WAITING is enough
enum class TaskCurrentState : char
{
    STOP = 0,  // not to run
    WAITTING, // means: not in tasktime, hasn't been register 

    // follow state is used for task which is registered
    STARTING,
    RUNNING,
    REST,     // exchange is in rest time
    EXCEPT,   // exception : in running time but not in running state
};

  
enum class TypeEqSection : char
{
	STOP = 0,
	SELL,
	NOOP,
	BUY,
	CLEAR,
};

struct T_SectionAutom
{
	TypeEqSection  section_type;
	double represent_price;
	T_SectionAutom() : section_type(TypeEqSection::NOOP), represent_price(0.0){}
	T_SectionAutom(TypeEqSection type, double resp_price) : section_type(type), represent_price(resp_price){}
};

struct T_UserInformation
{
    int id;
    int level;
    std::string name;
    std::string nick_name;
    std::string password;
    int account_id;
    std::string account_no;
    std::string remark;
    T_UserInformation() : id(0), level(0), account_id(0){}
    
    T_UserInformation &operator = (const T_UserInformation&lh)
    {
        id = lh.id; level = lh.level; name = lh.name; nick_name = lh.nick_name;
        password = lh.password; account_id = lh.account_id; account_no = lh.account_no; remark = lh.remark;  
        return *this;
    }
};

struct T_AccountInformation
{
    int id;
    std::string account_no;
    std::string trade_acc_no;
    std::string trade_pwd;
    std::string comm_pwd;
    int broker_id; 
    std::string department_id; 
    std::string remark;
    T_AccountInformation() : id(-1), broker_id(-1) {}
};


class  QuotesData
{
public:
    int64_t time_stamp;
	double cur_price;
    double active_degree;
    double price_b_1;
    double price_s_1;
    double price_b_2;
    double price_s_2;
    double price_b_3;
    double price_s_3;
    double price_b_4;
    double price_s_4;
    double price_b_5;
    double price_s_5;
    QuotesData() 
        : time_stamp(0)
        , cur_price(0.0)  
        , active_degree(0.0)
        , price_b_1(0.0)
        , price_s_1(0.0)
        , price_b_2(0.0)
        , price_s_2(0.0)
        , price_b_3(0.0)
        , price_s_3(0.0)
        , price_b_4(0.0)
        , price_s_4(0.0)
        , price_b_5(0.0)
        , price_s_5(0.0)
    {
    }
    ~QuotesData()
    {
    }
};


struct T_BrokerInfo
{
    int         id;
    TypeBroker  type; 
    std::string ip;
    int         port;
    std::string com_ver;
    std::string remark; // name: 方正;  平安 ...
   /* T_BrokerInfo(){}
    T_BrokerInfo(T_BrokerInfo &&val) : id(val.id), type(val.type), ip(std::move(val.ip)), port(val.port), com_ver(std::move(val.com_ver)), remark(std::move(val.remark))
    { 
    }*/
    //bool operator (T_BrokerInfo &lh, T_BrokerInfo &rh) { return lh.id < rh.id; }
    T_BrokerInfo& operator = (T_BrokerInfo& lh)
    {
        id = lh.id; type = lh.type; ip = lh.ip; port = lh.port; com_ver = lh.com_ver; remark = lh.remark;
        return *this;
    }
};

bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh);

struct  T_UserAccountInfo
{
    int level_;
    int account_id;
    int broker_id_;
    //std::string broker_interface_version_;
    std::string account_no_in_broker_;
    std::string trade_no_in_broker_;
    std::string trade_pwd_;
    std::string comm_pwd_;
    std::string department_id_;
    T_UserAccountInfo(): level_(0), account_id(0), broker_id_(0){}
    T_UserAccountInfo(const T_UserAccountInfo &lh) : level_(lh.level_), account_id(lh.account_id), broker_id_(lh.broker_id_), account_no_in_broker_(lh.account_no_in_broker_)
        , trade_no_in_broker_(lh.trade_no_in_broker_), trade_pwd_(lh.trade_pwd_), comm_pwd_(lh.comm_pwd_), department_id_(lh.department_id_){}
    T_UserAccountInfo& operator = (T_UserAccountInfo lh)
    {
        level_ = lh.level_; account_id = lh.account_id; broker_id_ = lh.broker_id_; account_no_in_broker_ = lh.account_no_in_broker_;
        trade_no_in_broker_ = lh.trade_no_in_broker_; trade_pwd_ = lh.trade_pwd_; comm_pwd_ = lh.comm_pwd_; 
        department_id_ = lh.department_id_;
        return *this;
    }
};

struct T_SectionTask
{ 
	double raise_percent;
	double fall_percent;
	double raise_infection;
	double fall_infection; 
	int    multi_qty;
    int    max_position;
    int    min_position;
	double max_trig_price;
	double min_trig_price;
	std::string sections_info; //format: section0_type$section0_price#section1_type$section1_price
	bool is_original;
	T_SectionTask():raise_percent(0.0), fall_percent(0.0), raise_infection(0.0), fall_infection(0.0)
		, multi_qty(0),max_trig_price(EQSEC_MAX_STOP_PRICE), min_trig_price(EQSEC_MIN_CLEAR_PRICE), sections_info(), is_original(true), max_position(EQSEC_MAX_POSITION), min_position(EQSEC_MIN_POSITION){}
    T_SectionTask(const T_SectionTask&lh): raise_percent(lh.raise_percent), fall_percent(lh.fall_percent), raise_infection(lh.raise_infection), fall_infection(lh.fall_infection)
        , multi_qty(lh.multi_qty),max_trig_price(lh.max_trig_price), min_trig_price(lh.min_trig_price), sections_info(lh.sections_info), is_original(lh.is_original), max_position(lh.max_position), min_position(lh.min_position) {}
};

enum class TindexTaskType : char
{
	ALERT,
	RELSTOCK,
	CLEAR,
};

struct T_IndexRelateTask
{ 
	TindexTaskType rel_type; // releate type
	bool is_down_trigger;
	std::string stock_code;  // sub related stock 
	bool is_buy; // if false means sell
	T_IndexRelateTask() : rel_type(TindexTaskType::ALERT), is_down_trigger(true), stock_code(), is_buy(false){}
	T_IndexRelateTask(const T_IndexRelateTask &lh) : rel_type(lh.rel_type), is_down_trigger(lh.is_down_trigger), stock_code(lh.stock_code), is_buy(lh.is_buy){}
};

struct T_AdvanceSectionTask
{
	std::string portion_sections;
	std::string portion_states;
    double pre_trade_price;
	bool is_original;
};

struct T_TaskInformation
{
    unsigned int id;
    TypeTask  type;
    std::string stock;        // TEXT not null, alse can be index code--when type is INDEX_RISKMAN 
    std::string stock_pinyin; // TEXT 

	double alert_price;       // DOUBLE, 
	bool back_alert_trigger;  // 是否重回则触发
	double rebounce;          // 反弹或回撤 百分值
	int continue_second;      // 0-- not open >0 -- open
	double step;              // 阶梯幅度(以后每涨,以后每降) 百分值
	unsigned int quantity;    // 买入数量, 每次数量
	T_SectionTask secton_task;
	T_IndexRelateTask index_rel_task;
	T_AdvanceSectionTask advance_section_task;
	int target_price_level;   // 0--即时价  1--买一和卖一 2--买二和卖二 3--买三和卖三 ...
	int start_time; 
	int end_time;
	bool is_loop;
    int  state;               // TaskCurrentState  
    int  bs_times;            // 卖出次数, used by batches sell batches buy
    std::string  assistant_field;       // used by batches sell; batches buy
    T_TaskInformation() : id(0), type(TypeTask::BREAK_SELL), alert_price(0),back_alert_trigger(0), rebounce(0), continue_second(0), step(0), quantity(0), target_price_level(0), start_time(0), end_time(0), is_loop(0), state(0), bs_times(0), assistant_field() {} 
     
    T_TaskInformation(const T_TaskInformation& lh) : id(lh.id), type(lh.type), stock(lh.stock), stock_pinyin(lh.stock_pinyin), alert_price(lh.alert_price), back_alert_trigger(lh.back_alert_trigger)
		, rebounce(lh.rebounce), continue_second(lh.continue_second), step(lh.step), quantity(lh.quantity), secton_task(lh.secton_task), index_rel_task(lh.index_rel_task)
		, target_price_level(lh.target_price_level), start_time(lh.start_time), end_time(lh.end_time), is_loop(lh.is_loop), state(lh.state), bs_times(lh.bs_times)
		, assistant_field(lh.assistant_field) { }

};

struct StockPrice
{
    std::string code;
    std::string cur_price;
    //std::string active_degree;
    std::string price_b_1;
    std::string price_s_1;
    std::string price_b_2;
    std::string price_s_2;
    std::string price_b_3;
    std::string price_s_3;
    std::string price_b_4;
    std::string price_s_4;
    std::string price_b_5;
    std::string price_s_5;
    __int64 time_stamp;
};

struct T_StockCodeName
{
    std::string code;
    std::string name;
    T_StockCodeName(const std::string &cd, const std::string &nm) : code(cd), name(nm){}
    T_StockCodeName(const T_StockCodeName &lh) : code(lh.code), name(lh.name){}
};


//struct T_StockPosition
//{
//    std::string  code;
//    unsigned int position;
//};

TypeMarket GetStockMarketType(const std::string &stock_code);

QString ToQString(TypeTask val);
QString ToQString(TypeQuoteLevel val);
QString ToQString(TaskCurrentState val);
QString ToQString(TindexTaskType val);

void Delay(unsigned short mseconds);

bool Equal(double lh, double rh);

QTime Int2Qtime(int hhmmss);

bool IsStrAlpha(const std::string& str);
bool IsStrNum(const std::string& str);

//bool Equal(double lh, double rh, double esp=1e-4);
//============================

std::string TagOfLog();
std::string TagOfOrderLog();


std::tuple<int, std::string> CurrentDateTime();
bool IsNowTradeTime();

QString IndexCode2IndexName(const QString& code);

void utf8ToGbk(std::string& strUtf8);
void gbkToUtf8(std::string& strGbk);

double Round(double dVal, short iPlaces);

double Get2UpRebouncePercent(double base, double bottom, double cur);
double Get2DownRebouncePercent(double base, double top, double cur);

 

#endif