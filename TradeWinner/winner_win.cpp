#include "winner_win.h"

#include <qdebug.h>
#include <qmessagebox.h> 
#include <QtGui/QStandardItemModel>
#include <qtextcodec.h>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

//#include "mythread.h"
#include "winner_app.h"

#include "strategy_task.h"
#include "breakdown_task.h"

#include "stock_ticker.h"
#include "MySpinBox.h"
#include "HintList.h"

#include "records_win.h"
#include "calc_win.h"

#include "timer_container.h"

//static const QString cst_str_breakout_buy = QString::fromLocal8Bit("突破买入");
//static const QString cst_str_Inflection_buy = QString::fromLocal8Bit("拐点买入");


static const unsigned short cst_col_count = 11;

static const int cst_tbview_tasks_rowindex_task_id = 0;
static const int cst_tbview_tasks_rowindex_state = 1;
static const int cst_tbview_tasks_rowindex_task_name = 2;
static const int cst_tbview_tasks_rowindex_stock_name = 3;
static const int cst_tbview_tasks_rowindex_cur_price = 4;
static const int cst_tbview_tasks_rowindex_trigger_price = 5;
static const int cst_tbview_tasks_rowindex_quantity = 6;
static const int cst_tbview_tasks_rowindex_price_level = 7;
static const int cst_tbview_tasks_rowindex_start_time = 8;
static const int cst_tbview_tasks_rowindex_end_time = 9;
static const int cst_tbview_tasks_rowindex_task_type = 10;

static const int cst_tab_index_task_list = 0;
static const int cst_tab_index_buy_task = 1;
static const int cst_tab_index_sell_task = 2;
static const int cst_tab_index_eqsec_task = 3;
static const int cst_tab_advsec_task = 4;
static const int cst_tab_capital = 5;
static const int cst_tab_index_log = 6;
static const int cst_tab_index_stkindex_task = 7;
static const int cst_tab_index_backtest = 8;

WinnerWin::WinnerWin(WinnerApp *app, QWidget *parent)
    : QMainWindow(parent)
    , app_(app)
    , tbv_tasks_popMenu_(nullptr)
    , m_list_hint_(nullptr)
	, is_open_hint_(true)
    , m_bt_list_hint_(nullptr)
	, m_eqsec_list_hint_(nullptr)
	, m_indtrd_list_hint_(nullptr)
    , m_backtest_list_hint_(nullptr)
    , cur_price_(0.0)
    , buytask_cur_price_(0.0)
	, eqsec_task_cur_price_(0.0)
    , backtest_cur_price_(0.0)
    , status_label_(nullptr)
	, records_win_(nullptr)
    , calc_win_(nullptr)
    , flash_win_timer_(nullptr)
    , oneceshot_timer_contain_(nullptr)
{
    app_->local_logger().SetDir(".//");

    ui.setupUi(this);
    connect(ui.pbtn_query_capital, SIGNAL(clicked()), this, SLOT(DoQueryCapital()));
     
#if 1
    //------------------tab task list 
    QStandardItemModel * model = new QStandardItemModel(0, cst_col_count, this);
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_task_id, new QStandardItem(QString::fromLocal8Bit("任务号")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_task_id)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_state, new QStandardItem(QString::fromLocal8Bit("运行状态")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_state)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_task_name, new QStandardItem(QString::fromLocal8Bit("任务名称")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_task_name)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_stock_name, new QStandardItem(QString::fromLocal8Bit("股票或指数")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_stock_name)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_cur_price, new QStandardItem(QString::fromLocal8Bit("当前价格")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_cur_price)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_trigger_price, new QStandardItem(QString::fromLocal8Bit("触发价格")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_trigger_price)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_quantity, new QStandardItem(QString::fromLocal8Bit("交易数量")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_quantity)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_price_level, new QStandardItem(QString::fromLocal8Bit("交易使用价格")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_price_level)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_start_time, new QStandardItem(QString::fromLocal8Bit("开始时间")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_start_time)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_end_time, new QStandardItem(QString::fromLocal8Bit("结束时间")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_end_time)->setTextAlignment(Qt::AlignCenter);

    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_task_type, new QStandardItem("类型"));
     
    ui.tbview_tasks->setModel(model);
    ui.tbview_tasks->setColumnWidth(cst_tbview_tasks_rowindex_task_type, 5);

    ui.tbview_tasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bool ret = connect(ui.tbview_tasks, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(DoTabTasksDbClick(const QModelIndex)));
    //------------------tab add sell task
    SetupSellTaskWin();
    InitSellTaskWin();

#endif
   
    //------------------tab add buy task------------ 
    InitBuyTaskWin();

    //------------------tab equal section task----------------
	InitEqSectionTaskWin();
	 
    //------------------tab index trade task----------------
	InitIndexTradeWin();

    //------------------tab backtest ------------------
    InitBacktestWin();

	status_label_ = new QLabel("");
    status_label_->setIndent(3);
    statusBar()->addWidget(status_label_, 1);
	statusBar()->update();

    //------------------------
    oneceshot_timer_contain_ = std::make_shared<TimerContainner>(true);
}

WinnerWin::~WinnerWin()
{
    UnInstallBacktest();
}

