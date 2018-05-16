#ifndef POSITION_MOCKER_SDFD_H_
#define POSITION_MOCKER_SDFD_H_

#include <memory>
#include <unordered_map>

#include "common_base.h"

typedef std::unordered_map<std::string, T_PositionData> T_CodeMapPosition;

class PositionMocker
{
public:

    PositionMocker();

private:

    T_CodeMapPosition  positions_;


};
#endif // POSITION_MOCKER_SDFD_H_