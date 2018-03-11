
#include "common.h"

#include <cctype>
#include <algorithm>
#include <regex>
 
#include <qtextcodec.h>

#include <TLib/core/tsystem_time.h>
#include <TLib/core/tsystem_utility_functions.h>

TypeMarket GetStockMarketType(const std::string &stock_code)
{
    return (stock_code.c_str()[0] == '6' ? TypeMarket::SH : TypeMarket::SZ);
}

QString ToQString(TypeTask val)
{
    switch(val)
    {
    case TypeTask::INFLECTION_BUY:
        return QString::fromLocal8Bit("拐点买入");
    case TypeTask::BREAKUP_BUY:
        return QString::fromLocal8Bit("突破买入");
    case TypeTask::BATCHES_BUY:
        return QString::fromLocal8Bit("分批买入");
    case TypeTask::INFLECTION_SELL:
        return QString::fromLocal8Bit("拐点卖出");
    case TypeTask::BREAK_SELL:
        return QString::fromLocal8Bit("破位卖出");
    case TypeTask::FOLLOW_SELL:
        return QString::fromLocal8Bit("跟踪止盈");
    case TypeTask::BATCHES_SELL:
        return QString::fromLocal8Bit("分批出货");
    case TypeTask::EQUAL_SECTION:
        return QString::fromLocal8Bit("等区间交易");
	case TypeTask::INDEX_RISKMAN:
        return QString::fromLocal8Bit("指数关联交易");
    default: assert(0);
    }
    return "";
}

QString ToQString(TypeQuoteLevel val)
{
    switch(val)
    {
    case TypeQuoteLevel::PRICE_CUR:
        return QString::fromLocal8Bit("即时价");
    case TypeQuoteLevel::PRICE_BUYSELL_1:
        return QString::fromLocal8Bit("买一和卖一");
    case TypeQuoteLevel::PRICE_BUYSELL_2:
        return QString::fromLocal8Bit("买二和卖二");
    case TypeQuoteLevel::PRICE_BUYSELL_3:
        return QString::fromLocal8Bit("买三和卖三");
    case TypeQuoteLevel::PRICE_BUYSELL_4:
        return QString::fromLocal8Bit("买四和卖四");
    case TypeQuoteLevel::PRICE_BUYSELL_5:
        return QString::fromLocal8Bit("买五和卖五");
    default: assert(0);
    }
    return "";
}

QString ToQString(TaskCurrentState val)
{
    switch(val)
    {
    case TaskCurrentState::STOP:
        return QString::fromLocal8Bit("停止"); 
    case TaskCurrentState::WAITTING:
        return QString::fromLocal8Bit("等待");
    case TaskCurrentState::STARTING:
        return QString::fromLocal8Bit("启动中");
    case TaskCurrentState::RUNNING:
        return QString::fromLocal8Bit("运行");
    case TaskCurrentState::REST:
        return QString::fromLocal8Bit("休市");
    case TaskCurrentState::EXCEPT:
        return QString::fromLocal8Bit("异常");
    default: assert(0);
    }
    return "";
}

QString ToQString(TindexTaskType val)
{
    switch(val)
    {
    case TindexTaskType::ALERT: return QString::fromLocal8Bit("预警");
    case TindexTaskType::RELSTOCK: return QString::fromLocal8Bit("关联交易");
    case TindexTaskType::CLEAR: return QString::fromLocal8Bit("清仓");
    default: assert(0);
    }
    return "";
}

void Delay(unsigned short mseconds)
{
    //TSystem::WaitFor([]()->bool { return false;}, mseconds); // only make effect to timer
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}

bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh)
{
     return lh.id < rh.id; 
}

bool Equal(double lh, double rh)
{
    return fabs(lh-rh) < 0.0001;
}

QTime Int2Qtime(int val)
{
    return QTime(val / 10000, (val % 10000) / 100, val % 100);
}

bool IsStrAlpha(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalpha(val);
		});
		 return iter == str.end();
	}catch(...)
	{
		return false;
	}
   
}

bool IsStrNum(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalnum(val);
		});
		return iter == str.end();
	}catch(...)
	{
		return false;
	}
    
}
 
std::string TagOfLog()
{
    return TSystem::utility::FormatStr("app_%d", TSystem::Today());
    //return  "OrderData_" + TSystem::Today()
}

std::string TagOfOrderLog()
{
    return TSystem::utility::FormatStr("OrderData_%d", TSystem::Today());
    //return  "OrderData_" + TSystem::Today()
}

std::string TagOfEqSecLog(const std::string& code)
{
    return TSystem::utility::FormatStr("EqSec_%s_%d", code.c_str(), TSystem::Today());
}

