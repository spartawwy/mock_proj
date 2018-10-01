#include "winner_win.h"

#include <QtGui/QStandardItemModel>
#include <qtextcodec.h>

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

#include "MySpinBox.h"
#include "HintList.h"
#include "equal_section_task.h"
#include "advance_section_task.h"
#include "batches_buy_task.h"

#include "winner_hq_api.h"

#include "back_tester.h"
#include "detail_file.h"
#include "detail_win.h"

#define TMPDEBUG  //nddel 

typedef std::tuple<std::shared_ptr<StrategyTask>, std::shared_ptr<T_TaskInformation> > T_Task_Inf_Pair;
 
static const std::string cst_back_test_tag = "bktest";

static const char cst_str_eqsec_bktest[] = {"区间交易回测"};
static const char cst_str_advancesec_bktest[] = {"贝塔交易回测"};
static const char cst_str_batchbuy_bktest[] = {"分批买入回测"};

static const int cst_bktest_tbview_col_count = 10;
static const int cst_bktest_tbview_rowindex_result = 0;
static const int cst_bktest_tbview_rowindex_task_id = 1;
static const int cst_bktest_tbview_rowindex_task_type = 2;
static const int cst_bktest_tbview_rowindex_task_qty = 3;
static const int cst_bktest_tbview_rowindex_task_inflect = 4; 
static const int cst_bktest_tbview_rowindex_top_price = 5;
static const int cst_bktest_tbview_rowindex_clear_price = 6;
static const int cst_bktest_tbview_rowindex_date_begin = 7;
static const int cst_bktest_tbview_rowindex_date_end = 8;
static const int cst_bktest_tbview_rowindex_detail_f = 9;

