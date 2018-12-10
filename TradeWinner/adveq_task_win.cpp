#include "winner_win.h"

#include <qdebug.h>
#include <qmessagebox.h> 

#include "winner_app.h"
#include "stock_ticker.h"

#include "HintList.h"
#include "advance_section_task.h"

#define  ADVSEC_BUILDPOS_CONDIDER_EXISTPOS

void WinnerWin::InitAdveqTaskWin()
{
    ui.combox_adveq_price_level->addItem(QString::fromLocal8Bit("即时价"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_CUR)));
    ui.combox_adveq_price_level->addItem(QString::fromLocal8Bit("买一和卖一"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_1)));
    ui.combox_adveq_price_level->addItem(QString::fromLocal8Bit("买二和卖二"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_2)));
    ui.combox_adveq_price_level->addItem(QString::fromLocal8Bit("买三和卖三"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_3)));
    ui.combox_adveq_price_level->addItem(QString::fromLocal8Bit("买四和卖四"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_4)));
    ui.combox_adveq_price_level->addItem(QString::fromLocal8Bit("买五和卖五"), QVariant(static_cast<int>(TypeQuoteLevel::PRICE_BUYSELL_5)));
	   
    m_adveq_list_hint_ = new HintList(this, ui.le_adveq_stock);
    m_adveq_list_hint_->hide();
    bool ret = QObject::connect(ui.le_adveq_stock, SIGNAL(textChanged(QString)), this, SLOT(FlushFromStationListWidget(QString)));
    ret = QObject::connect(m_adveq_list_hint_, SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickedListWidget(QModelIndex)));
    ret = QObject::connect(m_adveq_list_hint_, SIGNAL(choiceStr(QString)), this, SLOT(ChangeFromStationText(QString)));

    ResetAdveqTaskTime();
    ret = connect(ui.pbtn_add_adveq_task, SIGNAL(clicked()), this, SLOT(DoAddAdveqTask()));
    ret = connect(ui.pbtn_adveq_need_capital, SIGNAL(clicked()), this, SLOT(DoAdveqGetNeedCapital()));
}

void WinnerWin::DoAddAdveqTask()
{
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString text_str = ui.le_adveq_stock->text().trimmed();
    QString stock_str = text_str.section('/', 0, 0, flag);
    QString stock_pinyin = text_str.section('/', 1, 1, flag);
    if( !CheckAdveqTaskWinInput(stock_str, false) )
        return;

    auto task_info = std::make_shared<T_TaskInformation>();
    task_info->type = TypeTask::ADVANCE_SECTION;
	task_info->stock = stock_str.toLocal8Bit();
	task_info->stock_pinyin = stock_pinyin.toLocal8Bit();
     
    //task_info->continue_second = 5;
    task_info->advance_section_task.is_original = true;
	task_info->rebounce = ui.spb_adveq_rebounce->value();
    task_info->quantity = ui.spinBox_adveq_qty->value();
    task_info->target_price_level = ui.combox_adveq_price_level->currentData().toInt();
    task_info->start_time = ui.timeEdit_adveq_begin->time().toString("Hmmss").toInt();
    task_info->end_time = ui.timeEdit_adveq_end->time().toString("Hmmss").toInt();
    task_info->state = 1;		

    double top_price = ui.dbspb_adveq_max_price->value();
    double bottom_price = ui.dbspb_adveq_min_price->value();
    const int section_count = ui.spb_adveq_section_count->value(); 
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
     
    auto advance_section_task = std::make_shared<AdvanceSectionTask>(*task_info, this->app_);
     
#ifdef ADVSEC_BUILDPOS_CONDIDER_EXISTPOS
    // ajust postion state base on current positoin -----------
    auto p_position = app_->QueryPosition(task_info->stock);
    if( p_position && p_position->total >= task_info->quantity )
    {
        T_StockPriceInfo *p_info = app_->GetStockPriceInfo(task_info->stock, false);
        if( p_info )
        {
            advance_section_task->SetSectionState(p_info->cur_price, p_position->total);
            task_info->advance_section_task.portion_states  = advance_section_task->task_info().advance_section_task.portion_states;
        }
    }
#endif 
    if( !app_->db_moudle().AddTaskInfo(task_info) ) // allocate task id and save to db
    {
        // log error
		app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("贝塔区间任务添加失败!"));
		app_->AppendLog2Ui("添加贝塔区间任务 : %d 失败! fail db_moudle().AddTaskInfo \n", task_info->id);
        return;
    }
    advance_section_task->task_info().id = task_info->id;

    app_->AppendTaskInfo(task_info->id, task_info);
    // add to task list ui
    InsertIntoTbvTasklist(ui.tbview_tasks, *task_info);             
    app_->AppendStrategyTask(std::shared_ptr<StrategyTask>(advance_section_task));
    

    app_->ticker_strand().PostTask([advance_section_task, this]()
    {
        this->app_->stock_ticker().Register(std::shared_ptr<StrategyTask>(advance_section_task));
    });
    
    app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("贝塔区间任务添加成功!"));
    app_->AppendLog2Ui("添加贝塔区间任务 : %d %s 成功\n", task_info->id, task_info->stock.c_str());

    //ResetAdveqTaskTime();
}

