/**
 * @file include/feature/SimulationFeature.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Tactical features for Monte-Carlo simulation.
 * @~japanese
 * @brief シミュレーション用の戦術的特徴
 */
#ifndef _SIMULATION_FEATURE_HPP_
#define _SIMULATION_FEATURE_HPP_

#include <string>

#include "board/GoBoard.hpp"
#include "feature/FeatureUtility.hpp"

/**
 * @def TF_INDEX(p, f)
 * @brief \~english Get tactical feature's index.
 *        \~japanese 戦術的特徴を表すインデックスの取得
 */
#define TF_INDEX(p, f) ((p) * ALL_MAX + (f))


/**
 * @enum ALL_FEATURE
 * @~english
 * @brief Tactical features indice for Monte-Carlo simulation.
 * @var CAPTURE
 * Capture feature index.
 * @var SAVE_EXTENSION
 * Save extension feature index.
 * @var ATARI
 * Atari feature index.
 * @var EXTENSION
 * Extension feature index.
 * @var DAME
 * Dame feature index.
 * @var THROW_IN
 * Throw-in feature index.
 * @var ALL_MAX
 * Sentinel.
 * @~japanese
 * @brief 特徴を格納しているインデックス
 * @var CAPTURE
 * トリの特徴のインデックス
 * @var SAVE_EXTENSION
 * アタリから逃げる特徴のインデックス
 * @var ATARI
 * アタリの特徴のインデックス
 * @var EXTENSION
 * ノビの特徴のインデックス
 * @var DAME
 * ダメを詰める特徴のインデックス
 * @var THROW_IN
 * ホウリコミの特徴のインデックス
 * @var ALL_MAX
 * 番兵
 */
enum ALL_FEATURE {
  CAPTURE,
  SAVE_EXTENSION,
  ATARI,
  EXTENSION,
  DAME,
  THROW_IN,
  ALL_MAX,
};


/**
 * @enum CAPTURE_FEATURE
 * @~english
 * @brief Capture features for Monte-Carlo simulation.
 * @var SIM_CAPTURE_NONE
 * Neutral feature.
 * @var SIM_SELF_ATARI_SMALL
 * Self atari less than or equal to 2 stones.
 * @var SIM_SELF_ATARI_NAKADE
 * Self atari more than or equal to 3 stones (nakade shape).
 * @var SIM_SELF_ATARI_LARGE
 * Self atari more than or equal to 3 stones (not nakade shape).
 * @var SIM_CAPTURE
 * Simple capture.
 * @var SIM_3POINT_CAPTURE_SMALL
 * Capture capturing-race stones.
 * @var SIM_2POINT_CAPTURE_SMALL
 * Capture capturing-race stones.
 * @var SIM_3POINT_CAPTURE_LARGE
 * Capture capturing-race stones.
 * @var SIM_2POINT_CAPTURE_LARGE
 * Capture capturing-race stones.
 * @var SIM_CAPTURE_AFTER_KO
 * Capture solving ko.
 * @var SIM_SAVE_CAPTURE_SELF_ATARI
 * Capture capturing-race stones (captured by snap back).
 * @var SIM_SAVE_CAPTURE_1_1
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_1_2
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_1_3
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_2_1
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_2_2
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_2_3
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_3_1
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_3_2
 * Saving atari stones capture.
 * @var SIM_SAVE_CAPTURE_3_3
 * Saving atari stones capture.
 * @var SIM_CAPTURE_MAX
 * Sentinel.
 * @~japanese
 * @brief トリの特徴
 * @var SIM_CAPTURE_NONE
 * 特徴なし
 * @var SIM_SELF_ATARI_SMALL
 * 2子以下の自己アタリ
 * @var SIM_SELF_ATARI_NAKADE
 * 3子以上の自己アタリ(ナカデになる)
 * @var SIM_SELF_ATARI_LARGE
 * 3子以上の自己アタリ(ナカデにならない)
 * @var SIM_CAPTURE
 * トリ
 * @var SIM_3POINT_CAPTURE_SMALL
 * 呼吸点が3つになった連に隣接する石のトリ
 * @var SIM_2POINT_CAPTURE_SMALL
 * 呼吸点が2つになった連に隣接する石のトリ
 * @var SIM_3POINT_CAPTURE_LARGE
 * 呼吸点が3つになった連に隣接する石のトリ
 * @var SIM_2POINT_CAPTURE_LARGE
 * 呼吸点が2つになった連に隣接する石のトリ
 * @var SIM_CAPTURE_AFTER_KO
 * 劫を解消するトリ
 * @var SIM_SAVE_CAPTURE_SELF_ATARI
 * アタリにされた石を守るトリ (ウッテガエシで取られる)
 * @var SIM_SAVE_CAPTURE_1_1
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_1_2
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_1_3
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_2_1
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_2_2
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_2_3
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_3_1
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_3_2
 * アタリにされた石を守るトリ
 * @var SIM_SAVE_CAPTURE_3_3
 * アタリにされた石を守るトリ
 * @var SIM_CAPTURE_MAX
 * 番兵
 */
