#include "trade_agent.h"

#include <boost/lexical_cast.hpp>
#include <qmessagebox.h>

#include <TLib/core/tsystem_utility_functions.h>



TradeAgent::TradeAgent()
    :/* app_(app)
     ,*/ TdxApiHMODULE(nullptr)
     , client_id_(-1)
     , CloseTdx(nullptr)
     , Logon(nullptr)
     , Logoff(nullptr)
     , QueryData(nullptr) 
     , SendOrder(nullptr) 
     , CancelOrder(nullptr)
     , GetQuote(nullptr) 
     , Repay(nullptr) 
     , QueryDatas(nullptr)
     , QueryHistoryData(nullptr)
     , SendOrders(nullptr)
     , CancelOrders(nullptr)
     , GetQuotes(nullptr) 
{
    memset(&account_data_, 0, sizeof(account_data_));
}

TradeAgent::~TradeAgent()
{
    FreeDynamic();
}
void FreeDynamic()
{

}

bool TradeAgent::Setup(TypeBroker broker_type, std::string &account_no)
{
    assert(account_no.length() > 0);
    broker_type_ = broker_type;

    FreeDynamic();

    OpenTdx = nullptr;

    char path_str[256] = {0};
    sprintf_s(path_str, "trade_%s.dll\0", account_no.c_str());
    TdxApiHMODULE = LoadLibrary(path_str);
    if( TdxApiHMODULE == nullptr )
    {
        QMessageBox::information(nullptr, "info", "load trade.dll fail");
        //throw excepton;
        return false;
    }

    OpenTdx = (OpenTdxDelegate)GetProcAddress(TdxApiHMODULE, "OpenTdx");
    CloseTdx = (CloseTdxDelegate)GetProcAddress(TdxApiHMODULE, "CloseTdx");
    Logon = (LogonDelegate)GetProcAddress(TdxApiHMODULE, "Logon");
    Logoff = (LogoffDelegate)GetProcAddress(TdxApiHMODULE, "Logoff");
    QueryData = (QueryDataDelegate)GetProcAddress(TdxApiHMODULE, "QueryData");
    SendOrder = (SendOrderDelegate)GetProcAddress(TdxApiHMODULE, "SendOrder");
    CancelOrder = (CancelOrderDelegate)GetProcAddress(TdxApiHMODULE, "CancelOrder");
    GetQuote = (GetQuoteDelegate)GetProcAddress(TdxApiHMODULE, "GetQuote");
    Repay = (RepayDelegate)GetProcAddress(TdxApiHMODULE, "Repay");


    //以下是普通批量版功能函数
    QueryDatas = (QueryDatasDelegate)GetProcAddress(TdxApiHMODULE, "QueryDatas");
    QueryHistoryData = (QueryHistoryDataDelegate)GetProcAddress(TdxApiHMODULE, "QueryHistoryData");
    SendOrders = (SendOrdersDelegate)GetProcAddress(TdxApiHMODULE, "SendOrders");
    CancelOrders = (CancelOrdersDelegate)GetProcAddress(TdxApiHMODULE, "CancelOrders");
    GetQuotes = (GetQuotesDelegate)GetProcAddress(TdxApiHMODULE, "GetQuotes");

    //       //以下是高级批量版功能函数
    //QueryMultiAccountsDatasDelegate QueryMultiAccountsDatas = (QueryMultiAccountsDatasDelegate)GetProcAddress(TdxApiHMODULE, "QueryMultiAccountsDatas");
    //SendMultiAccountsOrdersDelegate SendMultiAccountsOrders = (SendMultiAccountsOrdersDelegate)GetProcAddress(TdxApiHMODULE, "SendMultiAccountsOrders");
    //CancelMultiAccountsOrdersDelegate CancelMultiAccountsOrders = (CancelMultiAccountsOrdersDelegate)GetProcAddress(TdxApiHMODULE, "CancelMultiAccountsOrders");
    //GetMultiAccountsQuotesDelegate GetMultiAccountsQuotes = (GetMultiAccountsQuotesDelegate)GetProcAddress(TdxApiHMODULE, "GetMultiAccountsQuotes");

    OpenTdx();
    return true;
}

bool TradeAgent::IsInited() const
{
    return TdxApiHMODULE != nullptr && OpenTdx != nullptr;
}

void TradeAgent::SetupAccountInfo( char*str)
{
    std::string result_str(str);
    TSystem::utility::replace_all_distinct(result_str, "\n", "\t");

    auto result_array = TSystem::utility::split(result_str, "\t");

    unsigned short shared_holder_code_index = 0;
    unsigned short name_index = 1;
    unsigned short type_index = 1;
    unsigned short capital_code_index = 1;
    unsigned short seat_code_index = 1;
    unsigned short rzrq_tag_index = 1;
    unsigned short sec_num = 7;
    unsigned short start_index = 0;
    switch(broker_type_)
    {
    case TypeBroker::FANG_ZHENG:
    case TypeBroker::ZHONGYGJ:
        start_index = 7;
        shared_holder_code_index = 0;
        name_index = 1;
        type_index = 2;
        capital_code_index = 3;
        seat_code_index = 4;
        rzrq_tag_index = 5;
        break;
    case TypeBroker::PING_AN:
        start_index = 8;
        capital_code_index = 0;
        shared_holder_code_index = 1;
        name_index = 2;
        type_index = 3;
        seat_code_index = 4;
        rzrq_tag_index = 5;
        break;
    case TypeBroker::ZHONGXIN:
        start_index = 10;
        capital_code_index = 0;
        shared_holder_code_index = 1;
        name_index = 2;
        type_index = 3;
        seat_code_index = 4;
        rzrq_tag_index = 6;
        sec_num = 9;
        break;
    default: assert(0);break;
    }
    for( int n = 0, i = 0; i < 2 && n < result_array.size() / sec_num; ++n )
    {
        strcpy(account_data_[i].shared_holder_code, result_array.at(start_index + shared_holder_code_index + n * sec_num).c_str());
        strcpy(account_data_[i].name, result_array.at(start_index + name_index + n * sec_num).c_str());
        try
        {
            //
            account_data_[i].type = (TypeMarket)boost::lexical_cast<int>(result_array.at(start_index + type_index + n * sec_num).c_str());
        }catch(boost::exception&)
        {
            continue;
        }
        strcpy(account_data_[i].capital_code, result_array.at(start_index + capital_code_index + n * sec_num).c_str());
        strcpy(account_data_[i].seat_code, result_array.at(start_index + seat_code_index + n * sec_num).c_str());
        strcpy(account_data_[i].rzrq_tag, result_array.at(start_index + rzrq_tag_index + n * sec_num).c_str());
        ++i;
    }
}

const T_AccountData * TradeAgent::account_data(TypeMarket type_market) const
{
    T_AccountData* p_account = nullptr;
    for(int i=0; i < sizeof(account_data_)/sizeof(account_data_[0]); ++i)
    {
        if( account_data_[i].type == type_market )
        {
            return &account_data_[i];
        }
    }
    return p_account;
}

void TradeAgent::FreeDynamic()
{
    if( CloseTdx )
        CloseTdx();
    if( TdxApiHMODULE )
        FreeLibrary(TdxApiHMODULE);
}