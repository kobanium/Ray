/**
 * @file Message.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Console message writer.
 * @~japanese
 * @brief コンソール出力処理
 */
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>


#include "board/Point.hpp"
#include "common/Message.hpp"
#include "mcts/UctSearch.hpp"


/**
 * @~english
 * @brief Output flag for debug message.
 * @~japanese
 * @brief デバッグメッセージ出力フラグ
 */
static bool debug_message = true;


/**
 * @~english
 * @brief
 * @param[in] flag
 * @~japanese
 * @brief
 * @param[in] flag
 */
void
SetDebugMessageMode( const bool flag )
{
  debug_message = flag;
}


/**
 * @~english
 * @brief Print board information.
 * @param[in] game Board position data.
 * @~japanese
 * @brief 盤面の表示
 * @param[in] game 局面情報
 */
void
PrintBoard( const game_info_t *game )
{
  const char stone[S_MAX] = { '+', 'B', 'W', '#' };
  std::ostringstream oss;

  if (!debug_message) return;

  oss << "Prisoner(Black) : " << game->prisoner[S_BLACK] << "\n";
  oss << "Prisoner(White) : " << game->prisoner[S_WHITE] << "\n";
  oss << "Move : " << game->moves << "\n";

  oss << "    ";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    oss << " " << gogui_x[i];
  }
  oss << "\n";

  oss << "   +";
  for (int i = 0; i < pure_board_size * 2 + 1; i++) {
    oss << "-";
  }
  oss << "+" << "\n";

  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    oss << std::setw(2) << (pure_board_size + 1 - i) << ":|";
    for (int x = board_start; x <= board_end; x++) {
      oss << " " << stone[static_cast<int>(game->board[POS(x, y)])];
    }
    oss << " |" << "\n";
  }

  oss << "   +";
  for (int i = 1; i <= pure_board_size * 2 + 1; i++) {
    oss << "-";
  }
  oss << "+";

  std::cerr << oss.str() << std::endl;
}


/**
 * @~english
 * @brief Print string's information.
 * @param[in] game Board position data.
 * @~japanese
 * @brief 連の情報の表示 (呼吸点の数と座標, 連を構成する石の数と座標)
 * @param[in] game 局面情報
 */
void
PrintString( const game_info_t *game )
{
  const string_t *string = game->string;
  int pos, neighbor;

  if (!debug_message) return;

  std::cerr << "  :: :: String :: ::" << std::endl;

  for (int i = 0; i < MAX_STRING; i++){
    if (string[i].flag) {
      if (game->board[string[i].origin] == S_BLACK) {
        std::cerr << "Black String   ";
      } else {
        std::cerr << "White String   ";
      }
      std::cerr << "ID : " << i << " (libs : " << string[i].libs << ", size : " << string[i].size << ")" << std::endl;
      pos = string[i].lib[0];

      std::cerr << "  Liberty : " << std::endl;
      std::cerr << "  ";
      while (pos != STRING_END) {
        std::cerr << GOGUI_X(pos) << GOGUI_Y(pos) << " ";
        pos = string[i].lib[pos];
      }
      std::cerr << std::endl;

      pos = string[i].origin;
      std::cerr << "  Stone : " << std::endl;
      std::cerr << "    ";
      while (pos != STRING_END) {
        std::cerr << GOGUI_X(pos) << GOGUI_Y(pos) << " ";
        pos = game->string_next[pos];
        if (pos == game->string_next[pos]) getchar();
      }
      std::cerr << std::endl;

      neighbor = string[i].neighbor[0];
      if (neighbor == 0) getchar();
      std::cerr << "  Neighbor : " << std::endl;
      std::cerr << "    ";
      while (neighbor < NEIGHBOR_END) {
        std::cerr << neighbor << " ";
        neighbor = string[i].neighbor[neighbor];
      }
      std::cerr << std::endl;
    }
  }

  std::cerr << std::endl;
}


/**
 * @~english
 * @brief Print string ID for each position.
 * @param[in] game Board position data.
 * @~japanese
 * @brief 各座標の連IDの表示
 * @param[in] game 局面情報　
 */
void
PrintStringID( const game_info_t *game )
{
  if (!debug_message) return;

  std::cerr << "    ";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << "   " << gogui_x[i];
  }
  std::cerr << std::endl;

  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << std::setw(3) << (pure_board_size + 1 - i) << ":";
    for (int x = board_start; x <= board_end; x++) {
      const int pos = x + y * board_size;
      if (game->string[game->string_id[pos]].flag) {
        std::cerr << " " << std::setw(3) << game->string_id[pos];
      } else {
        std::cerr << "   -";
      }
    }
    std::cerr << std::endl;
  }
  std::cerr << std::endl;
}


