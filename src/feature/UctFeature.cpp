#include "feature/Ladder.hpp"
#include "feature/Nakade.hpp"
#include "feature/Semeai.hpp"
#include "feature/UctFeature.hpp"


game_info_t snapback_game;

static void CheckFeatureLib1ForTree( game_info_t *game, int color, int id, bool ladder, uct_features_t *uct_features );

static void CheckFeatureLib2ForTree( game_info_t *game, int color, int id, uct_features_t *uct_features );

static void CheckFeatureLib3ForTree( game_info_t *game, int color, int id, uct_features_t *uct_features );




/////////////////////////////////////////
//  呼吸点が1つの連に対する特徴の判定  //
/////////////////////////////////////////
static void
CheckFeatureLib1ForTree( game_info_t *game, int color, int id, bool ladder, uct_features_t *uct_features )
{
  const string_t *string = game->string;
  int lib, neighbor;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

  // 呼吸点が1つになった連の呼吸点を取り出す
  lib = string[id].lib[0];

  // シチョウを逃げる手かどうかで特徴を判定
  if (ladder) {
    tactical_features1[lib] |= bit_mask[UCT_LADDER_EXTENSION];
  } else {
    if (string[id].size == 1) {
      tactical_features1[lib] |= bit_mask[UCT_SAVE_EXTENSION_1];
    } else if (string[id].size == 2) {
      tactical_features1[lib] |= bit_mask[UCT_SAVE_EXTENSION_2];
    } else {
      tactical_features1[lib] |= bit_mask[UCT_SAVE_EXTENSION_3];
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
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_1_1];
        } else if (string[neighbor].size == 2) {
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_1_2];
        } else {
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_1_3];
        }
      } else if (string[id].size == 2){
        if (string[neighbor].size == 1) {
          if (IsSelfAtariCapture(game, lib, color, id)) {
            tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_SELF_ATARI];
          } else {
            tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_2_1];
          }
        } else if (string[neighbor].size == 2) {
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_2_2];
        } else {
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_2_3];
        }
      } else {
        if (string[neighbor].size == 1) {
          if (IsSelfAtariCapture(game, lib, color, id)) {
            tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_SELF_ATARI];
          } else {
            tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_3_1];
          }
        } else if (string[neighbor].size == 2) {
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_3_2];
        } else {
          tactical_features1[lib] |= bit_mask[UCT_SAVE_CAPTURE_3_3];
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
CheckFeatureLib2ForTree( game_info_t *game, int color, int id, uct_features_t *uct_features )
{
  const string_t *string = game->string;
  int lib1, lib2, neighbor, lib1_state, lib2_state;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

  // 呼吸点が2つになった連の呼吸点を取り出す
  lib1 = string[id].lib[0];
  lib2 = string[id].lib[lib1];

  // 呼吸点に打つ特徴
  lib1_state = CheckLibertyState(game, lib1, color, id);
  lib2_state = CheckLibertyState(game, lib2, color, id);
  switch (lib1_state) {
  case L_DECREASE :
    tactical_features1[lib1] |= bit_mask[UCT_2POINT_EXTENSION_DECREASE];
    break;
  case L_EVEN :
    tactical_features1[lib1] |= bit_mask[UCT_2POINT_EXTENSION_EVEN];
    break;
  case L_INCREASE:
    tactical_features1[lib1] |= bit_mask[UCT_2POINT_EXTENSION_INCREASE];
    break;
  default :
    break;
  }
  switch (lib2_state) {
  case L_DECREASE :
    tactical_features1[lib2] |= bit_mask[UCT_2POINT_EXTENSION_DECREASE];
    break;
  case L_EVEN :
    tactical_features1[lib2] |= bit_mask[UCT_2POINT_EXTENSION_EVEN];
    break;
  case L_INCREASE:
    tactical_features1[lib2] |= bit_mask[UCT_2POINT_EXTENSION_INCREASE];
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
          tactical_features1[lib1] |= bit_mask[UCT_2POINT_CAPTURE_S_S];
        } else {
          tactical_features1[lib1] |= bit_mask[UCT_2POINT_CAPTURE_S_L];
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_C_ATARI_S_S];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_ATARI_S_S];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_C_ATARI_S_S];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_ATARI_S_S];
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_C_ATARI_S_L];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_ATARI_S_L];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_C_ATARI_S_L];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_ATARI_S_L];
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
          tactical_features1[lib1] |= bit_mask[UCT_2POINT_CAPTURE_L_S];
        } else {
          tactical_features1[lib1] |= bit_mask[UCT_2POINT_CAPTURE_L_L];
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_C_ATARI_L_S];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_ATARI_L_S];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_C_ATARI_L_S];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_ATARI_L_S];
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_C_ATARI_L_L];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_2POINT_ATARI_L_L];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_C_ATARI_L_L];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_2POINT_ATARI_L_L];
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
CheckFeatureLib3ForTree( game_info_t *game, int color, int id, uct_features_t *uct_features )
{
  const string_t *string = game->string;
  int lib1, lib2, lib3, neighbor, lib1_state, lib2_state, lib3_state;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

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
    tactical_features1[lib1] |= bit_mask[UCT_3POINT_EXTENSION_DECREASE];
    break;
  case L_EVEN :
    tactical_features1[lib1] |= bit_mask[UCT_3POINT_EXTENSION_EVEN];
    break;
  case L_INCREASE:
    tactical_features1[lib1] |= bit_mask[UCT_3POINT_EXTENSION_INCREASE];
    break;
  default :
    break;
  }
  switch (lib2_state) {
  case L_DECREASE :
    tactical_features1[lib2] |= bit_mask[UCT_3POINT_EXTENSION_DECREASE];
    break;
  case L_EVEN :
    tactical_features1[lib2] |= bit_mask[UCT_3POINT_EXTENSION_EVEN];
    break;
  case L_INCREASE:
    tactical_features1[lib2] |= bit_mask[UCT_3POINT_EXTENSION_INCREASE];
    break;
  default :
    break;
  }
  switch (lib3_state) {
  case L_DECREASE :
    tactical_features1[lib3] |= bit_mask[UCT_3POINT_EXTENSION_DECREASE];
    break;
  case L_EVEN :
    tactical_features1[lib3] |= bit_mask[UCT_3POINT_EXTENSION_EVEN];
    break;
  case L_INCREASE:
    tactical_features1[lib3] |= bit_mask[UCT_3POINT_EXTENSION_INCREASE];
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
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_CAPTURE_S_S];
        } else {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_CAPTURE_S_L];
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_C_ATARI_S_S];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_ATARI_S_S];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_C_ATARI_S_S];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_ATARI_S_S];
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_C_ATARI_S_L];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_ATARI_S_L];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_C_ATARI_S_L];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_ATARI_S_L];
          }
        }
      } else if (string[neighbor].libs == 3) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        lib3 = string[neighbor].lib[lib2];
        if (string[neighbor].size <= 2) {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_DAME_S_S];
          tactical_features1[lib2] |= bit_mask[UCT_3POINT_DAME_S_S];
          tactical_features1[lib3] |= bit_mask[UCT_3POINT_DAME_S_S];
        } else {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_DAME_S_L];
          tactical_features1[lib2] |= bit_mask[UCT_3POINT_DAME_S_L];
          tactical_features1[lib3] |= bit_mask[UCT_3POINT_DAME_S_L];
        }
      }
      neighbor = string[id].neighbor[neighbor];
    }
  } else {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib1 = string[neighbor].lib[0];
        if (string[neighbor].size <= 2) {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_CAPTURE_L_S];
        } else {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_CAPTURE_L_L];
        }
      } else if (string[neighbor].libs == 2) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        if (string[neighbor].size <= 2) {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_C_ATARI_L_S];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_ATARI_L_S];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_C_ATARI_L_S];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_ATARI_L_S];
          }
        } else {
          if (IsCapturableAtari(game, lib1, color, string[neighbor].origin)) {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_C_ATARI_L_L];
          } else {
            tactical_features1[lib1] |= bit_mask[UCT_3POINT_ATARI_L_L];
          }
          if (IsCapturableAtari(game, lib2, color, string[neighbor].origin)) {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_C_ATARI_L_L];
          } else {
            tactical_features1[lib2] |= bit_mask[UCT_3POINT_ATARI_L_L];
          }
        }
      } else if (string[neighbor].libs == 3) {
        lib1 = string[neighbor].lib[0];
        lib2 = string[neighbor].lib[lib1];
        lib3 = string[neighbor].lib[lib2];
        if (string[neighbor].size <= 2) {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_DAME_L_S];
          tactical_features1[lib2] |= bit_mask[UCT_3POINT_DAME_L_S];
          tactical_features1[lib3] |= bit_mask[UCT_3POINT_DAME_L_S];
        } else {
          tactical_features1[lib1] |= bit_mask[UCT_3POINT_DAME_L_L];
          tactical_features1[lib2] |= bit_mask[UCT_3POINT_DAME_L_L];
          tactical_features1[lib3] |= bit_mask[UCT_3POINT_DAME_L_L];
        }
      }
      neighbor = string[id].neighbor[neighbor];
    }
  }
}


