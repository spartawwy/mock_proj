#ifndef  TASK_FACTORY_H_W3SDFSD_
#define  TASK_FACTORY_H_W3SDFSD_

#include <memory>
#include <unordered_map>

#include "common.h"

class StrategyTask;
class WinnerApp;
class TaskFactory
{
public:

    TaskFactory();

    static void CreateAllTasks(std::unordered_map<int, std::shared_ptr<T_TaskInformation> >& task_info_holder
            , std::list<std::shared_ptr<StrategyTask> >& task_objs, WinnerApp *app);

};
#endif