/**
 * @~english
 * @brief Print string list connection (for debugging)
 * @param[in] game
 * @~japanese
 * @brief 連リストのつながりの表示 (デバッグ用)
 * @param[in] game
 */
void
PrintStringNext( const game_info_t *game )
{
  if (!debug_message) return;

  std::cerr << "    ";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << "   " << gogui_x[i];
  }
  std::cerr << std::endl;

  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << std::setw(3) << (pure_board_size + 1 - i);
    for (int x = board_start; x <= board_end; x++) {
      const int pos = x + y * board_size;
      if (game->string[game->string_id[pos]].flag) {
        if (game->string_next[pos] != STRING_END) {
          std::cerr << std::setw(3) << game->string_next[pos];
        } else {
          std::cerr << " END";
        }
      } else {
        std::cerr << "   -";
      }
    }
    std::cerr << std::endl;
  }
  std::cerr << std::endl;
}


/**
 * @~english
 * @brief Print ownership.
 * @param[in] root MCTS root node.
 * @param[in] statistic Statistic information for Monte-Carlo simulation.
 * @param[in] color Player's color.
 * @param[out] own Ownership.
 * @~japanese
 * @brief Ownerの表示
 * @param[in] root MCTSルートノード
 * @param[in] statistic モンテカルロ・シミュレーションの統計情報
 * @param[in] color 手番の色
 * @param[out] own Owner
 */
void
PrintOwner( const uct_node_t *root, const statistic_t *statistic, const int color, double *own )
{
  int player = 0, opponent = 0;
  double owner, score;
  std::ostringstream oss;

  if (!debug_message) return;

  oss << "   ";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    oss << "   " << gogui_x[i];
  }
  oss << "\n";

  oss << "   +";
  for (int i = 0; i < pure_board_size * 4; i++) {
    oss << "-";
  }
  oss << "+" << "\n";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    oss << std::setw(2) << (pure_board_size + 1 - i) << ":|";
    for (int x = board_start; x <= board_end; x++) {
      const int pos = POS(x, y);
      owner = static_cast<double>(statistic[pos].colors[color]) / root->move_count;
      if (owner > 0.5) {
        player++;
      } else {
        opponent++;
      }
      own[pos] = owner * 100.0;
      oss << std::setw(3) << static_cast<int>(owner * 100) << " ";
    }
    oss << "|" << "\n";
  }

  oss << "   +";
  for (int i = 0; i < pure_board_size * 4; i++) {
    oss << "-";
  }
  oss << "+" << "\n";

  if (color == S_BLACK) {
    if (player - opponent > komi[0]) {
      score = player - opponent - komi[0];
      oss << "BLACK+" << score << "\n";
    } else {
      score = -(player - opponent - komi[0]);
      oss << "WHITE+" << score << "\n";
    }
  } else {
    if (player - opponent > -komi[0]) {
      score = player - opponent + komi[0];
      oss << "WHITE+" << score << "\n";
    } else {
      score = -(player - opponent + komi[0]);
      oss << "BLACK+" << score << "\n";
    }
  }

  std::cerr << oss.str();
}


/**
 * @~english
 * @brief Print best move sequence.
 * @param[in] game Board position data.
 * @param[in] uct_node MCTS nodes.
 * @param[in] root Root node index.
 * @param[in] start_color Player's color.
 * @~japanese
 * @brief 最善応手列の出力
 * @param[in] game 局面情報
 * @param[in] uct_node MCTSノード
 * @param[in] root Root ルートノードのインデックス
 * @param[in] start_color 手番の色
 */
void
PrintBestSequence( const game_info_t *game, const uct_node_t *uct_node, const int root, const int start_color )
{
  game_info_t *search_result;
  int current = root;
  int index = -1;
  int max = 0;
  int color = start_color;
  int child_num;
  const child_node_t *uct_child;

  if (!debug_message) return ;

  search_result = AllocateGame();
  CopyGame(search_result, game);

  std::cerr << "Best Sequence : ";

  uct_child = uct_node[current].child;
  child_num = uct_node[current].child_num;

  for (int i = 0; i < child_num; i++) {
    if (uct_child[i].move_count > max) {
      max = uct_child[i].move_count;
      index = i;
    }
  }

  if (uct_child[index].pos == PASS) std::cerr << "PASS";
  else std::cerr << GOGUI_X(uct_child[index].pos) << GOGUI_Y(uct_child[index].pos);
  if (node_hash[current].color == S_BLACK) std::cerr << "(BLACK : ";
  else if (node_hash[current].color == S_WHITE) std::cerr << "(WHITE : ";


  PutStone(search_result, uct_child[index].pos, color);
  color = GetOppositeColor(color);

  std::cerr << uct_child[index].win << "/" << uct_child[index].move_count << ")";

  current = uct_child[index].index;
  
  while (current != NOT_EXPANDED) {
    uct_child = uct_node[current].child;
    child_num = uct_node[current].child_num;

    max = 50;
    index = -1;

    for (int i = 0; i < child_num; i++) {
      if (uct_child[i].move_count > max) {
        max = uct_child[i].move_count;
        index = i;
      }
    }

    if (index == -1) break;

    std::cerr << "->";

    if (uct_child[index].pos == PASS) std::cerr << "PASS";
    else std::cerr << GOGUI_X(uct_child[index].pos) << GOGUI_Y(uct_child[index].pos);

    if (node_hash[current].color == S_BLACK) std::cerr << "(BLACK : ";
    else if (node_hash[current].color == S_WHITE) std::cerr << "(WHITE : ";

    PutStone(search_result, uct_child[index].pos, color);

    color = GetOppositeColor(color);

    std::cerr << uct_child[index].win << "/" << uct_child[index].move_count << ")";

    current = uct_child[index].index;

  }

  std::cerr << std::endl;

  PrintBoard(search_result);
  FreeGame(search_result);
}