enum CAPTURE_FEATURE {
  SIM_CAPTURE_NONE,
  SIM_SELF_ATARI_SMALL,
  SIM_SELF_ATARI_NAKADE,
  SIM_SELF_ATARI_LARGE,
  SIM_CAPTURE,

  SIM_3POINT_CAPTURE_SMALL,
  SIM_2POINT_CAPTURE_SMALL,
  SIM_3POINT_CAPTURE_LARGE,
  SIM_2POINT_CAPTURE_LARGE,
  SIM_CAPTURE_AFTER_KO,

  SIM_SAVE_CAPTURE_SELF_ATARI,
  SIM_SAVE_CAPTURE_1_1,
  SIM_SAVE_CAPTURE_1_2,
  SIM_SAVE_CAPTURE_1_3,
  SIM_SAVE_CAPTURE_2_1,

  SIM_SAVE_CAPTURE_2_2,
  SIM_SAVE_CAPTURE_2_3,
  SIM_SAVE_CAPTURE_3_1,
  SIM_SAVE_CAPTURE_3_2,
  SIM_SAVE_CAPTURE_3_3,

  SIM_CAPTURE_MAX,
};


/**
 * @enum SAVE_EXTENSION_FEATURE
 * @~english
 * @brief Extension features saving atari stones for Monte-Carlo simulation.
 * @var SIM_SAVE_EXTENSION_NONE
 * Neutral feature.
 * @var SIM_SAVE_EXTENSION_1
 * Extension move saving 1 stone.
 * @var SIM_SAVE_EXTENSION_2
 * Extension move saving 2 stones.
 * @var SIM_SAVE_EXTENSION_3
 * Extension move saving more than or equal to 3 stones.
 * @var SIM_SAVE_EXTENSION_SAFELY_1
 * Extension move saving 1 stone with increasing the number of liberties.
 * @var SIM_SAVE_EXTENSION_SAFELY_2
 * Extension move saving 2 stones with increasing the number of liberties.
 * @var SIM_SAVE_EXTENSION_SAFELY_3
 * Extension move saving more than or equal to 3 stones with increasing the number of liberties.
 * @var SIM_SAVE_EXTENSION_MAX
 * Sentinel.
 * @~japanese
 * @brief アタリにされた石を守る手の特徴
 * @var SIM_SAVE_EXTENSION_NONE
 * 特徴なし
 * @var SIM_SAVE_EXTENSION_1
 * 1つの石を守るためのノビ
 * @var SIM_SAVE_EXTENSION_2
 * 2つの石を守るためのノビ
 * @var SIM_SAVE_EXTENSION_3
 * 3つ以上の石を守るためのノビ
 * @var SIM_SAVE_EXTENSION_SAFELY_1
 * 1つの石を守るためのノビ (呼吸点は増加)
 * @var SIM_SAVE_EXTENSION_SAFELY_2
 * 2つの石を守るためのノビ (呼吸点は増加)
 * @var SIM_SAVE_EXTENSION_SAFELY_3
 * 3つ以上の石を守るためのノビ (呼吸点は増加)
 * @var SIM_SAVE_EXTENSION_MAX
 * 番兵
 */
