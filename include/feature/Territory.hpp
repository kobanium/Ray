/**
 * @file include/feature/Territory.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Territory checker.
 * @~japanese
 * @brief 領地の判定
 */
#ifndef _TERRITORY_HPP_
#define _TERRITORY_HPP_

#include "board/GoBoard.hpp"
#include "mcts/Statistic.hpp"


// 残っている死石があるかを判定
bool IsRemainingDeadStone( const game_info_t *game, const int color, const statistic_t statistic[] );

#endif
