#include "demo.h"

#include <Windows.h>

#include "stk_quoter_api.h"

#define TEST_URL_METHOD   

demo::demo(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    auto stk_handle = LoadLibrary("StkQuoter.dll");
    if( stk_handle )
    {

      StkQuoteGetQuoteDelegate stk_get_quote = (StkQuoteGetQuoteDelegate)GetProcAddress(stk_handle, "StkQuoteGetQuote");

#ifdef  TEST_URL_METHOD

      char stocks[3][16]; 
#if 0
	  strcpy_s(stocks[0], "600000");
	  strcpy_s(stocks[1], "900300");
	  strcpy_s(stocks[2], "603126");
#endif
      T_StockPriceInfo stock_price[3];
      strcpy_s(stocks[0], "000001");
      strcpy_s(stocks[1], "399001"); //深证成指
      strcpy_s(stocks[2], "399006"); //创业板指
      stk_get_quote(stocks, 3, stock_price);
      
      int i = 0;
      i = i;

#else
      char stocks[3][16]; 
	  strcpy_s(stocks[0], "600000");
         
    StkHisDataDelegate stk_his_data = (StkHisDataDelegate)GetProcAddress(stk_handle, "StkHisData");
    
    StkRelHisDataDelegate release_his_data = (StkRelHisDataDelegate)GetProcAddress(stk_handle, "StkRelHisData");
    T_StockHisDataItem *his_data;
    int count = 0;
    stk_his_data("600487", 20171204, 20171208, &his_data);

    release_his_data(his_data);
#endif
    }
}

demo::~demo()
{

}