void WinnerWin::ResetAdveqTaskTime()
{
	ui.timeEdit_adveq_begin->setTime(QTime(9, 30, 0));
    ui.timeEdit_adveq_end->setTime(QTime(15, 00, 0));
}

void WinnerWin::DoAdveqGetNeedCapital()
{
    if( ui.dbspb_adveq_max_price->value() < ui.dbspb_adveq_min_price->value() + 0.05 )
    {
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("顶部价格必须大于底部价格一定值!"));
        return;
    }
   
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString text_str = ui.le_adveq_stock->text().trimmed();
    QString stock_str = text_str.section('/', 0, 0, flag);

    if( !CheckAdveqTaskWinInput(stock_str, true) )
        return;
     
    double qty = ui.spinBox_adveq_qty->value();
    double top_price = ui.dbspb_adveq_max_price->value();
    double bottom_price = ui.dbspb_adveq_min_price->value();
    const int section_count = ui.spb_adveq_section_count->value(); 
    double atom_h = (top_price - bottom_price ) / section_count;
    double need_capital = 0.0;
    for( int i = 0; i < section_count; ++i )
    {
        double cur_sec_bottom = bottom_price + atom_h * i;
        need_capital += (cur_sec_bottom + atom_h / 2) * qty;
    } 
    ui.dbspb_adveq_start_capital->setValue(need_capital);
}

bool WinnerWin::CheckAdveqTaskWinInput(const QString &stock_str, bool is_calc_capital)
{ 
    if( stock_str.length() != 6 )
    {
        // todo: show erro info
        ui.le_adveq_stock->setFocus();
        return false;
    } 
    if( !is_calc_capital )
    {
        if( app_->db_moudle().IsTaskExists(app_->user_info().id, TypeTask::ADVANCE_SECTION, stock_str.toStdString()) )
        {
            //QMessageBox::information(nullptr, "notice", QString::fromLocal8Bit("任务已经存在!"));
            app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("任务已经存在!"));
            return false;
        }
    }
    if( ui.dbspb_adveq_max_price->value() < ui.dbspb_adveq_min_price->value() + 0.05 )
    {
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("顶部价格必须大于底部价格一定值!"));
        return false;
    }
    int section_count = ui.spb_adveq_section_count->value();
    if( section_count < 2 )
    {
        app_->msg_win().ShowUI(QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("区间数必须大于1!"));
        return false;
    }
    
    if( ui.spinBox_adveq_qty->value() < 100 )
    {
        ui.spinBox_adveq_qty->setFocus();
        return false;
    }

    if( ui.dbspb_adveq_max_price->value() > 999.0 )
    {
        ui.dbspb_adveq_max_price->setFocus();
        return false;
    }
    if( ui.dbspb_adveq_min_price->value() < 0.01 )
    {
        ui.dbspb_adveq_min_price->setFocus();
        return false;
    }
    
    if( !is_calc_capital )
    {
        if( ui.spb_adveq_rebounce->value() < 0.01 )
        {
            ui.spb_adveq_rebounce->setFocus();
            return false;
        }
        auto start_time = ui.timeEdit_adveq_begin->time().toString("Hmmss").toInt();
        auto end_time = ui.timeEdit_adveq_end->time().toString("Hmmss").toInt();
        if( start_time >= end_time )
        {
            ui.timeEdit_adveq_begin->setFocus();
            return false;
        }
    }
    return true;
}