//////////////////
//  特徴の判定  //
//////////////////
void
CheckFeaturesForTree( game_info_t *game, int color, uct_features_t *uct_features )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  int checked = 0, previous_move = PASS, id;
  int neighbor4[4], check[4] = { 0 };
  bool ladder, already_checked;

  if (game->moves > 1) previous_move = game->record[game->moves - 1].pos;

  if (previous_move == PASS) return;

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
        CheckFeatureLib1ForTree(game, color, id, ladder, uct_features);
      } else if (string[id].libs == 2) {
        CheckFeatureLib2ForTree(game, color, id, uct_features);
      } else if (string[id].libs == 3) {
        CheckFeatureLib3ForTree(game, color, id, uct_features);
      }
      check[checked++] = id;
    }
  }
}



////////////////////////
//  劫を解消するトリ  //
////////////////////////
void
//UctCheckCaptureAfterKo( game_info_t *game, int color, uct_features_t *uct_features )
CheckCaptureAfterKoForTree( game_info_t *game, int color, uct_features_t *uct_features )
{
  const string_t *string = game->string;
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  const int previous_move_2 = game->record[game->moves - 2].pos;
  int id, lib, neighbor4[4];
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

  GetNeighbor4(neighbor4, previous_move_2);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other) {
      id = string_id[neighbor4[i]];
      if (string[id].libs == 1) {
        lib = string[id].lib[0];
        tactical_features1[lib] |= bit_mask[UCT_CAPTURE_AFTER_KO];
      }
    }
  }
}


