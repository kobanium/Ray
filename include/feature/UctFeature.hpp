#ifndef _UCT_FEATURE_HPP_
#define _UCT_FEATURE_HPP_


#include <string>

#include "board/GoBoard.hpp"
#include "feature/FeatureUtility.hpp"

enum UCT_FEATURE1{
  UCT_SAVE_CAPTURE_1_1,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_1_2,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_1_3,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_2_1,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_2_2,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_2_3,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_3_1,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_3_2,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_3_3,  // アタリにされた石を守るためのトリ
  UCT_SAVE_CAPTURE_SELF_ATARI,  // アタリにされた石を守るためのトリ

  UCT_CAPTURE,          // トリ
  UCT_CAPTURE_AFTER_KO,  // 新たな劫を解消するトリ
  UCT_2POINT_CAPTURE_S_S,    // 呼吸点が2つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_S_L,    // 呼吸点が2つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_L_S,    // 呼吸点が2つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_L_L,    // 呼吸点が2つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_S_S,    // 呼吸点が3つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_S_L,    // 呼吸点が3つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_L_S,    // 呼吸点が3つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_L_L,    // 呼吸点が3つになった連に隣接するトリ

  UCT_SEMEAI_CAPTURE,    // 呼吸点が1つの連に隣接する連のトリ
  UCT_SELF_ATARI_SMALL,  // 2子以下の自己アタリ
  UCT_SELF_ATARI_NAKADE, // 3子以上の自己アタリ(ナカデになる)
  UCT_SELF_ATARI_LARGE,  // 3子以上の自己アタリ(ナカデにならない)
  UCT_SAVE_EXTENSION_1,    // 1つの石を守るためのノビ
  UCT_SAVE_EXTENSION_2,    // 1つの石を守るためのノビ
  UCT_SAVE_EXTENSION_3,    // 1つの石を守るためのノビ
  UCT_LADDER_EXTENSION,  // シチョウで取られるノビ
  UCT_ATARI,             // アタリ
  UCT_CAPTURABLE_ATARI,  // 1手で捕獲できるアタリ

  UCT_OIOTOSHI,          // 追い落とし
  UCT_SNAPBACK,          // ウッテガエシ
  UCT_2POINT_ATARI_S_S,      // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_S_L,      // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_L_S,      // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_L_L,      // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_2POINT_C_ATARI_S_S,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_S_L,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_L_S,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_L_L,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_3POINT_ATARI_S_S,   // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_3POINT_ATARI_S_L,   // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_3POINT_ATARI_L_S,   // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_3POINT_ATARI_L_L,   // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_3POINT_C_ATARI_S_S, // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_3POINT_C_ATARI_S_L, // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_3POINT_C_ATARI_L_S, // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_3POINT_C_ATARI_L_L, // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_3POINT_DAME_S_S,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_S_L,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_L_S,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_L_L,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_2POINT_EXTENSION_DECREASE, // 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
  UCT_2POINT_EXTENSION_EVEN,     // 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
  UCT_2POINT_EXTENSION_INCREASE, // 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
  UCT_3POINT_EXTENSION_DECREASE, // 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
  UCT_3POINT_EXTENSION_EVEN,     // 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
  UCT_3POINT_EXTENSION_INCREASE, // 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
  UCT_THROW_IN_2,                // 2目の抜き跡に対するホウリコミ
  UCT_NAKADE_3,                  // 3目の抜き跡に対するナカデ
  UCT_KEIMA_TSUKEKOSHI,          // ケイマのツケコシ
  UCT_DOUBLE_KEIMA,              // 両ケイマ
  UCT_KO_CONNECTION,             // 劫の解消

  UCT_MAX,
};


enum PASS_FEATURES {
  UCT_PASS_AFTER_MOVE,
  UCT_PASS_AFTER_PASS,
  UCT_PASS_MAX,
};


constexpr int MOVE_DISTANCE_MAX = 16;

constexpr int UCT_TACTICAL_FEATURE_MAX = UCT_MAX;

