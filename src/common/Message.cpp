#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "board/Point.hpp"
#include "common/Message.hpp"
#include "mcts/UctSearch.hpp"


bool debug_message = true;


////////////////////////////////////
//  エラーメッセージの出力の設定  //
////////////////////////////////////
void
SetDebugMessageMode( const bool flag )
{
  debug_message = flag;
}

//////////////////
//  盤面の表示  //
//////////////////
void
PrintBoard( const game_info_t *game )
{
  const char stone[S_MAX] = { '+', 'B', 'W', '#' };

  if (!debug_message) return ;

  std::cerr << "Prisoner(Black) : " << game->prisoner[S_BLACK] << std::endl;
  std::cerr << "Prisoner(White) : " << game->prisoner[S_WHITE] << std::endl;
  std::cerr << "Move : " << game->moves << std::endl;

  std::cerr << "    ";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << " " << gogui_x[i];
  }
  std::cerr << std::endl;

  std::cerr << "   +";
  for (int i = 0; i < pure_board_size * 2 + 1; i++) {
    std::cerr << "-";
  }
  std::cerr << "+" << std::endl;

  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << std::setw(2) << (pure_board_size + 1 - i) << ":|";
    for (int x = board_start; x <= board_end; x++) {
      std::cerr << " " << stone[(int)game->board[POS(x, y)]];
    }
    std::cerr << " |" << std::endl;
  }

  std::cerr << "   +";
  for (int i = 1; i <= pure_board_size * 2 + 1; i++) {
    std::cerr << "-";
  }
  std::cerr << "+" << std::endl;
}


