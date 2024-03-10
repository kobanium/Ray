/**
 * @file src/feature/SimulationFeature.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Features for Monte-Carlo simulation.
 * @~japanese
 * @brief モンテカルロ・シミュレーション用の特徴
 */
#include "feature/Nakade.hpp"
#include "feature/Semeai.hpp"
#include "feature/SimulationFeature.hpp"


/**
 * @~english
 * @brief Diagonal coordinates.
 * @~japanese
 * @brief 斜めの座標.
 */
static int cross[4];


//  呼吸点が1つの連に対する特徴の判定
static void CheckFeatureLib1ForSimulation( game_info_t *game, const int color, const int id, int update[], int &update_num );

//  呼吸点が2つの連に対する特徴の判定
static void CheckFeatureLib2ForSimulation( game_info_t *game, const int color, const int id, int update[], int &update_num );

//  呼吸点が3つの連に対する特徴の判定
static void CheckFeatureLib3ForSimulation( game_info_t *game, const int color, const int id, int update[], int &update_num );


/**
 * @~english
 * @brief Set diagonal coordinates.
 * @~japanese
 * @brief 斜め4箇所の座標の設定.
 */
void
SetCrossPosition( void )
{
  cross[0] = -board_size - 1;
  cross[1] = -board_size + 1;
  cross[2] =  board_size - 1;
  cross[3] =  board_size + 1;
}


/**
 * @~english
 * @brief Check features for the string with only 1 liberty.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] id String ID.
 * @param[in, out] update Update intersections.
 * @param[in, out] update_num The number of update intersections.
 * @~japanese
 * @brief 呼吸点が1つの連に対する特徴の判定
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 * @param[in] id 連ID
 * @param[in, out] update 更新箇所
 * @param[in, out] update_num 更新箇所の数
 */
static void
CheckFeatureLib1ForSimulation( game_info_t *game, const int color, const int id, int update[], int &update_num )
{
  const int other = GetOppositeColor(color);
  const char *board = game->board;
  const string_t *string = game->string;
  int neighbor = string[id].neighbor[0];
  int lib, liberty;
  bool contact = false;

  // 呼吸点が1つになった連の呼吸点を取り出す
  lib = string[id].lib[0];
  liberty = lib;

  // 呼吸点の上下左右が敵石に接触しているか確認
  if (board[NORTH(lib)] == other) contact = true;
  if (board[ EAST(lib)] == other) contact = true;
  if (board[ WEST(lib)] == other) contact = true;
  if (board[SOUTH(lib)] == other) contact = true;

  // 呼吸点の上下左右が敵連に接しているか
  // また逃げ出す連の大きさで特徴を判定
  if (contact) {
    if (string[id].size == 1) {
      CompareSwapFeature(game->tactical_features, lib, SAVE_EXTENSION, SIM_SAVE_EXTENSION_1);
    } else if (string[id].size == 2) {
      CompareSwapFeature(game->tactical_features, lib, SAVE_EXTENSION, SIM_SAVE_EXTENSION_2);
    } else {
      CompareSwapFeature(game->tactical_features, lib, SAVE_EXTENSION, SIM_SAVE_EXTENSION_3);
    }
  } else {
    if (string[id].size == 1) {
      CompareSwapFeature(game->tactical_features, lib, SAVE_EXTENSION, SIM_SAVE_EXTENSION_SAFELY_1);
    } else if (string[id].size == 2) {
      CompareSwapFeature(game->tactical_features, lib, SAVE_EXTENSION, SIM_SAVE_EXTENSION_SAFELY_2);
    } else {
      CompareSwapFeature(game->tactical_features, lib, SAVE_EXTENSION, SIM_SAVE_EXTENSION_SAFELY_3);
    }
  }

  // レートの更新対象に入れる
  update[update_num++] = lib;

  // 敵連を取ることによって連を助ける手の特徴の判定
  // 自分の連の大きさと敵の連の大きさで特徴を判定
  if (string[id].size == 1) {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib = string[neighbor].lib[0];
        if (string[neighbor].size == 1) {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_1_1);
        } else if (string[neighbor].size == 2) {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_1_2);
        } else {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_1_3);
        }
        update[update_num++] = lib;
      }
      neighbor = string[id].neighbor[neighbor];
    }
  } else if (string[id].size == 2) {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib = string[neighbor].lib[0];
        if (string[neighbor].size == 1) {
          if (IsSelfAtariCaptureForSimulation(game, lib, color, liberty)) {
            CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_SELF_ATARI);
          } else {
            CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_2_1);
          }
        } else if (string[neighbor].size == 2) {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_2_2);
        } else {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_2_3);
        }
        update[update_num++] = lib;
      }
      neighbor = string[id].neighbor[neighbor];
    }
  } else if (string[id].size >= 3) {
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        lib = string[neighbor].lib[0];
        if (string[neighbor].size == 1) {
          if (IsSelfAtariCaptureForSimulation(game, lib, color, liberty)) {
            CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_SELF_ATARI);
          } else {
            CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_3_1);
          }
        } else if (string[neighbor].size == 2) {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_3_2);
        } else {
          CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_SAVE_CAPTURE_3_3);
        }
        update[update_num++] = lib;
      }
      neighbor = string[id].neighbor[neighbor];
    }
  }
}


