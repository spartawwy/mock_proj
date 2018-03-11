/*
* wwy: todo:使用 qt读取 sqlit3 数据库 获取股票代码
*/

#include "flashingorder.h"
 
#include <qt_windows.h>    
#include <TlHelp32.h>    
   
#include <locale.h>     
#include <stdio.h>

#include <Qdir.h>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

#include "gloakm_capture_api.h"

#include "dbmoudle.h"
#include "ticker.h"
#include "msg_win.h"


#define WINDOW_TEXT_LENGTH 256    
#define MAIN_PROCESS_WIN_TAG "方正证券泉友通"  //"通达信"
#define  CONFIG_FILE_NAME "flashingorder.ini"

char g_win_process_win_tag[256] = MAIN_PROCESS_WIN_TAG; // "方正证券泉友通";  

bool g_is_accept_order_ = false;

FlashingOrder& AppInstance()
{
	static FlashingOrder app;
	return app;
}

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)    
{    
	DWORD dwPid = 0;    
	GetWindowThreadProcessId(hWnd, &dwPid); // 获得找到窗口所属的进程    
	if(dwPid == lParam) // 判断是否是目标进程的窗口    
	{         
		TCHAR buf[WINDOW_TEXT_LENGTH];    
		SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);    
		//wprintf(L"%s/n", buf);    
		printf("%s \n", buf);
		qDebug() << "hwnd: "<< (int)hWnd << " " << QString::fromLocal8Bit(buf) << "\n";
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);     
	}    
	return TRUE;    
}    

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)    
{    
	DWORD dwPid = 0;    
	GetWindowThreadProcessId(hWnd, &dwPid); // 获得找到窗口所属的进程    
	if(dwPid == lParam) // 判断是否是目标进程的窗口    
	{    
		//printf("hwnd: %d ", (int)hWnd); // 输出窗口信息    
		TCHAR buf[WINDOW_TEXT_LENGTH];    
		SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);     
		if( strstr(buf, MAIN_PROCESS_WIN_TAG) )
		{
			//wprintf(L"%s/n", buf);    
			qDebug() << "hwnd: " << (int)hWnd << "  " << QString::fromLocal8Bit(buf) << "\n";
			EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // 继续查找子窗口 
			return FALSE;
		}else
			qDebug() << "hwnd: " << (int)hWnd << "  " << QString::fromLocal8Bit(buf) << "\n";
		      
	}    
	return TRUE;    
}    

//int CallBackFunc(BOOL is_buy, char *stock_name)
int CallBackFunc(void)
{
	//qDebug() << "CallBackFunc: " << (is_buy ? "buy " : "sell ") << "\n";
	  
	QString title;
	QString name;
	if( AppInstance().GetWinTileAndStockName(title, name) )
	{ 
		AppInstance().EmitKeySig(name);
		//qDebug() << "todo " << (is_buy ? "buy " : "sell ") << name.c_str() << "\n";
		return 1; 
	}
    return 0;
}

FlashingOrder::FlashingOrder(QWidget *parent)
	: QWidget(parent) 
    , exit_flag_(false)
    , target_win_title_tag_()
    , qty_buy_(100)
    , qty_sell_(100)
    , quote_level_(TypeQuoteLevel::PRICE_BUYSELL_1)
	, key_sig_mutex_()
	, key_sig_wait_cond_()
	, thread_(this)
	, stock_name2code_(1024*4)
	, trade_proxy_()
	, trade_client_id_(0)
	, ticker_(std::make_shared<Ticker>())
    , msg_win_(new MsgWin(1000))
{
	ui.setupUi(this);
    //------------  
    //auto str0 = QDir::currentPath();
    ui.spinBox_buy_quantity->setMaximum(10000*100);
    ui.spinBox_buy_quantity->setSingleStep(100);
    ui.spinBox_buy_quantity->setValue(qty_buy_);
    
    ui.spinBox_sell_quantity->setMaximum(10000*100);
    ui.spinBox_sell_quantity->setSingleStep(100);
    ui.spinBox_sell_quantity->setValue(qty_sell_);

    ui.combox_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
    
    ui.combox_price_level->setCurrentText(ToQString(static_cast<TypeQuoteLevel>(quote_level_)));
    //------------------------------

	connect(&normal_timer_, SIGNAL(timeout()), this, SLOT(DoNormalTimer()));
	bool ret = connect(this, SIGNAL(key_sig(QString)), this, SLOT(DoKeySig(QString)));
    ret = connect( this, SIGNAL(show_msg_sig(QString, QString)), this, SLOT(DoShowMsgSig(QString, QString)) );
    ret = connect(ui.pbtn_fresh, SIGNAL(clicked()), this, SLOT(DoReadCfg()));
    ret = connect(ui.pbtn_save, SIGNAL(clicked()), this, SLOT(DoSaveCfg()));

	thread_.start();
}

