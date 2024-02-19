/**
 * @file include/feature/Seki.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Seki checker.
 * @~japanese
 * @brief セキの判定
 */
#ifndef _SEKI_HPP_
#define _SEKI_HPP_

#include "board/GoBoard.hpp"


// セキの判定
void CheckSeki( const game_info_t *game, bool seki[] );

#endif
