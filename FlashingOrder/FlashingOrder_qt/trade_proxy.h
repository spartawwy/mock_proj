#ifndef TRADE_AGENT_H_SDF3DSFS
#define TRADE_AGENT_H_SDF3DSFS

#define NOMINMAX  // cause, for qt adapt window.h
#include "TdxTradeApi.h"

#include "fo_common.h"

class TradeProxy
{
public:

    TradeProxy(/*WinnerApp* app*/);
    ~TradeProxy();

    bool Setup(TypeBroker broker_type, std::string &account_no);
    bool IsInited() const;
     
	 CloseTdxDelegate CloseTdx;
	 LogonDelegate Logon;
	 LogoffDelegate Logoff;
	 QueryDataDelegate QueryData; 
	 SendOrderDelegate SendOrder; 
	 CancelOrderDelegate CancelOrder;
	 GetQuoteDelegate GetQuote; 
	 RepayDelegate Repay; 
	 
	 //是普通批量版功能函数
	 QueryDatasDelegate QueryDatas;
	 QueryHistoryDataDelegate QueryHistoryData;
	 SendOrdersDelegate SendOrders;
	 CancelOrdersDelegate CancelOrders;
	 GetQuotesDelegate GetQuotes; 

     void SetupAccountInfo(char *str);
     const T_AccountData * account_data(TypeMarket type_market) const;

private:
     void FreeDynamic();

     //WinnerApp *app_;
     HINSTANCE TdxApiHMODULE;
     int client_id_;

     OpenTdxDelegate OpenTdx;  

     T_AccountData  account_data_[2];
     TypeBroker  broker_type_;
};
#endif