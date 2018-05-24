#ifndef TRADE_AGENT_H_SDF3DSFS
#define TRADE_AGENT_H_SDF3DSFS

#define NOMINMAX  // cause, for qt adapt window.h
#include "TdxTradeApi.h"

#include "common.h"
class WinnerApp; 

class TradeAgent
{
public:

    TradeAgent(/*WinnerApp* app*/);
    ~TradeAgent();

    bool Setup(TypeBroker broker_type, std::string &account_no);
    bool IsInited() const;
    int  client_id() { return client_id_; }

#ifdef USE_MOCK_FLAG
    void OpenTdx(){}
    void CloseTdx(){}
    int Logon( char* IP, short Port, char* Version,short YybID,  char* AccountNo, char* TradeAccount, char* JyPassword, char* TxPassword, char* ErrInfo)
    {
        return 0;
    }
    void Logon(int ClientID){}
    void Logoff(int ClientID){}
    void QueryData(int ClientID, int Category, char* Result, char* ErrInfo){ return; }
    void SendOrder(int ClientID, int Category, int PriceType, char* Gddm, char* Zqdm, float Price, int Quantity, char* Result, char* ErrInfo){}
    void CancelOrder(int ClientID, char* ExchangeID, char* hth, char* Result, char* ErrInfo){}

#else
     OpenTdxDelegate OpenTdx; 
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
	 //QueryHistoryDataDelegate QueryHistoryData;
	 SendOrdersDelegate SendOrders;
	 CancelOrdersDelegate CancelOrders;
	 //GetQuotesDelegate GetQuotes; 
#endif
     void SetupAccountInfo(char *str);
     const T_AccountData * account_data(TypeMarket type_market) const;

private:
     void FreeDynamic();

     WinnerApp *app_;
     HINSTANCE TdxApiHMODULE;
     int client_id_;

      

     T_AccountData  account_data_[2];
     TypeBroker  broker_type_;
};
#endif