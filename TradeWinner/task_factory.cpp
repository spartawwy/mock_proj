#include "task_factory.h"

#include "breakdown_task.h"
#include "inflection_sell_task.h"
#include "follow_sell_task.h"

#include "inflection_buy_task.h"
#include "breakup_buy_task.h"
#include "batches_buy_task.h"
#include "batches_sell_task.h"
#include "equal_section_task.h"
#include "advance_section_task.h"
#include "index_task.h"

#include "winner_app.h"
 
void TaskFactory::CreateAllTasks(std::unordered_map<int, std::shared_ptr<T_TaskInformation> >& task_info_holder
                                 , std::list<std::shared_ptr<StrategyTask> >& out_task_objs, WinnerApp *app)
{
    std::list<int> temp_list;
    std::for_each( std::begin(task_info_holder), std::end(task_info_holder)
        , [&temp_list](std::unordered_map<int, std::shared_ptr<T_TaskInformation> >::reference entry)
    {
        temp_list.push_back(entry.first);
    });
    temp_list.sort();
    
    std::for_each( std::begin(temp_list), std::end(temp_list), [&app, &task_info_holder, &out_task_objs](int id)
    {
        auto iter = task_info_holder.find(id);
        if( iter != task_info_holder.end() )
        {
            switch(iter->second->type)
            {
            case TypeTask::INFLECTION_BUY:
                { 
                   out_task_objs.push_back( std::move(std::make_shared<InflectionBuyTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::BREAKUP_BUY:
                { 
                   out_task_objs.push_back( std::move(std::make_shared<BreakUpBuyTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::BATCHES_BUY:
                {
                   out_task_objs.push_back( std::move(std::make_shared<BatchesBuyTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::INFLECTION_SELL:
                { 
                    out_task_objs.push_back( std::move(std::make_shared<InflectionSellTask>(*iter->second, app)) );
                    break;
                }
            case TypeTask::BREAK_SELL:
                {
                    out_task_objs.push_back( std::move(std::make_shared<BreakDownTask>(*iter->second, app)) );
                    break;
                }
            case TypeTask::FOLLOW_SELL:
                {  
                   out_task_objs.push_back( std::move(std::make_shared<FollowSellTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::BATCHES_SELL:
                {  
                   out_task_objs.push_back( std::move(std::make_shared<BatchesSellTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::EQUAL_SECTION:
                {  
                   out_task_objs.push_back( std::move(std::make_shared<EqualSectionTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::ADVANCE_SECTION:
                {  
                   out_task_objs.push_back( std::move(std::make_shared<AdvanceSectionTask>(*iter->second, app)) );
                   break;
                }
            case TypeTask::INDEX_RISKMAN:
            {  
                out_task_objs.push_back( std::move(std::make_shared<IndexTask>(*iter->second, app)) );
                break;
            }
            default: break;
            }
        }
    });
}