enum SAVE_EXTENSION_FEATURE {
  SIM_SAVE_EXTENSION_NONE,
  SIM_SAVE_EXTENSION_1,
  SIM_SAVE_EXTENSION_2,
  SIM_SAVE_EXTENSION_3,
  SIM_SAVE_EXTENSION_SAFELY_1,

  SIM_SAVE_EXTENSION_SAFELY_2,
  SIM_SAVE_EXTENSION_SAFELY_3,

  SIM_SAVE_EXTENSION_MAX,
};


/**
 * @enum ATARI_FEATURE
 * @~english
 * @brief Atari features for Monte-Carlo simulation.
 * @var SIM_ATARI_NONE
 * Neutral feature.
 * @var SIM_ATARI
 * Atari.
 * @var SIM_3POINT_ATARI_SMALL
 * Atari string with less than or equal to 2 stones for saving a string with 3 liberties.
 * @var SIM_2POINT_ATARI_SMALL
 * Atari string with less than or equal to 2 stones for saving a string with 2 liberties.
 * @var SIM_3POINT_ATARI_LARGE
 * Atari string with more than or equal to 3 stones for saving a string with 3 liberties.
 * @var SIM_2POINT_ATARI_LARGE
 * Atari string with more than or equal to 3 stones for saving a string with 2 liberties.
 * @var SIM_3POINT_C_ATARI_SMALL
 * Capturable atari string with less than or equal to 2 stones for saving a string with 3 liberties.
 * @var SIM_2POINT_C_ATARI_SMALL
 * Capturable atari string with less than or equal to 2 stones for saving a string with 2 liberties.
 * @var SIM_3POINT_C_ATARI_LARGE
 * Capturable atari string with more than or equal to 3 stones for saving a string with 3 liberties.
 * @var SIM_2POINT_C_ATARI_LARGE
 * Capturable atari string with more than or equal to 3 stones for saving a string with 2 liberties.
 * @var SIM_ATARI_MAX
 * Sentinel
 * @~japanese
 * @brief アタリの特徴 (モンテカルロ・シミュレーション用)
 * @var SIM_ATARI_NONE
 * 特徴なし
 * @var SIM_ATARI
 * アタリ
 * @var SIM_3POINT_ATARI_SMALL
 * 3つの呼吸点を持つ連を助けるために2個以下の石で構成された連をアタリにする手
 * @var SIM_2POINT_ATARI_SMALL
 * 2つの呼吸点を持つ連を助けるために2個以下の石で構成された連をアタリにする手
 * @var SIM_3POINT_ATARI_LARGE
 * 3つの呼吸点を持つ連を助けるために3個以上の石で構成された連をアタリにする手
 * @var SIM_2POINT_ATARI_LARGE
 * 2つの呼吸点を持つ連を助けるために3個以上の石で構成された連をアタリにする手
 * @var SIM_3POINT_C_ATARI_SMALL
 * 3つの呼吸点を持つ連を助けるために2個以下の石で構成された連をアタリにする手 (捕獲可能)
 * @var SIM_2POINT_C_ATARI_SMALL
 * 2つの呼吸点を持つ連を助けるために2個以下の石で構成された連をアタリにする手 (捕獲可能)
 * @var SIM_3POINT_C_ATARI_LARGE
 * 3つの呼吸点を持つ連を助けるために3個以上の石で構成された連をアタリにする手 (捕獲可能)
 * @var SIM_2POINT_C_ATARI_LARGE
 * 2つの呼吸点を持つ連を助けるために3個以上の石で構成された連をアタリにする手 (捕獲可能)
 * @var SIM_ATARI_MAX
 * 番兵
 */
