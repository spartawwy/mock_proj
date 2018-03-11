#include "ticker.h"

//#include <iostream>

#include <qt_windows.h>

#include <iostream>
#include <chrono>

#include <qmessagebox.h>
#include <qdebug.h>

//#include <boost/lexical_cast.hpp>

#include "TdxHqApi.h"

//#include <TLib/core/tsystem_utility_functions.h>
//#include <TLib/tool/tsystem_rational_number.h>
//#include "common.h"
//#include "strategy_task.h"
#include "flashingorder.h"

 //获取api函数
TdxHq_ConnectDelegate TdxHq_Connect = nullptr;
TdxHq_DisconnectDelegate TdxHq_Disconnect = nullptr;
TdxHq_GetSecurityBarsDelegate TdxHq_GetSecurityBars = nullptr;
TdxHq_GetIndexBarsDelegate TdxHq_GetIndexBars = nullptr;
TdxHq_GetMinuteTimeDataDelegate TdxHq_GetMinuteTimeData = nullptr;
TdxHq_GetHistoryMinuteTimeDataDelegate TdxHq_GetHistoryMinuteTimeData = nullptr;
TdxHq_GetTransactionDataDelegate TdxHq_GetTransactionData = nullptr;
TdxHq_GetHistoryTransactionDataDelegate TdxHq_GetHistoryTransactionData = nullptr;
TdxHq_GetSecurityQuotesDelegate TdxHq_GetSecurityQuotes = nullptr;
TdxHq_GetCompanyInfoCategoryDelegate TdxHq_GetCompanyInfoCategory = nullptr;
TdxHq_GetCompanyInfoContentDelegate TdxHq_GetCompanyInfoContent = nullptr;
TdxHq_GetXDXRInfoDelegate TdxHq_GetXDXRInfo = nullptr;
TdxHq_GetFinanceInfoDelegate TdxHq_GetFinanceInfo = nullptr;

//using namespace TSystem;

static char cst_hq_server[] = "122.224.66.108";
static int  cst_hq_port = 7709;


static bool is_codes_in(char* stock_codes[cst_max_stock_code_count], const char *str);
//static int GetRegisteredCodes(TTaskIdMapStrategyTask  &registered_tasks, char* stock_codes[cst_max_stock_code_count], byte markets[cst_max_stock_code_count]);

Ticker::Ticker()
    : tasks_list_mutex_()
    //, logger_(logger)
{

}

Ticker::~Ticker()
{
    /*if( TdxApiHMODULE )
        FreeLibrary(TdxApiHMODULE);*/
    if( TdxHq_Disconnect )
        TdxHq_Disconnect();
}

