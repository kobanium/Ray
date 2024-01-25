#ifndef _TERRITORY_HPP_
#define _TERRITORY_HPP_

#include "board/GoBoard.hpp"
#include "mcts/Statistic.hpp"


bool IsRemainingDeadStone( const game_info_t *game, const int color, const statistic_t statistic[] );

#endif