/**
 * @~english
 * @brief Check features for the string with 2 liberties.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] id String ID.
 * @param[in, out] update Update intersections.
 * @param[in, out] update_num The number of update intersections.
 * @~japanese
 * @brief 呼吸点が2つの連に対する特徴の判定
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 * @param[in] id 連ID
 * @param[in, out] update 更新箇所
 * @param[in, out] update_num 更新箇所の数
 */
static void
CheckFeatureLib2ForSimulation( game_info_t *game, const int color, const int id, int update[], int &update_num )
{
  const int *string_id = game->string_id;
  const string_t *string = game->string;
  const char *board = game->board;
  int neighbor = string[id].neighbor[0];
  int lib1, lib2;
  bool capturable1, capturable2;

  // 呼吸点が2つになった連の呼吸点を取り出す
  lib1 = string[id].lib[0];
  lib2 = string[id].lib[lib1];

  // 呼吸点の周囲が空点3つ, または呼吸点が3つ以上の自分の連に接続できるかで特徴を判定
  if (nb4_empty[Pat3(game->pat, lib1)] == 3 ||
      (board[NORTH(lib1)] == color && string_id[NORTH(lib1)] != id &&
       string[string_id[NORTH(lib1)]].libs >= 3) ||
      (board[ WEST(lib1)] == color && string_id[ WEST(lib1)] != id &&
       string[string_id[WEST(lib1)]].libs >= 3) ||
      (board[ EAST(lib1)] == color && string_id[ EAST(lib1)] != id &&
       string[string_id[EAST(lib1)]].libs >= 3) ||
      (board[SOUTH(lib1)] == color && string_id[SOUTH(lib1)] != id &&
       string[string_id[SOUTH(lib1)]].libs >= 3)) {
    CompareSwapFeature(game->tactical_features, lib1, EXTENSION, SIM_2POINT_EXTENSION_SAFELY);
  } else {
    CompareSwapFeature(game->tactical_features, lib1, EXTENSION, SIM_2POINT_EXTENSION);
  }

  // 呼吸点の周囲が空点3つ, または呼吸点が3つ以上の自分の連に接続できるかで特徴を判定
  if (nb4_empty[Pat3(game->pat, lib2)] == 3 ||
      (board[NORTH(lib2)] == color && string_id[NORTH(lib2)] != id &&
       string[string_id[NORTH(lib2)]].libs >= 3) ||
      (board[ WEST(lib2)] == color && string_id[ WEST(lib2)] != id &&
       string[string_id[WEST(lib2)]].libs >= 3) ||
      (board[ EAST(lib2)] == color && string_id[ EAST(lib2)] != id &&
       string[string_id[EAST(lib2)]].libs >= 3) ||
      (board[SOUTH(lib2)] == color && string_id[SOUTH(lib2)] != id &&
       string[string_id[SOUTH(lib2)]].libs >= 3)) {
    CompareSwapFeature(game->tactical_features, lib2, EXTENSION, SIM_2POINT_EXTENSION_SAFELY);
  } else {
    CompareSwapFeature(game->tactical_features, lib2, EXTENSION, SIM_2POINT_EXTENSION);
  }

  // レートの更新対象に入れる
  update[update_num++] = lib1;
  update[update_num++] = lib2;

  // 呼吸点が2つになった連の周囲の敵連を調べる
  // 1. 呼吸点が1つの敵連
  // 2. 呼吸点が2つの敵連
  // それぞれに対して, 特徴を判定する
  while (neighbor != NEIGHBOR_END) {
    if (string[neighbor].libs == 1) {
      lib1 = string[neighbor].lib[0];
      update[update_num++] = lib1;
      if (string[neighbor].size <= 2) {
        CompareSwapFeature(game->tactical_features, lib1, CAPTURE, SIM_2POINT_CAPTURE_SMALL);
      } else {
        CompareSwapFeature(game->tactical_features, lib1, CAPTURE, SIM_2POINT_CAPTURE_LARGE);
      }
    } else if (string[neighbor].libs == 2) {
      lib1 = string[neighbor].lib[0];
      lib2 = string[neighbor].lib[lib1];
      update[update_num++] = lib1;
      update[update_num++] = lib2;
      capturable1 = IsCapturableAtariForSimulation(game, lib1, color, neighbor);
      capturable2 = IsCapturableAtariForSimulation(game, lib2, color, neighbor);
      if (string[neighbor].size <= 2) {
        if (capturable1) {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_2POINT_C_ATARI_SMALL);
        } else {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_2POINT_ATARI_SMALL);
        }
        if (capturable2) {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_2POINT_C_ATARI_SMALL);
        } else {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_2POINT_ATARI_SMALL);
        }
      } else {
        if (capturable1) {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_2POINT_C_ATARI_LARGE);
        } else {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_2POINT_ATARI_LARGE);
        }
        if (capturable2) {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_2POINT_C_ATARI_LARGE);
        } else {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_2POINT_ATARI_LARGE);
        }
      }
    }
    neighbor = string[id].neighbor[neighbor];
  }
}


