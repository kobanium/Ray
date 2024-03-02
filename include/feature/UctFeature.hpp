/**
 * @file include/feature/UctFeature.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Features for tree search to order moves.
 * @~japanese
 * @brief 木探索の着手評価用の特徴
 */
#ifndef _UCT_FEATURE_HPP_
#define _UCT_FEATURE_HPP_


#include <string>

#include "board/GoBoard.hpp"
#include "feature/FeatureUtility.hpp"


/**
 * @enum UCT_ALL_FEATURE
 * @~english
 * @brief Indices for tactical features.
 * @var UCT_CAPTURE_INDEX
 * Capture feature index.
 * @var UCT_SAVE_EXTENSION_INDEX
 * Save extension feature index.
 * @var UCT_ATARI_INDEX
 * Atari feature index.
 * @var UCT_EXTENSION_INDEX
 * Extension feature index.
 * @var UCT_DAME_INDEX
 * Dame feature index.
 * @var UCT_CONNECT_INDEX
 * Connection feature index.
 * @var UCT_THROW_IN_INDEX
 * Throw-in feature index.
 * @var UCT_INDEX_MAX
 * Sentinel.
 * @~japanese
 * @brief 特徴を格納しているインデックス
 * @var UCT_CAPTURE_INDEX
 * トリの特徴のインデックス
 * @var UCT_SAVE_EXTENSION_INDEX
 * アタリから逃げる特徴のインデックス
 * @var UCT_ATARI_INDEX
 * アタリの特徴のインデックス
 * @var UCT_EXTENSION_INDEX
 * ノビの特徴のインデックス
 * @var UCT_DAME_INDEX
 * ダメを詰める特徴のインデックス
 * @var UCT_CONNECT_INDEX
 * ツギの特徴のインデックス
 * @var UCT_THROW_IN_INDEX
 * ホウリコミの特徴のインデックス
 * @var UCT_INDEX_MAX
 * 番兵
 */
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


