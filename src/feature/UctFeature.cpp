#include "feature/Ladder.hpp"
#include "feature/Nakade.hpp"
#include "feature/Semeai.hpp"
#include "feature/UctFeature.hpp"


//game_info_t snapback_game;




static void CheckFeaturesLib1ForTree( const game_info_t *game, const int color, const int id, const bool ladder, unsigned int *tactical_features );
static void CheckFeaturesLib2ForTree( const game_info_t *game, const int color, const int id, unsigned int *tactical_features );
static void CheckFeaturesLib3ForTree( const game_info_t *game, const int color, const int id, unsigned int *tactical_features );



static void
CompareSwapFeature( unsigned int *tactical_features, const int pos, const int type, const unsigned int new_feature )
{
  const int index = UctFeatureIndex(pos, type);

  if (tactical_features[index] < new_feature) {
    tactical_features[index] = new_feature;
  }
}




/////////////////////////////////////////
//  呼吸点が1つの連に対する特徴の判定  //
/////////////////////////////////////////
static void
CheckFeaturesLib1ForTree( const game_info_t *game, const int color, const int id, const bool ladder, unsigned int *tactical_features )
{
  const string_t *string = game->string;
  int lib, neighbor;

  // 呼吸点が1つになった連の呼吸点を取り出す
  lib = string[id].lib[0];

  // シチョウを逃げる手かどうかで特徴を判定
  if (ladder) {
    CompareSwapFeature(tactical_features, lib, UCT_SAVE_EXTENSION_INDEX, UCT_LADDER_EXTENSION);
  } else {
    if (string[id].size == 1) {
      CompareSwapFeature(tactical_features, lib, UCT_SAVE_EXTENSION_INDEX, UCT_SAVE_EXTENSION_1);
    } else if (string[id].size == 2) {
      CompareSwapFeature(tactical_features, lib, UCT_SAVE_EXTENSION_INDEX, UCT_SAVE_EXTENSION_2);
    } else {
      CompareSwapFeature(tactical_features, lib, UCT_SAVE_EXTENSION_INDEX, UCT_SAVE_EXTENSION_3);
    }
  }

  // 敵連を取ることによって連を助ける手の特徴の判定
  // 自分の連の大きさと敵の連の大きさで特徴を判定
  neighbor = string[id].neighbor[0];
  while (neighbor != NEIGHBOR_END) {
    if (string[neighbor].libs == 1) {
      lib = string[neighbor].lib[0];
      if (string[id].size == 1) {
        if (string[neighbor].size == 1) {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_1_1);
        } else if (string[neighbor].size == 2) {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_1_2);
        } else {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_1_3);
        }
      } else if (string[id].size == 2){
        if (string[neighbor].size == 1) {
          if (IsSelfAtariCapture(game, lib, color, id)) {
            CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_SELF_ATARI);
          } else {
            CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_2_1);
          }
        } else if (string[neighbor].size == 2) {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_2_2);
        } else {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_2_3);
        }
      } else {
        if (string[neighbor].size == 1) {
          if (IsSelfAtariCapture(game, lib, color, id)) {
            CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_SELF_ATARI);
          } else {
            CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_3_1);
          }
        } else if (string[neighbor].size == 2) {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_3_2);
        } else {
          CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_SAVE_CAPTURE_3_3);
        }
      }
    }
    neighbor = string[id].neighbor[neighbor];
  }
}