/**
 * @~english
 * @brief Check features for the string with 3 liberties.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] id String ID.
 * @param[in, out] update Update intersections.
 * @param[in, out] update_num The number of update intersections.
 * @~japanese
 * @brief 呼吸点が3つの連に対する特徴の判定
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 * @param[in] id 連ID
 * @param[in, out] update 更新箇所
 * @param[in, out] update_num 更新箇所の数
 */
static void
CheckFeatureLib3ForSimulation( game_info_t *game, const int color, const int id, int update[], int &update_num )
{
  const int *string_id = game->string_id;
  const string_t *string = game->string;
  const char *board = game->board;
  int neighbor = string[id].neighbor[0];
  int lib1, lib2, lib3;
  bool capturable1, capturable2;

  // 呼吸点が3つになった連の呼吸点を取り出す
  lib1 = string[id].lib[0];
  lib2 = string[id].lib[lib1];
  lib3 = string[id].lib[lib2];

  // 呼吸点の周囲が空点3つ, または呼吸点が3つ以上の自分の連に接続できるかで特徴を判定
  if (nb4_empty[Pat3(game->pat, lib1)] == 3 ||
      (board[NORTH(lib1)] == color && string_id[NORTH(lib1)] != id &&
       string[string_id[NORTH(lib1)]].libs >= 3) ||
      (board[ WEST(lib1)] == color && string_id[ WEST(lib1)] != id &&
       string[string_id[WEST(lib1)]].libs >= 3) ||
      (board[ EAST(lib1)] == color && string_id[ EAST(lib1)] != id &&
       string[string_id[EAST(lib1)]].libs >= 3) ||
      (board[SOUTH(lib1)] == color && string_id[SOUTH(lib1)] != id &&
       string[string_id[SOUTH(lib1)]].libs >= 3)) {
    CompareSwapFeature(game->tactical_features, lib1, EXTENSION, SIM_3POINT_EXTENSION_SAFELY);
  } else {
    CompareSwapFeature(game->tactical_features, lib1, EXTENSION, SIM_3POINT_EXTENSION);
  }

  // 呼吸点の周囲が空点3つ, または呼吸点が3つ以上の自分の連に接続できるかで特徴を判定
  if (nb4_empty[Pat3(game->pat, lib2)] == 3 ||
      (board[NORTH(lib2)] == color && string_id[NORTH(lib2)] != id &&
       string[string_id[NORTH(lib2)]].libs >= 3) ||
      (board[ WEST(lib2)] == color && string_id[ WEST(lib2)] != id &&
       string[string_id[WEST(lib2)]].libs >= 3) ||
      (board[ EAST(lib2)] == color && string_id[ EAST(lib2)] != id &&
       string[string_id[EAST(lib2)]].libs >= 3) ||
      (board[SOUTH(lib2)] == color && string_id[SOUTH(lib2)] != id &&
       string[string_id[SOUTH(lib2)]].libs >= 3)) {
    CompareSwapFeature(game->tactical_features, lib2, EXTENSION, SIM_3POINT_EXTENSION_SAFELY);
  } else {
    CompareSwapFeature(game->tactical_features, lib2, EXTENSION, SIM_3POINT_EXTENSION);
  }

  // 呼吸点の周囲が空点3つ, または呼吸点が3つ以上の自分の連に接続できるかで特徴を判定
  if (nb4_empty[Pat3(game->pat, lib3)] == 3 ||
      (board[NORTH(lib3)] == color && string_id[NORTH(lib3)] != id &&
       string[string_id[NORTH(lib3)]].libs >= 3) ||
      (board[ WEST(lib3)] == color && string_id[ WEST(lib3)] != id &&
       string[string_id[ WEST(lib3)]].libs >= 3) ||
      (board[ EAST(lib3)] == color && string_id[ EAST(lib3)] != id &&
       string[string_id[ EAST(lib3)]].libs >= 3) ||
      (board[SOUTH(lib3)] == color && string_id[SOUTH(lib3)] != id &&
       string[string_id[SOUTH(lib3)]].libs >= 3)) {
    CompareSwapFeature(game->tactical_features, lib3, EXTENSION, SIM_3POINT_EXTENSION_SAFELY);
  } else {
    CompareSwapFeature(game->tactical_features, lib3, EXTENSION, SIM_3POINT_EXTENSION);
  }

  // レートの更新対象に入れる
  update[update_num++] = lib1;
  update[update_num++] = lib2;
  update[update_num++] = lib3;

  // 呼吸点が3つになった連の周囲の敵連を調べる
  // 1. 呼吸点が1つの敵連
  // 2. 呼吸点が2つの敵連
  // 3. 呼吸点が3つの敵連
  // それぞれに対して, 特徴を判定する
  while (neighbor != NEIGHBOR_END) {
    if (string[neighbor].libs == 1) {
      lib1 = string[neighbor].lib[0];
      update[update_num++] = lib1;
      if (string[neighbor].size <= 2) {
        CompareSwapFeature(game->tactical_features, lib1, CAPTURE, SIM_3POINT_CAPTURE_SMALL);
      } else {
        CompareSwapFeature(game->tactical_features, lib1, CAPTURE, SIM_3POINT_CAPTURE_LARGE);
      }
    } else if (string[neighbor].libs == 2) {
      lib1 = string[neighbor].lib[0];
      lib2 = string[neighbor].lib[lib1];
      update[update_num++] = lib1;
      update[update_num++] = lib2;
      capturable1 = IsCapturableAtariForSimulation(game, lib1, color, neighbor);
      capturable2 = IsCapturableAtariForSimulation(game, lib2, color, neighbor);
      if (string[neighbor].size <= 2) {
        if (capturable1) {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_3POINT_C_ATARI_SMALL);
        } else {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_3POINT_ATARI_SMALL);
        }
        if (capturable2) {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_3POINT_C_ATARI_SMALL);
        } else {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_3POINT_ATARI_SMALL);
        }
      } else {
        if (capturable1) {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_3POINT_C_ATARI_LARGE);
        } else {
          CompareSwapFeature(game->tactical_features, lib1, ATARI, SIM_3POINT_ATARI_LARGE);
        }
        if (capturable2) {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_3POINT_C_ATARI_LARGE);
        } else {
          CompareSwapFeature(game->tactical_features, lib2, ATARI, SIM_3POINT_ATARI_LARGE);
        }
      }
    } else if (string[neighbor].libs == 3) {
      lib1 = string[neighbor].lib[0];
      lib2 = string[neighbor].lib[lib1];
      lib3 = string[neighbor].lib[lib2];
      update[update_num++] = lib1;
      update[update_num++] = lib2;
      update[update_num++] = lib3;
      if (string[neighbor].size <= 2) {
        CompareSwapFeature(game->tactical_features, lib1, DAME, SIM_3POINT_DAME_SMALL);
        CompareSwapFeature(game->tactical_features, lib2, DAME, SIM_3POINT_DAME_SMALL);
        CompareSwapFeature(game->tactical_features, lib3, DAME, SIM_3POINT_DAME_SMALL);
      } else {
        CompareSwapFeature(game->tactical_features, lib1, DAME, SIM_3POINT_DAME_LARGE);
        CompareSwapFeature(game->tactical_features, lib2, DAME, SIM_3POINT_DAME_LARGE);
        CompareSwapFeature(game->tactical_features, lib3, DAME, SIM_3POINT_DAME_LARGE);
      }
    }
    neighbor = string[id].neighbor[neighbor];
  }
}


