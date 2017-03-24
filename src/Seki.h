#ifndef _SEKI_H_
#define _SEKI_H_

#include "GoBoard.h"

// セキの判定
void CheckSeki( game_info_t *game, bool seki[] );

void PrintSeki( game_info_t *game );

#endif