// notice : task_info.stock_pinyin 's code will change to gbk
void WinnerWin::InsertIntoTbvTasklist(QTableView *tbv , T_TaskInformation &task_info)
{
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv->model());
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto align_way = Qt::AlignCenter;
    auto item = new QStandardItem( utility::FormatStr("%d", task_info.id).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_task_id, item);
    model->item(row_index, cst_tbview_tasks_rowindex_task_id)->setTextAlignment(align_way);
    // run state
	item = new QStandardItem(ToQString(static_cast<TaskCurrentState>(task_info.state)));
    model->setItem(row_index, cst_tbview_tasks_rowindex_state, item);
    model->item(row_index, cst_tbview_tasks_rowindex_state)->setTextAlignment(align_way);

	QString stock_str; 
    // task name
	if( task_info.type == TypeTask::INDEX_RISKMAN )
	{
		if( task_info.index_rel_task.rel_type == TindexTaskType::ALERT )
		{
			item = new QStandardItem(QString::fromLocal8Bit("指数关联预警"));
			stock_str = QString("%1/%2").arg(task_info.stock.c_str()).arg(QString::fromLocal8Bit(task_info.stock.c_str()));
		}else if( task_info.index_rel_task.rel_type == TindexTaskType::CLEAR )
		{
			item = new QStandardItem(QString::fromLocal8Bit("指数关联清仓"));
			stock_str = QString("%1/%2").arg(task_info.stock.c_str()).arg(QString::fromLocal8Bit(task_info.stock.c_str()));
		}else if( task_info.index_rel_task.rel_type == TindexTaskType::RELSTOCK )
		{
			//QString name_str = QString::fromLocal8Bit("指数关联") + task_info.index_rel_task.stock_code.c_str();
			item = new QStandardItem(QString::fromLocal8Bit("指数关联个股") + task_info.index_rel_task.stock_code.c_str());
			stock_str = QString("%1").arg(task_info.index_rel_task.stock_code.c_str());
		}
	}else
	{
		item = new QStandardItem(ToQString(task_info.type)); 
		stock_str = QString("%1/%2").arg(task_info.stock.c_str()).arg(QString::fromLocal8Bit(task_info.stock_pinyin.c_str()));
	}
    model->setItem(row_index, cst_tbview_tasks_rowindex_task_name, item);
    model->item(row_index, cst_tbview_tasks_rowindex_task_name)->setTextAlignment(align_way);
    //  stock name 
	item = new QStandardItem(stock_str);
    model->setItem(row_index, cst_tbview_tasks_rowindex_stock_name, item);
    model->item(row_index, cst_tbview_tasks_rowindex_stock_name)->setTextAlignment(align_way);
    // current price
    item = new QStandardItem("");
    model->setItem(row_index, cst_tbview_tasks_rowindex_cur_price, item);
    model->item(row_index, cst_tbview_tasks_rowindex_cur_price)->setTextAlignment(align_way);
    // trigger price 
    item = new QStandardItem( utility::FormatStr("%.2f", task_info.alert_price).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_trigger_price, item);
    model->item(row_index, cst_tbview_tasks_rowindex_trigger_price)->setTextAlignment(align_way);

    // quntity
    item = new QStandardItem( utility::FormatStr("%d", task_info.quantity).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_quantity, item);
    model->item(row_index, cst_tbview_tasks_rowindex_quantity)->setTextAlignment(align_way);

    // price level
    item = new QStandardItem( ToQString(static_cast<TypeQuoteLevel>(task_info.target_price_level)) );
    model->setItem(row_index, cst_tbview_tasks_rowindex_price_level, item);
    model->item(row_index, cst_tbview_tasks_rowindex_price_level)->setTextAlignment(align_way);
    // start time
    item = new QStandardItem( utility::FormatStr("%d", task_info.start_time).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_start_time, item);
    model->item(row_index, cst_tbview_tasks_rowindex_start_time)->setTextAlignment(align_way);
    // end time
    item = new QStandardItem( utility::FormatStr("%d", task_info.end_time).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_end_time, item);
    model->item(row_index, cst_tbview_tasks_rowindex_end_time)->setTextAlignment(align_way);        
    
    item = new QStandardItem( utility::FormatStr("%d", task_info.type).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_task_type, item);
    model->item(row_index, cst_tbview_tasks_rowindex_task_type)->setTextAlignment(align_way);  
}
 