/**
 * @~english
 * @brief Print search information.
 * @param[in] root UCT root node.
 * @param[in] po_speed Search speed.
 * @param[in] finish_time Elapsed time.
 * @param[in] pre_simulated Pre-simulated count.
 * @~japanese
 * @brief 探索情報の表示
 * @param[in] root ルートノード
 * @param[in] po_speed 探索速度
 * @param[in] finish_time 消費時間
 * @param[in] pre_simulated 探索開始前までに探索した回数
 */
void
PrintPlayoutInformation( const uct_node_t *root, const int po_speed, const double finish_time, const int pre_simulated )
{
  const double winning_percentage = static_cast<double>(root->win) / root->move_count;

  if (!debug_message) return;

  std::cerr << "All Playouts       :  " << std::setw(7) << root->move_count << std::endl;
  std::cerr << "Pre Simulated      :  " << std::setw(7) << pre_simulated << std::endl;
  std::cerr << "Win                :  " << std::setw(7) << root->win << std::endl;
  std::cerr << "Thinking Time      :  " << std::setw(7) << finish_time << " seconds" << std::endl;
  std::cerr << "Winning Percentage :  " << std::setw(7) << (winning_percentage * 100) << " %" << std::endl;
  std::cerr << "Playout Speed      :  " << std::setw(7) << po_speed << " PO/s" << std::endl;
}


/**
 * @~english
 * @brief Print coordinate string.
 * @param[in] pos Coordinate
 * @~japanese
 * @brief 座標の文字列の出力
 * @param[in] pos 座標
 */
void
PrintPoint( const int pos )
{
  if (!debug_message) return;

  if (pos == PASS) {
    std::cerr << "PASS" << std::endl;
  } else if (pos == RESIGN) {
    std::cerr << "RESIGN" << std::endl;
  } else {
    std::cerr << GOGUI_X(pos) << GOGUI_Y(pos) << std::endl;
  }
}


/**
 * @~english
 * @brief Print current komi value.
 * @~japanese
 * @brief 現在のコミの値の出力
 */
void
PrintKomiValue( void )
{
  if (!debug_message) return ;

  std::cerr << "Dynamic Komi : " << std::setw(4) << dynamic_komi[0] << std::endl;
}


/**
 * @~english
 * @brief Print the number of pondered playout results.
 * @param[in] count The number of pondered playout results.
 * @~japanese
 * @brief 予測読みした探索回数の出力
 * @param[in] count 予測読みした探索回数
 */
void
PrintPonderingCount( const int count )
{
  if (!debug_message) return ;

  std::cerr << "Ponder : " << count << " Playouts" << std::endl;
}


/**
 * @~english
 * @brief Print playout limits.
 * @param[in] time_limit Time limits for search.
 * @param[in] playout_limit Playout limits for search
 * @~japanese
 * @brief 探索時間と探索回数の制限値の出力
 * @param[in] time_limit 探索時間の制限値
 * @param[in] playout_limit 探索回数の制限値
 */
void
PrintPlayoutLimits( const double time_limit, const int playout_limit )
{
  if (!debug_message) return ;

  std::cerr << "Time Limit    : " << time_limit << " Sec" << std::endl;
  std::cerr << "Playout Limit : " << playout_limit << " PO" << std::endl; 
}


/**
 * @~english
 * @brief Print the number of reused playout results.
 * @param[in] count The number of reused playout results.
 * @~japanese
 * @brief 再利用する探索回数の出力
 * @param[in] count 再利用する探索回数
 */
void
PrintReuseCount( const int count )
{
  if (!debug_message) return ;

  std::cerr << "Reuse : " << count << " Playouts" << std::endl;
}


