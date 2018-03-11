#include "winner_win.h"

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"

#include "stock_ticker.h"
#include "strategy_task.h"
#include "equal_section_task.h"

void WinnerWin::InitEqSectionTaskWin()
{
    // reset some widget ------------

     
    ui.dbspbox_eqsec_raise_percent->setDecimals(1);
    ui.dbspbox_eqsec_raise_percent->setMaximum(100);
    ui.dbspbox_eqsec_raise_percent->setMinimum(0.1);

	ui.dbspbox_eqsec_fall_percent->setDecimals(1);
    ui.dbspbox_eqsec_fall_percent->setMaximum(100);
    ui.dbspbox_eqsec_fall_percent->setMinimum(0.1);

	ui.spinBox_eqsec_rebounce->setDecimals(1);
	ui.spinBox_eqsec_rebounce->setMaximum(100);
	ui.spinBox_eqsec_rebounce->setMinimum(0.0);
#if 1
	auto obj_name = ui.spinBox_eqsec_quantity->objectName();
    const QRect geome_val = ui.spinBox_eqsec_quantity->geometry();
    delete ui.spinBox_eqsec_quantity;
    ui.spinBox_eqsec_quantity = new MySpinBox(ui.tab_eq_section_trd);
    ui.spinBox_eqsec_quantity->setSingleStep(100);
    ui.spinBox_eqsec_quantity->setObjectName(obj_name);
    ui.spinBox_eqsec_quantity->setGeometry(geome_val);
    ui.spinBox_eqsec_quantity->setMaximum(1000000000);
#endif
    ui.dbspbox_eqsec_max_price->setValue(EQSEC_MAX_STOP_PRICE);
    ui.dbspbox_eqsec_min_price->setValue(EQSEC_MIN_CLEAR_PRICE);
     
	ui.dbspbox_eqsec_max_price->setDisabled(true);
	ui.dbspbox_eqsec_min_price->setDisabled(true);

    ui.spinBox_max_qty->setValue(EQSEC_MAX_POSITION);
    ui.spinBox_min_qty->setValue(EQSEC_MIN_POSITION);
    ui.spinBox_max_qty->setDisabled(true);
	ui.spinBox_min_qty->setDisabled(true);

    ui.cb_max_qty->setChecked(false);
    ui.cb_min_qty->setChecked(false);
	ui.cb_max_stop_trigger->setChecked(false);
	
	bool ret = QObject::connect(ui.le_eqsec_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    m_eqsec_list_hint_ = new HintList(this, ui.le_eqsec_stock);
    m_eqsec_list_hint_->hide();
    ret = QObject::connect(m_eqsec_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = QObject::connect(m_eqsec_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));
     
	ret = QObject::connect(ui.cb_eqsec_rebounce, SIGNAL(stateChanged(int)), SLOT(DoRebounceCheckBoxChanged(int)));
    ret = QObject::connect(ui.cb_max_qty, SIGNAL(stateChanged(int)), SLOT(DoMaxQtyCheckBoxChanged(int)));
	ret = QObject::connect(ui.cb_min_qty, SIGNAL(stateChanged(int)), SLOT(DoMinQtyCheckBoxChanged(int)));

	ret = QObject::connect(ui.cb_max_stop_trigger, SIGNAL(stateChanged(int)), SLOT(DoMaxStopTrigCheckBoxChanged(int)));
	ret = QObject::connect(ui.cb_min_clear_trigger, SIGNAL(stateChanged(int)), SLOT(DoMinClearTrigCheckBoxChanged(int)));


	ui.combox_eqsec_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.combox_eqsec_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.combox_eqsec_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.combox_eqsec_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.combox_eqsec_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.combox_eqsec_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
	  
    ResetEqSectionTaskTime();

	ret = connect(ui.pbtn_add_eqsection_task, SIGNAL(clicked()), this, SLOT(DoAddEqSectionTask()));
}


void WinnerWin::DoAddEqSectionTask()
{
    static auto check_le_stock = [this](TypeTask type) ->bool
    {
       // check stock codes
		QString::SectionFlag flag = QString::SectionSkipEmpty;
		QString text_str = ui.le_eqsec_stock->text().trimmed();
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
        if( ui.dbspbox_eqsec_raise_percent->value() < 0.1 )
        {
            ui.dbspbox_eqsec_raise_percent->setFocus();
            return false;
        }
        if( ui.dbspbox_eqsec_fall_percent->value() < 0.1 )
        {
            ui.dbspbox_eqsec_fall_percent->setFocus();
            return false;
        }
         
        auto start_time = ui.timeEdit_eqsec_begin->time().toString("Hmmss").toInt();
        auto end_time = ui.timeEdit_eqsec_end->time().toString("Hmmss").toInt();
            
        if( ui.dbspbox_eqsec_start_price->value() < 0.01 )
        {
            ui.dbspbox_eqsec_start_price->setFocus();
            return false;
        }
        if( ui.spinBox_eqsec_quantity->value() < 100 )
        {
            ui.spinBox_eqsec_quantity->setFocus();
            return false;
        }
        
        if( ui.cb_max_stop_trigger->isChecked() && ui.dbspbox_eqsec_max_price->value() > 999.0 )
        {
            ui.dbspbox_eqsec_max_price->setFocus();
            return false;
        }
        if( ui.cb_min_clear_trigger->isChecked() && ui.dbspbox_eqsec_min_price->value() < 0.01 )
        {
            ui.dbspbox_eqsec_min_price->setFocus();
            return false;
        }
        if( start_time >= end_time )
        {
            ui.timeEdit_eqsec_begin->setFocus();
            return false;
        }
        return true;
    };
    if( !check_le_stock(TypeTask::EQUAL_SECTION) )
        return;

    auto task_info = std::make_shared<T_TaskInformation>();
    task_info->type = TypeTask::EQUAL_SECTION;

    QString::SectionFlag flag = QString::SectionSkipEmpty;
	QString stock_str = ui.le_eqsec_stock->text().trimmed();
	QString stock_pinyin = stock_str.section('/', 1, 1, flag);
	task_info->stock = stock_str.section('/', 0, 0, flag).toLocal8Bit();
	task_info->stock_pinyin = stock_str.section('/', 1, 1, flag).toLocal8Bit();

	task_info->alert_price = ui.dbspbox_eqsec_start_price->value();  
    //task_info->continue_second = 5;
    task_info->secton_task.is_original = true;
    task_info->secton_task.raise_percent = ui.dbspbox_eqsec_raise_percent->value();
    task_info->secton_task.fall_percent = ui.dbspbox_eqsec_fall_percent->value();
	if( ui.cb_eqsec_rebounce->isChecked() )
		task_info->rebounce = ui.spinBox_eqsec_rebounce->value();
	else
		task_info->rebounce = 0.0;
	if( ui.cb_eqsec_back_trigger->isChecked() )
		task_info->back_alert_trigger = true;
	else
		task_info->back_alert_trigger = false;
    if( ui.cb_max_stop_trigger->isChecked() )
        task_info->secton_task.max_trig_price = ui.dbspbox_eqsec_max_price->value();
    if( ui.cb_min_clear_trigger->isChecked() )
        task_info->secton_task.min_trig_price = ui.dbspbox_eqsec_min_price->value();
    if( ui.cb_max_qty->isChecked() )
        task_info->secton_task.max_position = ui.spinBox_max_qty->value();
    if( ui.cb_min_qty->isChecked() )
        task_info->secton_task.min_position = ui.spinBox_min_qty->value();

	task_info->quantity = ui.spinBox_eqsec_quantity->value();
    task_info->target_price_level = ui.combox_eqsec_price_level->currentData().toInt();
    task_info->start_time = ui.timeEdit_eqsec_begin->time().toString("Hmmss").toInt();
    task_info->end_time = ui.timeEdit_eqsec_end->time().toString("Hmmss").toInt();
    
    task_info->state = 1;		
    if( !app_->db_moudle().AddTaskInfo(task_info) )
    {
        // log error
		app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("区间任务添加失败!"));
		app_->AppendLog2Ui("添加区间任务 : %d 失败! fail db_moudle().AddTaskInfo \n", task_info->id);
        return;
    }
    app_->AppendTaskInfo(task_info->id, task_info);
            
    auto equal_section_task = std::make_shared<EqualSectionTask>(*task_info, this->app_);
    app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(equal_section_task));

    app_->ticker_strand().PostTask([equal_section_task, this]()
    {
        this->app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(equal_section_task));
    });
    // add to task list ui
    InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);
    app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("区间任务添加成功!"));
    app_->AppendLog2Ui("添加区间任务 : %d 成功\n", task_info->id);
}