bool WinnerWin::InitBacktestWin()
{
    bool ret = true;
    ui.cb_bktest_type->addItem(QString::fromLocal8Bit(cst_str_eqsec_bktest), QVariant(static_cast<int>(TypeTask::EQUAL_SECTION)));
    ui.cb_bktest_type->addItem(QString::fromLocal8Bit(cst_str_advancesec_bktest), QVariant(static_cast<int>(TypeTask::ADVANCE_SECTION)));
    ui.cb_bktest_type->addItem(QString::fromLocal8Bit(cst_str_batchbuy_bktest), QVariant(static_cast<int>(TypeTask::BATCHES_BUY)));
    //ui.wid_bktest_eqsec->show();
    ui.wid_bktest_adv_sec->setGeometry(ui.wid_bktest_eqsec->geometry().x(), ui.wid_bktest_eqsec->geometry().y()
                                       , ui.wid_bktest_adv_sec->geometry().width(), ui.wid_bktest_adv_sec->geometry().height());
    ui.wid_bktest_adv_sec->hide();
    ret = QObject::connect(ui.cb_bktest_type, SIGNAL(currentTextChanged(const QString&)), this, SLOT(DoBktestTypeChanged(const QString&)));

    m_backtest_list_hint_ = new HintList(this, ui.le_bktest_stock);
    m_backtest_list_hint_->hide();
    ret = QObject::connect(ui.le_bktest_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    ret = QObject::connect(m_backtest_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = QObject::connect(m_backtest_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));
     
    ret = QObject::connect(ui.pbtn_bktest_need_capital, SIGNAL(clicked(bool)), this, SLOT(DoAdveqGetNeedCapital()));

    ret = connect(ui.pbtn_start_backtest, SIGNAL(clicked(bool)), this, SLOT(DoStartBacktest(bool)));
    ret = connect(this->app_, SIGNAL(SigEnableBtnBackTest()), this, SLOT(DoEnableBtnBackTest()));
    ret = connect(ui.pbtn_bktest_add_task, SIGNAL(clicked(bool)), this, SLOT(DoBktestAddTask()));
    ret = connect(ui.pbtn_bktest_clear_task, SIGNAL(clicked(bool)), this, SLOT(DoBktestClearTask()));
    ret = connect(ui.pbtn_bktest_order_detail, SIGNAL(clicked(bool)), this, SLOT(DoBktestShowOrderDetail()));
#ifdef TMPDEBUG
    auto cur_date = 20180705;
    auto begin_date = 20180702;
    ui.de_bktest_begin->setDate(QDate(begin_date/10000, begin_date % 10000 / 100, begin_date % 100));
    ui.de_bktest_end->setDate(QDate(cur_date/10000, cur_date % 10000 / 100, cur_date % 100));
#else
    auto cur_date = std::get<0>(CurrentDateIntTime());
    auto begin_date = app_->exchange_calendar().TodayAddDays(-30);
    ui.de_bktest_begin->setDate(QDate(begin_date/10000, begin_date % 10000 / 100, begin_date % 100));
    ui.de_bktest_end->setDate(QDate(cur_date/10000, cur_date % 10000 / 100, cur_date % 100));
#endif
    // ----------tbview_bktest_tasks----------------------
    QStandardItemModel * model = new QStandardItemModel(0, cst_bktest_tbview_col_count, ui.wid_bktest_task_tbview);
    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_result, new QStandardItem(QString::fromLocal8Bit("交易明细")));
    //model->horizontalHeaderItem(cst_bktest_tbview_rowindex_result)->setTextAlignment(Qt::AlignCenter);

    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_task_id, new QStandardItem(QString::fromLocal8Bit("任务号")));
    model->horizontalHeaderItem(cst_bktest_tbview_rowindex_task_id)->setTextAlignment(Qt::AlignCenter);
     
    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_task_type, new QStandardItem(QString::fromLocal8Bit("类型")));
	model->horizontalHeaderItem(cst_bktest_tbview_rowindex_task_id)->setTextAlignment(Qt::AlignCenter);
	
    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_task_qty, new QStandardItem(QString::fromLocal8Bit("每次数量")));
	model->horizontalHeaderItem(cst_bktest_tbview_rowindex_task_qty)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_task_inflect, new QStandardItem(QString::fromLocal8Bit("拐点")));
	model->horizontalHeaderItem(cst_bktest_tbview_rowindex_task_inflect)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_top_price, new QStandardItem(QString::fromLocal8Bit("顶部价格")));
	model->horizontalHeaderItem(cst_bktest_tbview_rowindex_top_price)->setTextAlignment(Qt::AlignCenter);

	model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_clear_price, new QStandardItem(QString::fromLocal8Bit("清仓价格")));
	model->horizontalHeaderItem(cst_bktest_tbview_rowindex_clear_price)->setTextAlignment(Qt::AlignCenter);

    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_date_begin, new QStandardItem(QString::fromLocal8Bit("起始日期")));
    model->horizontalHeaderItem(cst_bktest_tbview_rowindex_date_begin)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_date_end, new QStandardItem(QString::fromLocal8Bit("结束日期")));
    model->horizontalHeaderItem(cst_bktest_tbview_rowindex_date_end)->setTextAlignment(Qt::AlignCenter);

    model->setHorizontalHeaderItem(cst_bktest_tbview_rowindex_detail_f, new QStandardItem(QString::fromLocal8Bit("结束日期")));

    ui.tbview_bktest_tasks->setModel(model);
    ui.tbview_bktest_tasks->setColumnWidth(cst_bktest_tbview_rowindex_task_type, 5);
    ui.tbview_bktest_tasks->setColumnWidth(cst_bktest_tbview_rowindex_detail_f, 0);

    ui.tbview_bktest_tasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ret = connect(ui.tbview_bktest_tasks, SIGNAL(clicked(const QModelIndex &)), this, SLOT(DoBktestShowOrderDetail(const QModelIndex &)) );
    // ---------------------------------------------------

    return true;
}

void WinnerWin::UnInstallBacktest()
{ 
}

void WinnerWin::ShowBktestOrderDetail(const std::string& content)
{ 
    detail_win_->ShowUI("回测交易明细", content);
}

void WinnerWin::DoBktestTypeChanged(const QString&)
{
    int val = ui.cb_bktest_type->currentData().toInt();
    switch (val)
    {
    case (int)TypeTask::EQUAL_SECTION: 
        ui.wid_bktest_eqsec->show();
        ui.wid_bktest_adv_sec->hide();
        break;
    case (int)TypeTask::ADVANCE_SECTION: 
        ui.wid_bktest_eqsec->hide();
        ui.wid_bktest_adv_sec->show();
        break;
    default: break;
    }

}

void WinnerWin::DoStartBacktest(bool)
{
    this->ui.pbtn_start_backtest->setDisabled(true);
    oneceshot_timer_contain_->InsertTimer(2 * 2 * 1000, [this]()
    {
        this->ui.pbtn_start_backtest->setEnabled(true);
    }); 

    //auto date_vector = app_->GetSpanTradeDates(20170914, 20171031);
    auto start_date = ui.de_bktest_begin->date().toString("yyyyMMdd").toInt();
    auto end_date = ui.de_bktest_end->date().toString("yyyyMMdd").toInt();
    auto date_vector = app_->GetSpanTradeDates(start_date, end_date);
    if( date_vector.size() < 1 )
    {
        app_->winner_win().DoStatusBar("无可用交易日!");
        return;
    }
    app_->back_tester()->StartTest(start_date, end_date);

}