/**
 * @enum UCT_CAPTURE_FEATURE
 * @~english
 * @brief Capture feature.
 * @var UCT_CAPTURE_NONE
 * Neutral feature.
 * @var UCT_SELF_ATARI_SMALL
 * Self atari.
 * @var UCT_SELF_ATARI_NAKADE
 * Self atari (nakade shape).
 * @var UCT_SELF_ATARI_LARGE
 * Self atari (not nakade shape).
 * @var UCT_CAPTURE
 * Simple capture.
 * @var UCT_SEMEAI_CAPTURE
 * Capturing-race capture.
 * @var UCT_KO_RECAPTURE
 * Recapture ko stone.
 * @var UCT_CAPTURE_AFTER_KO
 * Capture solving ko.
 * @var UCT_3POINT_CAPTURE_S_S
 * Capture capturing-race stones.
 * @var UCT_2POINT_CAPTURE_S_S
 * Capture capturing-race stones.
 * @var UCT_3POINT_CAPTURE_S_L
 * Capture capturing-race stones.
 * @var UCT_2POINT_CAPTURE_S_L
 * Capture capturing-race stones.
 * @var UCT_3POINT_CAPTURE_L_S
 * Capture capturing-race stones.
 * @var UCT_2POINT_CAPTURE_L_S
 * Capture capturing-race stones.
 * @var UCT_3POINT_CAPTURE_L_L
 * Capture capturing-race stones.
 * @var UCT_2POINT_CAPTURE_L_L
 * Capture capturing-race stones.
 * @var UCT_SEMEAI_CAPTURE_SELF_ATARI
 * Capture capturing-race stones (captured by snap back).
 * @var UCT_SAVE_CAPTURE_1_1
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_1_2
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_1_3
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_2_1
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_2_2
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_2_3
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_3_1
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_3_2
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_3_3
 * Saving atari stones capture.
 * @var UCT_SAVE_CAPTURE_SELF_ATARI
 * Saving atari stones capture (captured by snap back).
 * @var UCT_CAPTURE_MAX
 * Sentinel.
 * @~japanese
 * @brief トリの特徴
 * @var UCT_CAPTURE_NONE
 * 特徴なし
 * @var UCT_SELF_ATARI_SMALL
 * 2子以下の自己アタリ
 * @var UCT_SELF_ATARI_NAKADE
 * 3子以上の自己アタリ(ナカデになる)
 * @var UCT_SELF_ATARI_LARGE
 * 3子以上の自己アタリ(ナカデにならない)
 * @var UCT_CAPTURE
 * トリ
 * @var UCT_SEMEAI_CAPTURE
 * 攻め合いになっている石のトリ
 * @var UCT_KO_RECAPTURE
 * 劫を取り返すトリ
 * @var UCT_CAPTURE_AFTER_KO
 * 劫を解消するトリ
 * @var UCT_3POINT_CAPTURE_S_S
 * 呼吸点が3つになった連に隣接する石のトリ
 * @var UCT_2POINT_CAPTURE_S_S
 * 呼吸点が2つになった連に隣接する石のトリ
 * @var UCT_3POINT_CAPTURE_S_L
 * 呼吸点が3つになった連に隣接する石のトリ
 * @var UCT_2POINT_CAPTURE_S_L
 * 呼吸点が2つになった連に隣接する石のトリ
 * @var UCT_3POINT_CAPTURE_L_S
 * 呼吸点が3つになった連に隣接する石のトリ
 * @var UCT_2POINT_CAPTURE_L_S
 * 呼吸点が2つになった連に隣接する石のトリ
 * @var UCT_3POINT_CAPTURE_L_L
 * 呼吸点が3つになった連に隣接する石のトリ
 * @var UCT_2POINT_CAPTURE_L_L
 * 呼吸点が2つになった連に隣接する石のトリ
 * @var UCT_SEMEAI_CAPTURE_SELF_ATARI
 * 呼吸点が2や3になった連に隣接する石のトリ (ウッテガエシで取られる)
 * @var UCT_SAVE_CAPTURE_1_1
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_1_2
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_1_3
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_2_1
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_2_2
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_2_3
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_3_1
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_3_2
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_3_3
 * アタリにされた石を守るためのトリ
 * @var UCT_SAVE_CAPTURE_SELF_ATARI
 * アタリにされた石を守るためのトリ (ウッテガエシで取られる)
 * @var UCT_CAPTURE_MAX
 * 番兵
 */
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

/**
 * @enum UCT_SAVE_EXTENSION_FEATURE
 * @~english
 * @brief Extension feature saving atari stones.
 * @var UCT_SAVE_EXTENSION_NONE
 * Neutral feature.
 * @var UCT_SAVE_EXTENSION_1
 * Extension move saving 1 stone.
 * @var UCT_SAVE_EXTENSION_2
 * Extension move saving 2 stones.
 * @var UCT_SAVE_EXTENSION_3
 * Extension move saving more than or equal to 3 stones.
 * @var UCT_LADDER_EXTENSION
 * Extension move captured by ladder.
 * @var UCT_SAVE_EXTENSION_MAX
 * Sentinel.
 * @~japanese
 * @brief 石を守るためのノビの特徴
 * @var UCT_SAVE_EXTENSION_NONE
 * 特徴無し
 * @var UCT_SAVE_EXTENSION_1
 * 1つの石を守るためのノビ
 * @var UCT_SAVE_EXTENSION_2
 * 2つの石を守るためのノビ
 * @var UCT_SAVE_EXTENSION_3
 * 3つの石を守るためのノビ
 * @var UCT_LADDER_EXTENSION
 * シチョウで取られるノビ
 * @var UCT_SAVE_EXTENSION_MAX
 * 番兵
 */
enum UCT_SAVE_EXTENSION_FEATURE {
  UCT_SAVE_EXTENSION_NONE,
  UCT_SAVE_EXTENSION_1,
  UCT_SAVE_EXTENSION_2,
  UCT_SAVE_EXTENSION_3,
  UCT_LADDER_EXTENSION,
  UCT_SAVE_EXTENSION_MAX,
};