enum ATARI_FEATURE {
  SIM_ATARI_NONE,
  SIM_ATARI,
  SIM_3POINT_ATARI_SMALL,
  SIM_2POINT_ATARI_SMALL,
  SIM_3POINT_ATARI_LARGE,

  SIM_2POINT_ATARI_LARGE,
  SIM_3POINT_C_ATARI_SMALL,
  SIM_2POINT_C_ATARI_SMALL,
  SIM_3POINT_C_ATARI_LARGE,
  SIM_2POINT_C_ATARI_LARGE,

  SIM_ATARI_MAX,
};

/**
 * @enum EXTENSION_FEATURE
 * @~english
 * @brief Extension features for Monte-Carlo simulation.
 * @var SIM_EXTENSION_NONE
 * Neutral feature.
 * @var SIM_3POINT_EXTENSION
 * Extension for saving a string with 3 liberties.
 * @var SIM_2POINT_EXTENSION
 * Extension for saving a string with 2 libertoes.
 * @var SIM_3POINT_EXTENSION_SAFELY
 * Safely extension for saving a string with 3 liberties.
 * @var SIM_2POINT_EXTENSION_SAFELY
 * Safely extension for saving a string with 2 liberties.
 * @var SIM_EXTENSION_MAX
 * Sentinel.
 * @~japanese
 * @brief ノビの特徴
 * @var SIM_EXTENSION_NONE
 * 特徴なし
 * @var SIM_3POINT_EXTENSION
 * 3つの呼吸点を持つ連を助ける安全なノビ
 * @var SIM_2POINT_EXTENSION
 * 2つの呼吸点を持つ連を助ける安全なノビ
 * @var SIM_3POINT_EXTENSION_SAFELY
 * 3つの呼吸点を持つ連を助ける安全なノビ
 * @var SIM_2POINT_EXTENSION_SAFELY
 * 2つの呼吸点を持つ連を助ける安全なノビ
 * @var SIM_EXTENSION_MAX
 * 番兵
 */
enum EXTENSION_FEATURE {
  SIM_EXTENSION_NONE,
  SIM_3POINT_EXTENSION,
  SIM_2POINT_EXTENSION,
  SIM_3POINT_EXTENSION_SAFELY,
  SIM_2POINT_EXTENSION_SAFELY,

  SIM_EXTENSION_MAX,
};

/**
 * @enum DAME_FEATURE
 * @~english
 * @brief Dame features for Monte-Carlo simulation.
 * @var SIM_DAME_NONE
 * Neutral feature.
 * @var SIM_3POINT_DAME_SMALL
 * Feature of move to a small string's dame for saving a string. 
 * @var SIM_3POINT_DAME_LARGE
 * Feature of move to a large string's dame for saving a string.
 * @var SIM_DAME_MAX
 * Sentinel.
 * @~japanese
 * @brief ダメに打つ手の特徴 (モンテカルロ・シミュレーション用)
 * @var SIM_DAME_NONE
 * 特徴なし
 * @var SIM_3POINT_DAME_SMALL
 * 3つの呼吸点を持つ2個以下の石で構成される連のダメに打つ手
 * @var SIM_3POINT_DAME_LARGE
 * 3つの呼吸点を持つ3個以上の石で構成される連のダメに打つ手
 * @var SIM_DAME_MAX
 * 番兵
 */
enum DAME_FEATURE {
  SIM_DAME_NONE,
  SIM_3POINT_DAME_SMALL,
  SIM_3POINT_DAME_LARGE,

  SIM_DAME_MAX,
};