void WinnerWin::DoEnableBtnBackTest()
{
    ui.pbtn_start_backtest->setEnabled(true);
}

void WinnerWin::DoBktestAddTask()
{
    static auto check_le_stock = [this](TypeTask &type) ->bool
    {
        // check stock codes
        QString::SectionFlag flag = QString::SectionSkipEmpty;
        QString text_str = ui.le_bktest_stock->text().trimmed();
        QString stock_str = text_str.section('/', 0, 0, flag);
        if( stock_str.length() != 6 )
        {
            // todo: show erro info
            ui.le_bktest_stock->setFocus();
            this->DoStatusBar("股票代码有误!");
            return false;
        } 
        if( type != TypeTask::ADVANCE_SECTION )
        {
            if( ui.dbspbox_bktest_start_price->value() < 0.01 )
            {
                ui.dbspbox_bktest_start_price->setFocus();
                this->DoStatusBar("起始价格不能为0!");
                return false;
            }
            if( ui.dbspbox_bktest_start_capital->value() < 0.01 )
            {
                ui.dbspbox_bktest_start_capital->setFocus();
                this->DoStatusBar("起始资金不能为0!");
                return false;
            }

            if( ui.dbspbox_bktest_raise_percent->value() < 0.1 )
            {
                ui.dbspbox_bktest_raise_percent->setFocus();
                this->DoStatusBar("上升百分比不能为0!");
                return false;
            }
            if( ui.dbspbox_bktest_fall_percent->value() < 0.1 )
            {
                ui.dbspbox_bktest_fall_percent->setFocus();
                this->DoStatusBar("下降百分比不能为0!");
                return false;
            }
            if( ui.cb_bktest_rebounce->isChecked() && ui.spinBox_bktest_rebounce->value() < 0.1 )
            { 
                ui.spinBox_bktest_rebounce->setFocus();
                this->DoStatusBar("拐点值不能为0!");
                return false;
            }  
            if( ui.spinBox_bktest_quantity->value() < 100 )
            {
                ui.spinBox_bktest_quantity->setFocus();
                this->DoStatusBar("买卖数量不能为0!");
                return false;
            }

            if( ui.cb_bktest_max_qty->isChecked() && ui.spinBox_bktest_max_qty->value() < 100 )
            {
                ui.cb_bktest_max_qty->setFocus();
                this->DoStatusBar("最大仓位不能为0!");
                return false;
            } 
        }else  // advance section task
        {  
            if( ui.dbspb_bktest_adv_max_price->value() < 0.01 )
            {
                ui.dbspb_bktest_adv_max_price->setFocus();
                this->DoStatusBar("顶部价格不能为0!");
                return false;
            }
            if( ui.dbspb_bktest_adv_min_price->value() < 0.01 )
            {
                ui.dbspb_bktest_adv_min_price->setFocus();
                this->DoStatusBar("清仓价格不能为0!");
                return false;
            }
            if( ui.spinBox_bktest_adv_qty->value() < 100 )
            {
                ui.spinBox_bktest_adv_qty->setFocus();
                this->DoStatusBar("单区间股数不能为0!");
                return false;
            }
        }
        
        auto start_date = ui.de_bktest_begin->date().toString("yyyyMMdd").toInt();
        auto end_date = ui.de_bktest_end->date().toString("yyyyMMdd").toInt();

        if( end_date < start_date )
        {
            ui.de_bktest_begin->setFocus();
            this->DoStatusBar("开始日期不能大于结束日期!");
            return false;
        }
        return true;
    };

    static auto set_detail_file = [this](std::shared_ptr<T_MockStrategyPara> &mock_para, const T_TaskInformation& tsk_info)
    {
        mock_para->detail_file = nullptr;
        auto file = std::make_shared<DetailFile>(this->app_->back_tester()->detail_file_dir(), true);

        if( file->Init(ToString(tsk_info)) )
            mock_para->detail_file = file;
    };

    auto task_info = std::make_shared<T_TaskInformation>();

    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString stock_str = ui.le_bktest_stock->text().trimmed();
    QString stock_pinyin = stock_str.section('/', 1, 1, flag);
    task_info->stock = stock_str.section('/', 0, 0, flag).toLocal8Bit();

    task_info->id = app_->back_tester()->AllocTaskId(); 
    task_info->type = (TypeTask)ui.cb_bktest_type->currentData().toInt();

    task_info->back_alert_trigger = false;
    task_info->assistant_field = ""; 
    task_info->continue_second = 0; 

    auto start_date = ui.de_bktest_begin->date().toString("yyyyMMdd").toInt();
    auto end_date = ui.de_bktest_end->date().toString("yyyyMMdd").toInt();

    auto mock_para = std::make_shared<T_MockStrategyPara>();

    if( task_info->type == TypeTask::BATCHES_BUY || task_info->type == TypeTask::EQUAL_SECTION )
    {
        if( !check_le_stock(task_info->type) ) 
            return;
        task_info->rebounce = ui.cb_bktest_rebounce->isChecked() ? ui.spinBox_bktest_rebounce->value() : 0.0;
        task_info->quantity = ui.spinBox_bktest_quantity->value();
        //const double alert_price = 20.3;
        task_info->alert_price = ui.dbspbox_bktest_start_price->value();

        task_info->secton_task.fall_percent = 1;
        //task_info->secton_task.fall_infection = 0.2;
        task_info->secton_task.raise_percent = 1;
        //task_info->secton_task.raise_infection = 0.2;
        task_info->secton_task.max_position = ui.cb_bktest_max_qty->isChecked() ? ui.spinBox_bktest_max_qty->value() : EQSEC_MAX_POSITION;
        task_info->secton_task.min_position = ui.cb_bktest_min_qty->isChecked() ? ui.spinBox_bktest_min_qty->value() : EQSEC_MIN_POSITION;
        task_info->secton_task.max_trig_price = ui.cb_bktest_max_stop_trigger->isChecked() ? ui.dbspbox_bktest_max_price->value() : MAX_STOCK_PRICE;
        task_info->secton_task.min_trig_price = ui.cb_bktest_min_clear_trigger->isChecked() ? ui.dbspbox_bktest_min_price->value() : MIN_STOCK_PRICE;
        if( task_info->type == TypeTask::BATCHES_BUY )
        {
            task_info->step = 1;
            task_info->bs_times = 1;
        }
        auto tmp_price = app_->GetStockPriceInfo(task_info->stock.c_str(), false);
        double cur_stock_price = tmp_price ? tmp_price->cur_price : 2.0;
        mock_para->avaliable_position = ui.spinBox_bktest_start_pos->value();
        mock_para->capital = ui.dbspbox_bktest_start_capital->value() + cur_stock_price * mock_para->avaliable_position;
        mock_para->ori_capital = mock_para->capital;
        set_detail_file(mock_para, *task_info);

    }else if( task_info->type == TypeTask::ADVANCE_SECTION )
    {
        if( !check_le_stock(task_info->type) ) 
            return;
        task_info->advance_section_task.is_original = true;   
        double top_price = ui.dbspb_bktest_adv_max_price->value();
        double bottom_price = ui.dbspb_bktest_adv_min_price->value();
        if( top_price < bottom_price + 0.05 )
        {
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("顶部价格必须大于底部价格一定值!"));
            return;
        }
        int section_count = ui.spb_bktest_adv_section_count->value();
        if( section_count < 2 )
        {
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("区间数必须大于1!"));
            return;
        }
        task_info->quantity = ui.spinBox_bktest_adv_qty->value();
        double atom_h = (top_price - bottom_price ) / section_count;
        if( atom_h < 0.1 )
        {
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("单区间价格过小,请调整顶底价格或区间数目!"));
            return;
        }
        char buf[16] = {0}; 
        for( int i = 0; i < section_count; ++i )
        {
            sprintf_s(buf, sizeof(buf), "%.2f\0", bottom_price + atom_h * i);
            task_info->advance_section_task.portion_sections.append(buf);
            task_info->advance_section_task.portion_sections.append(";");
            task_info->advance_section_task.portion_states.append(std::to_string(int(AdvanceSectionTask::PortionState::UNKNOW)));
            task_info->advance_section_task.portion_states.append(";");
        }
        sprintf_s(buf, "%.2f\0", top_price);
        task_info->advance_section_task.portion_sections.append(buf);
        task_info->advance_section_task.portion_states.append(std::to_string(int(AdvanceSectionTask::PortionState::UNKNOW)));

        task_info->rebounce = ui.spb_bktest_adv_rebounce->value();

        mock_para->avaliable_position = 0;
        mock_para->capital = (top_price + bottom_price) * task_info->quantity * ui.spb_bktest_adv_section_count->value() / 2;
        mock_para->ori_capital = mock_para->capital; 
        set_detail_file(mock_para, *task_info);
         
    }else
        return;

    std::shared_ptr<StrategyTask> task = nullptr;
    switch (task_info->type)
    {
    case TypeTask::EQUAL_SECTION:
        {
            task = std::make_shared<EqualSectionTask>(*task_info, app_, mock_para.get()); 
            break;
        }
    case TypeTask::ADVANCE_SECTION:
        {
            task = std::make_shared<AdvanceSectionTask>(*task_info, app_, mock_para.get()); 
            break;
        }
    case TypeTask::BATCHES_BUY:
        {
            task = std::make_shared<BatchesBuyTask>(*task_info, app_, mock_para.get()); 
            break;
        }
    }
 
	InsertIntoBktestTbvTask(*task_info, *mock_para, start_date, end_date);
    app_->back_tester()->AddBackTestItem(task, task_info, mock_para);
     
	this->DoStatusBar("添加成功!");
}