/////////////////////////////////
//  連の情報の表示              //
//    呼吸点の数, 座標          //
//    連を構成する石の数, 座標  //
/////////////////////////////////
void
PrintString( const game_info_t *game )
{
  const string_t *string = game->string;
  int pos, neighbor;

  if (!debug_message) return ;

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


//////////////////////////
//  各座標の連IDの表示  //
//////////////////////////
void
PrintStringID( const game_info_t *game )
{
  if (!debug_message) return ;

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


///////////////////////////////////////
//  連リストの繋がりを表示(Debug用)  //
///////////////////////////////////////
void
PrintStringNext( const game_info_t *game )
{
  if (!debug_message) return ;

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


///////////////////
//  Ownerの表示  //
///////////////////
void
PrintOwner( const uct_node_t *root, const statistic_t *statistic, const int color, double *own )
{
  int player = 0, opponent = 0;
  double owner, score;

  if (!debug_message) return ;

  std::cerr << "   ";
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << "   " << gogui_x[i];
  }
  std::cerr << std::endl;

  std::cerr << "   +";
  for (int i = 0; i < pure_board_size * 4; i++) {
    std::cerr << "-";
  }
  std::cerr << "+" << std::endl;
  for (int i = 1, y = board_start; y <= board_end; y++, i++) {
    std::cerr << std::setw(2) << (pure_board_size + 1 - i) << ":|";
    for (int x = board_start; x <= board_end; x++) {
      const int pos = POS(x, y);
      owner = (double)statistic[pos].colors[color] / root->move_count;
      if (owner > 0.5) {
        player++;
      } else {
        opponent++;
      }
      own[pos] = owner * 100.0;
      std::cerr << std::setw(3) << (int)(owner * 100) << " ";
    }
    std::cerr << "|" << std::endl;
  }

  std::cerr << "   +";
  for (int i = 0; i < pure_board_size * 4; i++) {
    std::cerr << "-";
  }
  std::cerr << "+" << std::endl;

  if (color == S_BLACK) {
    if (player - opponent > komi[0]) {
      score = player - opponent - komi[0];
      std::cerr << "BLACK+" << score << std::endl;
    } else {
      score = -(player - opponent - komi[0]);
      std::cerr << "WHITE+" << score << std::endl;
    }
  } else {
    if (player - opponent > -komi[0]) {
      score = player - opponent + komi[0];
      std::cerr << "WHITE+" << score << std::endl;
    } else {
      score = -(player - opponent + komi[0]);
      std::cerr << "BLACK+" << score << std::endl;
    }
  }
}


///////////////////////
//  最善応手列の出力  //
///////////////////////
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


///////////////////////
//  探索の情報の表示  //
///////////////////////
void
PrintPlayoutInformation( const uct_node_t *root, const int po_speed, const double finish_time, const int pre_simulated )
{
  const double winning_percentage = (double)root->win / root->move_count;

  if (!debug_message) return ;

  std::cerr << "All Playouts       :  " << std::setw(7) << root->move_count << std::endl;
  std::cerr << "Pre Simulated      :  " << std::setw(7) << pre_simulated << std::endl;
  std::cerr << "Win                :  " << std::setw(7) << root->win << std::endl;
  std::cerr << "Thinking Time      :  " << std::setw(7) << finish_time << " sec" << std::endl;
  std::cerr << "Winning Percentage :  " << std::setw(7) << (winning_percentage * 100) << "%" << std::endl;
  std::cerr << "Playout Speed      :  " << std::setw(7) << po_speed << " PO/sec " << std::endl;
}


//////////////////
//  座標の出力  //
//////////////////
void
PrintPoint( const int pos )
{
  if (!debug_message) return ;

  if (pos == PASS) {
    std::cerr << "PASS" << std::endl;
  } else if (pos == RESIGN) {
    std::cerr << "RESIGN" << std::endl;
  } else {
    std::cerr << GOGUI_X(pos) << GOGUI_Y(pos) << std::endl;
  }
}


/////////////////////
//  コミの値の出力  //
/////////////////////
void
PrintKomiValue( void )
{
  if (!debug_message) return ;

  std::cerr << "Dynamic Komi : " << std::setw(4) << dynamic_komi[0] << std::endl;
}


////////////////////////////////////////
//  Ponderingのプレイアウト回数の出力  //
////////////////////////////////////////
void
PrintPonderingCount( const int count )
{
  if (!debug_message) return ;

  std::cerr << "Ponder : " << count << " Playouts" << std::endl;
}


//////////////////////
//  探索時間の出力  //
/////////////////////
void
PrintPlayoutLimits( const double time_limit, const int playout_limit )
{
  if (!debug_message) return ;

  std::cerr << "Time Limit    : " << time_limit << " Sec" << std::endl;
  std::cerr << "Playout Limit : " << playout_limit << " PO" << std::endl; 
}

////////////////////////////////////////
//  Ponderingのプレイアウト回数の出力  //
////////////////////////////////////////
void
PrintReuseCount( const int count )
{
  if (!debug_message) return ;

  std::cerr << "Reuse : " << count << " Playouts" << std::endl;
}

void
PrintLeelaZeroAnalyze( const uct_node_t *root )
{
  struct verbose_t {
    std::string lz_pos;
    int visits, winrate, prior, lcb, index;
    const char *pv;
  };

  std::vector<verbose_t> child_verbose;

  for ( int i = 0; i < root->child_num; ++i ) {
    verbose_t v;
    const child_node_t *c = &root->child[i];

    char cpos[10];
    LzIntegerToString(c->pos, cpos);

    v.lz_pos = std::string{cpos};
    v.visits = c->move_count.load(std::memory_order_relaxed);

    double raw_winrate = 0.5f;
    if (v.visits > 0) {
        raw_winrate = (double)(c->win.load(std::memory_order_relaxed)) / v.visits;
    }

    v.winrate = std::min(10000, (int)(10000 * raw_winrate));
    v.prior = std::min(10000, (int)(10000 * c->rate));
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

  for ( int i = 0; i < (int)child_verbose.size(); i++ ) {
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
      LzIntegerToString(uct_child[index].pos, cpos);
      fprintf(STD_STREAM, " %s", cpos);

      current = uct_child[index].index;
    }

    if (i != (int)child_verbose.size()-1) {
      fprintf(STD_STREAM, " ");
    }
  }
  fprintf(STD_STREAM, "\n");
  fflush(STD_STREAM);
}
