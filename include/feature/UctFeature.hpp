#ifndef _UCT_FEATURE_HPP_
#define _UCT_FEATURE_HPP_


#include <string>

#include "board/GoBoard.hpp"
#include "feature/FeatureUtility.hpp"



enum UCT_ALL_FEATURE {
  UCT_CAPTURE_INDEX,
  UCT_SAVE_EXTENSION_INDEX,
  UCT_ATARI_INDEX,
  UCT_EXTENSION_INDEX,
  UCT_DAME_INDEX,
  UCT_CONNECT_INDEX,
  UCT_THROW_IN_INDEX,
  UCT_INDEX_MAX,
};

enum UCT_CAPTURE_FEATURE {
  UCT_CAPTURE_NONE,
  UCT_SELF_ATARI_SMALL,  // 2子以下の自己アタリ
  UCT_SELF_ATARI_NAKADE, // 3子以上の自己アタリ(ナカデになる)
  UCT_SELF_ATARI_LARGE,  // 3子以上の自己アタリ(ナカデにならない)
  UCT_CAPTURE,           // トリ
  UCT_SEMEAI_CAPTURE,
  UCT_KO_RECAPTURE,      // 劫を取り返す
  UCT_CAPTURE_AFTER_KO,  // 新たな劫を解消するトリ
  UCT_3POINT_CAPTURE_S_S,    // 呼吸点が3つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_S_S,    // 呼吸点が2つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_S_L,    // 呼吸点が3つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_S_L,    // 呼吸点が2つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_L_S,    // 呼吸点が3つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_L_S,    // 呼吸点が2つになった連に隣接するトリ
  UCT_2POINT_CAPTURE_L_L,    // 呼吸点が2つになった連に隣接するトリ
  UCT_3POINT_CAPTURE_L_L,    // 呼吸点が3つになった連に隣接するトリ
  UCT_SEMEAI_CAPTURE_SELF_ATARI,
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
  UCT_CAPTURE_MAX,
};

enum UCT_SAVE_EXTENSION_FEATURE {
  UCT_SAVE_EXTENSION_NONE,
  UCT_SAVE_EXTENSION_1,
  UCT_SAVE_EXTENSION_2,
  UCT_SAVE_EXTENSION_3,
  UCT_LADDER_EXTENSION,
  UCT_SAVE_EXTENSION_MAX,
};

enum UCT_ATARI_FEATURE {
  UCT_ATARI_NONE,
  UCT_ATARI,
  UCT_3POINT_ATARI_S_S,    // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_S_S,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_3POINT_ATARI_S_L,    // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_S_L,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_3POINT_ATARI_L_S,    // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_L_S,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_3POINT_ATARI_L_L,    // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(逃げられる)
  UCT_2POINT_ATARI_L_L,    // ダメ2つの自分の連に隣接する敵連に対するアタリ(逃げられる)
  UCT_3POINT_C_ATARI_S_S,  // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_S_S,  // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_3POINT_C_ATARI_S_L,  // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_S_L,  // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_3POINT_C_ATARI_L_S,  // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_L_S,  // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_3POINT_C_ATARI_L_L,  // 呼吸点が3つの連に隣接する呼吸点が2つの敵連に対するアタリ(捕獲できる)
  UCT_2POINT_C_ATARI_L_L,  // ダメ2つの自分の連に隣接する敵連に対するアタリ(捕獲できる)
  UCT_ATARI_MAX,
};


enum UCT_EXTENSION_FEATURE {
  UCT_EXTENSION_NONE,
  UCT_3POINT_EXTENSION_DECREASE, // 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
  UCT_2POINT_EXTENSION_DECREASE, // 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
  UCT_3POINT_EXTENSION_EVEN,     // 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
  UCT_2POINT_EXTENSION_EVEN,     // 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
  UCT_3POINT_EXTENSION_INCREASE, // 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
  UCT_2POINT_EXTENSION_INCREASE, // 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
  UCT_EXTENSION_MAX,
};


enum UCT_DAME_FEATURE {
  UCT_DAME_NONE,
  UCT_3POINT_DAME_S_S,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_S_L,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_L_S,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_L_L,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_DAME_MAX,
};

enum UCT_CONNECT_FEATURE {
  UCT_CONNECT_NONE,
  UCT_KO_CONNECTION,             // 劫の解消
  UCT_CONNECT_MAX,
};

enum UCT_THROW_IN_FEATURE {
  UCT_THROW_IN_NONE,
  UCT_THROW_IN_2,
  //UCT_THROW_IN_CORRECT_EYE,
  UCT_THROW_IN_MAX,
};

enum PASS_FEATURES {
  UCT_PASS_AFTER_MOVE,
  UCT_PASS_AFTER_PASS,
  UCT_PASS_MAX,
};


constexpr int MOVE_DISTANCE_MAX = 16;

constexpr int POS_ID_MAX = 64;