void WinnerWin::DoBktestClearTask()
{
	QStandardItemModel * model = static_cast<QStandardItemModel *>(ui.tbview_bktest_tasks->model());
	model->removeRows(0, model->rowCount());

    app_->back_tester()->ClearTestItems();
	this->DoStatusBar("清除成功!");
}

void WinnerWin::DoBktestShowOrderDetail(const QModelIndex &index)
{
    if( index.column() == cst_bktest_tbview_rowindex_result )
    {
        QStandardItemModel * model = static_cast<QStandardItemModel *>(ui.tbview_bktest_tasks->model());
        std::string detail_file_name = model->item(index.row(), cst_bktest_tbview_rowindex_detail_f)->text().toLocal8Bit().data();
        std::string content;
        app_->back_tester()->GetDetailFileContent(detail_file_name, content);
        ShowBktestOrderDetail(content);
    }
}

void WinnerWin::DoAdveqGetNeedCapital()
{
    if( ui.dbspb_bktest_adv_max_price->value() < ui.dbspb_bktest_adv_min_price->value() + 0.05 )
    {
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("顶部价格必须大于底部价格一定值!"));
        return;
    }
   
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString text_str = ui.le_bktest_stock->text().trimmed();
    QString stock_str = text_str.section('/', 0, 0, flag);
     
    double qty = ui.spinBox_bktest_adv_qty->value();
    double top_price = ui.dbspb_bktest_adv_max_price->value();
    double bottom_price = ui.dbspb_bktest_adv_min_price->value();
    const int section_count = ui.spb_bktest_adv_section_count->value(); 
    double atom_h = (top_price - bottom_price ) / section_count;
    double need_capital = 0.0;
    for( int i = 0; i < section_count; ++i )
    {
        double cur_sec_bottom = bottom_price + atom_h * i;
        need_capital += (cur_sec_bottom + atom_h / 2) * qty;
    } 
    ui.dbspb_bktest_adv_start_capital->setValue(need_capital);
}
  