void WinnerWin::Init()
{
    flash_win_timer_ = new QTimer(this);
    flash_win_timer_->setSingleShot(false); 
    flash_win_timer_->setInterval(600);
    connect(flash_win_timer_, SIGNAL(timeout()), this, SLOT(DoFlashWin()));

    bool ret = connect(this->app_, SIGNAL(SigAppendLog(char*)), this, SLOT(SlotAppendLog(char*)));
	connect(ui.tabwid_holder, SIGNAL(currentChanged(int)), this, SLOT(SlotTabChanged(int)));

    //
    ret = connect(ui.actionStopAllTask, SIGNAL(triggered(bool)), this->app_, SLOT(SlotStopAllTasks(bool)));
#ifdef USE_MOCK_FLAG
    ret = connect(ui.actionResetMockSys, SIGNAL(triggered(bool)), this->app_, SLOT(SlotResetMockSys(bool)));
#endif
    // title menu 
	ret = connect(ui.actionOpenRecordsWin, SIGNAL(triggered(bool)), this, SLOT(SlotOpenRecordsWin(bool)));
    ret = connect(ui.actionOpenCalcWin, SIGNAL(triggered(bool)), this, SLOT(SlotOpenCalcWin(bool)));
    ret = connect(ui.actionAbout, SIGNAL(triggered(bool)), this, SLOT(SlotOpenAbout(bool)));

    ui.tbview_tasks->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.tbview_tasks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tbview_tasks->setSelectionMode(QAbstractItemView::SingleSelection);
    ;
    //set menu context of tableview tasklist -------------------
    auto action_start = new QAction (this);
    action_start ->setText ( QStringLiteral( "启动" ));
    connect(action_start, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionStart(bool)));

    auto action_stop = new QAction (this);
    action_stop ->setText ( QStringLiteral( "停止" ));
    connect(action_stop, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionStop(bool)));

    auto action_del = new QAction (this);
    action_del ->setText ( QStringLiteral( "删除" ));
    bool ret_con_del = connect(action_del, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionDel(bool)));

    auto action_detail = new QAction (this);
    action_detail ->setText ( QStringLiteral( "查看参数" ));
    connect(action_detail, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionDetail(bool)));

    tbv_tasks_popMenu_ = new QMenu(ui.tbview_tasks);
    tbv_tasks_popMenu_->addAction(action_start);
    tbv_tasks_popMenu_->addAction(action_stop);
    tbv_tasks_popMenu_->addAction(action_del);
    
    tbv_tasks_popMenu_->addAction(action_detail);
    
    bool ret_of_con = connect(ui.tbview_tasks, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(SlotTbvTasksContextMenu(QPoint))); 
    //-----------------------------------------

    std::for_each( std::begin(app_->task_infos()), std::end(app_->task_infos()), [&, this](WinnerApp::T_IdMapTaskInfo::reference entry)
    {
        InsertIntoTbvTasklist(ui.tbview_tasks, *entry.second);
    });

    // load log content -------------------
    for(int i = 0; i < 10; ++i)
    {
        std::ifstream input_file(utility::FormatStr("trade_winner/trade_winner.OrderData_%d.%d.log", Today(), i));
        if( !input_file ) 
           break;
        // get length of file:
        input_file.seekg (0, input_file.end);
        int length = input_file.tellg();
        input_file.seekg (0, input_file.beg);

        // allocate memory:
        auto buf = std::make_shared<Buffer>(length);  
        // read data as a block:
        input_file.read (buf->data(), length);
        input_file.close();
     
        ui.pte_log->appendPlainText(QString::fromLocal8Bit(buf->c_data()));
    }
     
}

void WinnerWin::keyPressEvent(QKeyEvent *event)
{ 
    if( event->key() == Qt::Key_Escape )
    {
        if( ui.le_stock->hasFocus() )
        {
            m_list_hint_->close();
        }else if(ui.le_buytask_stock->hasFocus() )
        {
            m_bt_list_hint_->close();
		}else if( ui.le_eqsec_stock->hasFocus() )
		{
			m_eqsec_list_hint_->close();
		}
        else if( ui.le_bktest_stock->hasFocus() )
		{
			m_backtest_list_hint_->close();
		}
	}else if( event->key() == Qt::Key_F12 )
	{
		SlotOpenRecordsWin(true);
	}else
    {
        if( ui.le_stock->hasFocus() )
        {
            m_list_hint_->setFocus();
        }else if(ui.le_buytask_stock->hasFocus() )
        {
            m_bt_list_hint_->setFocus();
        }else if( ui.le_eqsec_stock->hasFocus() )
		{
			m_eqsec_list_hint_->setFocus();
		}else if( ui.le_bktest_stock->hasFocus() )
		{
			m_backtest_list_hint_->setFocus();
		}
    }
}


void WinnerWin::DoQueryCapital()
{ 
    ui.pte_capital->clear();
    T_CodeMapPosition pos_map = app_->QueryPosition();
    T_Capital captital = app_->QueryCapital();
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit(utility::FormatStr("总资产:\t%.2f\n", captital.total).c_str())); 
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit("资金:\n"));
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit(utility::FormatStr("可用:\t%.2f\n", captital.available).c_str()));
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit(utility::FormatStr("余额:\t%.2f\n\n", captital.remain).c_str()));

    //ui.pte_capital->appendPlainText(QString::fromLocal8Bit("资金:\n"));

    ui.pte_capital->appendPlainText(QString::fromLocal8Bit("股票: \t\t总数\t可用\t浮动盈亏\t盈亏比例\n"));
    std::for_each(std::begin(pos_map), std::end(pos_map), [this]( std::unordered_map<std::string, T_PositionData>::reference entry)
    {  
        auto code_name = QTextCodec::codecForName( "utf-8" )->toUnicode(app_->db_moudle().GetStockName(entry.first).c_str());
        auto str = QString::fromLocal8Bit(
            utility::FormatStr("%s\t %s\t %.2f\t %.2f\t %.2f\t %.2f\n", entry.first.c_str(), code_name.toLocal8Bit().data()
                , entry.second.total, entry.second.avaliable, entry.second.profit, entry.second.profit_percent).c_str()  );
        qDebug() << str << " -------\n";
        ui.pte_capital->appendPlainText(str);
         
    });
}

void WinnerWin::SlotAppendLog(char *p_chars)
{
    ui.pte_log->appendPlainText(QString::fromLocal8Bit(p_chars));
    delete p_chars; p_chars = nullptr;
}

