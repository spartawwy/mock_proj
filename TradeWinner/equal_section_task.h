#ifndef EQUAL_SECTION_TASK_H_SDW3DFSDK_
#define EQUAL_SECTION_TASK_H_SDW3DFSDK_

#include "strategy_task.h"

class EqualSectionTask: public StrategyTask
{
public:

	enum class TypeAction : char { NOOP = 0, PREPARE_BUY, PREPARE_SELL, CLEAR};
    std::string ToString(TypeAction val);

	static void CalculateSections(double price, IN T_TaskInformation &task_info, OUT std::vector<T_SectionAutom> &sections);
	static void TranslateSections(IN std::vector<T_SectionAutom> &sections, OUT std::string &sections_str);

    EqualSectionTask(T_TaskInformation &task_info, WinnerApp *app);

    virtual ~EqualSectionTask(){}
    virtual void HandleQuoteData() override;
    virtual void UnReg() override { timed_mutex_wrapper_.unlock(); } 

private:

	TypeAction JudgeTypeAction(std::shared_ptr<QuotesData> & quote_data, int total_position, int valide_position, int *out_index=nullptr);

	std::vector<T_SectionAutom> sections_;

	//------used for rebounce trade
	double bottom_price_;
	double top_price_;
	double prepare_rebounce_price_;

	double cond4_sell_backtrigger_price_;
	double cond4_buy_backtrigger_price_;

	TypeAction cur_type_action_; 
	
};

#endif