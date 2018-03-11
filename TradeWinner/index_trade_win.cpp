#include "winner_win.h"

#include <QMessageBox>

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"

#include "stock_ticker.h"
#include "strategy_task.h"
#include "index_task.h"
  
void WinnerWin::InitIndexTradeWin()
{
	 // reset some widget ------------
#if 1
	ui.combox_stkindex->addItem(QString::fromLocal8Bit(cst_sh_index_name), QVariant(cst_sh_index));
	ui.combox_stkindex->addItem(QString::fromLocal8Bit(cst_sz_compre_index_name), QVariant(cst_sz_compre_index));
	ui.combox_stkindex->addItem(QString::fromLocal8Bit(cst_entrepren_plate_index_name), QVariant(cst_entrepren_plate_index));
	ui.combox_stkindex->addItem(QString::fromLocal8Bit(cst_entreplate_compre_index_name), QVariant(cst_entreplate_compre_index));

	//ui.combox_stkindex->currentData().toString();
	ui.dbspbox_index_val->setMaximum(99999.9);
    ui.dbspbox_index_val->setValue(1000.0); 

	bool ret = QObject::connect(ui.le_indtrd_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
     
	auto obj_name = ui.spinBox_indtrd_quantity->objectName();
    const QRect geome_val = ui.spinBox_indtrd_quantity->geometry();
    delete ui.spinBox_indtrd_quantity;
    ui.spinBox_indtrd_quantity = new MySpinBox(ui.grp_relstock_trade);
    ui.spinBox_indtrd_quantity->setSingleStep(100);
    ui.spinBox_indtrd_quantity->setObjectName(obj_name);
    ui.spinBox_indtrd_quantity->setGeometry(geome_val);
    ui.spinBox_indtrd_quantity->setMaximum(1000000000);
#endif
     
    // radio button
	ret = QObject::connect(ui.radiobtn_cross_up, SIGNAL(clicked (bool)), SLOT(DoTrdIndexRadioCrossUpChecked(bool)));
	 
    ret = QObject::connect(ui.radiobtn_alert, SIGNAL(clicked (bool)), SLOT(DoTrdIndexAlertBtnBtnChecked(bool)));
    ret = QObject::connect(ui.radiobtn_reltrade, SIGNAL(clicked (bool)), SLOT(DoTrdIndexRelBtnBtnChecked(bool)));
    ret = QObject::connect(ui.radiobtn_clearall, SIGNAL(clicked (bool)), SLOT(DoTrdIndexClearBtnChecked(bool)));
	 
    ret = QObject::connect(ui.le_indtrd_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    m_indtrd_list_hint_ = new HintList(this, ui.le_indtrd_stock);
    m_indtrd_list_hint_->hide();
    ret = QObject::connect(m_indtrd_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = QObject::connect(m_indtrd_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));
     
	ui.indtrd_trd_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.indtrd_trd_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.indtrd_trd_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.indtrd_trd_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.indtrd_trd_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.indtrd_trd_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
	  
    ui.indtrd_timeEdit_begin->setTime(QTime(9, 30, 0));
    ui.indtrd_timeEdit_end->setTime(QTime(15, 00, 0));

	ret = connect(ui.pbtn_add_indtrd_task, SIGNAL(clicked()), this, SLOT(DoAddIndexTradeTask()));
}

void WinnerWin::DoAddIndexTradeTask()
{
	// check
	 if( ui.radiobtn_clearall->isChecked() )
	 {
		 if( ui.dbspbox_index_val->value() > 3000.0 )
		 {
		 auto ret_button = QMessageBox::question(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("清仓指数值可能过小,确定添加?"),
         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		 if( ret_button == QMessageBox::Cancel )
			return;
		 }
	 }

	auto task_info = std::make_shared<T_TaskInformation>();
	task_info->type = TypeTask::INDEX_RISKMAN;

    QString::SectionFlag flag = QString::SectionSkipEmpty;
	QString stock_str = ui.combox_stkindex->currentData().toString();
	//QString stock_pinyin = stock_str.section('/', 1, 1, flag);
	//task_info->stock = stock_str.section('/', 0, 0, flag).toLocal8Bit();
	task_info->stock_pinyin = ui.combox_stkindex->currentText().toLocal8Bit();
	task_info->stock = ui.combox_stkindex->currentData().toString().toLocal8Bit();

	task_info->alert_price = ui.dbspbox_index_val->value();  
	task_info->index_rel_task.is_down_trigger = ui.radiobtn_cross_down->isChecked();
    task_info->continue_second = ui.spinBox_index_continue_time->value();

	if( ui.radiobtn_alert->isChecked() ) 
	{
		task_info->index_rel_task.rel_type = TindexTaskType::ALERT;
	}else if( ui.radiobtn_clearall->isChecked() )
	{
		task_info->index_rel_task.rel_type = TindexTaskType::CLEAR;
	}else if( ui.radiobtn_reltrade->isChecked() )
	{
		task_info->index_rel_task.rel_type = TindexTaskType::RELSTOCK;
		task_info->index_rel_task.is_buy = ui.radioBtn_buy->isChecked();
		task_info->quantity = ui.spinBox_eqsec_quantity->value();

	}
    task_info->target_price_level = ui.indtrd_trd_price_level->currentData().toInt();
    task_info->start_time = ui.indtrd_timeEdit_begin->time().toString("Hmmss").toInt();
    task_info->end_time = ui.indtrd_timeEdit_end->time().toString("Hmmss").toInt();
    
    task_info->state = 1;		
    if( !app_->db_moudle().AddTaskInfo(task_info) )
    {
        // log error
        return;
    }
    app_->AppendTaskInfo(task_info->id, task_info);
       
    auto index_trade_task = std::make_shared<IndexTask>(*task_info, this->app_);
    app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(index_trade_task));

    app_->ticker_strand().PostTask([index_trade_task, this]()
    {
        app_->index_ticker().Register(std::shared_ptr<StrategyTask>(index_trade_task));
    });
    // add to task list ui
    InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);

    app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("指数关联任务添加成功!"));
    app_->AppendLog2Ui("添加指数关联任务 : %d 成功\n", task_info->id);
}

void WinnerWin::DoTrdIndexRadioCrossDownChecked(bool checked)
{

}

void WinnerWin::DoTrdIndexRadioCrossUpChecked(bool checked)
{
	if( checked )
	{
		ui.radiobtn_reltrade->setChecked(true);
		ui.radiobtn_clearall->setChecked(false);
	}
}

void WinnerWin::DoTrdIndexAlertBtnBtnChecked(bool checked)
{
	if( checked )
	{
		ui.grp_relstock_trade->setDisabled(true);
		ui.le_indtrd_stock->clear();
        m_indtrd_list_hint_->hide();
		ui.spinBox_indtrd_quantity->clear();
	}
}

void WinnerWin::DoTrdIndexRelBtnBtnChecked(bool checked)
{
	if( checked )
		ui.grp_relstock_trade->setEnabled(true);
}

void WinnerWin::DoTrdIndexClearBtnChecked(bool checked)
{
	if( checked )
	{
		ui.radiobtn_cross_down->setChecked(true);
		ui.grp_relstock_trade->setDisabled(true);

		ui.le_indtrd_stock->clear();
        m_indtrd_list_hint_->hide();
		ui.spinBox_indtrd_quantity->clear();
	}
}