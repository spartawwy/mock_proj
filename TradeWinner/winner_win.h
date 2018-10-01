#ifndef TRADE_WINNER_H
#define TRADE_WINNER_H

#include <memory>
#include <list>

#include <QtWidgets/QMainWindow>
#include "ui_trade_winner.h"
#include "about_soft_win.h"

#include "common.h"

#include "timer_container.h"

//class MyThread;
//class StockTicker;
class StrategyTask;
class WinnerApp;
class HintList;
class RecordsWin;
class CalcWin;
class T_MockStrategyPara;
class DetailWin;
class WinnerWin : public QMainWindow
{
    Q_OBJECT

public:

    WinnerWin(WinnerApp *app, QWidget *parent = 0);
    ~WinnerWin();

    void Init();

    virtual void keyPressEvent(QKeyEvent *event) override;

    void ClearLog();

public slots:

    void DoQueryCapital();

    void SlotAppendLog(char *);

	void SlotTabChanged(int);
    void SlotTbvTasksContextMenu(QPoint p);

    void SlotTbvTasksActionStart(bool);
    void SlotTbvTasksActionStop(bool);
    void SlotTbvTasksActionDel(bool);
    void SlotTbvTasksActionDetail(bool);

    void RemoveByTaskId(int task_id);

    void DoStatusBar(const std::string& str);
    void DoStatusBar(std::string* str, bool is_delete=false);

    void DoFlashWin();

    // sell task win--------------------
    void DoAlertPercentChanged(double val);
    void DoSellTypeChanged(const QString&);
    void ChangeTabSellAssistantImg(TypeTask type);

    void DoLeStockEditingFinished();
    void DoLeStockChanged(const QString &);

    
    void DoSpboxQuantityEditingFinished();
     
    void DoAddTask();
    void DoTaskStatChangeSignal(StrategyTask*, int);
    void DoQueryPosition();

    void InsertIntoTbvTasklist(QTableView *tbv , T_TaskInformation &task_info);

    void ResetSellTabTaskTime();

    //---------------buy task related---
    void DoBuyAlertPercentChanged(double);
	void DoBuyTypeChanged(const QString&);
    void DoAddBuyTask();
    void DoQueryQtyCanBuy();
	void DoBuyStock();

	void ResetBuyTabTaskTime();
	void InsertIntoBktestTbvTask(T_TaskInformation &task_info, T_MockStrategyPara &mock_para, int date_begin, int date_end);

    //---------------eqsection task related----
	void DoAddEqSectionTask();
	void ResetEqSectionTaskTime();
    void DoMaxQtyCheckBoxChanged(int);
    void DoMinQtyCheckBoxChanged(int);

	void DoMaxStopTrigCheckBoxChanged(int);
	void DoMinClearTrigCheckBoxChanged(int);

	void DoRebounceCheckBoxChanged(int);

    //---------------index trade task related----
	void DoAddIndexTradeTask();
	void DoTrdIndexRadioCrossDownChecked(bool);
	void DoTrdIndexRadioCrossUpChecked(bool);

	void DoTrdIndexAlertBtnBtnChecked(bool);
	void DoTrdIndexRelBtnBtnChecked(bool);
	void DoTrdIndexClearBtnChecked(bool);

    //---------------back test related ---
    void DoBktestTypeChanged(const QString&);
    void DoStartBacktest(bool);
    void DoEnableBtnBackTest();
    void DoAdveqGetNeedCapital();
    void DoBktestAddTask();
    void DoBktestClearTask();
    void DoBktestShowOrderDetail(const QModelIndex &); 
    //------------------
	
    void ChangeTabBuyAssistantImg(TypeTask type);

    int TbvTasksCurRowTaskId();
    void FlushFromStationListWidget(QString str);
    void OnClickedListWidget(QModelIndex index);
    void ChangeFromStationText(QString text);
	 
    void AssignHintListAndLineEdit(HintList *& p_list, QLineEdit *&p_edit, QDoubleSpinBox *&p_dbspb_alert_price, QDoubleSpinBox *&p_dbspb_percent);

	void SlotOpenRecordsWin(bool);
    void SlotOpenCalcWin(bool);
    void SlotOpenAbout(bool);

    void TriggerFlashWinTimer(bool enable=true);

    void DoTabTasksDbClick(const QModelIndex index);

signals:

    //void SigRemoveTask(int task_id);

protected:

    virtual void closeEvent(QCloseEvent * event) override;
    virtual void changeEvent(QEvent * event) override;

private:

    void DoShowTaskDetail(int task_id);

    // sell task related 
    void InitSellTaskWin();
    void SetupSellTaskWin();
    void FillSellTaskWin(TypeTask type, T_TaskInformation& info);
    
    // buy task related
    void InitBuyTaskWin();
    void FillBuyTaskWin(TypeTask type, T_TaskInformation& info);

    // equal section task related
    void InitEqSectionTaskWin();

	// index trade task related 
	void InitIndexTradeWin();

    // back test related 
    bool InitBacktestWin();
	void UnInstallBacktest();
    void ShowBktestOrderDetail(const std::string& content);
    //-------------------
    Ui::TradeWinnerClass ui;
    AboutSoftWin  about_win_;
    DetailWin     *detail_win_;
    WinnerApp *app_;
    QMenu *tbv_tasks_popMenu_;

    HintList *m_list_hint_;
	bool is_open_hint_;

    // buy task related
    HintList *m_bt_list_hint_; 

	// eqsection task related
    HintList *m_eqsec_list_hint_; 

	// index trade task related
    HintList *m_indtrd_list_hint_; 

    // back test related 
    HintList *m_backtest_list_hint_;

    QLabel  *status_label_;
	std::shared_ptr<RecordsWin> records_win_;
    std::shared_ptr<CalcWin> calc_win_;

    double cur_price_;
    double buytask_cur_price_;
    double eqsec_task_cur_price_;
    double backtest_cur_price_;

    QTimer *flash_win_timer_;

    std::shared_ptr<TimerContainner>  oneceshot_timer_contain_;
};

#endif // TRADE_WINNER_H
