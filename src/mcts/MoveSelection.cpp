#include "mcts/MoveSelection.hpp"




int
SelectMaxVisitChild( const uct_node_t &root )
{
  const child_node_t *child = root.child;
  int select_index = PASS_INDEX;
  int max_count = child[PASS_INDEX].move_count;

  for (int i = 1; i < root.child_num; i++) {
    if (child[i].move_count > max_count) {
      select_index = i;
      max_count = child[i].move_count;
    }
  }

  return select_index;
}



int
SelectMove( const game_info_t *game, const uct_node_t &root, double &best_wp )
{
  const child_node_t *child = root.child;
  const int select_index = SelectMaxVisitChild(root);
  const double pass_wp = CalculatePassWinningPercentage(root);

  best_wp = (double)child[select_index].win / child[select_index].move_count;
  
  if (pass_wp >= PASS_THRESHOLD &&
      (game->record[game->moves - 1].pos == PASS)) {
    return PASS;
  } else if (game->moves >= MAX_MOVES) {
    return PASS;
  } else if (game->moves > 3 &&
             game->record[game->moves - 1].pos == PASS &&
             game->record[game->moves - 3].pos == PASS) {
    return PASS;
  } else if (best_wp <= RESIGN_THRESHOLD) {
    return RESIGN;
  } else {
    return child[select_index].pos;
  }
}
  