/**
 * @enum UCT_ATARI_FEATURE
 * @~english
 * @brief Atari features.
 * @var UCT_ATARI_NONE
 * Neutral feature.
 * @var UCT_ATARI
 * Simple atari feature.
 * @var UCT_3POINT_ATARI_S_S
 * Atari move capturing-race stones to save string with less than or equal 2 stones (escapable).
 * @var UCT_2POINT_ATARI_S_S
 * Atari move capturing-race stones to save string with less than or equal 2 stones (escapable).
 * @var UCT_3POINT_ATARI_S_L
 * Atari move capturing-race stones to save string with less than or equal 2 stones (escapable).
 * @var UCT_2POINT_ATARI_S_L
 * Atari move capturing-race stones to save string with less than or equal 2 stones (escapable).
 * @var UCT_3POINT_ATARI_L_S
 * Atari move capturing-race stones to save string with more than or equal 3 stones (escapable).
 * @var UCT_2POINT_ATARI_L_S
 * Atari move capturing-race stones to save string with more than or equal 3 stones (escapable).
 * @var UCT_3POINT_ATARI_L_L
 * Atari move capturing-race stones to save string with more than or equal 3 stones (escapable).
 * @var UCT_2POINT_ATARI_L_L
 * Atari move capturing-race stones to save string with more than or equal 3 stones (escapable).
 * @var UCT_3POINT_C_ATARI_S_S
 * Atari move capturing-race stones to save string with less than or equal 2 stones (capturable).
 * @var UCT_2POINT_C_ATARI_S_S
 * Atari move capturing-race stones to save string with less than or equal 2 stones (capturable).
 * @var UCT_3POINT_C_ATARI_S_L
 * Atari move capturing-race stones to save string with less than or equal 2 stones (capturable).
 * @var UCT_2POINT_C_ATARI_S_L
 * Atari move capturing-race stones to save string with less than or equal 2 stones (capturable).
 * @var UCT_3POINT_C_ATARI_L_S
 * Atari move capturing-race stones to save string with more than or equal 3 stones (capturable).
 * @var UCT_2POINT_C_ATARI_L_S
 * Atari move capturing-race stones to save string with more than or equal 3 stones (capturable).
 * @var UCT_3POINT_C_ATARI_L_L
 * Atari move capturing-race stones to save string with more than or equal 3 stones (capturable).
 * @var UCT_2POINT_C_ATARI_L_L
 * Atari move capturing-race stones to save string with more than or equal 3 stones (capturable).
 * @var UCT_ATARI_MAX
 * Sentinel
 * @~japanese
 * @brief アタリの特徴
 * @var UCT_ATARI_NONE
 * 特徴なし
 * @var UCT_ATARI
 * アタリ
 * @var UCT_3POINT_ATARI_S_S
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_2POINT_ATARI_S_S
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_3POINT_ATARI_S_L
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_2POINT_ATARI_S_L
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_3POINT_ATARI_L_S
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_2POINT_ATARI_L_S
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_3POINT_ATARI_L_L
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_2POINT_ATARI_L_L
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(逃げられる)
 * @var UCT_3POINT_C_ATARI_S_S
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_2POINT_C_ATARI_S_S
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_3POINT_C_ATARI_S_L
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_2POINT_C_ATARI_S_L
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_3POINT_C_ATARI_L_S
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_2POINT_C_ATARI_L_S
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_3POINT_C_ATARI_L_L
 * 呼吸点が3つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_2POINT_C_ATARI_L_L
 * 呼吸点が2つの連に隣接する敵連に対するアタリ(捕獲できる)
 * @var UCT_ATARI_MAX
 * 番兵
 */
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

