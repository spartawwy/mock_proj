#include "winner_win.h"

#include <qdebug.h>
#include <qmessagebox.h> 
#include <QTextCodec> 
#include <QStandardItemModel>

//#include <QtGui/QStandardItemModel>

#include <TLib/core/tsystem_utility_functions.h>
//#include <boost/lexical_cast.hpp>

#include "winner_app.h"

#include "stock_ticker.h"
#include "MySpinBox.h"
#include "HintList.h"
#include "message_win.h"

#include "breakup_buy_task.h"
#include "inflection_buy_task.h"
#include "batches_buy_task.h"

//
 
static const QString cst_str_direct_buy = QString::fromLocal8Bit("直接买入");
static const QString cst_str_inflection_buy = QString::fromLocal8Bit("拐点买入");
static const QString cst_str_breakout_buy = QString::fromLocal8Bit("突破买入");
static const QString cst_str_batches_buy = QString::fromLocal8Bit("分批买入");

void WinnerWin::InitBuyTaskWin()
{
    // reset some widget ------------
#if 1
    auto obj_name = ui.spinBox_buytask_quantity->objectName();
    const QRect geome_val_buy = ui.spinBox_buytask_quantity->geometry();
    delete ui.spinBox_buytask_quantity;
    ui.spinBox_buytask_quantity = new MySpinBox(ui.wid_bt_mid_line);
    ui.spinBox_buytask_quantity->setSingleStep(100);
    ui.spinBox_buytask_quantity->setObjectName(obj_name);
    ui.spinBox_buytask_quantity->setGeometry(geome_val_buy);
    ui.spinBox_buytask_quantity->setMaximum(1000000000);

    ui.dbspbox_bt_step_range->setDecimals(1);
    ui.dbspbox_bt_step_range->setMaximum(100);
    ui.dbspbox_bt_step_range->setMinimum(0.1);

#endif
    ui.combox_buy_type->addItem(cst_str_direct_buy);
    ui.combox_buy_type->addItem(cst_str_inflection_buy);
    ui.combox_buy_type->addItem(cst_str_breakout_buy);
    ui.combox_buy_type->addItem(cst_str_batches_buy);
     
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.combox_bt_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
    
	ui.spinBox_buytask_continue_time->setMaximum(5 * 60);
    //ui.spinBox_quantity->setMaximum(1000000000);

    ui.wid_bt_retreat->hide();
	ui.wid_bt_step_range->hide();
	ui.wid_ticker->hide();

    ResetBuyTabTaskTime();
    ChangeTabBuyAssistantImg(TypeTask::INFLECTION_BUY);

    //----------------------------------
    bool ret = QObject::connect(ui.le_buytask_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    m_bt_list_hint_ = new HintList(this, ui.le_buytask_stock);
    m_bt_list_hint_->hide();
    ret = connect(m_bt_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = connect(m_bt_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));
     
    ret = QObject::connect(ui.dbspbox_buytask_alert_percent, SIGNAL(valueChanged(double)), this, SLOT(DoBuyAlertPercentChanged(double)));

    DoBuyTypeChanged(cst_str_direct_buy);
    ret = QObject::connect(ui.combox_buy_type, SIGNAL(currentTextChanged(const QString&)), SLOT(DoBuyTypeChanged(const QString&)));
 
    ret = QObject::connect(ui.pbtn_buytask_all_quantity, SIGNAL(clicked()), this, SLOT(DoQueryQtyCanBuy()));
    ret = QObject::connect(ui.pbtn_add_buytask, SIGNAL(clicked()), this, SLOT(DoAddBuyTask())); 
	ret = QObject::connect(ui.pbtn_buy_stock, SIGNAL(clicked()), this, SLOT(DoBuyStock()));
}


void WinnerWin::ChangeTabBuyAssistantImg(TypeTask type)
{
    QPixmap  image;
    switch(type)
	{
    case TypeTask::BATCHES_BUY: 
        image.load("./img/part_buy.png");
		break;
    case TypeTask::INFLECTION_BUY: 
        image.load("./img/inflection_buy.png");
		break;
    case TypeTask::BREAKUP_BUY: 
        image.load("./img/breakup_buy.png");
		break; 
	case TypeTask::DERECT_BUY:
		break;
	default:
		assert(false);
	}
    if( type != TypeTask::DERECT_BUY )
	{ 
		ui.label_buytask_show_pic->setPixmap(image);
		ui.label_buytask_show_pic->show();
	}else
	{
		ui.label_buytask_show_pic->hide();
	}
}

void WinnerWin::FillBuyTaskWin(TypeTask type, T_TaskInformation& info)
{
    switch(type)
	{
    case TypeTask::INFLECTION_BUY: 
        ui.combox_buy_type->setCurrentText(cst_str_inflection_buy);
        ui.spinBox_bt_retreat->setValue(info.rebounce);
		ui.cb_bt_back_alert_trigger->setChecked(info.back_alert_trigger);
		break;
	case TypeTask::BREAKUP_BUY: 
        ui.combox_buy_type->setCurrentText(cst_str_breakout_buy);
		break;
	case TypeTask::BATCHES_BUY: 
        ui.combox_buy_type->setCurrentText(cst_str_batches_buy);
        ui.dbspbox_bt_step_range->setValue(info.step);
        ui.spinBox_buytask_times->setValue(info.bs_times);
		break;
	case TypeTask::DERECT_BUY:
		break;
	default:
		assert(false);
	}
	 
	ui.le_buytask_stock->setText( QString("%1/%2").arg(info.stock.c_str()).arg(QString::fromLocal8Bit(info.stock_pinyin.c_str())) );
    ui.dbspbox_buytask_alert_price->setValue(info.alert_price);
    ui.spinBox_buytask_continue_time->setValue(info.continue_second);
    ui.spinBox_buytask_quantity->setValue(info.quantity);
    ui.combox_bt_price_level->setCurrentText(ToQString(static_cast<TypeQuoteLevel>(info.target_price_level)));
    ui.timeEdit_buytask_begin->setTime(Int2Qtime(info.start_time));
    ui.timeEdit_buytask_end->setTime(Int2Qtime(info.end_time));
}

void WinnerWin::DoBuyAlertPercentChanged(double val)
{
    if( buytask_cur_price_ > 0.0 )
    {
        if( ui.combox_buy_type->currentText() == cst_str_breakout_buy )
            ui.dbspbox_buytask_alert_price->setValue(buytask_cur_price_ * (100 + val ) / 100);
        else if( ui.combox_buy_type->currentText() == cst_str_inflection_buy 
            || ui.combox_buy_type->currentText() == cst_str_batches_buy
            )
        {
            if( val < 100 )
                ui.dbspbox_buytask_alert_price->setValue(buytask_cur_price_ * (100 - val ) / 100);
            else
                ui.dbspbox_buytask_alert_price->setValue(0.0);
        }
    }

}

void WinnerWin::DoBuyTypeChanged(const QString&str)
{ 
    static const QRect wid_bt_mid_line_rect = ui.wid_bt_mid_line->geometry();
	static const QRect wid_bt_retreat_rect = ui.wid_bt_retreat->geometry();
	static const QRect wid_bt_step_range_rect = ui.wid_bt_step_range->geometry();
     
    static auto ajust_pos_y = [](QWidget* wid, int y)
    {
        wid->setGeometry(wid->geometry().x(), y, wid->geometry().width(), wid->geometry().height());
    };

	m_bt_list_hint_->hide();
	if( str == cst_str_direct_buy )
	{
		ui.label_buytask_show_pic->hide();
		ui.wid_buytask_price->hide();
		ui.wid_bt_retreat->hide();
		ui.wid_bt_step_range->hide();
		ui.wid_buytask_continue_time->hide();
		ui.wid_buytask_time->hide();
		ui.pbtn_add_buytask->hide();
		 
		ui.pbtn_buy_stock->show();
	}else
	{
		ui.label_buytask_show_pic->show();
		ui.wid_buytask_price->show();
		ui.wid_bt_retreat->show();
		ui.wid_bt_step_range->show();
		ui.wid_buytask_continue_time->show();
		ui.wid_buytask_time->show();
		ui.pbtn_add_buytask->show();
		 
		ui.pbtn_buy_stock->hide();
	}

    ResetBuyTabTaskTime();

   if( str == cst_str_inflection_buy )
   {  
	   ChangeTabBuyAssistantImg(TypeTask::INFLECTION_BUY);

	   ui.label_buytask_alert_price->setText(QString::fromLocal8Bit("股票低于:"));

       ajust_pos_y(ui.wid_bt_retreat, wid_bt_mid_line_rect.y());
       ajust_pos_y(ui.wid_bt_mid_line, wid_bt_mid_line_rect.y() + ui.wid_bt_retreat->geometry().height());
       
	   ui.wid_bt_step_range->setGeometry(wid_bt_step_range_rect);

	   ui.wid_bt_step_range->hide();
	   ui.wid_bt_retreat->show();

   }else if( str == cst_str_breakout_buy )
   { 
	   ChangeTabBuyAssistantImg(TypeTask::BREAKUP_BUY); 

	    ui.label_buytask_alert_price->setText(QString::fromLocal8Bit("股票高于:"));
		ui.wid_bt_mid_line->setGeometry(wid_bt_mid_line_rect);

		ui.wid_bt_retreat->setGeometry(wid_bt_retreat_rect);
		ui.wid_bt_step_range->setGeometry(wid_bt_step_range_rect);
 
		ui.wid_bt_retreat->hide();
		ui.wid_bt_step_range->hide();

   }else if( str == cst_str_batches_buy )
   {
	   ChangeTabBuyAssistantImg(TypeTask::BATCHES_BUY);
	   ui.label_buytask_alert_price->setText(QString::fromLocal8Bit("股票低于:"));

	   ajust_pos_y(ui.wid_bt_step_range, wid_bt_mid_line_rect.y()); 
       ajust_pos_y(ui.wid_bt_mid_line, wid_bt_mid_line_rect.y() + wid_bt_step_range_rect.height());
        
	   ui.wid_retreat->setGeometry(wid_bt_retreat_rect);

	   ui.wid_bt_retreat->hide();
	   ui.wid_bt_step_range->show();
 
   }
}

void WinnerWin::DoAddBuyTask()
{
    static auto check_le_stock = [this](TypeTask type) ->bool
    {
       // check stock codes
		QString::SectionFlag flag = QString::SectionSkipEmpty;
		QString text_str = ui.le_buytask_stock->text().trimmed();
		QString stock_str = text_str.section('/', 0, 0, flag);
        if( stock_str.length() != 6 )
        {
			// todo: show erro info
            ui.le_buytask_stock->setFocus();
            return false;
        } 

        if( app_->db_moudle().IsTaskExists(app_->user_info().id, type, stock_str.toStdString()) )
        {
            //QMessageBox::information(nullptr, "notice", QString::fromLocal8Bit("任务已经存在!"));
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("任务已经存在!"));
            return false;
        }
        auto start_time = ui.timeEdit_buytask_begin->time().toString("Hmmss").toInt();
        auto end_time = ui.timeEdit_buytask_end->time().toString("Hmmss").toInt();
            
        if( ui.dbspbox_buytask_alert_price->value() < 0.01 )
        {
            ui.dbspbox_buytask_alert_price->setFocus();
            return false;
        }
        if( ui.spinBox_buytask_quantity->value() < 100 )
        {
            ui.spinBox_buytask_quantity->setFocus();
            return false;
        }
        if( start_time >= end_time )
        {
            ui.timeEdit_buytask_begin->setFocus();
            return false;
        }
        return true;
    };
     
	static auto fill_common_ui = [this](std::shared_ptr<T_TaskInformation> &task_info)
	{
        /*QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
        assert(utf8Codec);*/
		QString::SectionFlag flag = QString::SectionSkipEmpty;
		QString stock_str = ui.le_buytask_stock->text().trimmed();
		QString stock_pinyin = stock_str.section('/', 1, 1, flag);
		task_info->stock = stock_str.section('/', 0, 0, flag).toLocal8Bit();
        task_info->stock_pinyin = stock_str.section('/', 1, 1, flag).toLocal8Bit().data();
        /*QByteArray bytes = utf8Codec->fromUnicode(stock_str.section('/', 1, 1, flag));
        task_info->stock_pinyin = bytes.data();
*/
		task_info->alert_price = ui.dbspbox_buytask_alert_price->value(); 
        task_info->back_alert_trigger = ui.cb_bt_back_alert_trigger->isChecked();
        task_info->continue_second = ui.spinBox_buytask_continue_time->value();

		task_info->quantity = ui.spinBox_buytask_quantity->value();
        task_info->target_price_level = ui.combox_bt_price_level->currentData().toInt();
        task_info->start_time = ui.timeEdit_buytask_begin->time().toString("Hmmss").toInt();
        task_info->end_time = ui.timeEdit_buytask_end->time().toString("Hmmss").toInt();
        task_info->state = 1;		
	};

    if( ui.combox_buy_type->currentText() == cst_str_breakout_buy )
    {
        if( !check_le_stock(TypeTask::BREAKUP_BUY) )
            return;
          
        //  BreakDownTask  info setup 
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::BREAKUP_BUY; 
		fill_common_ui(task_info); 
        
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto breakup_buy_task = std::make_shared<BreakUpBuyTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(breakup_buy_task));

        app_->ticker_strand().PostTask([breakup_buy_task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(breakup_buy_task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("突破买入任务添加成功!"));
        app_->AppendLog2Ui("添加突破买入任务 : %d 成功\n", task_info->id);

    }else if( ui.combox_buy_type->currentText() == cst_str_inflection_buy )
    {
        if( !check_le_stock(TypeTask::INFLECTION_BUY) )
            return;
#if 1
        ui.dbspbox_buytask_alert_price->value();
        //  Inflection sell task  info setup 
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::INFLECTION_BUY;

        fill_common_ui(task_info);
         
        task_info->rebounce = ui.spinBox_bt_retreat->value();
        //task_info->state = 1;		
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto inflection_buy_task = std::make_shared<InflectionBuyTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(inflection_buy_task));

        app_->ticker_strand().PostTask([inflection_buy_task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(inflection_buy_task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("拐点买入任务添加成功!"));
        app_->AppendLog2Ui("添加拐点买入任务 : %d 成功!\n", task_info->id);
         
#endif
    }else if( ui.combox_buy_type->currentText() == cst_str_batches_buy )
    {
        if( !check_le_stock(TypeTask::BATCHES_BUY) )
            return;
  
        //  Inflection sell task  info setup 
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::BATCHES_BUY;

        fill_common_ui(task_info);
          
        task_info->step = ui.dbspbox_bt_step_range->value();
        task_info->bs_times = ui.spinBox_buytask_times->value();

        //task_info->state = 1;		
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto task = std::make_shared<BatchesBuyTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(task));

        app_->ticker_strand().PostTask([task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("分批买入任务添加成功!"));
        app_->AppendLog2Ui("添加分批买入任务 : %d 成功!\n", task_info->id);

	}
}

void WinnerWin::DoQueryQtyCanBuy()
{
    auto capital = app_->QueryCapital();
    double price = 0.0;
    if( ui.combox_buy_type->currentText() == cst_str_direct_buy )
    { 
        QString::SectionFlag flag = QString::SectionSkipEmpty;
        QString stock_str = ui.le_buytask_stock->text().trimmed(); 

        std::string std_stock = stock_str.section('/', 0, 0, flag).toLocal8Bit().data(); 
        if( std_stock.size() != 6 || !IsStrNum(std_stock) )
            return;
         
       /* char result[1024] = {0};
        char error_info[1024] = {0};*/
        char *stock_codes[1] = {0};
        stock_codes[0] = const_cast<char*>(std_stock.c_str());
        
        TCodeMapQuotesData ret_quotes_data; 
        if( !app_->stock_ticker().GetQuoteDatas(stock_codes, 1, ret_quotes_data) )
            return;
        auto iter = ret_quotes_data.find(stock_codes[0]);
        if( iter == ret_quotes_data.end() )
            return;
        if( iter->second->cur_price < 0.0001 )
            return;

        if( iter->second->price_s_1 > 0.0001 )
            price = iter->second->price_s_1;
        else if( iter->second->price_b_1 > 0.0001 )
            price = iter->second->price_b_1;
        else
            return;
    }
    else
        price = ui.dbspbox_buytask_alert_price->value();

    if( price < 0 || Equal(price, 0.0) )
        return;

    auto val = capital.available / (price * 100);
    if( val < 1.0 )
        return;
    int handles = (int)(val);

    ui.spinBox_buytask_quantity->setValue(handles * 100);
    ui.label_buytask_total_num->setText( QString("%1").arg(handles * 100) );
}

double GetQuoteTargetPrice(TypeQuoteLevel quote_level, const QuotesData& data, bool is_buy)
{
    switch(quote_level)
    {
    case TypeQuoteLevel::PRICE_CUR: return data.cur_price;
         
    case TypeQuoteLevel::PRICE_BUYSELL_1: return is_buy ? data.price_s_1 : data.price_b_1;
    case TypeQuoteLevel::PRICE_BUYSELL_2: return is_buy ? data.price_s_2 : data.price_b_2;
    case TypeQuoteLevel::PRICE_BUYSELL_3: return is_buy ? data.price_s_3 : data.price_b_3;
    case TypeQuoteLevel::PRICE_BUYSELL_4: return is_buy ? data.price_s_4 : data.price_b_4;
    case TypeQuoteLevel::PRICE_BUYSELL_5: return is_buy ? data.price_s_5 : data.price_b_5;
    default: return data.cur_price; 
    }
}

void WinnerWin::DoBuyStock()
{
	static auto add_fill2db = [this](std::string &stock, std::string &stock_pinyin, double price, double qty, bool is_buy)
	{
		T_FillItem fill_item;
		fill_item.user_id = app_->user_info().id;
		auto date_time = CurrentDateIntTime();

		if( !app_->exchange_calendar().IsTradeDate(std::get<0>(date_time)) )
			fill_item.date = app_->exchange_calendar().NextTradeDate(std::get<0>(date_time), 1);
		else
			fill_item.date = std::get<0>(date_time);
		fill_item.time_stamp = std::get<1>(date_time);
		fill_item.stock = stock;
		fill_item.pinyin = stock_pinyin;
		fill_item.price = price;
		fill_item.quantity = qty;
		fill_item.is_buy = is_buy;
		fill_item.amount = price * qty;
		fill_item.fee = CaculateFee(price * qty, is_buy);
		app_->db_moudle().AddFillRecord(fill_item);
	};


	QString::SectionFlag flag = QString::SectionSkipEmpty;
	QString stock_str = ui.le_buytask_stock->text().trimmed();
	QString stock_pinyin = stock_str.section('/', 1, 1, flag);

	std::string std_stock = stock_str.section('/', 0, 0, flag).toLocal8Bit().data();
    std::string std_stock_pinyin = stock_str.section('/', 1, 1, flag).toLocal8Bit().data();
	
	if( std_stock.size() != 6 || !IsStrNum(std_stock) )
	{
		app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("股票代码有误!"));
		ui.le_buytask_stock->setFocus();
		return;
	}

	auto market_type = GetStockMarketType(std_stock);
	 
	char result[1024] = {0};
    char error_info[1024] = {0};
	            
	int qty = ui.spinBox_buytask_quantity->value(); 
	if( qty < 100 )
	{
		app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("股票数量有误!"));
		ui.spinBox_buytask_quantity->setFocus();
		return;
	}
    // to choice price to buy
    auto price = 0.0;
	char *stock_codes[1] = {0};
	stock_codes[0] = const_cast<char*>(std_stock.c_str());
	short count = 1;
	TCodeMapQuotesData ret_quotes_data; 
	if( !app_->stock_ticker().GetQuoteDatas(stock_codes, 1, ret_quotes_data)
		|| ret_quotes_data.find(stock_codes[0]) == ret_quotes_data.end() )
	{
		app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无法获取此股报价!"));
		ui.le_buytask_stock->setFocus();
		return;
	}

    char buf[1024] = {0};
	TypeQuoteLevel quote_level = static_cast<TypeQuoteLevel>(ui.combox_bt_price_level->currentData().toInt());
	price = GetQuoteTargetPrice(quote_level, *ret_quotes_data[std_stock], true);
    if( price < 0.0001 )
    {
        sprintf(buf, "股票:%s 无对应目标档价格!", stock_str.toLocal8Bit().data());
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit(buf));
        return;
    }
	// buy the stock
    this->app_->trade_agent().SendOrder(this->app_->trade_client_id(), (int)TypeOrderCategory::BUY, 0
		, const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type))->shared_holder_code, const_cast<char*>(std_stock.c_str())
        , price, qty
        , result, error_info); 

	
	if( strlen(error_info) == 0 )
	{ 
		sprintf(buf, "买入%s %d股 价格:%.2f 成功!", stock_str.toLocal8Bit().data(), qty, price);
		add_fill2db(std_stock, std_stock_pinyin, price, qty, true);
	}else
	{
		sprintf(buf, "买入%s %d股 价格:%.2f 失败: %s!", stock_str.toLocal8Bit().data(), qty, price, error_info);
	}
	app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit(buf));
	app_->AppendLog2Ui(buf);
}

void WinnerWin::ResetBuyTabTaskTime()
{
    ui.timeEdit_buytask_begin->setTime(QTime(9, 30, 0));
    ui.timeEdit_buytask_end->setTime(QTime(15, 00, 0));
}
 
