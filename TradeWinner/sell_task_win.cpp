#include "winner_win.h"

#include <qdebug.h>
#include <qmessagebox.h> 
//#include <QPixmap>
#include <QtGui/QStandardItemModel>
#include <QTextCodec>

#include <TLib/core/tsystem_utility_functions.h>
//#include "mythread.h"
#include "winner_app.h"

#include "strategy_task.h"
#include "breakdown_task.h"
#include "inflection_sell_task.h"
#include "follow_sell_task.h"
#include "batches_sell_task.h"

#include "stock_ticker.h"
#include "MySpinBox.h"
#include "HintList.h"
#include "message_win.h"

//
static const QString cst_str_breakout_sell = QString::fromLocal8Bit("破位卖出");
static const QString cst_str_inflection_sell = QString::fromLocal8Bit("拐点卖出");
static const QString cst_str_follow_sell = QString::fromLocal8Bit("跟踪止盈");
static const QString cst_str_batches_sell = QString::fromLocal8Bit("分批出货");

void WinnerWin::InitSellTaskWin()
{ 
    // reset some widget ------------
#if 1
    auto obj_name = ui.spinBox_quantity->objectName();
    const QRect geome_val = ui.spinBox_quantity->geometry();
    delete ui.spinBox_quantity;
    ui.spinBox_quantity = new MySpinBox(ui.wid_mid_line);
    ui.spinBox_quantity->setSingleStep(100);
    ui.spinBox_quantity->setObjectName(obj_name);
    ui.spinBox_quantity->setGeometry(geome_val);
    ui.spinBox_quantity->setMaximum(1000000000);

    ui.dbspbox_step_range->setDecimals(1);
    ui.dbspbox_step_range->setMaximum(100);
    ui.dbspbox_step_range->setMinimum(0.1);
#endif
    ui.spinBox_continue_time->setMaximum(5 * 60);
    //ui.spinBox_quantity->setMaximum(1000000000);

    ui.wid_retreat->hide();
	ui.wid_step_range->hide();

    ResetSellTabTaskTime();
    ChangeTabSellAssistantImg(TypeTask::BREAK_SELL);
    
    bool ret = QObject::connect(ui.combox_sell_type, SIGNAL(currentTextChanged(const QString&)), SLOT(DoSellTypeChanged(const QString&)));
    ret = QObject::connect(ui.le_stock, SIGNAL(editingFinished()), this, SLOT(DoLeStockEditingFinished()));
    ret = QObject::connect(ui.le_stock, SIGNAL(textChanged(const QString & )), this, SLOT(DoLeStockChanged(const QString &)));
    
    ret = QObject::connect(ui.pbtn_add_task, SIGNAL(clicked()), this, SLOT(DoAddTask())); 
    ret = ret; 
    //ret = QObject::connect(ui.spinBox_quantity, SIGNAL(editingFinished()), this, SLOT(DoSpboxQuantityEditingFinished())));
     
    ret = QObject::connect(ui.pbtn_all_quantity, SIGNAL(clicked()), this, SLOT(DoQueryPosition()));
    //----------------------------------
    ret = connect(ui.le_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    m_list_hint_ = new HintList(this, ui.le_stock);
    m_list_hint_->hide();
    ret = connect(m_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = connect(m_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));

    ret = connect(ui.dbspbox_alert_percent, SIGNAL(valueChanged(double)), this, SLOT(DoAlertPercentChanged(double)));
}

void WinnerWin::SetupSellTaskWin()
{
    ui.combox_sell_type->addItem(cst_str_breakout_sell);
    ui.combox_sell_type->addItem(cst_str_inflection_sell);
    ui.combox_sell_type->addItem(cst_str_follow_sell);
    ui.combox_sell_type->addItem(cst_str_batches_sell);

    ui.combox_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.combox_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
   
   
}

void WinnerWin::ChangeTabSellAssistantImg(TypeTask type)
{
    QPixmap  image;
    switch(type)
	{
	case TypeTask::BREAK_SELL: 
        image.load("./img/breakdown_sell.png");
		break;
	case TypeTask::INFLECTION_SELL: 
        image.load("./img/inflection_sell.png");
		break;
	case TypeTask::FOLLOW_SELL: 
        image.load("./img/follow_sell.png");
		break;
    case TypeTask::BATCHES_SELL: 
        image.load("./img/part_sell.png");
		break;
	default:
		assert(false);
	}
     
    //image.width();
    ui.label_show_pic->setPixmap(image);
    ui.label_show_pic->show();
}

void WinnerWin::FillSellTaskWin(TypeTask type, T_TaskInformation& info)
{
	switch(type)
	{
	case TypeTask::BREAK_SELL: 
        ui.combox_sell_type->setCurrentText(cst_str_breakout_sell);
		break;
	case TypeTask::INFLECTION_SELL: 
        ui.combox_sell_type->setCurrentText(cst_str_inflection_sell);
        ui.spinBox_retreat->setValue(info.rebounce);
		ui.cb_back_alert_trigger->setChecked(info.back_alert_trigger);
		break;
	case TypeTask::FOLLOW_SELL: 
        ui.combox_sell_type->setCurrentText(cst_str_follow_sell);
        ui.dbspbox_step_range->setValue(info.step);
		break;
    case TypeTask::BATCHES_SELL: 
        ui.combox_sell_type->setCurrentText(cst_str_batches_sell);
        ui.dbspbox_step_range->setValue(info.step); 
		break;
	default:
		assert(false);
	}
	 
	ui.le_stock->setText( QString("%1/%2").arg(info.stock.c_str()).arg(QString::fromLocal8Bit(info.stock_pinyin.c_str())) );
    ui.dbspbox_alert_price->setValue(info.alert_price);
    ui.spinBox_continue_time->setValue(info.continue_second);
    ui.spinBox_quantity->setValue(info.quantity);
    ui.combox_price_level->setCurrentText(ToQString(static_cast<TypeQuoteLevel>(info.target_price_level)));
    ui.timeEdit_begin->setTime(Int2Qtime(info.start_time));
    ui.timeEdit_end->setTime(Int2Qtime(info.end_time));

}

void WinnerWin::DoAddTask()
{
    static auto check_le_stock = [this](TypeTask type) ->bool
    {
       // check stock codes
		QString::SectionFlag flag = QString::SectionSkipEmpty;
		QString text_str;
		text_str = ui.le_stock->text().trimmed();
		QString stock_str = text_str.section('/', 0, 0, flag);
        if( stock_str.length() != 6 )
        {
			// todo: show erro info
            ui.le_stock->setFocus();
            return false;
        }  
        if( app_->db_moudle().IsTaskExists(app_->user_info().id, type, stock_str.toStdString()) )
        {  //QMessageBox::information(nullptr, "notice", QString::fromLocal8Bit("任务已经存在!"));
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("任务已经存在!"));
            return false;
        }
        auto start_time = ui.timeEdit_begin->time().toString("Hmmss").toInt();
        auto end_time = ui.timeEdit_end->time().toString("Hmmss").toInt();
            
        if( ui.dbspbox_alert_price->value() < 0.01 )
        {
            ui.dbspbox_alert_price->setFocus();
            return false;
        }
        if( ui.spinBox_quantity->value() < 100 )
        {
            ui.spinBox_quantity->setFocus();
            return false;
        }
        if( start_time >= end_time )
        {
            ui.timeEdit_begin->setFocus();
            return false;
        }
        return true;
    };

	static auto fill_common_ui = [this](std::shared_ptr<T_TaskInformation> &task_info)
	{
        /*QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
        assert(utf8Codec);*/
		QString::SectionFlag flag = QString::SectionSkipEmpty;
		QString stock_str = ui.le_stock->text().trimmed();
		QString stock_pinyin = stock_str.section('/', 1, 1, flag);
		task_info->stock = stock_str.section('/', 0, 0, flag).toLocal8Bit();
		task_info->stock_pinyin = stock_str.section('/', 1, 1, flag).toLocal8Bit();
         
		task_info->alert_price = ui.dbspbox_alert_price->value(); 
        task_info->back_alert_trigger = ui.cb_back_alert_trigger->isChecked();
        task_info->continue_second = ui.spinBox_continue_time->value();

		task_info->quantity = ui.spinBox_quantity->value();
        task_info->target_price_level = ui.combox_price_level->currentData().toInt();
        task_info->start_time = ui.timeEdit_begin->time().toString("Hmmss").toInt();
        task_info->end_time = ui.timeEdit_end->time().toString("Hmmss").toInt();
        task_info->state = 1;		
	};

    if( ui.combox_sell_type->currentText() == cst_str_breakout_sell )
    {
        if( !check_le_stock(TypeTask::BREAK_SELL) )
            return;
         
        ui.dbspbox_alert_price->value();
        //  BreakDownTask  info setup 
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::BREAK_SELL; 
		fill_common_ui(task_info); 
        
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto break_down_task = std::make_shared<BreakDownTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(break_down_task));

        app_->ticker_strand().PostTask([break_down_task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(break_down_task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示!"), QString::fromLocal8Bit("破位卖出任务添加成功!"));
        app_->AppendLog2Ui("添加破位卖出任务 : %d 成功\n", task_info->id);

    }else if( ui.combox_sell_type->currentText() == cst_str_inflection_sell )
    {
        if( !check_le_stock(TypeTask::INFLECTION_SELL) )
            return;
         
        ui.dbspbox_alert_price->value();
        //  Inflection sell task  info setup 
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::INFLECTION_SELL;

        fill_common_ui(task_info); 
        task_info->rebounce = ui.spinBox_retreat->value();
         
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto inflection_sell_task = std::make_shared<InflectionSellTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(inflection_sell_task));

        app_->ticker_strand().PostTask([inflection_sell_task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(inflection_sell_task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示!"), QString::fromLocal8Bit("拐点卖出任务添加成功!"));
        app_->AppendLog2Ui("添加拐点卖出任务 : %d 成功\n", task_info->id);

    }else if( ui.combox_sell_type->currentText() == cst_str_follow_sell )
    {
        if( !check_le_stock(TypeTask::FOLLOW_SELL) )
            return;
         
        ui.dbspbox_alert_price->value();
        //  Inflection sell task  info setup 
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::FOLLOW_SELL; 
		fill_common_ui(task_info); 
        task_info->step = ui.dbspbox_step_range->value();
		 
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto follow_sell_task = std::make_shared<FollowSellTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(follow_sell_task));

        app_->ticker_strand().PostTask([follow_sell_task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(follow_sell_task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示!"), QString::fromLocal8Bit("跟踪止盈任务添加成功!"));
        app_->AppendLog2Ui("添加跟踪止盈任务 : %d 成功\n", task_info->id);

    }else if( ui.combox_sell_type->currentText() == cst_str_batches_sell )
    {
        if( !check_le_stock(TypeTask::BATCHES_SELL) )
            return;
          
        auto task_info = std::make_shared<T_TaskInformation>();
        task_info->type = TypeTask::BATCHES_SELL;

		fill_common_ui(task_info);
        task_info->step = ui.dbspbox_step_range->value();
		 
        if( !app_->db_moudle().AddTaskInfo(task_info) )
        {
            // log error
            return;
        }
        app_->AppendTaskInfo(task_info->id, task_info);
            
        auto task = std::make_shared<BatchesSellTask>(*task_info, this->app_);
        app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(task));

        app_->ticker_strand().PostTask([task, this]()
        {
            app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(task));
        });
        // add to task list ui
        InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示!"), QString::fromLocal8Bit("分批卖出任务添加成功!"));
        app_->AppendLog2Ui("添加分批卖出任务 : %d 成功\n", task_info->id);
    }

    ui.timeEdit_begin->setTime(QTime(9, 30, 0));
    ui.timeEdit_end->setTime(QTime(15, 00, 0));
       
}

void WinnerWin::DoAlertPercentChanged(double val)
{
    if( cur_price_ > 0 )
    {
        if( ui.combox_sell_type->currentText() == cst_str_breakout_sell )
        {
            if( val < 100 )
                ui.dbspbox_alert_price->setValue(cur_price_ * (100 - val ) / 100);
            else
                ui.dbspbox_alert_price->setValue(0.0);
        }
        else if( ui.combox_sell_type->currentText() == cst_str_inflection_sell 
            || ui.combox_sell_type->currentText() == cst_str_follow_sell
            || ui.combox_sell_type->currentText() == cst_str_batches_sell
            ) 
            ui.dbspbox_alert_price->setValue(cur_price_ * (100 + val ) / 100);
         
    }

}

void WinnerWin::DoSellTypeChanged(const QString&str)
{
#if 0
	static const QRect label_continue_time_rect = ui.label_continue_time->geometry();
	static const QRect spinBox_continue_time_rect = ui.spinBox_continue_time->geometry();
	static const QRect label_continue_sec_rect = ui.label_continue_sec->geometry();
	 
    static const QRect label_sell_num_rect = ui.label_sell_num->geometry();
    static const QRect spinBox_quantity_rect = ui.spinBox_quantity->geometry();

    static const QRect label_pt_max_num_rect = ui.label_pt_max_num->geometry();
    static const QRect label_total_num_rect = ui.label_total_num->geometry();
    static const QRect pbtn_all_quantity_rect = ui.pbtn_all_quantity->geometry();

    static const QRect label_price_level_rect = ui.label_price_level->geometry();
    static const QRect combox_price_level_rect = ui.combox_price_level->geometry();

    static const QRect label_monitor_rect = ui.label_monitor->geometry();
    static const QRect timeEdit_begin_rect = ui.timeEdit_begin->geometry();
    static const QRect label_time_mid_rect = ui.label_time_mid->geometry();
    static const QRect timeEdit_end_rect = ui.timeEdit_end->geometry();
#endif
    static const QRect wid_mid_line_rect = ui.wid_mid_line->geometry();
	static const QRect wid_retreat_rect = ui.wid_retreat->geometry();
	static const QRect wid_step_range_rect = ui.wid_step_range->geometry();
     
    static auto ajust_pos_y = [](QWidget* wid, int y)
    {
        wid->setGeometry(wid->geometry().x(), y, wid->geometry().width(), wid->geometry().height());
    };

    

	m_list_hint_->hide();
    ResetSellTabTaskTime();

   if( str == cst_str_inflection_sell )
   {  
       ChangeTabSellAssistantImg(TypeTask::INFLECTION_SELL);

	   ui.label_alert_price->setText(QString::fromLocal8Bit("股票高于:"));

       ajust_pos_y(ui.wid_retreat, wid_mid_line_rect.y());
       ajust_pos_y(ui.wid_mid_line, wid_mid_line_rect.y() + ui.wid_retreat->geometry().height());
       
	   ui.wid_step_range->setGeometry(wid_step_range_rect);

	   ui.wid_step_range->hide();
	   ui.wid_retreat->show();

   }else if( str == cst_str_breakout_sell )
   { 
    ChangeTabSellAssistantImg(TypeTask::BREAK_SELL);
    ui.label_alert_price->setText(QString::fromLocal8Bit("股票低于:"));
    ui.wid_mid_line->setGeometry(wid_mid_line_rect);

    ui.wid_retreat->setGeometry(wid_retreat_rect);
	ui.wid_step_range->setGeometry(wid_step_range_rect);
 
	ui.wid_retreat->hide();
	ui.wid_step_range->hide();

   }else if( str == cst_str_follow_sell )
   {
       ChangeTabSellAssistantImg(TypeTask::FOLLOW_SELL);
	   ui.label_alert_price->setText(QString::fromLocal8Bit("股票高于:"));
       ui.label_step_range->setText(QString::fromLocal8Bit("阶梯幅度"));

	   ajust_pos_y(ui.wid_step_range, wid_mid_line_rect.y());
       ajust_pos_y(ui.wid_mid_line, wid_mid_line_rect.y() + ui.wid_retreat->geometry().height());
        
	   ui.wid_retreat->setGeometry(wid_retreat_rect);

	   ui.wid_retreat->hide();
	   ui.wid_step_range->show();
   }else if( str == cst_str_batches_sell )
   {
       ChangeTabSellAssistantImg(TypeTask::BATCHES_SELL);
	   ui.label_alert_price->setText(QString::fromLocal8Bit("股票高于:"));
       ui.label_step_range->setText(QString::fromLocal8Bit("以后每涨"));

	   ajust_pos_y(ui.wid_step_range, wid_mid_line_rect.y());
       ajust_pos_y(ui.wid_mid_line, wid_mid_line_rect.y() + ui.wid_retreat->geometry().height());
        
	   ui.wid_retreat->setGeometry(wid_retreat_rect);

	   ui.wid_retreat->hide();
	   ui.wid_step_range->show();
   }
}

void WinnerWin::ResetSellTabTaskTime()
{
    ui.timeEdit_begin->setTime(QTime(9, 30, 0));
    ui.timeEdit_end->setTime(QTime(15, 00, 0));
}