/**
 * @~english
 * @brief Check features around previous move.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in, out] update Update intersections.
 * @param[in, out] update_num The number of update intersections.
 * @return Feature status.
 * @~japanese
 * @brief 直前の着手の周辺の特徴の判定
 * @param[in] game 盤面情報
 * @param[in] color 手番の色
 * @param[in, out] update 更新箇所の座標
 * @param[in, out] update_num 更新箇所の個数
 * @return 特徴の状態
 */
void
CheckFeaturesForSimulation( game_info_t *game, const int color, int update[], int &update_num )
{
  const string_t *string = game->string;
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int previous_move = game->record[game->moves - 1].pos;
  int id, checked = 0;
  int check[3] = { 0 };

  if (game->moves < 2) return;

  if (previous_move == PASS) return;

  // 直前の着手の上を確認
  if (board[NORTH(previous_move)] == color) {
    id = string_id[NORTH(previous_move)];
    if (string[id].libs == 1) {
      CheckFeatureLib1ForSimulation(game, color, id, update, update_num);
    } else if (string[id].libs == 2) {
      CheckFeatureLib2ForSimulation(game, color, id, update, update_num);
    } else if (string[id].libs == 3) {
      CheckFeatureLib3ForSimulation(game, color, id, update, update_num);
    }
    check[checked++] = id;
  }

  // 直前の着手の左を確認
  if (board[WEST(previous_move)] == color) {
    id = string_id[WEST(previous_move)];
    if (id != check[0]) {
      if (string[id].libs == 1) {
        CheckFeatureLib1ForSimulation(game, color, id, update, update_num);
      } else if (string[id].libs == 2) {
        CheckFeatureLib2ForSimulation(game, color, id, update, update_num);
      } else if (string[id].libs == 3) {
        CheckFeatureLib3ForSimulation(game, color, id, update, update_num);
      }
    }
    check[checked++] = id;
  }

  // 直前の着手の右を確認
  if (board[EAST(previous_move)] == color) {
    id = string_id[EAST(previous_move)];
    if (id != check[0] && id != check[1]) {
      if (string[id].libs == 1) {
        CheckFeatureLib1ForSimulation(game, color, id, update, update_num);
      } else if (string[id].libs == 2) {
        CheckFeatureLib2ForSimulation(game, color, id, update, update_num);
      } else if (string[id].libs == 3) {
        CheckFeatureLib3ForSimulation(game, color, id, update, update_num);
      }
    }
    check[checked++] = id;
  }

  // 直前の着手の下の確認
  if (board[SOUTH(previous_move)] == color) {
    id = string_id[SOUTH(previous_move)];
    if (id != check[0] && id != check[1] && id != check[2]) {
      if (string[id].libs == 1) {
        CheckFeatureLib1ForSimulation(game, color, id, update, update_num);
      } else if (string[id].libs == 2) {
        CheckFeatureLib2ForSimulation(game, color, id, update, update_num);
      } else if (string[id].libs == 3) {
        CheckFeatureLib3ForSimulation(game, color, id, update, update_num);
      }
    }
  }
}