/**
 * @~english
 * @brief Print too large resign threshold message.
 * @param[in] threshold Resign threshold.
 * @~japanese
 * @brief 投了の閾値が大きすぎる時のメッセージ出力
 * @param[in] threshold 投了の閾値
 */
void
PrintResignThresholdIsTooLarge( const double threshold )
{
  std::ostringstream oss;

  oss << "Resign threshold = " << threshold << " is too large.\n";
  oss << "Resign threshold must be larger than or equal to 0.0, smaller than or equal to 1.0.\n";
  oss << "Ignore this setting.\n";
  std::cerr << oss.str() << std::flush;
}


/**
 * @~english
 * @brief Print too small resign threshold message.
 * @param[in] threshold Resign threshold.
 * @~japanese
 * @brief 投了の閾値が小さすぎるときのメッセージ出力
 * @param[in] threshold 投了の閾値
 */
void
PrintResignThresholdIsTooSmall( const double threshold )
{
  std::ostringstream oss;

  oss << "Resign threshold = " << threshold << " is too small.\n";
  oss << "Resign threshold is set to 0.0.\n";
  oss << "Ray never resigns.\n";
  std::cerr << oss.str() << std::flush;
}


/**
 * @~english
 * @brief Console message output for lz-analyze.
 * @param[in] root MCTS root node.
 * @~japanese
 * @brief lz-analyze用のコンソール出力
 * @param[in] root MCTSルートノード
 */
void
PrintLeelaZeroAnalyze( const uct_node_t *root )
{
  struct verbose_t {
    std::string lz_pos;
    int visits, winrate, prior, lcb, index;
  };

  std::vector<verbose_t> child_verbose;

  for ( int i = 0; i < root->child_num; ++i ) {
    verbose_t v;
    const child_node_t *c = &root->child[i];

    char cpos[10];
    IntegerToString(c->pos, cpos);

    v.lz_pos = std::string{cpos};
    v.visits = c->move_count.load(std::memory_order_relaxed);

    double winrate = 0.5f;
    if (v.visits > 0) {
      winrate = static_cast<double>(c->win.load(std::memory_order_relaxed)) / v.visits;
    }

    v.winrate = std::min(10000, static_cast<int>(10000 * winrate));
    v.prior = std::min(10000, static_cast<int>(10000 * c->rate));
    v.lcb = v.winrate;
    v.index = c->index;

    if (v.visits > 0) {
      child_verbose.emplace_back(v);
    }
  }

  if ( child_verbose.empty() ) return;

  std::sort(std::begin(child_verbose), std::end(child_verbose),
              []( verbose_t &a, verbose_t &b ){
                return a.winrate > b.winrate;
              }
           );

  FILE *STD_STREAM = stdout;

  for ( int i = 0; i < static_cast<int>(child_verbose.size()); i++ ) {
    verbose_t *v = &child_verbose[i];
    fprintf(STD_STREAM, "info move %s visits %d winrate %d prior %d lcb %d order %d pv %s",
              v->lz_pos.c_str(),
              v->visits,
              v->winrate,
              v->prior,
              v->lcb,
              i,
              v->lz_pos.c_str()
           );

    int current = v->index;
    int child_num;
    const child_node_t *uct_child;

    while (current != NOT_EXPANDED) {
      uct_child = uct_node[current].child;
      child_num = uct_node[current].child_num;

      int max = 50;
      int index = -1;

      for (int j = 0; j < child_num; j++) {
        if (uct_child[j].move_count > max) {
          max = uct_child[j].move_count;
          index = j;
        }
      }

      if (index == -1) break;

      char cpos[10];
      IntegerToString(uct_child[index].pos, cpos);
      fprintf(STD_STREAM, " %s", cpos);

      current = uct_child[index].index;
    }

    if (i != static_cast<int>(child_verbose.size()) - 1) {
      fprintf(STD_STREAM, " ");
    }
  }
  fprintf(STD_STREAM, "\n");
  fflush(STD_STREAM);
}


/**
 * @~english
 * @brief Print rating values for all moves.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @~japanese
 * @brief 全ての手に対するレート値の出力
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 */
void
PrintRate( const game_info_t *game, const int color )
{
  std::ostringstream oss;

  oss << "Sum : " << game->sum_rate[color - 1] << "\n";

  for (int y = board_start; y <= board_end; y++) {
    oss << std::setw(6) << game->sum_rate_row[color - 1][y] << " | ";
    for (int x = board_start; x <= board_end; x++) {
      const int pos = POS(x, y);
      oss << std::setw(5) << game->rate[color - 1][pos] << " ";
    }
    oss << "\n";
  }

  std::cerr << oss.str() << std::endl;
}