/////////////////////////////////////////
//  呼吸点が2つの連に対する特徴の判定  //
/////////////////////////////////////////
void
CheckFeaturesLib2ForTree( const game_info_t *game, const int color, const int id, unsigned int *tactical_features )
{
  const string_t *string = game->string;
  int lib1, lib2, neighbor, lib1_state, lib2_state;

  // 呼吸点が2つになった連の呼吸点を取り出す
  lib1 = string[id].lib[0];
  lib2 = string[id].lib[lib1];

  // 呼吸点に打つ特徴
  lib1_state = CheckLibertyState(game, lib1, color, id);
  lib2_state = CheckLibertyState(game, lib2, color, id);
  switch (lib1_state) {
  case L_DECREASE :
    CompareSwapFeature(tactical_features, lib1, UCT_EXTENSION_INDEX, UCT_2POINT_EXTENSION_DECREASE);
    break;
  case L_EVEN :
    CompareSwapFeature(tactical_features, lib1, UCT_EXTENSION_INDEX, UCT_2POINT_EXTENSION_EVEN);
    break;
  case L_INCREASE:
    CompareSwapFeature(tactical_features, lib1, UCT_EXTENSION_INDEX, UCT_2POINT_EXTENSION_INCREASE);
    break;
  default :
    break;
  }
  switch (lib2_state) {
  case L_DECREASE :
    CompareSwapFeature(tactical_features, lib2, UCT_EXTENSION_INDEX, UCT_2POINT_EXTENSION_DECREASE);
    break;
  case L_EVEN :
    CompareSwapFeature(tactical_features, lib2, UCT_EXTENSION_INDEX, UCT_2POINT_EXTENSION_EVEN);
    break;
  case L_INCREASE:
    CompareSwapFeature(tactical_features, lib2, UCT_EXTENSION_INDEX, UCT_2POINT_EXTENSION_INCREASE);
    break;
  default :
    break;
  }


  // 呼吸点が2つになった連の周囲の敵連を調べる
  // 1. 呼吸点が1つの敵連
  // 2. 呼吸点が2つの敵連
  // それぞれに対して, 特徴を判定する
  // さらに2.に関しては1手で取れるかどうかも考慮する
  neighbor = string[id].neighbor[0];
  if (string[id].size <= 2) {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib1 = string[neighbor].lib[0];
        if (string[neighbor].size <= 2) {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_2POINT_CAPTURE_S_S);
        } else {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_2POINT_CAPTURE_S_L);
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_S_S);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_ATARI_S_S);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_S_S);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_ATARI_S_S);
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_S_L);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_ATARI_S_L);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_S_L);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_ATARI_S_L);
          }
        }
      }
      neighbor = string[id].neighbor[neighbor];
    }
  } else {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib1 = string[neighbor].lib[0];
        if (string[neighbor].size <= 2) {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_2POINT_CAPTURE_L_S);
        } else {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_2POINT_CAPTURE_L_L);
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_L_S);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_ATARI_L_S);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_L_S);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_ATARI_L_S);
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_L_L);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_2POINT_ATARI_L_L);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_C_ATARI_L_L);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_2POINT_ATARI_L_L);
          }
        }
      }
      neighbor = string[id].neighbor[neighbor];
    }
  }
}

