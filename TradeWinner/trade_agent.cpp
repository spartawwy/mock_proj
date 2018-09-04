#include "trade_agent.h"

#include <boost/lexical_cast.hpp>
#include <qmessagebox.h>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

#include "db_moudle.h"
#include "position_mocker.h"
#include "exchange_calendar.h"

TradeAgent::TradeAgent()
    :/* app_(app)
     ,*/ TdxApiHMODULE(nullptr)
     , client_id_(-1)
#ifndef USE_MOCK_FLAG
     , CloseTdx(nullptr)
     , Logon(nullptr)
     , Logoff(nullptr)
     , QueryData(nullptr) 
     , SendOrder(nullptr) 
     , CancelOrder(nullptr)
     //, GetQuote(nullptr) 
     , Repay(nullptr) 
     , QueryDatas(nullptr)
     //, QueryHistoryData(nullptr)
     , SendOrders(nullptr)
     , CancelOrders(nullptr)
     //, GetQuotes(nullptr) 
#else
     , position_mocker_(nullptr)
     , p_db_moudle_(nullptr)
     , user_id_(0)
#endif
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
     
#ifndef USE_MOCK_FLAG
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
    //QueryHistoryData = (QueryHistoryDataDelegate)GetProcAddress(TdxApiHMODULE, "QueryHistoryData");
    SendOrders = (SendOrdersDelegate)GetProcAddress(TdxApiHMODULE, "SendOrders");
    CancelOrders = (CancelOrdersDelegate)GetProcAddress(TdxApiHMODULE, "CancelOrders");
    GetQuotes = (GetQuotesDelegate)GetProcAddress(TdxApiHMODULE, "GetQuotes");

    //       //以下是高级批量版功能函数
    //QueryMultiAccountsDatasDelegate QueryMultiAccountsDatas = (QueryMultiAccountsDatasDelegate)GetProcAddress(TdxApiHMODULE, "QueryMultiAccountsDatas");
    //SendMultiAccountsOrdersDelegate SendMultiAccountsOrders = (SendMultiAccountsOrdersDelegate)GetProcAddress(TdxApiHMODULE, "SendMultiAccountsOrders");
    //CancelMultiAccountsOrdersDelegate CancelMultiAccountsOrders = (CancelMultiAccountsOrdersDelegate)GetProcAddress(TdxApiHMODULE, "CancelMultiAccountsOrders");
    //GetMultiAccountsQuotesDelegate GetMultiAccountsQuotes = (GetMultiAccountsQuotesDelegate)GetProcAddress(TdxApiHMODULE, "GetMultiAccountsQuotes");
#endif
    OpenTdx();
    return true;
}

bool TradeAgent::IsInited() const
{
#ifndef USE_MOCK_FLAG
    return TdxApiHMODULE != nullptr && OpenTdx != nullptr;
#else
    return true;
#endif
}

void TradeAgent::SetupAccountInfo( char*str)
{
#ifndef USE_MOCK_FLAG
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
    case TypeBroker::ZHONGY_GJ:
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
    case TypeBroker::ZHONG_XIN:
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
#endif
}
 
const T_AccountData * TradeAgent::account_data(TypeMarket type_market) const
{
    T_AccountData* p_account = nullptr;
#ifndef USE_MOCK_FLAG
    for(int i=0; i < sizeof(account_data_)/sizeof(account_data_[0]); ++i)
    {
        if( account_data_[i].type == type_market )
        {
            return &account_data_[i];
        }
    }
#else
     return &account_data_[0];
#endif
    return p_account;
}

#ifdef USE_MOCK_FLAG
// ps: have to called in trade strand
void TradeAgent::SendOrder(int ClientID, int Category, int PriceType, char* /*Gddm*/, char* Zqdm, float Price, int Quantity, char* Result, char* ErrInfo)
{
    assert(p_db_moudle_);
    assert(position_mocker_);
    assert(Zqdm && strlen(Zqdm) > 0);

    auto today = TSystem::Today();
    auto date = today;
    if( !position_mocker_->exchange_calendar().IsTradeDate(date) )
        date = position_mocker_->exchange_calendar().NextTradeDate(date, 1);
    if( Category == (int)TypeOrderCategory::BUY )
    {
        auto fee = CaculateFee(Price * Quantity, true);
        auto cost = Price * Quantity + fee;
        auto capital_pos = position_mocker_->ReadPosition(today, CAPITAL_SYMBOL);
        assert(capital_pos);
        if( capital_pos->avaliable < cost )
        {
            strcpy(ErrInfo, "capital not enough!");
            return;
        }
        capital_pos->avaliable -= cost;
        capital_pos->total -= cost;

        position_mocker_->AddTotalPosition(date, Zqdm, Quantity, true);
        
    }else // sell 
    {
        auto p_pos_mocked = position_mocker_->ReadPosition(today, Zqdm);
        assert(p_pos_mocked);
        if( p_pos_mocked->avaliable < Quantity )
        {
            strcpy(ErrInfo, "stock avaliable not enough!");
            return;
        }
        p_pos_mocked->avaliable -= Quantity;
        p_pos_mocked->total -= Quantity;
         
        auto capital_pos = position_mocker_->ReadPosition(today, CAPITAL_SYMBOL);
        assert(capital_pos);
        auto fee = CaculateFee(Price * Quantity, false); 
		double delta_capital = Price * Quantity - fee;
        capital_pos->avaliable += delta_capital;
        capital_pos->total += delta_capital;
    }
    // save to db  
    if( today == date )
    {
        p_db_moudle_->UpdateOnePositionMock(*position_mocker_, Zqdm, date, user_id_);
        p_db_moudle_->UpdateOnePositionMock(*position_mocker_, CAPITAL_SYMBOL, date, user_id_);
    }else
    {
        p_db_moudle_->AddOnePositionMock(*position_mocker_, Zqdm, date, user_id_);
        p_db_moudle_->AddOnePositionMock(*position_mocker_, CAPITAL_SYMBOL, date, user_id_);
    }
}
#endif

void TradeAgent::FreeDynamic()
{
#ifndef USE_MOCK_FLAG
    if( CloseTdx )
        CloseTdx();
    if( TdxApiHMODULE )
        FreeLibrary(TdxApiHMODULE);
#endif
}