void WinnerWin::SlotTabChanged(int /*index*/)
{
	assert(m_list_hint_ && m_bt_list_hint_);
	m_list_hint_->hide();
    m_bt_list_hint_->hide();
    m_eqsec_list_hint_->hide();
    m_indtrd_list_hint_->hide();
    m_backtest_list_hint_->hide();
}

void WinnerWin::SlotTbvTasksContextMenu(QPoint p)
{
    QModelIndex pmindex = ui.tbview_tasks->indexAt(p);
    if( pmindex.isValid() )
    {
        tbv_tasks_popMenu_->exec(QCursor::pos());
    }
    ui.tbview_tasks->currentIndex();
}

void WinnerWin::SlotTbvTasksActionStart(bool)
{ 
    auto task_id = TbvTasksCurRowTaskId();
     
    auto strategy_task = app_->FindStrategyTask(task_id); 
    assert(strategy_task);

    if( strategy_task->cur_state() == TaskCurrentState::STOP )
    { 
        strategy_task->cur_state(TaskCurrentState::WAITTING);
        this->app_->Emit(strategy_task.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
        auto cur_time = QTime::currentTime();
        // in task time
        if( cur_time >= strategy_task->tp_start() && cur_time <= strategy_task->tp_end() )
        {
            app_->ticker_strand().PostTask([strategy_task, this]()
            {
            this->app_->local_logger().LogLocal(utility::FormatStr("SlotTbvTasksActionStart %d", strategy_task->task_id()));
            this->app_->stock_ticker().Register(strategy_task);
            });
            if( IsNowTradeTime() )
                strategy_task->cur_state(TaskCurrentState::STARTING);
            else
                strategy_task->cur_state(TaskCurrentState::REST);
            this->app_->Emit(strategy_task.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
        } 
    }

    if( !strategy_task->is_to_run() )
    {
      strategy_task->SetOriginalState(TaskCurrentState::WAITTING);
      app_->db_moudle().UpdateTaskInfo(strategy_task->task_info());
    }
}

void WinnerWin::SlotTbvTasksActionStop(bool)
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    QModelIndex mod_index = ui.tbview_tasks->currentIndex();
    auto task_id = model->item(mod_index.row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();
    auto strategy_task = app_->FindStrategyTask(task_id);
    assert(strategy_task);

    if( strategy_task->is_to_run() )
    {
        // change record state in db
        strategy_task->SetOriginalState(TaskCurrentState::STOP);
        app_->db_moudle().UpdateTaskInfo(strategy_task->task_info());
    }
    this->app_->local_logger().LogLocal(utility::FormatStr("SlotTbvTasksActionStop step1 %d", task_id));

    // unregisterd task
    auto p_strategy_task = strategy_task.get();
    app_->ticker_strand().PostTask([p_strategy_task, task_id, this]()
    {
        this->app_->local_logger().LogLocal(utility::FormatStr("SlotTbvTasksActionStop step2 %d", task_id));
        p_strategy_task->cur_state(TaskCurrentState::STOP);
        this->app_->Emit(p_strategy_task, static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
        this->app_->stock_ticker().UnRegister(task_id);
        
    });

    model->submit();
    update();

}

void WinnerWin::SlotTbvTasksActionDel(bool)
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    QModelIndex mod_index = ui.tbview_tasks->currentIndex();
    auto task_id = model->item(mod_index.row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();
    auto task_type = model->item(mod_index.row(), cst_tbview_tasks_rowindex_task_type)->text().toInt();
    
    app_->DelTaskById(task_id, (TypeTask)task_type);

    model->removeRow(mod_index.row());
    model->submit();
    update();
    return;
}

void WinnerWin::SlotTbvTasksActionDetail(bool)
{    
    auto task_id = TbvTasksCurRowTaskId();
    DoShowTaskDetail(task_id);

#if 0
    auto p_tskinfo = app_->FindTaskInfo(task_id);
    if( !p_tskinfo )
        return;

	is_open_hint_ = false;
     
    switch(p_tskinfo->type)
    {
    case TypeTask::BREAK_SELL: 
    case TypeTask::INFLECTION_SELL:
    case TypeTask::FOLLOW_SELL:
    case TypeTask::BATCHES_SELL:
        cur_price_ = 0.0;
        FillSellTaskWin(p_tskinfo->type, *p_tskinfo);
		ui.tabwid_holder->setCurrentIndex(cst_tab_index_sell_task);
        break;
    case TypeTask::BREAKUP_BUY:
    case TypeTask::INFLECTION_BUY:
    case TypeTask::BATCHES_BUY:
        buytask_cur_price_ = 0.0;
        FillBuyTaskWin(p_tskinfo->type, *p_tskinfo);
		ui.tabwid_holder->setCurrentIndex(cst_tab_index_buy_task);
        break;
    case TypeTask::EQUAL_SECTION:
        {
            ui.le_eqsec_stock->setText( QString("%1/%2").arg(p_tskinfo->stock.c_str()).arg(QString::fromLocal8Bit(p_tskinfo->stock_pinyin.c_str())) );
            
            ui.dbspbox_eqsec_start_price->setValue(p_tskinfo->alert_price);
            ui.dbspbox_eqsec_raise_percent->setValue(p_tskinfo->secton_task.raise_percent);
            ui.dbspbox_eqsec_fall_percent->setValue(p_tskinfo->secton_task.fall_percent);
            ui.spinBox_eqsec_quantity->setValue(p_tskinfo->quantity);
             
            if( p_tskinfo->secton_task.max_position == EQSEC_MAX_POSITION )
                ui.cb_max_qty->setChecked(false);
            else
                ui.cb_max_qty->setChecked(true);
            if( p_tskinfo->secton_task.min_position == EQSEC_MIN_POSITION )
                ui.cb_min_qty->setChecked(false);
            else
                ui.cb_min_qty->setChecked(true);
			if( p_tskinfo->rebounce > 0.0 )
			{
				ui.wid_eqsec_rebounce_grp->setEnabled(true);
				ui.cb_eqsec_rebounce->setChecked(true);
				ui.spinBox_eqsec_rebounce->setValue(p_tskinfo->rebounce);
				if( p_tskinfo->back_alert_trigger )
					ui.cb_eqsec_back_trigger->setChecked(true);
				else
					ui.cb_eqsec_back_trigger->setChecked(false);
			}else
			{
				ui.wid_eqsec_rebounce_grp->setDisabled(true);
				ui.cb_eqsec_rebounce->setChecked(false);
				ui.spinBox_eqsec_rebounce->setValue(0.0);
				ui.cb_eqsec_back_trigger->setChecked(false);
			}
			
            ui.spinBox_max_qty->setValue(p_tskinfo->secton_task.max_position);
            ui.spinBox_min_qty->setValue(p_tskinfo->secton_task.min_position);

            if( Equal(p_tskinfo->secton_task.max_trig_price, EQSEC_MAX_STOP_PRICE) )
                ui.cb_max_stop_trigger->setChecked(false);
            else
                ui.cb_max_stop_trigger->setChecked(true);
            if( Equal(p_tskinfo->secton_task.min_trig_price, EQSEC_MIN_CLEAR_PRICE) )
                ui.cb_min_clear_trigger->setChecked(false);
            else
                ui.cb_min_clear_trigger->setChecked(true);
             
            ui.dbspbox_eqsec_max_price->setValue(p_tskinfo->secton_task.max_trig_price);
            ui.dbspbox_eqsec_min_price->setValue(p_tskinfo->secton_task.min_trig_price);

            ui.combox_eqsec_price_level->setCurrentText(ToQString(static_cast<TypeQuoteLevel>(p_tskinfo->target_price_level)));
            
            ui.timeEdit_eqsec_begin->setTime(Int2Qtime(p_tskinfo->start_time));
            ui.timeEdit_eqsec_end->setTime(Int2Qtime(p_tskinfo->end_time));
            ui.tabwid_holder->setCurrentIndex(cst_tab_index_eqsec_task);
        }
        break;
	case TypeTask::INDEX_RISKMAN:
        {
			ui.combox_stkindex->setCurrentText(IndexCode2IndexName(p_tskinfo->stock.c_str()));
             
			ui.dbspbox_index_val->setValue(p_tskinfo->alert_price);
			ui.radiobtn_cross_down->setChecked( p_tskinfo->index_rel_task.is_down_trigger );
			switch(p_tskinfo->index_rel_task.rel_type)
			{
			case TindexTaskType::ALERT: ui.radiobtn_alert->setChecked(true); DoTrdIndexAlertBtnBtnChecked(true);break;
			case TindexTaskType::RELSTOCK: ui.radiobtn_reltrade->setChecked(true); DoTrdIndexRelBtnBtnChecked(true); break;
			case TindexTaskType::CLEAR: ui.radiobtn_clearall->setChecked(true); DoTrdIndexClearBtnChecked(true);break;
			default: assert(false);
			}
            ui.indtrd_timeEdit_begin->setTime(Int2Qtime(p_tskinfo->start_time));
            ui.indtrd_timeEdit_end->setTime(Int2Qtime(p_tskinfo->end_time));
            ui.tabwid_holder->setCurrentIndex(cst_tab_index_stkindex_task);
		}
		break;
    }
     

EXIT_PROC:

	is_open_hint_ = true;
#endif 
}

void WinnerWin::RemoveByTaskId(int task_id)
{ 
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    int i = 0;
    for( ; i < model->rowCount(); ++i )
    {
        if( task_id == model->item(i, cst_tbview_tasks_rowindex_task_id)->text().toInt() )
           break;
    }
    if( i < model->rowCount() )
    {
        model->removeRow(i);
        model->submit();
        update();
    }
    
}

void WinnerWin::DoStatusBar(const std::string& str)
{
    status_label_->setText(QString::fromLocal8Bit(str.c_str()));
}

void WinnerWin::DoStatusBar(std::string* str, bool is_delete)
{
    assert(str);
    status_label_->setText(QString::fromLocal8Bit(str->c_str()));
    if( is_delete )
        delete str;
}

void WinnerWin::DoFlashWin()
{
    FlashWindow( HWND(this->winId()), 1);
}

void WinnerWin::DoLeStockEditingFinished()
{
    if( !ui.le_stock->isModified() )
        return;

    auto val = ui.le_stock->isModified();
    ui.le_stock->setModified(false);
    //ui.te_stock_match->hide();

#if 0
    auto ckval =  ui.le_stock->text();
    auto stock_code = ui.le_stock->text().trimmed();
    if( stock_code.length() != 6 )
        return;

    byte category = 9;
    int market = stock_code.at(0) == "6" ? 1 : 0;

    auto zqdm = stock_code.toLocal8Bit().data();

    //char  result[1024*1024] = {0};
    auto result = std::make_shared<Buffer>(1024*1024);
    char error[2*1024] = {0};
    short count = 1;
    app_->stock_ticker().GetSecurityBars(category, market, stock_code.toLocal8Bit().data(), 0, count, result->data(), error);
#endif
}

void WinnerWin::DoLeStockChanged(const QString &str)
{
    if( str.length() > 0 )
    {
        /*auto temp = ui.le_stock->text();
        ui.te_stock_match->append(temp);

        std::vector<T_StockCodeName> stockCodeNames;
        app_->db_moudle().GetStockCode(str.toLocal8Bit().data(), stockCodeNames);*/
    }
}

void WinnerWin::DoSpboxQuantityEditingFinished()
{
    
    //ui.spinBox_quantity->mo
}

void WinnerWin::closeEvent(QCloseEvent * event)
{
    auto ret_button = QMessageBox::question(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("确定退出系统?"),
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if( ret_button == QMessageBox::Cancel )
        event->ignore();
    else
        app_->Stop();
}

void WinnerWin::changeEvent(QEvent * event)
{ // if( event->type() == QEvent::WindowStateChange )
    if( flash_win_timer_ && flash_win_timer_->isActive() )  
        flash_win_timer_->stop();
}

void WinnerWin::DoTaskStatChangeSignal(StrategyTask* p_task, int val)
{
    assert(p_task);

    switch(static_cast<TaskStatChangeType>(val))
    {
    case TaskStatChangeType::CUR_PRICE_CHANGE:
        {
            for( int i = 0; i < ui.tbview_tasks->model()->rowCount(); ++i )
            {
                if( p_task->task_id() == static_cast<QStandardItemModel *>(ui.tbview_tasks->model())->item(i, cst_tbview_tasks_rowindex_task_id)->text().toUInt() ) 
                {
                    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
                    model->item(i, cst_tbview_tasks_rowindex_cur_price)->setText(utility::FormatStr("%.2f", p_task->cur_price()).c_str());
                    return;
                }
            }
            break;
        }
    case TaskStatChangeType::CUR_STATE_CHANGE:
        {
            for( int i = 0; i < ui.tbview_tasks->model()->rowCount(); ++i )
            {
                if( p_task->task_id() == static_cast<QStandardItemModel *>(ui.tbview_tasks->model())->item(i, cst_tbview_tasks_rowindex_task_id)->text().toUInt() ) 
                {
                    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
                    model->item(i, cst_tbview_tasks_rowindex_state)->setText(ToQString(p_task->cur_state()));
                    return;
                }
            }
            break;
        }
    }

}

void WinnerWin::DoQueryPosition()
{
    QString text = ui.le_stock->text();
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString tgt_tag = text.section('/', 0, 0, flag);

    if( IsStrNum(tgt_tag.toLocal8Bit().data()) && tgt_tag.length() == 6 )
    {
        auto pos = app_->QueryPosition(tgt_tag.toLocal8Bit().data());
        if( pos )
        {
            ui.label_total_num->setText(QString("%1").arg(pos->avaliable));
            ui.spinBox_quantity->setValue(pos->avaliable);
        }
    }
}

int WinnerWin::TbvTasksCurRowTaskId()
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    return model->item(ui.tbview_tasks->currentIndex().row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();

}

void WinnerWin::FlushFromStationListWidget(QString str)
{
	if( !is_open_hint_ )
		return;
     
    HintList *p_list = nullptr;
    QLineEdit *p_edit = nullptr;
    QDoubleSpinBox *p_dbspb_price = nullptr;
    QDoubleSpinBox *p_dbspb_percent = nullptr;
    AssignHintListAndLineEdit(p_list, p_edit, p_dbspb_price, p_dbspb_percent);

    p_list->clear();
	 
    //qDebug() << "FlushFromStationListWidget " << str << "\n";

    std::vector<T_StockCodeName> stockCodeNames;
    app_->db_moudle().GetStockCode(str.toLocal8Bit().data(), stockCodeNames);
     
    std::for_each( std::begin(stockCodeNames), std::end(stockCodeNames), [p_list, this](T_StockCodeName& entry)
    { 
        auto str = QTextCodec::codecForName( "utf-8" )->toUnicode(entry.name.c_str());
        
        QString tmp_str = QString("%1/%2").arg(entry.code.c_str()).arg(str);
        p_list->addItem(tmp_str);
    });
    if( p_list->count() > 0 )
    {
        int x,y, w,h;
        p_edit->geometry().getRect(&x, &y, &w, &h);
        if( ui.tabwid_holder->currentIndex() != cst_tab_index_stkindex_task )
        {   // grp_relstock_trade
            p_list->setGeometry( x + 4, y + 90, w, h + 40);
        }else
        {
            int x_g, y_g, w_g, h_g;
            ui.grp_relstock_trade->geometry().getRect(&x_g, &y_g, &w_g, &h_g);
            p_list->setGeometry( x_g + x + 4, y_g + y + 90, w, h + 40);
        }
        p_list->setCurrentRow(0);
        p_list->ShowUI();
    }
} 

void WinnerWin::OnClickedListWidget(QModelIndex index)
{
    HintList *p_list = nullptr;
    QLineEdit *p_edit = nullptr;
    QDoubleSpinBox *p_dbspb_price = nullptr;
    QDoubleSpinBox *p_dbspb_percent = nullptr;
    AssignHintListAndLineEdit(p_list, p_edit, p_dbspb_price, p_dbspb_percent);

    QString text = p_list->item(index.row())->text();
    ChangeFromStationText(text);
     
}

void WinnerWin::ChangeFromStationText(QString text)
{
    HintList *p_list = nullptr;
    QLineEdit *p_edit = nullptr;
    QDoubleSpinBox *p_dbspb_price = nullptr;
    QDoubleSpinBox *p_dbspb_percent = nullptr;
    AssignHintListAndLineEdit(p_list, p_edit, p_dbspb_price, p_dbspb_percent);
     
    p_edit->setText(text);
    p_list->hide();

    //qDebug() << "ChangeFromStationText " << tgt_tag << "\n";

    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString tgt_tag = text.section('/', 0, 0, flag);
	T_StockPriceInfo *p_info = app_->GetStockPriceInfo(tgt_tag.toLocal8Bit().data(), false);
	if( p_info )
    {
		if( p_dbspb_price )
			p_dbspb_price->setValue(p_info->cur_price);
        if( p_dbspb_percent )
            p_dbspb_percent->setValue(0.0);

        if( ui.tabwid_holder->currentIndex() == cst_tab_index_sell_task )
            cur_price_ = p_info->cur_price;
        else if( ui.tabwid_holder->currentIndex() == cst_tab_index_buy_task )
            buytask_cur_price_ = p_info->cur_price;
		else if( ui.tabwid_holder->currentIndex() == cst_tab_index_backtest )
            backtest_cur_price_ = p_info->cur_price;

    }else
    {
        if( ui.tabwid_holder->currentIndex() == cst_tab_index_sell_task )
            cur_price_ = 0.0;
        else if( ui.tabwid_holder->currentIndex() == cst_tab_index_buy_task )
            buytask_cur_price_ = 0.0;
		else if( ui.tabwid_holder->currentIndex() == cst_tab_index_eqsec_task )
			eqsec_task_cur_price_ = 0.0;
        else if( ui.tabwid_holder->currentIndex() == cst_tab_index_backtest )
            backtest_cur_price_ = 0.0;
    }
}

void WinnerWin::AssignHintListAndLineEdit(HintList *& p_list, QLineEdit *&p_edit, QDoubleSpinBox *&p_dbspb_alert_price, QDoubleSpinBox *&p_dbspb_percent)
{
    if( ui.tabwid_holder->currentIndex() == cst_tab_index_sell_task )
    {
        p_list = m_list_hint_;
        p_edit = ui.le_stock;
        p_dbspb_alert_price = ui.dbspbox_alert_price;
        p_dbspb_percent = ui.dbspbox_alert_percent;
    }else if( ui.tabwid_holder->currentIndex() == cst_tab_index_buy_task )
    {   
        p_list = m_bt_list_hint_;
        p_edit = ui.le_buytask_stock;
        p_dbspb_alert_price = ui.dbspbox_buytask_alert_price;
        p_dbspb_percent = ui.dbspbox_buytask_alert_percent;
    }else if( ui.tabwid_holder->currentIndex() == cst_tab_index_eqsec_task )
    {   
        p_list = m_eqsec_list_hint_;
        p_edit = ui.le_eqsec_stock;
        p_dbspb_alert_price = ui.dbspbox_eqsec_start_price;
        p_dbspb_percent = nullptr;
    }else if( ui.tabwid_holder->currentIndex() == cst_tab_index_stkindex_task )
    {   
        p_list = m_indtrd_list_hint_;
        p_edit = ui.le_indtrd_stock;
        
    }else if( ui.tabwid_holder->currentIndex() == cst_tab_index_backtest )
    {   
        p_list = m_backtest_list_hint_;
        p_edit = ui.le_bktest_stock;
        p_dbspb_alert_price = ui.dbspbox_bktest_start_price;
    }else 
        assert(false);
}

void WinnerWin::SlotOpenRecordsWin(bool)
{
	if( !records_win_ )
		records_win_ = std::make_shared<RecordsWin>(this->app_);
	records_win_->UpdateTblviewFills();
	records_win_->show();
	records_win_->activateWindow();
}

void WinnerWin::SlotOpenCalcWin(bool)
{
    if( !calc_win_ )
        calc_win_ = std::make_shared<CalcWin>();

    ::SetWindowPos(HWND(calc_win_->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    //::SetWindowPos(HWND(calc_win_->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); 
    calc_win_->show();
    calc_win_->activateWindow();
}

void WinnerWin::SlotOpenAbout(bool)
{
    about_win_.show();
}

void WinnerWin::TriggerFlashWinTimer(bool enable)
{
    if( enable )
        flash_win_timer_->start();
    else
        flash_win_timer_->stop();
}

void WinnerWin::DoTabTasksDbClick(const QModelIndex index)
{
     QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
     int task_id = model->item(index.row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();
     DoShowTaskDetail(task_id);
}

void WinnerWin::DoShowTaskDetail(int task_id)
{
    auto p_tskinfo = app_->FindTaskInfo(task_id);
    if( !p_tskinfo )
        return;

	is_open_hint_ = false;
     
    switch(p_tskinfo->type)
    {
    case TypeTask::BREAK_SELL: 
    case TypeTask::INFLECTION_SELL:
    case TypeTask::FOLLOW_SELL:
    case TypeTask::BATCHES_SELL:
        cur_price_ = 0.0;
        FillSellTaskWin(p_tskinfo->type, *p_tskinfo);
		ui.tabwid_holder->setCurrentIndex(cst_tab_index_sell_task);
        break;
    case TypeTask::BREAKUP_BUY:
    case TypeTask::INFLECTION_BUY:
    case TypeTask::BATCHES_BUY:
        buytask_cur_price_ = 0.0;
        FillBuyTaskWin(p_tskinfo->type, *p_tskinfo);
		ui.tabwid_holder->setCurrentIndex(cst_tab_index_buy_task);
        break;
    case TypeTask::EQUAL_SECTION:
        {
            ui.le_eqsec_stock->setText( QString("%1/%2").arg(p_tskinfo->stock.c_str()).arg(QString::fromLocal8Bit(p_tskinfo->stock_pinyin.c_str())) );
            
            ui.dbspbox_eqsec_start_price->setValue(p_tskinfo->alert_price);
            ui.dbspbox_eqsec_raise_percent->setValue(p_tskinfo->secton_task.raise_percent);
            ui.dbspbox_eqsec_fall_percent->setValue(p_tskinfo->secton_task.fall_percent);
            ui.spinBox_eqsec_quantity->setValue(p_tskinfo->quantity);
             
            if( p_tskinfo->secton_task.max_position == EQSEC_MAX_POSITION )
                ui.cb_max_qty->setChecked(false);
            else
                ui.cb_max_qty->setChecked(true);
            if( p_tskinfo->secton_task.min_position == EQSEC_MIN_POSITION )
                ui.cb_min_qty->setChecked(false);
            else
                ui.cb_min_qty->setChecked(true);
			if( p_tskinfo->rebounce > 0.0 )
			{
				ui.wid_eqsec_rebounce_grp->setEnabled(true);
				ui.cb_eqsec_rebounce->setChecked(true);
				ui.spinBox_eqsec_rebounce->setValue(p_tskinfo->rebounce);
				if( p_tskinfo->back_alert_trigger )
					ui.cb_eqsec_back_trigger->setChecked(true);
				else
					ui.cb_eqsec_back_trigger->setChecked(false);
			}else
			{
				ui.wid_eqsec_rebounce_grp->setDisabled(true);
				ui.cb_eqsec_rebounce->setChecked(false);
				ui.spinBox_eqsec_rebounce->setValue(0.0);
				ui.cb_eqsec_back_trigger->setChecked(false);
			}
			
            ui.spinBox_max_qty->setValue(p_tskinfo->secton_task.max_position);
            ui.spinBox_min_qty->setValue(p_tskinfo->secton_task.min_position);

            if( Equal(p_tskinfo->secton_task.max_trig_price, EQSEC_MAX_STOP_PRICE) )
                ui.cb_max_stop_trigger->setChecked(false);
            else
                ui.cb_max_stop_trigger->setChecked(true);
            if( Equal(p_tskinfo->secton_task.min_trig_price, EQSEC_MIN_CLEAR_PRICE) )
                ui.cb_min_clear_trigger->setChecked(false);
            else
                ui.cb_min_clear_trigger->setChecked(true);
             
            ui.dbspbox_eqsec_max_price->setValue(p_tskinfo->secton_task.max_trig_price);
            ui.dbspbox_eqsec_min_price->setValue(p_tskinfo->secton_task.min_trig_price);

            ui.combox_eqsec_price_level->setCurrentText(ToQString(static_cast<TypeQuoteLevel>(p_tskinfo->target_price_level)));
            
            ui.timeEdit_eqsec_begin->setTime(Int2Qtime(p_tskinfo->start_time));
            ui.timeEdit_eqsec_end->setTime(Int2Qtime(p_tskinfo->end_time));
            ui.tabwid_holder->setCurrentIndex(cst_tab_index_eqsec_task);
        }
        break;
	case TypeTask::INDEX_RISKMAN:
        {
			ui.combox_stkindex->setCurrentText(IndexCode2IndexName(p_tskinfo->stock.c_str()));
             
			ui.dbspbox_index_val->setValue(p_tskinfo->alert_price);
			ui.radiobtn_cross_down->setChecked( p_tskinfo->index_rel_task.is_down_trigger );
			switch(p_tskinfo->index_rel_task.rel_type)
			{
			case TindexTaskType::ALERT: ui.radiobtn_alert->setChecked(true); DoTrdIndexAlertBtnBtnChecked(true);break;
			case TindexTaskType::RELSTOCK: ui.radiobtn_reltrade->setChecked(true); DoTrdIndexRelBtnBtnChecked(true); break;
			case TindexTaskType::CLEAR: ui.radiobtn_clearall->setChecked(true); DoTrdIndexClearBtnChecked(true);break;
			default: assert(false);
			}
            ui.indtrd_timeEdit_begin->setTime(Int2Qtime(p_tskinfo->start_time));
            ui.indtrd_timeEdit_end->setTime(Int2Qtime(p_tskinfo->end_time));
            ui.tabwid_holder->setCurrentIndex(cst_tab_index_stkindex_task);
		}
		break;
    }
     

EXIT_PROC:

	is_open_hint_ = true;
}