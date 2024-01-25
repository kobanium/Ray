#include <algorithm>

#include "feature/Territory.hpp"



static void CalculateOwner( const statistic_t statistic[], const int color, double owner[] );


static void
CalculateOwner( const statistic_t statistic[], const int color, double owner[] )
{
  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    const int sum = statistic[pos].colors[S_BLACK] + statistic[pos].colors[S_WHITE];
    if (sum != 0) {
      owner[pos] = static_cast<double>(statistic[pos].colors[color]) / sum;
    }
  }
}





bool
IsRemainingDeadStone( const game_info_t *game, const int color, const statistic_t statistic[] )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  double owner[BOARD_MAX];

  std::fill_n(owner, BOARD_MAX, 0);

  CalculateOwner(statistic, color, owner);

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (board[pos] == other && owner[pos] > 0.95) {
      return true;
    }
  }

  return false;
}