/**
 * @~english
 * @brief Check capturing features.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in, out] update Update intersections.
 * @param[in, out] update_num The number of update intersections.
 * @~japanese
 * @brief トリの特徴の判定
 * @param[in] game 盤面情報
 * @param[in] color 手番の色
 * @param[in, out] update 更新箇所の座標
 * @param[in, out] update_num 更新箇所の個数
 */
void
CheckCaptureAfterKoForSimulation( game_info_t *game, const int color, int update[], int &update_num )
{
  const string_t *string = game->string;
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  const int previous_move_2 = game->record[game->moves - 2].pos;
  int checked = 0;
  int check[4] = { 0 };

  //  上
  if (board[NORTH(previous_move_2)] == other) {
    const int id = string_id[NORTH(previous_move_2)];
    if (string[id].libs == 1) {
      const int lib = string[id].lib[0];
      update[update_num++] = lib;
      CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_CAPTURE_AFTER_KO);
    }
    check[checked++] = id;
  }

  //  右
  if (board[EAST(previous_move_2)] == other) {
    const int id = string_id[EAST(previous_move_2)];
    if (string[id].libs == 1 && check[0] != id) {
      const int lib = string[id].lib[0];
      update[update_num++] = lib;
      CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_CAPTURE_AFTER_KO);
    }
    check[checked++] = id;
  }

  //  下
  if (board[SOUTH(previous_move_2)] == other) {
    const int id = string_id[SOUTH(previous_move_2)];
    if (string[id].libs == 1 && check[0] != id && check[1] != id) {
      const int lib = string[id].lib[0];
      update[update_num++] = lib;
      CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_CAPTURE_AFTER_KO);
    }
    check[checked++] = id;
  }

  //  左
  if (board[WEST(previous_move_2)] == other) {
    const int id = string_id[WEST(previous_move_2)];
    if (string[id].libs == 1 && check[0] != id && check[1] != id && check[2] != id) {
      const int lib = string[id].lib[0];
      update[update_num++] = lib;
      CompareSwapFeature(game->tactical_features, lib, CAPTURE, SIM_CAPTURE_AFTER_KO);
    }
  }
}