void WinnerWin::ResetEqSectionTaskTime()
{
	ui.timeEdit_eqsec_begin->setTime(QTime(9, 30, 0));
    ui.timeEdit_eqsec_end->setTime(QTime(15, 00, 0));
}

void WinnerWin::DoMaxQtyCheckBoxChanged(int stat)
{
    switch( stat )
	{
		case Qt::Unchecked:
			ui.spinBox_max_qty->setDisabled(true);
			break;
		case Qt::Checked:
			ui.spinBox_max_qty->setEnabled(true);
			break;
	}
}

void WinnerWin::DoMinQtyCheckBoxChanged(int stat)
{
     switch( stat )
	{
		case Qt::Unchecked:
			ui.spinBox_min_qty->setDisabled(true);
			break;
		case Qt::Checked:
			ui.spinBox_min_qty->setEnabled(true);
			break;
	}
}

void WinnerWin::DoMaxStopTrigCheckBoxChanged(int stat)
{
	switch( stat )
	{
		case Qt::Unchecked:
			ui.dbspbox_eqsec_max_price->setDisabled(true);
			break;
		case Qt::Checked:
			ui.dbspbox_eqsec_max_price->setEnabled(true);
			break;
	}
}

void WinnerWin::DoMinClearTrigCheckBoxChanged(int stat)
{
	switch( stat )
	{
		case Qt::Unchecked:
			ui.dbspbox_eqsec_min_price->setDisabled(true);	
			break;
		case Qt::Checked:
			ui.dbspbox_eqsec_min_price->setEnabled(true);	
			break;
	}
}

void WinnerWin::DoRebounceCheckBoxChanged(int stat)
{
	switch( stat )
	{
	case Qt::Unchecked:
		ui.spinBox_eqsec_rebounce->setValue(0.0);
		ui.cb_eqsec_back_trigger->setChecked(false);
		ui.wid_eqsec_rebounce_grp->setDisabled(true);	
		break;
	case Qt::Checked:
		ui.wid_eqsec_rebounce_grp->setEnabled(true);	
		break;
	}
}