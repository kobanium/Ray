#include "mcts/MCTSNode.hpp"



void
InitializeNode( uct_node_t &node, const int pm1, const int pm2 )
{
  node.previous_move1 = pm1;
  node.previous_move2 = pm2;
  node.move_count = 0;
  node.win = 0;
  node.width = 0;
  node.child_num = 0;
  std::fill_n(node.seki, BOARD_MAX, false);
}


void
InitializeCandidate( child_node_t &child, int &child_num, const int pos, const bool ladder )
{
  child.pos = pos;
  child.move_count = 0;
  child.win = 0;
  child.index = NOT_EXPANDED;
  child.rate = 0.0;
  child.pw = false;
  child.open = false;
  child.ladder = ladder;

  child_num++;
}


void
AddVirtualLoss( uct_node_t &node, child_node_t &child )
{
  atomic_fetch_add(&node.move_count, VIRTUAL_LOSS);
  atomic_fetch_add(&child.move_count, VIRTUAL_LOSS);
}


void
UpdateResult( uct_node_t &node, child_node_t &child, const int result )
{
  atomic_fetch_add(&node.win, result);
  atomic_fetch_add(&node.move_count, 1 - VIRTUAL_LOSS);
  atomic_fetch_add(&child.win, result);
  atomic_fetch_add(&child.move_count, 1 - VIRTUAL_LOSS);
}


void
ReuseRootCandidateWithoutLadderMove( uct_node_t &node, const bool ladder[] )
{
  const int child_num = node.child_num;
  child_node_t *child = node.child;

  for (int i = 0; i < child_num; i++) {
    const int pos = child[i].pos;

    child[i].rate = 0.0;
    child[i].pw = false;
    child[i].open = false;
    if (ladder[pos]) {
      node.move_count -= child[i].move_count;
      node.win -= child[i].win;
      child[i].move_count = 0;
      child[i].win = 0;
    }

    child[i].ladder = ladder[pos];
  }
}


double
CalculatePassWinningPercentage( const uct_node_t &node )
{
  const child_node_t &pass_child = node.child[PASS_INDEX];

  if (pass_child.move_count != 0) {
    return (double)pass_child.win / pass_child.move_count;
  } else {
    return 0.0;
  }
}