/**
 * @~english
 * @brief Check self-atari features.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] pos Intersection.
 * @return Self atari activated flag.
 * @~japanese
 * @brief 自己アタリの特徴の判定
 * @param[in] game 盤面情報
 * @param[in] color 手番の色
 * @param[in] pos 確認する座標
 * @return 自己アタリの有無
 */
bool
CheckSelfAtariForSimulation( game_info_t *game, const int color, const int pos )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);
  int lib, id, count = 0, libs = 0, size = 0, already_num = 0;
  int already[4] = { 0 }, lib_candidate[10];
  bool flag, checked;

  // 上下左右が空点なら呼吸点の候補に入れる
  if (board[NORTH(pos)] == S_EMPTY) lib_candidate[libs++] = NORTH(pos);
  if (board[ WEST(pos)] == S_EMPTY) lib_candidate[libs++] =  WEST(pos);
  if (board[ EAST(pos)] == S_EMPTY) lib_candidate[libs++] =  EAST(pos);
  if (board[SOUTH(pos)] == S_EMPTY) lib_candidate[libs++] = SOUTH(pos);

  //  空点
  if (libs >= 2) return true;

  // 上を調べる
  if (board[NORTH(pos)] == color) {
    id = string_id[NORTH(pos)];
    if (string[id].libs > 2) return true;
    lib = string[id].lib[0];
    count = 0;
    while (lib != LIBERTY_END) {
      if (lib != pos) {
        checked = false;
        for (int i = 0; i < libs; i++) {
          if (lib_candidate[i] == lib) {
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
  } else if (board[NORTH(pos)] == other &&
             string[string_id[NORTH(pos)]].libs == 1) {
    return true;
  }

  // 左を調べる
  if (board[WEST(pos)] == color) {
    id = string_id[WEST(pos)];
    if (already[0] != id) {
      if (string[id].libs > 2) return true;
      lib = string[id].lib[0];
      count = 0;
      while (lib != LIBERTY_END) {
        if (lib != pos) {
          checked = false;
          for (int i = 0; i < libs; i++) {
            if (lib_candidate[i] == lib) {
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
    }
  } else if (board[WEST(pos)] == other &&
             string[string_id[WEST(pos)]].libs == 1) {
    return true;
  }

  // 右を調べる
  if (board[EAST(pos)] == color) {
    id = string_id[EAST(pos)];
    if (already[0] != id && already[1] != id) {
      if (string[id].libs > 2) return true;
      lib = string[id].lib[0];
      count = 0;
      while (lib != LIBERTY_END) {
        if (lib != pos) {
          checked = false;
          for (int i = 0; i < libs; i++) {
            if (lib_candidate[i] == lib) {
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
    }
  } else if (board[EAST(pos)] == other &&
             string[string_id[EAST(pos)]].libs == 1) {
    return true;
  }


  // 下を調べる
  if (board[SOUTH(pos)] == color) {
    id = string_id[SOUTH(pos)];
    if (already[0] != id && already[1] != id && already[2] != id) {
      if (string[id].libs > 2) return true;
      lib = string[id].lib[0];
      count = 0;
      while (lib != LIBERTY_END) {
        if (lib != pos) {
          checked = false;
          for (int i = 0; i < libs; i++) {
            if (lib_candidate[i] == lib) {
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
    }
  } else if (board[SOUTH(pos)] == other &&
             string[string_id[SOUTH(pos)]].libs == 1) {
    return true;
  }

  // 自己アタリになる連の大きさが2以下,
  // または大きさが5以下でナカデの形になる場合は
  // 打っても良いものとする
  if (size < 2) {
    CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_SELF_ATARI_SMALL);
    flag = true;
  } else if (size < 5) {
    if (IsNakadeSelfAtari(game, pos, color)) {
      CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_SELF_ATARI_NAKADE);
      flag = true;
    } else {
      CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_SELF_ATARI_SMALL);
      flag = false;
    }
  } else {
    CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_SELF_ATARI_LARGE);
    flag = false;
  }

  return flag;
}


/**
 * @~english
 * @brief Check capturing and atari features.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] pos Intersection.
 * @~japanese
 * @brief トリとアタリの特徴の判定
 * @param[in] game 盤面情報
 * @param[in] color 手番の色
 * @param[in] pos 確認する座標
 */
void
CheckCaptureAndAtariForSimulation( game_info_t *game, const int color, const int pos )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;
  const int other = GetOppositeColor(color);

  // 上を調べる
  // 1. 敵の石
  // 2. 呼吸点が1つ
  if (board[NORTH(pos)] == other) {
    const int libs = string[string_id[NORTH(pos)]].libs;
    if (libs == 1) {
      CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_CAPTURE);
    } else if (libs == 2) {
      CompareSwapFeature(game->tactical_features, pos, ATARI, SIM_ATARI);
    }
  }

  //  左を調べる
  // 1. 敵の石
  // 2. 呼吸点が1つ
  if (board[WEST(pos)] == other) {
    const int libs = string[string_id[WEST(pos)]].libs;
    if (libs == 1) {
      CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_CAPTURE);
    } else if (libs == 2) {
      CompareSwapFeature(game->tactical_features, pos, ATARI, SIM_ATARI);
    }
  }

  //  右を調べる
  // 1. 敵の石
  // 2. 呼吸点が1つ
  if (board[EAST(pos)] == other) {
    const int libs = string[string_id[EAST(pos)]].libs;
    if (libs == 1) {
      CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_CAPTURE);
    } else if (libs == 2) {
      CompareSwapFeature(game->tactical_features, pos, ATARI, SIM_ATARI);
    }
  }

  //  下を調べる
  // 1. 敵の石
  // 2. 呼吸点が1つ
  if (board[SOUTH(pos)] == other) {
    const int libs = string[string_id[SOUTH(pos)]].libs;
    if (libs == 1) {
      CompareSwapFeature(game->tactical_features, pos, CAPTURE, SIM_CAPTURE);
    } else if (libs == 2) {
      CompareSwapFeature(game->tactical_features, pos, ATARI, SIM_ATARI);
    }
  }
}


/**
 * @~english
 * @brief Check features making half-eye.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in, out] update Update intersections.
 * @param[in, out] update_num The number of update intersections.
 * @return Feature status.
 * @~japanese
 * @brief 2目の抜き跡へのホウリコミの特徴の判定
 * @param[in] game 盤面情報
 * @param[in] color 手番の色
 * @param[in, out] update 更新箇所の座標
 * @param[in, out] update_num 更新箇所の個数
 * @return 特徴の状態
 */
void
CheckRemove2StonesForSimulation( game_info_t *game, const int color, int update[], int &update_num )
{
  const int other = GetOppositeColor(color);
  int i, check;

  if (game->capture_num[other - 1] != 2) {
    return;
  }

  const int rm1 = game->capture_pos[other - 1][0];
  const int rm2 = game->capture_pos[other - 1][rm1];

  if (rm1 - rm2 != 1 &&
      rm2 - rm1 != 1 &&
      rm1 - rm2 != board_size &&
      rm2 - rm1 != board_size) {
    return;
  }

  for (i = 0, check = 0; i < 4; i++) {
    if ((game->board[rm1 + cross[i]] & color) == color) {
      check++;
    }
  }

  if (check >= 2) {
    CompareSwapFeature(game->tactical_features, rm1, THROW_IN, SIM_THROW_IN_2);
    update[update_num++] = rm1;
  }

  for (i = 0, check = 0; i < 4; i++) {
    if ((game->board[rm2 + cross[i]] & color) == color) {
      check++;
    }
  }

  if (check >= 2) {
    CompareSwapFeature(game->tactical_features, rm2, THROW_IN, SIM_THROW_IN_2);
    update[update_num++] = rm2;
  }
}