/////////////////////////////////////////
//  呼吸点が3つの連に対する特徴の判定  //
/////////////////////////////////////////
static void
CheckFeaturesLib3ForTree( const game_info_t *game, const int color, const int id, unsigned int *tactical_features )
{
  const string_t *string = game->string;
  int lib1, lib2, lib3, neighbor, lib1_state, lib2_state, lib3_state;

  // 呼吸点が3つになった連の呼吸点を取り出す
  lib1 = string[id].lib[0];
  lib2 = string[id].lib[lib1];
  lib3 = string[id].lib[lib2];

  // 呼吸点に打つ特徴
  lib1_state = CheckLibertyState(game, lib1, color, id);
  lib2_state = CheckLibertyState(game, lib2, color, id);
  lib3_state = CheckLibertyState(game, lib3, color, id);
  switch (lib1_state) {
  case L_DECREASE :
    CompareSwapFeature(tactical_features, lib1, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_DECREASE);
    break;
  case L_EVEN :
    CompareSwapFeature(tactical_features, lib1, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_EVEN);
    break;
  case L_INCREASE:
    CompareSwapFeature(tactical_features, lib1, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_INCREASE);
    break;
  default :
    break;
  }
  switch (lib2_state) {
  case L_DECREASE :
    CompareSwapFeature(tactical_features, lib2, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_DECREASE);
    break;
  case L_EVEN :
    CompareSwapFeature(tactical_features, lib2, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_EVEN);
    break;
  case L_INCREASE:
    CompareSwapFeature(tactical_features, lib2, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_INCREASE);
    break;
  default :
    break;
  }
  switch (lib3_state) {
  case L_DECREASE :
    CompareSwapFeature(tactical_features, lib3, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_DECREASE);
    break;
  case L_EVEN :
    CompareSwapFeature(tactical_features, lib3, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_EVEN);
    break;
  case L_INCREASE:
    CompareSwapFeature(tactical_features, lib3, UCT_EXTENSION_INDEX, UCT_3POINT_EXTENSION_INCREASE);
    break;
  default :
    break;
  }


  // 呼吸点が3つになった連の周囲の敵連を調べる
  // 1. 呼吸点が1つの敵連
  // 2. 呼吸点が2つの敵連
  // 3. 呼吸点が3つの敵連
  // それぞれに対して, 特徴を判定する
  // さらに2に関しては1手で取れるかを考慮する

  neighbor = string[id].neighbor[0];
  if (string[id].size <= 2) {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib1 = string[neighbor].lib[0];
        if (string[neighbor].size <= 2) {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_3POINT_CAPTURE_S_S);
        } else {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_3POINT_CAPTURE_S_L);
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_S_S);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_ATARI_S_S);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_S_S);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_ATARI_S_S);
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_S_L);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_ATARI_S_L);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_S_L);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_ATARI_S_L);
          }
        }
      } else if (string[neighbor].libs == 3) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        lib3 = string[neighbor].lib[lib2];
        if (string[neighbor].size <= 2) {
          CompareSwapFeature(tactical_features, lib1, UCT_DAME_INDEX, UCT_3POINT_DAME_S_S);
          CompareSwapFeature(tactical_features, lib2, UCT_DAME_INDEX, UCT_3POINT_DAME_S_S);
          CompareSwapFeature(tactical_features, lib3, UCT_DAME_INDEX, UCT_3POINT_DAME_S_S);
        } else {
          CompareSwapFeature(tactical_features, lib1, UCT_DAME_INDEX, UCT_3POINT_DAME_S_L);
          CompareSwapFeature(tactical_features, lib2, UCT_DAME_INDEX, UCT_3POINT_DAME_S_L);
          CompareSwapFeature(tactical_features, lib3, UCT_DAME_INDEX, UCT_3POINT_DAME_S_L);
        }
      }
      neighbor = string[id].neighbor[neighbor];
    }
  } else {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib1 = string[neighbor].lib[0];
        if (string[neighbor].size <= 2) {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_3POINT_CAPTURE_L_S);
        } else {
          CompareSwapFeature(tactical_features, lib1, UCT_CAPTURE_INDEX, UCT_3POINT_CAPTURE_L_L);
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_L_S);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_ATARI_L_S);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_L_S);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_ATARI_L_S);
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_L_L);
          } else {
            CompareSwapFeature(tactical_features, lib1, UCT_ATARI_INDEX, UCT_3POINT_ATARI_L_L);
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_C_ATARI_L_L);
          } else {
            CompareSwapFeature(tactical_features, lib2, UCT_ATARI_INDEX, UCT_3POINT_ATARI_L_L);
          }
        }
      } else if (string[neighbor].libs == 3) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        lib3 = string[neighbor].lib[lib2];
        if (string[neighbor].size <= 2) {
          CompareSwapFeature(tactical_features, lib1, UCT_DAME_INDEX, UCT_3POINT_DAME_L_S);
          CompareSwapFeature(tactical_features, lib2, UCT_DAME_INDEX, UCT_3POINT_DAME_L_S);
          CompareSwapFeature(tactical_features, lib3, UCT_DAME_INDEX, UCT_3POINT_DAME_L_S);
        } else {
          CompareSwapFeature(tactical_features, lib1, UCT_DAME_INDEX, UCT_3POINT_DAME_L_L);
          CompareSwapFeature(tactical_features, lib2, UCT_DAME_INDEX, UCT_3POINT_DAME_L_L);
          CompareSwapFeature(tactical_features, lib3, UCT_DAME_INDEX, UCT_3POINT_DAME_L_L);
        }
      }
      neighbor = string[id].neighbor[neighbor];
    }
  }
}