/**
 * @enum UCT_EXTENSION_FEATURE
 * @~english
 * @brief Extension features.
 * @var UCT_EXTENSION_NONE
 * Neutral feature.
 * @var UCT_3POINT_EXTENSION_DECREASE
 * Extension string which has 3 liberties (result in decreasing the number of liberties).
 * @var UCT_2POINT_EXTENSION_DECREASE
 * Extension string which has 2 liberties (result in decreasing the number of liberties).
 * @var UCT_3POINT_EXTENSION_EVEN
 * Extension string which has 3 liberties (result in unchanging the number of liberties).
 * @var UCT_2POINT_EXTENSION_EVEN
 * Extension string which has 2 liberties (result in unchanging the number of liberties).
 * @var UCT_3POINT_EXTENSION_INCREASE
 * Extension string which has 3 liberties (result in increasing the number of liberties).
 * @var UCT_2POINT_EXTENSION_INCREASE
 * Extension string which has 2 liberties (result in increasing the number of liberties).
 * @var UCT_EXTENSION_MAX
 * Sentinel.
 * @~japanese
 * @brief ダメを詰められた石に隣接して着手する特徴
 * @var UCT_EXTENSION_NONE
 * 特徴なし
 * @var UCT_3POINT_EXTENSION_DECREASE
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
 * @var UCT_2POINT_EXTENSION_DECREASE
 * 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
 * @var UCT_3POINT_EXTENSION_EVEN
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
 * @var UCT_2POINT_EXTENSION_EVEN
 * 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
 * @var UCT_3POINT_EXTENSION_INCREASE
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連があるときのノビ
 * @var UCT_2POINT_EXTENSION_INCREASE
 * 呼吸点が2つの連に隣接する呼吸点が2つの敵連があるときのノビ
 * @var UCT_EXTENSION_MAX
 * 番兵 
 */
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


/**
 * @enum UCT_DAME_FEATURE
 * @~english
 * @brief Dame features.
 * @var UCT_DAME_NONE
 * Neutral features.
 * @var UCT_3POINT_DAME_S_S
 * Place a stone to string's liberty which has 3 liberties.
 * @var UCT_3POINT_DAME_S_L
 * Place a stone to string's liberty which has 3 liberties.
 * @var UCT_3POINT_DAME_L_S
 * Place a stone to string's liberty which has 3 liberties.
 * @var UCT_3POINT_DAME_L_L
 * Place a stone to string's liberty which has 3 liberties.
 * @var UCT_DAME_MAX
 * Sentinel.
 * @~japanese
 * @brief ダメを詰める手の特徴
 * @var UCT_DAME_NONE
 * 特徴なし
 * @var UCT_3POINT_DAME_S_S
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
 * @var UCT_3POINT_DAME_S_L
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
 * @var UCT_3POINT_DAME_L_S
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
 * @var UCT_3POINT_DAME_L_L
 * 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
 * @var UCT_DAME_MAX
 * 番兵
 */
enum UCT_DAME_FEATURE {
  UCT_DAME_NONE,
  UCT_3POINT_DAME_S_S,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_S_L,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_L_S,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_3POINT_DAME_L_L,    // 呼吸点が3つの連に隣接する呼吸点が3つの敵連を詰める手
  UCT_DAME_MAX,
};


/**
 * @enum UCT_CONNECT_FEATURE
 * @~english
 * @brief Connect features.
 * @var UCT_CONNECT_NONE
 * Neutral feature.
 * @var UCT_KO_CONNECTION
 * Resolving ko by connection.
 * @var UCT_CONNECT_MAX
 * Sentinel.
 * @~japanese
 * @brief 他の石と接続する手の特徴
 * @var UCT_CONNECT_NONE
 * 特徴なし
 * @var UCT_KO_CONNECTION
 * 劫の解消
 * @var UCT_CONNECT_MAX
 * 番兵
 */
enum UCT_CONNECT_FEATURE {
  UCT_CONNECT_NONE,
  UCT_KO_CONNECTION,             // 劫の解消
  UCT_CONNECT_MAX,
};

/**
 * @enum UCT_THROW_IN_FEATURE
 * @~english
 * @brief Throw-in features.
 * @var UCT_THROW_IN_NONE
 * Neutral feature.
 * @var UCT_THROW_IN_2
 * Throw in after capturing 2 stones.
 * @var UCT_THROW_IN_MAX
 * Sentinel.
 * @~japanese
 * @brief ホウリコミの特徴
 * @var UCT_THROW_IN_NONE
 * 特徴なし
 * @var UCT_THROW_IN_2
 * 2目の抜き跡へのホウリコミ
 * @var UCT_THROW_IN_MAX
 * 番兵
 */
