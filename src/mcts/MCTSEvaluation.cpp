#include "mcts/MCTSEvaluation.hpp"
#include "mcts/ucb/UCBEvaluation.hpp"





int
SelectMaxUcbChild( uct_node_t &node, const int moves, const int color, std::mt19937_64 &mt )
{
  const int move_count = node.move_count;

  if ((move_count & 0x7f) == 0 && move_count != 0) {
    //SortByDynamicParameters(node, color);
  }

  /*
  if (IsOverExpansionCount(node)) {
    AddCandidate(node);
  }
  */
  return SelectBestChildIndexByUCB1(node, mt);
}