//////////////////
//  特徴の判定  //
//////////////////
int
CheckFeaturesForTree( const game_info_t *game, const int color, unsigned int *tactical_features )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  int status = 0;
  int checked = 0, previous_move = PASS, id;
  int neighbor4[4], check[4] = { 0 };
  bool ladder, already_checked;

  if (game->moves > 1) previous_move = game->record[game->moves - 1].pos;

  if (previous_move == PASS) return status;

  GetNeighbor4(neighbor4, previous_move);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == color) {
      id = string_id[neighbor4[i]];
      already_checked = false;
      for (int j = 0; j < checked; j++) {
        if (check[j] == id) {
          already_checked = true;
          break;
        }
      }
      if (already_checked) continue;
      if (string[id].libs == 1) {
        ladder = CheckLadderExtension(game, color, neighbor4[i]);
        CheckFeaturesLib1ForTree(game, color, id, ladder, tactical_features);
        status = 3;
      } else if (string[id].libs == 2) {
        CheckFeaturesLib2ForTree(game, color, id, tactical_features);
        status = (status <= 1) ? 2 : status;
      } else if (string[id].libs == 3) {
        CheckFeaturesLib3ForTree(game, color, id, tactical_features);
        status = (status <= 0) ? 1 : status;
      }
      check[checked++] = id;
    }
  }

  return status;
}



////////////////////////
//  劫を解消するトリ  //
////////////////////////
void
CheckCaptureAfterKoForTree( const game_info_t *game, const int color, unsigned int *tactical_features )
{
  const string_t *string = game->string;
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  const int previous_move_2 = game->record[game->moves - 2].pos;
  int id, lib, neighbor4[4];
  GetNeighbor4(neighbor4, previous_move_2);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other) {
      id = string_id[neighbor4[i]];
      if (string[id].libs == 1) {
        lib = string[id].lib[0];
        CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_CAPTURE_AFTER_KO);
      }
    }
  }
}


//////////////////
//  自己アタリ  //
//////////////////
bool
CheckSelfAtariForTree( const game_info_t *game, const int color, const int pos, unsigned int *tactical_features )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  int id, lib, already_num = 0, size = 0, libs = 0, count;
  int lib_candidate[PURE_BOARD_MAX], neighbor4[4], already[4] = { 0 };
  bool checked, flag, already_checked;

  GetNeighbor4(neighbor4, pos);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == S_EMPTY) {
      lib_candidate[libs++] = neighbor4[i];
    }
  }

  //  空点
  if (libs >= 2) return true;

  //  上下左右の確認
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == color) {
      id = string_id[neighbor4[i]];
      already_checked = false;
      for (int j = 0; j < already_num; j++) {
        if (already[j] == id) {
          already_checked = true;
        }
      }
      if (already_checked) continue;

      if (string[id].libs > 2) return true;
      lib = string[id].lib[0];
      count = 0;
      while (lib != LIBERTY_END) {
        if (lib != pos) {
          checked = false;
          for (int j = 0; j < libs; j++) {
            if (lib_candidate[j] == lib) {
              checked = true;
              break;
            }
          }
          if (!checked) {
            lib_candidate[libs + count] = lib;
            count++;
          }
        }
        lib = string[id].lib[lib];
      }
      libs += count;
      size += string[id].size;
      already[already_num++] = id;
      if (libs >= 2) return true;
    } else if (board[neighbor4[i]] == other &&
               string[string_id[neighbor4[i]]].libs == 1) {
      return true;
    }
  }

  // 自己アタリの分類
  // 1.大きさが2以下の自己アタリ
  // 2.大きさが6以下でナカデの形になる自己アタリ
  // 3.それ以外の自己アタリ
  if (size < 2) {
    CompareSwapFeature(tactical_features, pos, UCT_CAPTURE_INDEX, UCT_SELF_ATARI_SMALL);
    flag = true;
  } else if (size < 6) {
    if (IsUctNakadeSelfAtari(game, pos, color)) {
      CompareSwapFeature(tactical_features, pos, UCT_CAPTURE_INDEX, UCT_SELF_ATARI_NAKADE);
      flag = true;
    } else {
      CompareSwapFeature(tactical_features, pos, UCT_CAPTURE_INDEX, UCT_SELF_ATARI_LARGE);
      flag = false;
    }
  } else {
    CompareSwapFeature(tactical_features, pos, UCT_CAPTURE_INDEX, UCT_SELF_ATARI_LARGE);
    flag = false;
  }
  return flag;
}