enum UCT_THROW_IN_FEATURE {
  UCT_THROW_IN_NONE,
  UCT_THROW_IN_2,
  UCT_THROW_IN_MAX,
};

/**
 * @enum PASS_FEATURES
 * @~english
 * @brief Pass features.
 * @var UCT_PASS_AFTER_MOVE
 * Pass after opponent's move.
 * @var UCT_PASS_AFTER_PASS
 * Pass after opponent's pass.
 * @var UCT_PASS_MAX
 * Sentinel.
 * @~japanese
 * @brief パスの特徴
 * @var UCT_PASS_AFTER_MOVE
 * 相手が着手した後のパス
 * @var UCT_PASS_AFTER_PASS
 * 相手がパスした後のパス
 * @var UCT_PASS_MAX
 * 番兵
 */
enum PASS_FEATURES {
  UCT_PASS_AFTER_MOVE,
  UCT_PASS_AFTER_PASS,
  UCT_PASS_MAX,
};


/**
 * @~english
 * @brief Maximum number of move distance.
 * @~japanese
 * @brief 着手距離の上限値
 */
constexpr int MOVE_DISTANCE_MAX = 16;

/**
 * @~english
 * @brief Maxmimum number of coordinate feature.
 * @~japanese
 * @brief 盤上の位置の特徴の上限値
 */
constexpr int POS_ID_MAX = 64;

/**
 * @~english
 * @brief Names of capture features.
 * @~japanese
 * @brief トリの特徴の名前
 */
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

/**
 * @~english
 * @brief Names of save extension features.
 * @~japanese
 * @brief アタリにされた石を助ける手の特徴の名前
 */
const std::string uct_save_extension_name[UCT_SAVE_EXTENSION_MAX] = {
  "",
  "UCT_SAVE_EXTENSION_1  ",
  "UCT_SAVE_EXTENSION_2  ",
  "UCT_SAVE_EXTENSION_3  ",
  "UCT_LADDER_EXTENSION  ",
};

/**
 * @~english
 * @brief Names of atari features.
 * @~japanese
 * @brief アタリの特徴の名前
 */
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

/**
 * @~english
 * @brief Name of extension features.
 * @~japanese
 * @brief ダメを詰められた石を助ける特徴の名前
 */
const std::string uct_extension_name[UCT_EXTENSION_MAX] = {
  "",
  "UCT_3POINT_EXTENSION_DECREASE",
  "UCT_2POINT_EXTENSION_DECREASE",
  "UCT_3POINT_EXTENSION_EVEN    ",
  "UCT_2POINT_EXTENSION_EVEN    ",
  "UCT_3POINT_EXTENSION_INCREASE",
  "UCT_2POINT_EXTENSION_INCREASE",
};

/**
 * @~english
 * @brief Name of dame features.
 * @~japanese
 * @brief ダメを詰める手の特徴の名前
 */
const std::string uct_dame_name[UCT_DAME_MAX] = {
  "",
  "UCT_3POINT_DAME_S_S",
  "UCT_3POINT_DAME_S_L",
  "UCT_3POINT_DAME_L_S",
  "UCT_3POINT_DAME_L_L",
};

/**
 * @~english
 * @brief Name of connect features.
 * @~japanese
 * @brief 他の石に接続する手の特徴
 */
const std::string uct_connect_name[UCT_CONNECT_MAX] = {
  "",
  "UCT_KO_CONNECTION",
};

/**
 * @~english
 * @brief Name of throw-in features.
 * @~japanese
 * @brief ホウリコミの特徴の名前
 */
const std::string uct_throw_in_name[UCT_THROW_IN_MAX] = {
  "",
  "UCT_THROW_IN_2",
};


/**
 * @~english
 * @brief Get tactical feature's index for UCT.
 * @param[in] pos Coordinate on board.
 * @param[in] feature Tactical feature type.
 * @return Tactical feature's index for UCT.
 * @~japanese
 * @brief UCT用の戦術的特徴のインデックスの取得
 * @param[in] pos 座標
 * @param[in] feature 戦術的特徴の種別
 * @return UCT用の戦術的特徴のインデックス
 */
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