//////////////////
//  自己アタリ  //
//////////////////
bool
CheckSelfAtariForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  int id, lib, already_num = 0, size = 0, libs = 0, count;
  int lib_candidate[PURE_BOARD_MAX], neighbor4[4], already[4] = { 0 };
  bool checked, flag, already_checked;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

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
    tactical_features1[pos] |= bit_mask[UCT_SELF_ATARI_SMALL];
    flag = true;
  } else if (size < 6) {
    if (IsUctNakadeSelfAtari(game, pos, color)) {
      tactical_features1[pos] |= bit_mask[UCT_SELF_ATARI_NAKADE];
      flag = true;
    } else {
      tactical_features1[pos] |= bit_mask[UCT_SELF_ATARI_LARGE];
      flag = false;
    }
  } else {
    tactical_features1[pos] |= bit_mask[UCT_SELF_ATARI_LARGE];
    flag = false;
  }
  return flag;
}



//////////////////
//  トリの判定  //
//////////////////
void
CheckCaptureForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  int neighbor, id;
  int neighbor4[4];
  unsigned long long *tactical_features1 = uct_features->tactical_features1;
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
          tactical_features1[pos] |= bit_mask[UCT_SEMEAI_CAPTURE];
          return;
        } else {
          tactical_features1[pos] |= bit_mask[UCT_CAPTURE];
        }
      }
    }
  }
}



////////////////////
//  アタリの判定  //
////////////////////
void
CheckAtariForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  int id, size, neighbor4[4];
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

  GetNeighbor4(neighbor4, pos);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other) {
      id = string_id[neighbor4[i]];
      if (string[id].libs == 2) {
        size = CheckOiotoshi(game, pos, color, neighbor4[i]);
        if (size > 4) {
          tactical_features1[pos] |= bit_mask[UCT_OIOTOSHI];
          return;
        } else if (size > 0) {
          tactical_features1[pos] |= bit_mask[UCT_CAPTURABLE_ATARI];
        } else {
          tactical_features1[pos] |= bit_mask[UCT_ATARI];
        }
      }
    }
  }
}