//////////////////
//  トリの判定  //
//////////////////
void
CheckCaptureForTree( const game_info_t *game, const int color, const int pos, unsigned int *tactical_features )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  int neighbor, id;
  int neighbor4[4];
  bool check;

  GetNeighbor4(neighbor4, pos);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other) {
      if (string[string_id[neighbor4[i]]].libs == 1) {
        check = false;
        id = string_id[neighbor4[i]];
        neighbor = string[id].neighbor[0];
        while (neighbor != NEIGHBOR_END) {
          if (string[neighbor].libs == 1) {
            check = true;
            break;
          }
          neighbor = string[id].neighbor[neighbor];
        }
        if (check) {
          CompareSwapFeature(tactical_features, pos, UCT_CAPTURE_INDEX, UCT_SEMEAI_CAPTURE);
          return;
        } else {
          CompareSwapFeature(tactical_features, pos, UCT_CAPTURE_INDEX, UCT_CAPTURE);
        }
      }
    }
  }
}



////////////////////
//  アタリの判定  //
////////////////////
void
CheckAtariForTree( const game_info_t *game, const int color, const int pos, unsigned int *tactical_features )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  int id, neighbor4[4];

  GetNeighbor4(neighbor4, pos);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other) {
      id = string_id[neighbor4[i]];
      if (string[id].libs == 2) {
        CompareSwapFeature(tactical_features, pos, UCT_ATARI_INDEX, UCT_ATARI);
      }
    }
  }
}


////////////////
//  劫の解消  //
////////////////
void
CheckKoConnectionForTree( const game_info_t *game, unsigned int *tactical_features )
{
  if (game->ko_move == game->moves - 2) {
    CompareSwapFeature(tactical_features, game->ko_pos, UCT_CONNECT_INDEX, UCT_KO_CONNECTION);
  }
}


void
CheckKoRecaptureForTree( const game_info_t *game, const int color, unsigned int *tactical_features )
{
  const string_t *string = game->string;
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int pm3 = game->record[game->moves - 3].pos;
  const int other = GetOppositeColor(color);

  if (board[pm3] == other) {
    const int id = string_id[pm3];
    if (string[id].libs == 1) {
      const int lib = string[id].lib[0];
      CompareSwapFeature(tactical_features, lib, UCT_CAPTURE_INDEX, UCT_KO_RECAPTURE);
    }
  }
}



////////////////////////////////
// 2目取られた後のホウリコミ  //
////////////////////////////////
void
CheckRemove2StonesForTree( const game_info_t *game, const int color, unsigned int *tactical_features )
{
  const int other = GetOppositeColor(color);
  const int cross[4] = {- board_size - 1, - board_size + 1, board_size - 1, board_size + 1};
  int i, connect;

  if (game->capture_num[other] != 2) {
    return;
  }

  const int rm1 = game->capture_pos[other][0];
  const int rm2 = game->capture_pos[other][1];

  if (rm1 - rm2 != 1 &&
            rm2 - rm1 != 1 &&
            rm1 - rm2 != board_size &&
      rm2 - rm1 != board_size) {
    return;
  }

  for (i = 0, connect = 0; i < 4; i++) {
    if ((game->board[rm1 + cross[i]] & color) == color) {
      connect++;
    }
  }

  if (connect >= 2) {
    CompareSwapFeature(tactical_features, rm1, UCT_THROW_IN_INDEX, UCT_THROW_IN_2);
  }

  for (i = 0, connect = 0; i < 4; i++) {
    if ((game->board[rm2 + cross[i]] & color) == color) {
      connect++;
    }
  }

  if (connect >= 2) {
    CompareSwapFeature(tactical_features, rm2, UCT_THROW_IN_INDEX, UCT_THROW_IN_2);
  }
}