FlashingOrder::~FlashingOrder()
{
	UnInstallLaunchEv();

    if( msg_win_ )
	{
		//msg_win_->close();
		delete msg_win_;
	}
}

bool FlashingOrder::Init(int argc, char *argv[])
{ 
    DBMoudle  dbobj(this);

	int broker_id = 3;
	int account_id = 3;
    if( !dbobj.Init(broker_id, account_id) )
	{
		// todo: msg error
        //QString("dbobj.Init fail of broker id:%1 account id:%2").arg(broker_id).arg(account_id);
        QMessageBox::information(nullptr, "错误", QString("dbobj.Init fail of broker id:%1 account id:%2").arg(broker_id).arg(account_id));
		return false;
	}
	
    DoReadCfg();
#if 0 
    // -------read config file ------------
    QString iniFilePath = "flashingorder.ini";  
    QSettings settings(iniFilePath, QSettings::IniFormat);  
    QString app_title = QString::fromLocal8Bit(settings.value("config/broker_app_title").toString().toLatin1().constData());
    qDebug() << app_title << "\n";
    target_win_title_tag_ = app_title.toLocal8Bit();
    qty_buy_ = settings.value("config/buy_quantity").toInt();
    qty_sell_ = settings.value("config/sell_quantity").toInt();
    quote_level_ = (TypeQuoteLevel)settings.value("config/quote_level").toInt();
    
    ui.spinBox_buy_quantity->setSingleStep(100);
    ui.spinBox_buy_quantity->setValue(qty_buy_);
    
    ui.spinBox_sell_quantity->setSingleStep(100);
    ui.spinBox_sell_quantity->setValue(qty_sell_);

    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
    
    //-----------end-----------------
#endif 

	if( !ticker_->Init() )
	{
		QMessageBox::information(nullptr, "错误", "ticker init fail");
		return false;
	}
	// tradeProxy init ----------------
	 
	if( !trade_proxy_.Setup(broker_info_.type, account_info_.account_no_in_broker_) )
	{
		QMessageBox::information(nullptr, "错误", "trade proxy setup fail");
		return false;
	}

	assert(trade_proxy_.IsInited());
	Buffer result(1024);
	char error[1024] = {0};
	   
#if 1
	/*trade_client_id_ = trade_proxy_.Logon("122.224.113.121"
	, 7708, "2.20", 1, "32506627"
	, "32506627", "626261", "", error_info);*/
	//p_user_account_info->comm_pwd_;
#endif
    try
    {
	 trade_client_id_ = trade_proxy_.Logon(const_cast<char*>(broker_info_.ip.c_str())
		, broker_info_.port
		, const_cast<char*>(broker_info_.com_ver.c_str())
		, 1
		, const_cast<char*>(account_info_.account_no_in_broker_.c_str())
		, const_cast<char*>(account_info_.account_no_in_broker_.c_str())  // default trade no is account no  
		, const_cast<char*>(account_info_.trade_pwd_.c_str())
		, broker_info_.type == TypeBroker::ZHONGYGJ ? account_info_.trade_pwd_.c_str() : ""// communication password 
		, error);
    }catch(...)
    {
         QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("登录失败!"));
		 return false;
    }
	 trade_proxy_.QueryData(trade_client_id_, (int)TypeQueryCategory::SHARED_HOLDER_CODE, result.data(), error);
	 if( strlen(error) != 0 )
	 { 
		 QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("查询股权代码失败!"));
		 return false;
	 }
	 qDebug() << QString::fromLocal8Bit(result.data()) << "\n";
	  
	 trade_proxy_.SetupAccountInfo(result.data());
	 // end --------------------------------

	 