////////////////
//  劫の解消  //
////////////////
void
CheckKoConnectionForTree( game_info_t *game, uct_features_t *uct_features )
{
  if (game->ko_move == game->moves - 2) {
    uct_features->tactical_features1[game->ko_pos] |= bit_mask[UCT_KO_CONNECTION];
  }
}


////////////////////////////////
// 2目取られた後のホウリコミ  //
////////////////////////////////
void
CheckRemove2StonesForTree( game_info_t *game, int color, uct_features_t *uct_features )
{
  const int other = GetOppositeColor(color);
  const int cross[4] = {- board_size - 1, - board_size + 1, board_size - 1, board_size + 1};
  int i, connect;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

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
    tactical_features1[rm1] |= bit_mask[UCT_THROW_IN_2];
  }

  for (i = 0, connect = 0; i < 4; i++) {
    if ((game->board[rm2 + cross[i]] & color) == color) {
      connect++;
    }
  }

  if (connect >= 2) {
    tactical_features1[rm2] |= bit_mask[UCT_THROW_IN_2];
  }
}


/////////////////////////////
//  3目抜かれた後のナカデ  //
/////////////////////////////
void
CheckRemove3StonesForTree( game_info_t *game, int color, uct_features_t *uct_features )
{
  const int other = GetOppositeColor(color);
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

  if (game->capture_num[other] != 3) {
    return;
  }

  const int rm1 = game->capture_pos[other][0];
  const int rm2 = game->capture_pos[other][1];
  const int rm3 = game->capture_pos[other][2];

  if (DIS(rm1, rm2) == 2 && DIS(rm1, rm3) == 2){
    tactical_features1[rm1] |= bit_mask[UCT_NAKADE_3];
  } else if (DIS(rm1, rm2) == 2 && DIS(rm2, rm3) == 2){
    tactical_features1[rm2] |= bit_mask[UCT_NAKADE_3];
  } else if (DIS(rm1, rm3) == 2 && DIS(rm2, rm3) == 2){
    tactical_features1[rm3] |= bit_mask[UCT_NAKADE_3];
  }
}


