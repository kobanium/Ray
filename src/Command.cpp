#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Command.h"
#include "DynamicKomi.h"
#include "GoBoard.h"
#include "Message.h"
#include "UctSearch.h"
#include "ZobristHash.h"

using namespace std;


////////////
//  定数  //
////////////

//  コマンド
const string command[COMMAND_MAX] = {
  "--playout",
  "--time",
  "--size",
  "--const-time",
  "--thread",
  "--komi",
  "--handicap",
  "--reuse-subtree",
  "--pondering",
  "--tree-size",
  "--no-debug",
  "--superko",
};

//  コマンドの説明
const string errmessage[COMMAND_MAX] = {
  "Set playouts",
  "Set all thinking time",
  "Set board size",
  "Set mode const time, and set thinking time per move",
  "Set threads",
  "Set komi",
  "Set the number of handicap stones (for testing)",
  "Reuse subtree",
  "Set pondering mode",
  "Set tree size (tree size must be 2 ^ n)",
  "Prohibit any debug message",
  "Prohibit superko move",
};


//////////////////////
//  コマンドの処理  //
//////////////////////
void
AnalyzeCommand( int argc, char **argv )
{
  int n, size;
  
  for (int i = 1; i < argc; i++){
    n = COMMAND_MAX + 1;
    for (int j = 0; j < COMMAND_MAX; j++){
      if (!strcmp(argv[i], command[j].c_str())){
	n = j;
      }
    }

    switch (n) {
      case COMMAND_PLAYOUT:
	// プレイアウト数固定の探索の設定
	SetPlayout(atoi(argv[++i]));
	SetMode(CONST_PLAYOUT_MODE);
	break;
      case COMMAND_TIME:
	// 持ち時間の設定
	SetTime(atof(argv[++i]));
	SetMode(TIME_SETTING_MODE);
	break;
      case COMMAND_SIZE:
	// 碁盤の大きさの設定
	size = atoi(argv[++i]);
	if (pure_board_size != size &&
	    size > 0 && size <= PURE_BOARD_SIZE) {
	  SetBoardSize(size);
	  SetParameter();
	}
	break;
      case COMMAND_CONST_TIME:
	// 1手あたりの思考時間を固定した探索の設定
	SetConstTime(atof(argv[++i]));
	SetMode(CONST_TIME_MODE);
	break;
      case COMMAND_THREAD:
	// 探索スレッド数の設定
	SetThread(atoi(argv[++i]));
	break;
      case COMMAND_KOMI:
	// コミの設定
	SetKomi(atof(argv[++i]));
	break;
      case COMMAND_HANDICAP:
	// 置き石の個数の設定
	SetConstHandicapNum(atoi(argv[++i]));
	SetHandicapNum(0);
	break;
      case COMMAND_REUSE_SUBTREE:
	// 探索結果の再利用の設定
        SetReuseSubtree(true);
        break;
      case COMMAND_PONDERING :
	// 予測読みの設定
	SetReuseSubtree(true);
	SetPonderingMode(true);
	break;
      case COMMAND_TREE_SIZE:
	// UCTのノードの個数の設定
	SetHashSize((unsigned int)atoi(argv[++i]));
	break;
      case COMMAND_SUPERKO:
	// 超劫のh判定の設定
	SetSuperKo(true);
	break;
      case COMMAND_NO_DEBUG:
	// デバッグメッセージを出力しない設定
	SetDebugMessageMode(false);
	break;
      default:
	for (int j = 0; j < COMMAND_MAX; j++){
	  fprintf(stderr, "%-22s : %s\n", command[j].c_str(), errmessage[j].c_str());
	}
	exit(1);
    }
  }

}