const std::string uct_capture_name[UCT_CAPTURE_MAX] = {
  "",
  "UCT_SELF_ATARI_SMALL         ",
  "UCT_SELF_ATARI_NAKADE        ",
  "UCT_SELF_ATARI_LARGE         ",
  "UCT_CAPTURE                  ",
  "UCT_SEMEAI_CAPTURE           ",
  "UCT_KO_RECAPTURE             ",
  "UCT_CAPTURE_AFTER_KO         ",
  "UCT_3POINT_CAPTURE_S_S       ",
  "UCT_2POINT_CAPTURE_S_S       ",
  "UCT_3POINT_CAPTURE_S_L       ",
  "UCT_2POINT_CAPTURE_S_L       ",
  "UCT_3POINT_CAPTURE_L_S       ",
  "UCT_2POINT_CAPTURE_L_S       ",
  "UCT_2POINT_CAPTURE_L_L       ",
  "UCT_3POINT_CAPTURE_L_L       ",
  "UCT_SEMEAI_CAPTURE_SELF_ATARI",
  "UCT_SAVE_CAPTURE_1_1         ",
  "UCT_SAVE_CAPTURE_1_2         ",
  "UCT_SAVE_CAPTURE_1_3         ",
  "UCT_SAVE_CAPTURE_2_1         ",
  "UCT_SAVE_CAPTURE_2_2         ",
  "UCT_SAVE_CAPTURE_2_3         ",
  "UCT_SAVE_CAPTURE_3_1         ",
  "UCT_SAVE_CAPTURE_3_2         ",
  "UCT_SAVE_CAPTURE_3_3         ",
  "UCT_SAVE_CAPTURE_SELF_ATARI  ",
};

const std::string uct_save_extension_name[UCT_SAVE_EXTENSION_MAX] = {
  "",
  "UCT_SAVE_EXTENSION_1  ",
  "UCT_SAVE_EXTENSION_2  ",
  "UCT_SAVE_EXTENSION_3  ",
  "UCT_LADDER_EXTENSION  ",
};

const std::string uct_atari_name[UCT_ATARI_MAX] = {
  "",
  "UCT_ATARI             ",
  "UCT_3POINT_ATARI_S_S  ",
  "UCT_2POINT_ATARI_S_S  ",
  "UCT_3POINT_ATARI_S_L  ",
  "UCT_2POINT_ATARI_S_L  ",
  "UCT_3POINT_ATARI_L_S  ",
  "UCT_2POINT_ATARI_L_S  ",
  "UCT_3POINT_ATARI_L_L  ",
  "UCT_2POINT_ATARI_L_L  ",
  "UCT_3POINT_C_ATARI_S_S",
  "UCT_2POINT_C_ATARI_S_S",
  "UCT_3POINT_C_ATARI_S_L",
  "UCT_2POINT_C_ATARI_S_L",
  "UCT_3POINT_C_ATARI_L_S",
  "UCT_2POINT_C_ATARI_L_S",
  "UCT_3POINT_C_ATARI_L_L",
  "UCT_2POINT_C_ATARI_L_L",
};

const std::string uct_extension_name[UCT_EXTENSION_MAX] = {
  "",
  "UCT_3POINT_EXTENSION_DECREASE",
  "UCT_2POINT_EXTENSION_DECREASE",
  "UCT_3POINT_EXTENSION_EVEN    ",
  "UCT_2POINT_EXTENSION_EVEN    ",
  "UCT_3POINT_EXTENSION_INCREASE",
  "UCT_2POINT_EXTENSION_INCREASE",
};

const std::string uct_dame_name[UCT_DAME_MAX] = {
  "",
  "UCT_3POINT_DAME_S_S",
  "UCT_3POINT_DAME_S_L",
  "UCT_3POINT_DAME_L_S",
  "UCT_3POINT_DAME_L_L",
};

const std::string uct_connect_name[UCT_CONNECT_MAX] = {
  "",
  "UCT_KO_CONNECTION",
};

const std::string uct_throw_in_name[UCT_THROW_IN_MAX] = {
  "",
  "UCT_THROW_IN_2",
};


inline int
UctFeatureIndex( const int pos, const int feature )
{
  return pos * UCT_INDEX_MAX + feature;
}


// 特徴の判定
int CheckFeaturesForTree( const game_info_t *game, const int color, unsigned int *tactical_features );

// 劫を解消するトリの判定
void CheckCaptureAfterKoForTree( const game_info_t *game, const int color, unsigned int *tactical_features );

// 劫を取り返す手の判定
void CheckKoRecaptureForTree( const game_info_t *game, const int color, unsigned int *tactical_features );

// 自己アタリの判定
bool CheckSelfAtariForTree( const game_info_t *game, const int color, const int pos, unsigned int *tactical_features );

// トリノ判定
void CheckCaptureForTree( const game_info_t *game, const int color, const int pos, unsigned int *tactical_features );

// アタリの判定
void CheckAtariForTree( const game_info_t *game, const int color, const int pos, unsigned int *tactical_features );

// 劫の解消の判定
void CheckKoConnectionForTree( const game_info_t *game, unsigned int *tactical_features );

// 2目の抜き跡の判定
void CheckRemove2StonesForTree( const game_info_t *game, const int color, unsigned int *tactical_features );


#endif
