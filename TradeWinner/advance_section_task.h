#ifndef ADVANCE_SECTION_TASK_SSDFS3DFDS_H_
#define ADVANCE_SECTION_TASK_SSDFS3DFDS_H_

#include "strategy_task.h"
 
class WinnerApp;
class AdvanceSectionTask : public StrategyTask
{
public:
	enum class PortionState : unsigned char { WAIT_BUY = 0, WAIT_SELL, UNKNOW = 255 }; // UNKNOW means wait create position
	class Portion
	{
	public:
		Portion(int index,double bottom, double top, PortionState state);
		
		void state(PortionState val) {  state_ = val; }
		PortionState state() const { return state_; }

		int index() const { return index_; } 
		double bottom_price() const{ return bottom_price_; } 
		double mid_price() const{ return mid_price_; } 
		double top_price() const{ return top_price_; } 
        std::string Detail();

	private:
		int index_; 
		double bottom_price_;
		double mid_price_;
		double top_price_;
		PortionState state_; 

	};

	AdvanceSectionTask(T_TaskInformation &task_info, WinnerApp *app, T_MockStrategyPara *mock_para=nullptr);
	virtual ~AdvanceSectionTask(){}
	virtual void HandleQuoteData() override;

    virtual void Reset(bool is_mock) override;

    void SetSectionState(double price, int position);

protected:

private:
    
    void reset_flag_price(double cur_price);
    std::tuple<int, double, bool> judge_any_pos2buy(double cur_price, int cur_index, int para_qty_can_buy,  bool is_do_change);
    std::tuple<int, double, bool> judge_any_pos2sell(double cur_price, int cur_index, int para_avaliable_pos,  bool is_do_change);
      
    std::string Detail();
    std::string TagOfCurTask();
    void LogState(double cur_price, int cur_index, bool is_order, const std::string &other_info);

private:

    WinnerApp *app_;
	//unsigned int section_num_;
	std::vector<Portion> portions_; // from bottom portion to up portion
     
	bool is_any_portion_unknow_;

    //------used for rebounce trade
    double reb_bottom_price_;
    double reb_top_price_;
    double reb_base_price_;

    unsigned int is_not_enough_capital_continue_;
    unsigned int is_not_position_continue_;

    //volatile bool  is_wait_trade_result_; 
    unsigned int inter_count_for_debug_;
    unsigned int clearing_count_;
};

std::string ToString(AdvanceSectionTask::PortionState val);

#endif // ADVANCE_SECTION_TASK_SSDFS3DFDS_H_