void WinnerWin::InsertIntoBktestTbvTask(T_TaskInformation &task_info, T_MockStrategyPara &mock_para, int date_begin, int date_end)
{ 
	QStandardItemModel * model = static_cast<QStandardItemModel *>(ui.tbview_bktest_tasks->model());
 
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto align_way = Qt::AlignCenter;

    auto item = new QStandardItem( "detail" );
    model->setItem(row_index, cst_bktest_tbview_rowindex_result, item);

    item = new QStandardItem( utility::FormatStr("%d", task_info.id).c_str() );
    model->setItem(row_index, cst_bktest_tbview_rowindex_task_id, item);

	item = new QStandardItem( ToQString(task_info.type) );
	model->setItem(row_index, cst_bktest_tbview_rowindex_task_type, item);

	item = new QStandardItem( utility::FormatStr("%.2f", task_info.rebounce).c_str() );
	model->setItem(row_index, cst_bktest_tbview_rowindex_task_inflect, item);
	  
	item = new QStandardItem( utility::FormatStr("%d", task_info.quantity).c_str() );
	model->setItem(row_index, cst_bktest_tbview_rowindex_task_qty, item);
     
    item = new QStandardItem( utility::FormatStr("%d", date_begin).c_str() );
    model->setItem(row_index, cst_bktest_tbview_rowindex_date_begin, item);

    item = new QStandardItem( utility::FormatStr("%d", date_end).c_str() );
    model->setItem(row_index, cst_bktest_tbview_rowindex_date_end, item);

    std::string ck_val = ToString(task_info).c_str();
    item = new QStandardItem( ToString(task_info).c_str() );
    model->setItem(row_index, cst_bktest_tbview_rowindex_detail_f, item);

    std::string detail_file_name = model->item(row_index, cst_bktest_tbview_rowindex_detail_f)->text().toLocal8Bit().data();

    detail_file_name = detail_file_name;
    //model->item(row_index, cst_bktest_tbview_rowindex_task_type)->setTextAlignment(align_way);
	
	/*if( task_info.type == TypeTask::EQUAL_SECTION )
	{ 
	}*/
}