/**
 * @enum THROW_IN_FEATURE
 * @~english
 * @brief Throw-in features for Monte-Carlo simulation.
 * @var SIM_THROW_IN_NONE
 * Neutral feature.
 * @var SIM_THROW_IN_2
 * Throw in to force to be half-eye.
 * @var SIM_THROW_IN_MAX
 * Sentinel
 * @~japanese
 * @brief ホウリコミの特徴 (モンテカルロ・シミュレーション用)
 * @var SIM_THROW_IN_NONE
 * 特徴なし
 * @var SIM_THROW_IN_2
 * 2目の抜き跡を欠け眼にする手
 * @var SIM_THROW_IN_MAX
 * 番兵
 */
enum THROW_IN_FEATURE {
  SIM_THROW_IN_NONE,
  SIM_THROW_IN_2,

  SIM_THROW_IN_MAX,
};



/**
 * @~english
 * @brief Names of capture feature for Monte-Carlo simulation.
 * @~japanese
 * @brief トリの特徴の名前 (モンテカルロ・シミュレーション用)
 */
const std::string sim_capture_name[SIM_CAPTURE_MAX] = {
  "SIM_CAPTURE_NONE",
  "SIM_SELF_ATARI_SMALL",
  "SIM_SELF_ATARI_NAKADE",
  "SIM_SELF_ATARI_LARGE",
  "SIM_CAPTURE",
  "SIM_3POINT_CAPTURE_SMALL",
  "SIM_2POINT_CAPTURE_SMALL",
  "SIM_3POINT_CAPTURE_LARGE",
  "SIM_2POINT_CAPTURE_LARGE",
  "SIM_CAPTURE_AFTER_KO",
  "SIM_SAVE_CAPTURE_SELF_ATARI",
  "SIM_SAVE_CAPTURE_1_1",
  "SIM_SAVE_CAPTURE_1_2",
  "SIM_SAVE_CAPTURE_1_3",
  "SIM_SAVE_CAPTURE_2_1",
  "SIM_SAVE_CAPTURE_2_2",
  "SIM_SAVE_CAPTURE_2_3",
  "SIM_SAVE_CAPTURE_3_1",
  "SIM_SAVE_CAPTURE_3_2",
  "SIM_SAVE_CAPTURE_3_3",
};

/**
 * @~english
 * @brief Names of save extension features for Monte-Carlo simulation.
 * @~japanese
 * @brief アタリにされた石を守る手の特徴の名前 (モンテカルロ・シミュレーション用)
 */
const std::string sim_save_extension_name[SIM_SAVE_EXTENSION_MAX] = {
  "SIM_SAVE_EXTENSION_NONE",
  "SIM_SAVE_EXTENSION_1",
  "SIM_SAVE_EXTENSION_2",
  "SIM_SAVE_EXTENSION_3",
  "SIM_SAVE_EXTENSION_SAFELY_1",
  "SIM_SAVE_EXTENSION_SAFELY_2",
  "SIM_SAVE_EXTENSION_SAFELY_3",
};

/**
 * @~english
 * @brief Names of atari features for Monte-Carlo simulation.
 * @~japanese
 * @brief アタリの特徴の名前 (モンテカルロ・シミュレーション用)
 */
const std::string sim_atari_name[SIM_ATARI_MAX] = {
  "SIM_ATARI_NONE",
  "SIM_ATARI",
  "SIM_3POINT_ATARI_SMALL",
  "SIM_2POINT_ATARI_SMALL",
  "SIM_3POINT_ATARI_LARGE",
  "SIM_2POINT_ATARI_LARGE",
  "SIM_3POINT_C_ATARI_SMALL",
  "SIM_2POINT_C_ATARI_SMALL",
  "SIM_3POINT_C_ATARI_LARGE",
  "SIM_2POINT_C_ATARI_LARGE",
};

/**
 * @~english
 * @brief Names of extension features for Monte-Carlo simulation.
 * @~japanese
 * @brief ノビの特徴の名前 (モンテカルロ・シミュレーション用)
 */