bool Ticker::Init()
{
    //载入dll, dll要复制到debug和release目录下,必须采用多字节字符集编程设置,用户编程时需自己控制浮点数显示的小数位数与精度
    HMODULE TdxApiHMODULE = LoadLibrary("TdxHqApi20991230.dll");
    if( TdxApiHMODULE == nullptr )
    {
        QMessageBox::information(nullptr, "info", "null dll");
        //throw excepton;
        return false;
    }
    //获取api函数
    TdxHq_Connect = (TdxHq_ConnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Connect");
    TdxHq_Disconnect = (TdxHq_DisconnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Disconnect");
    TdxHq_GetSecurityBars = (TdxHq_GetSecurityBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityBars");
    TdxHq_GetIndexBars = (TdxHq_GetIndexBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetIndexBars");
    TdxHq_GetMinuteTimeData = (TdxHq_GetMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetMinuteTimeData");
    TdxHq_GetHistoryMinuteTimeData = (TdxHq_GetHistoryMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryMinuteTimeData");
    TdxHq_GetTransactionData = (TdxHq_GetTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetTransactionData");
    TdxHq_GetHistoryTransactionData = (TdxHq_GetHistoryTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryTransactionData");
    TdxHq_GetSecurityQuotes = (TdxHq_GetSecurityQuotesDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityQuotes");
    TdxHq_GetCompanyInfoCategory = (TdxHq_GetCompanyInfoCategoryDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoCategory");
    TdxHq_GetCompanyInfoContent = (TdxHq_GetCompanyInfoContentDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoContent");
    TdxHq_GetXDXRInfo = (TdxHq_GetXDXRInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetXDXRInfo");
    TdxHq_GetFinanceInfo = (TdxHq_GetFinanceInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetFinanceInfo");

    Buffer Result(cst_result_len);
    Buffer ErrInfo(cst_error_len);
    
    bool bool1 = TdxHq_Connect(cst_hq_server, cst_hq_port, Result.data(), ErrInfo.data());
    if (!bool1)
    { 
        qDebug() << ErrInfo.data() << "\n";//连接失败
        //logger_.LogLocal(std::string("Ticker::Init TdxHq_Connect fail:") + Result.data());
        return false;
    }
    qDebug() << Result.data() << endl;
    //logger_.LogLocal(std::string("Ticker::Init") + Result.data());
    return true;
}

bool Ticker::GetQuotes(char* stock_code, QuotesData& quote_data)
{
    byte markets[cst_max_stock_code_count];
    //for( int i = 0; i < count; ++i )
    {
        markets[0] = (byte)GetStockMarketType(stock_code);
    }
	char *stock_codes[1] = {stock_code};

	Buffer Result(cst_error_len*2);
    Buffer ErrInfo(cst_error_len);
	short count = 1;
    auto ret = TdxHq_GetSecurityQuotes(markets, stock_codes, count, Result.data(), ErrInfo.data());
    if ( !ret )
    {
        qDebug() << ErrInfo.data() << endl;
        //logger_.LogLocal(std::string("Ticker::GetQuotes TdxHq_GetSecurityQuotes fail:") + ErrInfo.data());
        if( !strstr(ErrInfo.data(), "请重新连接") )
            TdxHq_Disconnect();

        //logger_.LogLocal(utility::FormatStr("Ticker::GetQuotes TdxHq_Connect %s : %d ", cst_hq_server, cst_hq_port) );
        Result.reset(); 
        ErrInfo.reset();
        try
        {
        ret = TdxHq_Connect(cst_hq_server, cst_hq_port, Result.data(), ErrInfo.data());
        if ( !ret )
        {
            //logger_.LogLocal(std::string("Ticker::GetQuotes retry TdxHq_Connect fail:") + ErrInfo.data());
            return false;
        }
        //logger_.LogLocal(utility::FormatStr("Ticker::GetQuotes TdxHq_Connect ok!"));
        Result.reset(); 
        ErrInfo.reset();
        //// debug 
        //for( int i = 0; i < count; ++i )
        //    logger_.LogLocal(utility::FormatStr("Ticker::GetQuotes stock_codes[%d]:%s", i, stock_codes[i]));
        //// end debug
        ret = TdxHq_GetSecurityQuotes(markets, stock_codes, count, Result.data(), ErrInfo.data());
        if ( !ret )
        {
            //logger_.LogLocal(std::string("Ticker::GetQuotes retry TdxHq_GetSecurityQuotes fail:") + ErrInfo.data());
            return false;
        }
        }catch(std::exception &e)
        {
            //logger_.LogLocal(utility::FormatStr("Ticker::GetQuotes exception:%s", e.what()));
            return false;
        }catch(...)
        {
            //logger_.LogLocal("Ticker::GetQuotes exception");
            return false;
        }
         
    } //if

	std::string res_str = Result.data();
	replace_all_distinct(res_str, "\n", "\t");

	auto result_array = split(res_str, "\t");

	for( int n = 1; n < result_array.size() / 44; ++n )
	{
		auto stock_code = result_array.at(1 + n * 44);
		  
		try
		{ 
			quote_data.cur_price = atof(result_array.at(3 + n * 44).c_str() );
			quote_data.active_degree = atof(result_array.at(42 + n * 44).c_str());

			//quote_data.time_stamp = (__int64)t_t;

			quote_data.price_b_1 = atof(result_array.at(17 + n * 44).c_str());
			quote_data.price_s_1 = atof(result_array.at(18 + n * 44).c_str());
			quote_data.price_b_2 = atof(result_array.at(21 + n * 44).c_str());
			quote_data.price_s_2 = atof(result_array.at(22 + n * 44).c_str());
			quote_data.price_b_3 = atof(result_array.at(25 + n * 44).c_str());
			quote_data.price_s_3 = atof(result_array.at(26 + n * 44).c_str());
			quote_data.price_b_4 = atof(result_array.at(29 + n * 44).c_str());
			quote_data.price_s_4 = atof(result_array.at(30 + n * 44).c_str());
			quote_data.price_b_5 = atof(result_array.at(33 + n * 44).c_str());
			quote_data.price_s_5 = atof(result_array.at(34 + n * 44).c_str());
		}catch(...)
		{
			continue;
		}
	}//for

    return true;
}
	  
void Ticker::Procedure()
{  
    Buffer Result(cst_result_len);
    Buffer ErrInfo(cst_error_len);

    char* stock_codes[cst_max_stock_code_count];
    memset(stock_codes, 0, sizeof(char*)*cst_max_stock_code_count);
      
    byte markets[cst_max_stock_code_count];

    short stock_count = 0;
     
    //auto  cur_time = QTime::currentTime();
    //--------------------------- 
	/* stock_count = GetRegisteredCodes(registered_tasks_, stock_codes, markets);
	if( stock_count < 1 )
	return;*/

    /*if( !GetQuotes(stock_codes, 1, Result) )
        return;*/
#if 0 
    auto ret = TdxHq_GetSecurityQuotes(markets, stock_codes, stock_count, Result.data(), ErrInfo.data());
    if ( !ret )
    {
        qDebug() << ErrInfo.data() << endl;
        logger_.LogLocal(std::string("Ticker::Procedure TdxHq_GetSecurityQuotes fail:") + ErrInfo.data());
        if( !strstr(ErrInfo.data(), "请重新连接") )
            TdxHq_Disconnect();

        logger_.LogLocal(utility::FormatStr("Ticker::Procedure TdxHq_Connect %s : %d ", cst_hq_server, cst_hq_port) );
        Result.reset(); 
        ErrInfo.reset();
        try
        {
        ret = TdxHq_Connect(cst_hq_server, cst_hq_port, Result.data(), ErrInfo.data());
        if ( !ret )
        {
            logger_.LogLocal(std::string("Ticker::Procedure retry TdxHq_Connect fail:") + ErrInfo.data());
            return;
        }
        logger_.LogLocal(utility::FormatStr("Ticker::Procedure TdxHq_Connect ok!"));
        Result.reset(); 
        ErrInfo.reset();
        // debug 
        for( int i = 0; i < stock_count; ++i )
            logger_.LogLocal(utility::FormatStr("Ticker::Procedure stock_codes[%d]:%s", i, stock_codes[i]));
        // end debug
        ret = TdxHq_GetSecurityQuotes(markets, stock_codes, stock_count, Result.data(), ErrInfo.data());
        if ( !ret )
        {
            logger_.LogLocal(std::string("Ticker::Procedure retry TdxHq_GetSecurityQuotes fail:") + ErrInfo.data());
            return;
        }
        }catch(std::exception &e)
        {
            logger_.LogLocal(utility::FormatStr("Ticker::Procedure exception:%s", e.what()));
            return;
        }catch(...)
        {
            logger_.LogLocal("Ticker::Procedure exception");
            return;
        }
         
    }
#endif
    /*auto tp_now = std::chrono::system_clock::now();
    time_t t_t = std::chrono::system_clock::to_time_t(tp_now); */
    //--------------------------Docode result -------------

    //DecodeStkQuoteResult(Result, nullptr, std::bind(&Ticker::TellAllRelTasks, this, std::placeholders::_1, std::placeholders::_2));
      
}
  
bool Ticker::GetSecurityBars(int Category, int Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo)
{
    return TdxHq_GetSecurityBars(Category, Market, Zqdm, Start, Count, Result, ErrInfo);
}

  

bool is_codes_in(char* stock_codes[cst_max_stock_code_count], const char *str)
{ 
    for(int i = 0; i < cst_max_stock_code_count; ++i)
    {
        if( stock_codes[i] == nullptr )
            return false;
        if( !strcmp(stock_codes[i], str) )
            return true;
    }
    return false;
};