const std::string uct_features_name[UCT_TACTICAL_FEATURE_MAX] = {
  "SAVE_CAPTURE_1_1           ",
  "SAVE_CAPTURE_1_2           ",
  "SAVE_CAPTURE_1_3           ",
  "SAVE_CAPTURE_2_1           ",
  "SAVE_CAPTURE_2_2           ",

  "SAVE_CAPTURE_2_3           ",
  "SAVE_CAPTURE_3_1           ",
  "SAVE_CAPTURE_3_2           ",
  "SAVE_CAPTURE_3_3           ",
  "SAVE_CAPTURE_SELF_ATARI    ",

  "CAPTURE                    ",
  "CAPTURE_AFTER_KO           ",
  "2POINT_CAPTURE_S_S         ",
  "2POINT_CAPTURE_S_L         ",
  "2POINT_CAPTURE_L_S         ",

  "2POINT_CAPTURE_L_L         ",
  "3POINT_CAPTURE_S_S         ",
  "3POINT_CAPTURE_S_L         ",
  "3POINT_CAPTURE_L_S         ",
  "3POINT_CAPTURE_L_L         ",

  "SEMEAI_CAPTURE             ",
  "SELF_ATARI_SMALL           ",
  "SELF_ATARI_NAKADE          ",
  "SELF_ATARI_LARGE           ",
  "SAVE_EXTENSION_1           ",

  "SAVE_EXTENSION_2           ",
  "SAVE_EXTENSION_3           ",
  "LADDER_EXTENSION           ",
  "ATARI                      ",
  "CAPTURABLE_ATARI           ",

  "OIOTOSHI                   ",
  "SNAPBACK                   ",
  "2POINT_ATARI_S_S           ",
  "2POINT_ATARI_S_L           ",
  "2POINT_ATARI_L_S           ",
  "2POINT_ATARI_L_L           ",
  "2POINT_C_ATARI_S_S         ",
  "2POINT_C_ATARI_S_L         ",
  "2POINT_C_ATARI_L_S         ",
  "2POINT_C_ATARI_L_L         ",

  "3POINT_ATARI_S_S           ",
  "3POINT_ATARI_S_L           ",
  "3POINT_ATARI_L_S           ",
  "3POINT_ATARI_L_L           ",
  "3POINT_C_ATARI_S_S         ",
  "3POINT_C_ATARI_S_L         ",
  "3POINT_C_ATARI_L_S         ",
  "3POINT_C_ATARI_L_L         ",


  "3POINT_DAME_S_S            ",
  "3POINT_DAME_S_L            ",
  "3POINT_DAME_L_S            ",
  "3POINT_DAME_L_L            ",
  "2POINT_EXTENSION_DECREASE  ",
  "2POINT_EXTENSION_EVEN      ",
  "2POINT_EXTENSION_INCREASE  ",
  "3POINT_EXTENSION_DECREASE  ",

  "3POINT_EXTENSION_EVEN      ",
  "3POINT_EXTENSION_INCREASE  ",
  "THROW_IN_2                 ",
  "NAKADE_3                   ",
  "KEIMA_TSUKEKOSHI           ",

  "DOUBLE_KEIMA               ",
  "KO_CONNECTION              ",
};



struct uct_features_t {
  unsigned long long tactical_features1[BOARD_MAX];
  unsigned long long tactical_features2[BOARD_MAX];
  unsigned long long tactical_features3[BOARD_MAX];
};





// 特徴の判定
void CheckFeaturesForTree( game_info_t *game, int color, uct_features_t *uct_features );

// 劫を解消するトリの判定
void CheckCaptureAfterKoForTree( game_info_t *game, int color, uct_features_t *uct_features );

// 自己アタリの判定
bool CheckSelfAtariForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features );

// トリノ判定
void CheckCaptureForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features );

// アタリの判定
void CheckAtariForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features );

// 劫の解消の判定
void CheckKoConnectionForTree( game_info_t *game, uct_features_t *uct_features );

// 2目の抜き跡の判定
void CheckRemove2StonesForTree( game_info_t *game, int color, uct_features_t *uct_features );

// 3目の抜き跡の判定
void CheckRemove3StonesForTree( game_info_t *game, int color, uct_features_t *uct_features );

// ケイマのツケコシの判定
void CheckKeimaTsukekoshiForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features );

// 両ケイマの判定
void CheckDoubleKeimaForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features );

// ウッテガエシの判定
void CheckSnapBackForTree( game_info_t *game, int color, int pos, uct_features_t *uct_features );

#endif