//////////////////////////////
//  ケイマのツケコシの判定  //
//////////////////////////////
void
CheckKeimaTsukekoshiForTree(game_info_t *game, int color, int pos, uct_features_t *uct_features)
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  unsigned long long *tactical_features1 = uct_features->tactical_features1;
  int keima_pos[8], opponent_pos[8];

  keima_pos[0] = -2 * board_size - 1;
  keima_pos[1] = -2 * board_size + 1;
  keima_pos[2] = - board_size - 2;
  keima_pos[3] = - board_size + 2;
  keima_pos[4] = board_size - 2;
  keima_pos[5] = board_size + 2;
  keima_pos[6] = 2 * board_size - 1;
  keima_pos[7] = 2 * board_size + 1;

  opponent_pos[0] = - board_size - 1;
  opponent_pos[1] = - board_size;
  opponent_pos[2] = - board_size + 1;
  opponent_pos[3] = - 1;
  opponent_pos[4] = 1;
  opponent_pos[5] = board_size - 1;
  opponent_pos[6] = board_size;
  opponent_pos[7] = board_size + 1;

  // Pattern No.1
  // ?O+?
  // ?+X+
  // P+O+
  // ?+++
  if (board[pos + opponent_pos[0]] == other &&
            board[pos + opponent_pos[6]] == other &&
            board[pos + keima_pos[4]] == color &&
      board[NORTH(pos)] != other &&
      Pat3(game->pat, opponent_pos[6]) == 0x0000) {
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }

  // Pattern No.2
  // ??P?
  // O+++
  // +XO+
  // ?+++
  if (board[pos + opponent_pos[0]] == other &&
            board[pos + opponent_pos[4]] == other &&
            board[pos + keima_pos[1]] == color &&
      board[WEST(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[4]) == 0x0000) {
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }

  // Pattern No.3
  // ?+++
  // P+O+
  // ?+X+
  // ?O+?
  if (board[pos + opponent_pos[1]] == other &&
            board[pos + opponent_pos[5]] == other &&
            board[pos + keima_pos[2]] == color &&
      board[SOUTH(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[1]) == 0x0000) {
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }

  // Pattern No.4
  // +++?
  // +O+P
  // +X+?
  // ?+O?
  if (board[pos + opponent_pos[1]] == other &&
            board[pos + opponent_pos[7]] == other &&
            board[pos + keima_pos[3]] == color &&
      board[SOUTH(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[1]) == 0x0000) {
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }

  // Pattern No.5
  // +P??
  // +++O
  // +OX+
  // +++?
  if (board[pos + opponent_pos[2]] == other &&
            board[pos + opponent_pos[3]] == other &&
            board[pos + keima_pos[0]] == color &&
      board[EAST(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[3]) == 0x0000) {
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }

  // Pattern No.6
  // ?+O?
  // +X+?
  // +O+P
  // +++?
  if (board[pos + opponent_pos[2]] == other &&
            board[pos + opponent_pos[6]] == other &&
            board[pos + keima_pos[5]] == color &&
      board[NORTH(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[6]) == 0x0000){
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }

  // Pattern No.7
  // ?+++
  // +XO+
  // O+++
  // ??P?
  if (board[pos + opponent_pos[4]] == other &&
            board[pos + opponent_pos[5]] == other &&
            board[pos + keima_pos[7]] == color &&
      board[WEST(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[4]) == 0x0000){
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }


  // Pattern No.8
  // +++?
  // +OX+
  // +++O
  // ?P??
  if (board[pos + opponent_pos[3]] == other &&
            board[pos + opponent_pos[7]] == other &&
            board[pos + keima_pos[6]] == color &&
      board[EAST(pos)] != other &&
      Pat3(game->pat, pos + opponent_pos[3]) == 0x0000){
    tactical_features1[pos] |= bit_mask[UCT_KEIMA_TSUKEKOSHI];
    return;
  }
}


//////////////////////
//  両ケイマの判定  //
//////////////////////
void
CheckDoubleKeimaForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features )
{
  // ++O+O++
  // +O+++O+
  // +++X+++
  // +O+++O+
  // ++O+O++
  // Oのうち自分と相手の石が1個ずつ以上ある時の特徴
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  int keima_pos[8];
  int player = 0, opponent = 0;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;

  if (Pat3(game->pat, pos) != 0x0000) {
    return;
  }

  keima_pos[0] = -2 * board_size - 1;
  keima_pos[1] = -2 * board_size + 1;
  keima_pos[2] = - board_size - 2;
  keima_pos[3] = - board_size + 2;
  keima_pos[4] = board_size - 2;
  keima_pos[5] = board_size + 2;
  keima_pos[6] = 2 * board_size - 1;
  keima_pos[7] = 2 * board_size + 1;

  for (int i = 0; i < 8; i++) {
    if (board[pos + keima_pos[i]] == color) player++;
    if (board[pos + keima_pos[i]] == other) opponent++;
  }

  if (player > 0 && opponent > 0){
    tactical_features1[pos] |= bit_mask[UCT_DOUBLE_KEIMA];
  }

}


////////////////////
//  ウッテガエシ  //
////////////////////
void
CheckSnapBackForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features )
{
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  unsigned long long *tactical_features1 = uct_features->tactical_features1;
  int neighbor4[4];

  GetNeighbor4(neighbor4, pos);

  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other) {
      int id = string_id[neighbor4[i]];

      game_info_t *check_game;
      if (string[id].libs == 1) {
        check_game = game;
      } else if (string[id].libs == 2) {
        CopyGame(&snapback_game, game);
        PutStone(&snapback_game, pos, color);
        check_game = &snapback_game;
      } else {
        continue;
      }
      int id2 = check_game->string_id[neighbor4[i]];
      int lib = check_game->string[id2].lib[0];
      int capturable_pos = CapturableCandidate(check_game, id2);
      if (lib == capturable_pos) {
        tactical_features1[pos] |= bit_mask[UCT_SNAPBACK];
        return;
      }
    }
  }
}

