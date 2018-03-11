#include "ticker.h"

#include <chrono>

#include <qdebug.h> 
#include <memory>

#include <boost/lexical_cast.hpp>

#include <TLib/core/tsystem_utility_functions.h>

static char cst_hq_server[] = "122.224.66.108";
static int  cst_hq_port = 7709;

 const int cst_result_len = 5*1024;
 const int cst_error_len = 2*1024;

 
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


StockTicker::StockTicker()
{

}

StockTicker::~StockTicker()
{
    /*if( TdxApiHMODULE )
        FreeLibrary(TdxApiHMODULE);*/
    TdxHq_Disconnect();
}

bool StockTicker::Init()
{
    //载入dll, dll要复制到debug和release目录下,必须采用多字节字符集编程设置,用户编程时需自己控制浮点数显示的小数位数与精度
    HMODULE TdxApiHMODULE = LoadLibrary("TdxHqApi20991230.dll");
    if( TdxApiHMODULE == nullptr )
    {
        //QMessageBox::information(nullptr, "info", "null dll");
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
    // 
    
    bool bool1 = TdxHq_Connect(cst_hq_server, cst_hq_port, Result.data(), ErrInfo.data());
    if (!bool1)
    { 
        //qDebug() << ErrInfo.data() << "\n";//连接失败
        //logger_.LogLocal(std::string("StockTicker::Init TdxHq_Connect fail:") + Result.data());
        return false;
    }
    //qDebug() << Result.data() << endl;
   // logger_.LogLocal(std::string("StockTicker::Init") + Result.data());
    return true;
}

void StockTicker::test()
{
     const int cst_max_stock_code_count = 10;
     byte markets[cst_max_stock_code_count];

    Buffer Result(cst_result_len);
    Buffer ErrInfo(cst_error_len);

    short stock_count = 0;
    //auto  cur_time = QTime::currentTime();
    
    char* stock_codes[cst_max_stock_code_count];
    memset(stock_codes, 0, sizeof(char*)*cst_max_stock_code_count);
     
    //Buffer buf("999999", 10);
    Buffer buf("000001", 10);
    stock_codes[0] = buf.data();
    ++stock_count;

    if( stock_count < 1 )
        return;
#if 0 
    auto ret = TdxHq_GetSecurityQuotes(markets, stock_codes, stock_count, Result.data(), ErrInfo.data());
    if ( !ret )
    {
        qDebug() << ErrInfo.data() << endl;
         
        Result.reset(); 
        ErrInfo.reset();
        return;
    }
 
     short count = 0;
     auto ret = TdxHq_GetIndexBars((byte)0, (byte)1, "sh000001", 0, count, Result.data(), ErrInfo.data());  // no effect

#endif
    auto tp_now = std::chrono::system_clock::now();
    time_t t_t = std::chrono::system_clock::to_time_t(tp_now); 
     
    //--------------------------Docode result -------------

    qDebug() << "\n" << QString::fromLocal8Bit(Result.data()) << "\n";

    std::string res_str = Result.data();
    TSystem::utility::replace_all_distinct(res_str, "\n", "\t");
     
    auto result_array = TSystem::utility::split(res_str, "\t");
    
    for( int n = 1; n < result_array.size() / 44; ++n )
    {
        auto stock_code = result_array.at(1 + n * 44);
        /*auto task_ids_iter = codes_taskids_.find(stock_code);
        if( task_ids_iter == codes_taskids_.end() )
            continue;*/
        auto quote_data = std::make_shared<QuotesData>();

        try
        {
        quote_data->cur_price = boost::lexical_cast<double>( result_array.at(3 + n * 44) );
        quote_data->active_degree = boost::lexical_cast<double>(result_array.at(42 + n * 44));

        quote_data->time_stamp = (__int64)t_t;

        quote_data->price_b_1 = boost::lexical_cast<double>(result_array.at(17 + n * 44));
        quote_data->price_s_1 = boost::lexical_cast<double>(result_array.at(18 + n * 44));
        quote_data->price_b_2 = boost::lexical_cast<double>(result_array.at(21 + n * 44));
        quote_data->price_s_2 = boost::lexical_cast<double>(result_array.at(22 + n * 44));
        quote_data->price_b_3 = boost::lexical_cast<double>(result_array.at(25 + n * 44));
        quote_data->price_s_3 = boost::lexical_cast<double>(result_array.at(26 + n * 44));
        quote_data->price_b_4 = boost::lexical_cast<double>(result_array.at(29 + n * 44));
        quote_data->price_s_4 = boost::lexical_cast<double>(result_array.at(30 + n * 44));
        quote_data->price_b_5 = boost::lexical_cast<double>(result_array.at(33 + n * 44));
        quote_data->price_s_5 = boost::lexical_cast<double>(result_array.at(34 + n * 44));
        }catch(const boost::exception&)
        {
            continue;
        } 
    }
}
