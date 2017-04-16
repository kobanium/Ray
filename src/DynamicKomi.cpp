#include <iomanip>
#include <iostream>

#include "DynamicKomi.h"
#include "GoBoard.h"
#include "Message.h"

using namespace std;

// コミの値を直線的に減らすDynamic Komi
static void LinearHandicap( const game_info_t *game );
// 勝率に基づいてコミの値を変更するDynamic Komi
static void ValueSituational( const uct_node_t *root, const int color );


// 置き石の数
static int handicap_num = 0;
// テスト対局用
static int const_handicap_num = 0;
// Dynamic Komiのモード
enum DYNAMIC_KOMI_MODE dk_mode = DK_OFF;


////////////////////////
//  置き石の数の設定  //
////////////////////////
void
SetConstHandicapNum( const int num )
{
  const_handicap_num = num;
}


////////////////////////
//  置き石の数の設定  //
////////////////////////
void
SetHandicapNum( const int num )
{
  if (const_handicap_num == 0) {
    handicap_num = num;
    if (dk_mode != DK_OFF && 
	handicap_num == 0) {
      dk_mode = DK_OFF;
    } else if (dk_mode == DK_OFF &&
	       handicap_num != 0) {
      dk_mode = DK_LINEAR;
    } 
  } else {
    handicap_num = const_handicap_num;
    dk_mode = DK_LINEAR;
  }
}


////////////////////
//  Dynamic Komi  //
////////////////////
void
DynamicKomi( const game_info_t *game, const uct_node_t *root, const int color )
{
  if (handicap_num != 0) {
    switch(dk_mode) {
      case DK_LINEAR:
	LinearHandicap(game);
	break;
      case DK_VALUE:
	ValueSituational(root, color);
	break;
      default:
	break;
    }
  }
}


////////////////////////////////////////////////////
//  最初にコミを多めに見積もって徐々に減らしていく  //
////////////////////////////////////////////////////
static void
LinearHandicap( const game_info_t *game )
{
  double new_komi;

  if (game->moves > LINEAR_THRESHOLD - 15) {
  // 手数が進んだらコミを変動しない
    new_komi = (double)handicap_num + 0.5;
  } else {
    // 新しいコミの値の計算
    new_komi = HANDICAP_WEIGHT * handicap_num * (1.0 - ((double)game->moves / LINEAR_THRESHOLD));
  }
  // 新しいコミの値を代入
  dynamic_komi[0] = new_komi;
  dynamic_komi[S_BLACK] = new_komi + 1;
  dynamic_komi[S_WHITE] = new_komi - 1;

  PrintKomiValue();
}


//////////////////////////////////
//  勝率に応じてコミの値を変動  //
//////////////////////////////////
static void
ValueSituational( const uct_node_t *root, const int color )
{
  double win_rate = (double)root->win / root->move_count;

  // 次の探索の時のコミを求める
  if (color == S_BLACK) {
    if (win_rate < RED) {
      dynamic_komi[0]--;
    } else if (win_rate > GREEN) {
      dynamic_komi[0]++;
    }
  } else if (color == S_WHITE) {
    if (win_rate < RED) {
      dynamic_komi[0]++;
    } else if (win_rate > GREEN) {
      dynamic_komi[0]--;
    }
  }

  dynamic_komi[S_BLACK] = dynamic_komi[0] + 1.0;
  dynamic_komi[S_WHITE] = dynamic_komi[0] - 1.0;

  PrintKomiValue();
}
