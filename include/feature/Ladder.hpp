#ifndef _LADDER_HPP_
#define _LADDER_HPP_

#include "board/GoBoard.hpp"

// 全ての連に対して逃げて助かるシチョウかどうか確認
void LadderExtension( game_info_t *game, int color, bool *ladder_pos );
// 戦術的特徴用の関数
bool CheckLadderExtension( const game_info_t *game, const int color, const int pos );
#endif