#if 1
	//normal_timer_.start(2000);

   BOOL ret = InstallLaunchEv(CallBackFunc, const_cast<char*>(target_win_title_tag_.c_str()));
#else

#endif
	return true;
}

// called by thread, so can't set window directory
void FlashingOrder::HandleOrder(bool is_buy, const std::string &stock_name)
{
	assert(ticker_);
	assert(msg_win_);
#if 1
	Buffer result(1024);
	char error[1024] = {0};

	float price = 0.0;  
	
	int quantity = 100; // ndedt:
	auto iter = stock_name2code_.find(stock_name);
	if( iter == stock_name2code_.end() )
	{
		qDebug() << "FlashingOrder::HandleOrder cant find " << stock_name.c_str() << "\n";
		return;
	}
	QuotesData quote_data;
	if( !ticker_->GetQuotes(const_cast<char*>(iter->second.c_str()), quote_data) )
	{
		qDebug() << "FlashingOrder::HandleOrder GetQuotes fail " << stock_name.c_str() << "\n";
		return;
	} 
    double target_price = 0.0;
    switch(quote_level_)
    {
        case TypeQuoteLevel::PRICE_BUYSELL_1: target_price = (is_buy ? quote_data.price_s_1 : quote_data.price_b_1); break;
        case TypeQuoteLevel::PRICE_BUYSELL_2: target_price = (is_buy ? quote_data.price_s_2 : quote_data.price_b_2); break;
        case TypeQuoteLevel::PRICE_BUYSELL_3: target_price = (is_buy ? quote_data.price_s_3 : quote_data.price_b_3); break;
        case TypeQuoteLevel::PRICE_BUYSELL_4: target_price = (is_buy ? quote_data.price_s_4 : quote_data.price_b_4); break;
        case TypeQuoteLevel::PRICE_BUYSELL_5: target_price = (is_buy ? quote_data.price_s_5 : quote_data.price_b_5); break;
        default: target_price = quote_data.cur_price; break;
    }
	trade_proxy_.SendOrder(trade_client_id_
		, (is_buy ? (int)TypeOrderCategory::BUY : (int)TypeOrderCategory::SELL)
		, 0
		, const_cast<T_AccountData *>( trade_proxy_.account_data( GetStockMarketType(iter->second.c_str()) ) )
		->shared_holder_code
		, const_cast<char*>(iter->second.c_str())
		, target_price
		, (is_buy ? qty_buy_ : qty_sell_)
		, result.data()
		, error); 

    char buf[2048] = {0};
    sprintf(buf, "%s 股票:%s 数目:%d %s %s!", (is_buy ? "买入" : "卖出")
        , stock_name.c_str(), (is_buy ? qty_buy_ : qty_sell_), (strlen(error) == 0 ? "成功" : "失败"), error);
     
    EmitShowMsgSig(QString::fromLocal8Bit("闪电交易助手提示"), QString::fromLocal8Bit(buf));
#endif
}

bool FlashingOrder::GetWinTileAndStockName(QString& title, QString& stock_name)
{
	HWND wnd = GetForegroundWindow(); 
	//HWND wnd1 = FindWindow(NULL, NULL);  // not ok
	char buf[1024] = {0};
	GetWindowText(wnd, buf, sizeof(buf)); // "方正证券泉友通专业版V6.58 - [组合图-中国平安]" 
	qDebug() << "FlashingOrder::GetWinTileAndStockName wnd: "<< QString::fromLocal8Bit(buf) << "\n";
	 
	if( !strstr(buf, target_win_title_tag_.c_str()) )
		return false;
	title = QString::fromLocal8Bit(buf);

	std::string title_str = buf;   
	std::string key_str = "[组合图-";

	auto pos = title_str.find(key_str);
	if( std::string::npos == pos )
		return false; 

	stock_name = 
	QString::fromLocal8Bit(title_str.substr(pos + key_str.length(), title_str.length() - pos - key_str.length() - 1 ).c_str());
		 
	return stock_name.length() > 0;
}