const std::string sim_extension_name[SIM_EXTENSION_MAX] = {
  "SIM_EXTENSION_NONE",
  "SIM_3POINT_EXTENSION",
  "SIM_2POINT_EXTENSION",
  "SIM_3POINT_EXTENSION_SAFELY",
  "SIM_2POINT_EXTENSION_SAFELY",
};

/**
 * @~english
 * @brief Names of dame features for Monte-Carlo simulation.
 * @~japanese
 * @brief ダメの特徴の名前 (モンテカルロ・シミュレーション用)
 */
const std::string sim_dame_name[SIM_DAME_MAX] = {
  "SIM_DAME_NONE",
  "SIM_3POINT_DAME_SMALL",
  "SIM_3POINT_DAME_LARGE",
};

/**
 * @~english
 * @brief Names of throw-in features for Monte-Carlo simulation.
 * @~japanese
 * @brief ホウリコミの特徴の名前 (モンテカルロ・シミュレーション用)
 */
const std::string sim_throw_in_name[SIM_THROW_IN_MAX] = {
  "SIM_THROW_IN_NONE",
  "SIM_THROW_IN_2",
};

/**
 * @~english
 * @brief The number of tactical features for Monte-Carlo simulation.
 * @~japanese
 * @brief モンテカルロ・シミュレーション用の戦術的特徴の個数
 */
constexpr int SIM_TACTICAL_FEATURES = (SIM_CAPTURE_MAX - 1) +
  (SIM_SAVE_EXTENSION_MAX - 1) + (SIM_ATARI_MAX - 1) +
  (SIM_EXTENSION_MAX - 1) + (SIM_DAME_MAX - 1) + (SIM_THROW_IN_MAX - 1);

/**
 * @~english
 * @brief Maximum value of distance from previous move.
 * @~japanese
 * @brief 直前の手からの距離の最大値
 */
constexpr int PREVIOUS_DISTANCE_MAX = 3;


/**
 * @~english
 * @brief Set tactical features array to zero.
 * @param[in, out] features Tactical features array.
 * @~japanese
 * @brief 戦術的特徴の削除
 * @param[in, out] features 戦術的特徴の配列
 */
inline void
ClearTacticalFeatures( unsigned char *features )
{
  features[CAPTURE] = 0;
  features[SAVE_EXTENSION] = 0;
  features[ATARI] = 0;
  features[EXTENSION] = 0;
  features[DAME] = 0;
  features[THROW_IN] = 0;
}


/**
 * @~english
 * @brief Update tactical feature.
 * @param[in, out] tactical_features Tactical features array for all coordinates.
 * @param[in] pos Update coordinate.
 * @param[in] type Tactical feature type.
 * @param[in] new_feature New feature value.
 * @~japanese
 * @brief 戦術的特徴の更新
 * @param[in, out] tactical_features 全座標の戦術的特徴の配列
 * @param[in] pos 更新する座標
 * @param[in] type 戦術的特徴の種別
 * @param[in] new_feature 新しい特徴値
 */
inline void
CompareSwapFeature( unsigned char *tactical_features, const int pos, const int type, const unsigned int new_feature )
{
  if (tactical_features[TF_INDEX(pos, type)] < new_feature) {
    tactical_features[TF_INDEX(pos, type)] = new_feature;
  }
}


// 斜めの位置の取得
void SetCrossPosition( void );

// 直前の着手の周辺の特徴の判定
void CheckFeaturesForSimulation( game_info_t *game, const int color, int *update, int *update_num );

// 劫を解消するトリの判定
void CheckCaptureAfterKoForSimulation( game_info_t *game, const int color, int *update, int *update_num );

// 自己アタリの判定
bool CheckSelfAtariForSimulation( game_info_t *game, const int color, const int pos );

// トリとアタリの判定
void CheckCaptureAndAtariForSimulation( game_info_t *game, const int color, const int pos );

// 2目の抜き跡に対するホウリコミ
void CheckRemove2StonesForSimulation( game_info_t *game, const int color, int *update, int *update_num );

#endif

