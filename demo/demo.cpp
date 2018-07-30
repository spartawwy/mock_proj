
#include <qdebug.h>

#include <TLib/core/tsystem_utility_functions.h>
//#include <qt_Windows.h>

#include "demo.h"
#include "qtimer_container.h"

//#include "stk_quoter_api.h"

 
#define TEST_URL_METHOD   

void funcOfTimer(int *para)
{
    qDebug () << *para << "  \n";
}

demo::demo(QWidget *parent)
    : QDialog(parent)
    , one_shot_timers_(std::make_shared<QTimerContainner>(true))
    , serial_shot_timers_(std::make_shared<QTimerContainner>(false))
{
    ui.setupUi(this);
    
    

/*
    auto one_shot_timers_ = std::make_shared<QTimerContainner>(true);
    auto serial_shot_timers_ = std::make_shared<QTimerContainner>(false);
*/
#if 0 
    QTimerWapper* p_time_obj = new QTimerWapper(nullptr, false, std::bind(funcOfTimer, p_i));

    p_time_obj->Start(3*1000);
    
   /* bool ret = connect(
    DoMyTimer()*/
#endif 
     
    /*p_time_obj_ = new QTimerWapper(nullptr, true, [this]()
     {
         this->ui.pbt_test->setEnabled(true);
     });*/

    bool ret = connect(ui.pbt_test, SIGNAL(clicked()), this, SLOT(DoTest()));

    

    //-----------------------------------------------------
#if 0 
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

#endif
}

void demo::DoTest()
{
    __int64 val = 1000;
    std::string stock = "000026";
    char buf[1024] = {0};
    char ldstr[64] = {0};
    sprintf_s(ldstr, sizeof(ldstr), "%ld", val);
    //sprintf(buf, "%ld , '%s' ", val, "000026");
    sprintf(buf, "%s , '%s' ", ldstr, "000026");
    auto str = TSystem::utility::FormatStr("%ld , '%s' ", val, "000026");
    ui.pbt_test->setDisabled(true);
     
    one_shot_timers_->InsertTimer(2000, [this]()
    {
        this->ui.pbt_test->setEnabled(true);
    });

    serial_shot_timers_->InsertTimer(5000, [this]()
    {
        qDebug() << " in timer\n";
    });
}

demo::~demo()
{

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

