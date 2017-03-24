#ifndef _LADDER_H_
#define _LADDER_H_

#include "GoBoard.h"

// 蜈ィ縺ヲ縺ョ騾」縺ォ蟇セ縺励※騾£縺ヲ蜉ゥ縺九ｋ繧キ繝√Ι繧ヲ縺九←縺°遒コ隱
void LadderExtension( game_info_t *game, int color, bool *ladder_pos );

// 繧キ繝√Ι繧ヲ謗「邏「
bool IsLadderCaptured( int depth, game_info_t *game, int ren_xy, int turn_color );

// 謌ヲ陦鍋噪迚ケ蠕エ逕ィ縺ョ髢「謨ー
bool CheckLadderExtension( game_info_t *game, int color, int pos );

#endif
