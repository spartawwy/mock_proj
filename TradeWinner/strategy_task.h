#ifndef  STRATEGY_TASK_H_SDFDS_
#define STRATEGY_TASK_H_SDFDS_

#include <string>
//#include <queue>
#include <list>
#include <chrono>
#include "mutex_wrapper.h"

#include <TLib/core/tsystem_task_service.h>

#include "common.h"
#include "detail_file.h"

// bktest log use iter and ignore tag
#define DO_BKTST_DETAIL(b)  do{ this->WriteDetail((is_back_test_ ? DateTimeString(iter->time_stamp) : "") + " " + (b)); }while(0);
#define STTG_HEIGH_LOG(tag, b)  do{ if(is_back_test_){DO_BKTST_DETAIL(b)}else DO_TAG_LOG((tag), (b)) }while(0);
// if bktest use inner flag to control 
#define STTG_NORM_LOG(tag, b)  do{ if(is_back_test_){if(bktest_norm_log_flag_)DO_BKTST_DETAIL(b)}else DO_TAG_LOG((tag), (b)) }while(0);


class T_MockStrategyPara 
{
public:
    int  avaliable_position;
    int  frozon_position;
    double  ori_capital;
    double  capital;
    int  date_begin;
    int  date_end;
    //double pre_price;
    std::shared_ptr<QuotesData> pre_data;
    std::shared_ptr<DetailFile> detail_file;

    T_MockStrategyPara() : avaliable_position(0), frozon_position(0), ori_capital(0.0), capital(0.0), date_begin(0), date_end(0), pre_data(nullptr),/*pre_price(0.0),*/ detail_file(nullptr){}
    T_MockStrategyPara(const T_MockStrategyPara &lh) : avaliable_position(lh.avaliable_position), frozon_position(lh.frozon_position)
        , ori_capital(lh.ori_capital), capital(lh.capital), date_begin(lh.date_begin), date_end(lh.date_end), pre_data(lh.pre_data), /*pre_price(lh.pre_price),*/ detail_file(lh.detail_file){}
    T_MockStrategyPara & operator = (const T_MockStrategyPara &lh) 
    {
        if( this == &lh ) return *this;
        avaliable_position = lh.avaliable_position;
        frozon_position = lh.frozon_position;
        ori_capital = lh.ori_capital;
        capital = lh.capital; 
        date_begin = lh.date_begin;
        date_end = lh.date_end;
        //pre_price = lh.pre_price;
        pre_data = lh.pre_data;
        detail_file = lh.detail_file;
        return *this;
    }
};
class WinnerApp;
class StrategyTask
{
public: 

    StrategyTask(T_TaskInformation &task_info, WinnerApp *app, T_MockStrategyPara *bktest_para=nullptr);
    StrategyTask(const std::string &stock, WinnerApp *app, T_MockStrategyPara *bktest_para);

    virtual ~StrategyTask()
    { 
        //timed_mutex_.unlock(); 
    }

    virtual void HandleQuoteData() = 0;
    virtual void UnReg(){ }
    virtual std::string Detail(){ return "";}
     
    bool IsPriceJumpUp(double pre_price, double cur_price);
    bool IsPriceJumpDown(double pre_price, double cur_price);
     
    WinnerApp * app() { return app_;}

    unsigned int  task_id() { return para_.id; }
    const char* stock_code() { return para_.stock.c_str(); }
    char* code_data() { return const_cast<char*>(para_.stock.c_str()); }
    T_TaskInformation& task_info() { return para_; }

    double  cur_price() { return cur_price_; }

    TypeMarket  market_type() const { return market_type_; }

    void SetOriginalState(TaskCurrentState val) { para_.state = static_cast<int>(val); }
    bool is_to_run() const 
    { 
        return para_.state != static_cast<int>(TaskCurrentState::STOP);
    }

    double GetQuoteTargetPrice(const QuotesData& data, bool is_buy);

    void ObtainData(std::shared_ptr<QuotesData> &data);
    
    void cur_state(TaskCurrentState val) { cur_state_ = val; }
    TaskCurrentState cur_state() { return cur_state_; }

    QTime tp_start() { return tp_start_; }
    QTime tp_end() { return tp_end_; }
    double pre_trigged_price() { return pre_trigged_price_; }

    // ---------for mock ------------ 
    virtual void Reset(bool is_mock) {}
    int bktest_mock_date() { return bktest_mock_date_; }
    void do_mock_date_change(int date);
    double GetMockAssets(double price);
    double GetOriMockAssets();
    bool is_waitting_removed() const { return is_waitting_removed_; }
    
    void has_bktest_result_fetched(bool val) { has_bktest_result_fetched_ = val; }
    bool has_bktest_result_fetched() const { return has_bktest_result_fetched_; }
    void ResetBktestResult(){ bktest_para_ = ori_bktest_para_; }
    void WriteDetail(const std::string &content);
    void bktest_norm_log_flag(bool is_open) {bktest_norm_log_flag_ = is_open;}
    //-------------------------------

    unsigned int life_count_; 

protected:
     
   std::string OrderTag();
   std::string NormalTag();

   int HandleSellByStockPosition(double price, bool remove_task = true);
   int GetTototalPosition();
   int GetAvaliablePosition();
   void AddFill2DB(double price, double qty, bool is_buy);
   void ShowError(const std::string &msg);

   WinnerApp  *app_;
   T_TaskInformation  para_;
    
   TypeMarket  market_type_;
   
   std::list<std::shared_ptr<QuotesData> > quote_data_queue_;

   QTime  tp_start_;
   QTime  tp_end_;

   double  cur_price_;
   double  pre_trigged_price_;

   volatile TaskCurrentState cur_state_;
   volatile bool is_waitting_removed_; 

   TSystem::TaskStrand   strand_;
     
   TimedMutexWrapper  timed_mutex_wrapper_;
   
   // ---------for mock ------------
   bool is_back_test_;
   bool bktest_norm_log_flag_;
   T_MockStrategyPara ori_bktest_para_;
   T_MockStrategyPara bktest_para_;
   int bktest_mock_date_;
   bool has_get_mock_assets_;
   double bktest_mock_assets_;
   bool has_bktest_result_fetched_;
   double ori_bktest_price_;
   bool has_set_ori_bktest_price_;

};

// <task id, task>
typedef std::unordered_map<unsigned int, std::shared_ptr<StrategyTask> > TTaskIdMapStrategyTask;

#endif