void FlashingOrder::DoReadCfg()
{
    // -------read config file ------------
    QString iniFilePath = CONFIG_FILE_NAME;  
    QSettings settings(iniFilePath, QSettings::IniFormat);  
    QString app_title = QString::fromLocal8Bit(settings.value("config/broker_app_title").toString().toLatin1().constData());
    qDebug() << app_title << "\n";
    target_win_title_tag_ = app_title.toLocal8Bit();
    qty_buy_ = settings.value("config/buy_quantity").toInt();
    qty_sell_ = settings.value("config/sell_quantity").toInt();
    quote_level_ = (TypeQuoteLevel)settings.value("config/quote_level").toInt();
     
    ui.spinBox_buy_quantity->setValue(qty_buy_);
    ui.spinBox_sell_quantity->setValue(qty_sell_);
    ui.combox_price_level->setCurrentText(ToQString(static_cast<TypeQuoteLevel>(quote_level_)));
    //-----------end-----------------
}

void FlashingOrder::DoSaveCfg()
{
    QString iniFilePath = CONFIG_FILE_NAME;  
    QSettings settings(iniFilePath, QSettings::IniFormat);  
    qty_buy_ = ui.spinBox_buy_quantity->value();
    settings.setValue("config/buy_quantity", qty_buy_);
    qty_sell_ = ui.spinBox_sell_quantity->value();
    settings.setValue("config/sell_quantity", qty_sell_);
 
    quote_level_ = (TypeQuoteLevel)ui.combox_price_level->currentData().toInt();
    settings.setValue("config/quote_level", (int)quote_level_);

}
  
void FlashingOrder::DoKeySig(QString str)
{
	qDebug() << "Enter DoKeySig\n";
	key_sig_mutex_.lock();
	thread_.stock_name(str);
	key_sig_wait_cond_.wakeAll();
	key_sig_mutex_.unlock();
}

void FlashingOrder::set_key_sig(bool val)
{ 
	key_sig_mutex_.lock();
	key_sig_wait_cond_.wakeAll();
	key_sig_mutex_.unlock();
}
  
void FlashingOrder::DoShowMsgSig(QString title, QString content)
{
    msg_win_->ShowUI(title, content); 
}

#if 0
void FlashingOrder::DoNormalTimer()
{
#if 1
     HWND wnd = GetForegroundWindow(); 
    //HWND wnd1 = FindWindow(NULL, NULL);  // not ok
    char buf[1024] = {0};
    GetWindowText(wnd, buf, sizeof(buf)); // "方正证券泉友通专业版V6.58 - [组合图-中国平安]" 
    qDebug() << "FlashingOrder::DoNormalTimer wnd: "<< QString::fromLocal8Bit(buf) << "\n";

	if( strstr(buf, target_win_title_tag_.c_str()) )
		g_is_accept_order_ = true;
	else
		g_is_accept_order_ = false;

#else
	setlocale(LC_CTYPE, "chs");    

	DWORD targetPid = 0;    // 进程id    
	PROCESSENTRY32 pe;  // 进程信息    
	pe.dwSize = sizeof(PROCESSENTRY32);    
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 进程快照    
	if(!Process32First(hSnapshot, &pe)) // 得到第一个进程的快照    
		return;   

	bool is_find = false;
	do    
	{    
		if(!Process32Next(hSnapshot, &pe))    
			return;   
		is_find = strstr(pe.szExeFile, "TdxW.exe") != nullptr;
	} while( !is_find );

	//while (StrCmp(pe.szExeFile, L"QQ.exe"));  // 遍历进程直到找打目标进程    
	if( is_find )
	{
		targetPid = pe.th32ProcessID;    
		// wprintf(L"Find QQ.exe process: 0x%08X/n", qqPid);    
		//printf( "Find TdxW.exe process: 0x%08X/n", targetPid);  
		qDebug() << "Find TdxW.exe process: "<< targetPid << "\n";
		EnumWindows(EnumWindowCallBack, targetPid);   
	}
#endif
}
#endif