std::tuple<int, std::string> CurrentDateTime()
{
    time_t rawtime;
    time(&rawtime);

    const int cst_buf_len = 256;
	char szContent[cst_buf_len] = {0};
	  
    struct tm * timeinfo = localtime(&rawtime);
    sprintf_s( szContent, cst_buf_len, "%02d:%02d:%02d"
				, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec ); 

    return std::make_tuple((timeinfo->tm_year + 1900) * 10000 + (timeinfo->tm_mon + 1) * 100 + timeinfo->tm_mday
                          , std::string(szContent));

}

bool IsNowTradeTime()
{
	return true; //temp code
    static auto get_date = []()
    {
        time_t rawtime;
	    struct tm * timeinfo;
	    time( &rawtime );
	    timeinfo = localtime( &rawtime ); // from 1900 year
        return timeinfo->tm_year * 10000 + timeinfo->tm_mon *100 + timeinfo->tm_mday;
    };
     
     
    static int week_day = 0;  
    static int ori_day = 0;
    static time_t sec_beg = 0;
    static time_t sec_rest_beg = 0;
    static time_t sec_rest_end = 0;
    static time_t sec_end = 0;

    time_t rawtime = 0;
    struct tm * timeinfo = nullptr;
	time( &rawtime );
	
    auto cur_day = get_date();
    if( ori_day != cur_day )
    {
        ori_day = cur_day;
        
        timeinfo = localtime( &rawtime ); // from 1900 year
        week_day = timeinfo->tm_wday;

        struct tm tm_trade_beg;
	    tm_trade_beg.tm_year = timeinfo->tm_year;
	    tm_trade_beg.tm_mon = timeinfo->tm_mon;
	    tm_trade_beg.tm_mday = timeinfo->tm_mday;
	    tm_trade_beg.tm_hour = 9;
	    tm_trade_beg.tm_min = 25;
	    tm_trade_beg.tm_sec = 59;
	    sec_beg = mktime(&tm_trade_beg);

        struct tm tm_rest_beg; 
	    tm_rest_beg.tm_year = timeinfo->tm_year;
	    tm_rest_beg.tm_mon = timeinfo->tm_mon;
	    tm_rest_beg.tm_mday = timeinfo->tm_mday;
	    tm_rest_beg.tm_hour = 11;
	    tm_rest_beg.tm_min = 32;
	    tm_rest_beg.tm_sec = 00;
        sec_rest_beg = mktime(&tm_rest_beg);

        struct tm tm_rest_end; 
	    tm_rest_end.tm_year = timeinfo->tm_year;
	    tm_rest_end.tm_mon = timeinfo->tm_mon;
	    tm_rest_end.tm_mday = timeinfo->tm_mday;
	    tm_rest_end.tm_hour = 12;
	    tm_rest_end.tm_min = 58;
	    tm_rest_end.tm_sec = 00;
        sec_rest_end = mktime(&tm_rest_end);

	    struct tm tm_trade_end; 
	    tm_trade_end.tm_year = timeinfo->tm_year;
	    tm_trade_end.tm_mon = timeinfo->tm_mon;
	    tm_trade_end.tm_mday = timeinfo->tm_mday;
	    tm_trade_end.tm_hour = 15;
	    tm_trade_end.tm_min = 32;
	    tm_trade_end.tm_sec = 59;
	    sec_end = mktime(&tm_trade_end);
    }

	if( week_day == 6 || week_day == 0 ) // sunday: 0, monday : 1 ...
		return false; 
	if( (rawtime >= sec_beg && rawtime <= sec_rest_beg) || (rawtime >= sec_rest_end && rawtime <= sec_end) )
        return true;
    else 
        return false;
}

QString IndexCode2IndexName(const QString& code)
{
	if( code == cst_sh_index ) return QString::fromLocal8Bit(cst_sh_index_name);
	else if( code == cst_sz_compre_index ) return QString::fromLocal8Bit(cst_sz_compre_index_name);
	else if( code == cst_entrepren_plate_index ) return QString::fromLocal8Bit(cst_entrepren_plate_index_name);
	else if( code == cst_entreplate_compre_index ) return QString::fromLocal8Bit(cst_entreplate_compre_index_name);
	else return "";
}

void utf8ToGbk(std::string& strUtf8)
{
    QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
    QTextCodec* gbkCodec = QTextCodec::codecForName("gbk");

    QString strUnicode = utf8Codec->toUnicode(strUtf8.c_str());
    QByteArray ByteGbk = gbkCodec->fromUnicode(strUnicode);

    strUtf8 = ByteGbk.data();
}

void gbkToUtf8(std::string& strGbk)
{

    QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
    QTextCodec* gbkCodec = QTextCodec::codecForName("gbk");

    QString strUnicode = gbkCodec->toUnicode(strGbk.c_str());
    QByteArray ByteUtf8 = utf8Codec->fromUnicode(strUnicode);

    strGbk = ByteUtf8.data();
}
