#ifndef _SIMULATION_HPP_
#define _SIMULATION_HPP_

#include <random>

#include "board/GoBoard.hpp"

// 対局のシミュレーション(知識あり)
void Simulation( game_info_t *game, int color, std::mt19937_64 *mt );

#endif
