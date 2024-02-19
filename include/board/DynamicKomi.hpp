/**
 * @file include/board/DynamicKomi.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Dynamic komi management.
 * @~japanese
 * @brief ダイナミックコミの管理
 */
#ifndef _DYNAMICKOMI_HPP_
#define _DYNAMICKOMI_HPP_

#include "board/GoBoard.hpp"
#include "mcts/UctSearch.hpp"

/**
 * @~english
 * @brief Win rate threshold to relax komi value
 * @~japanese
 * @brief コミを緩める勝率の閾値
 */
constexpr double RED = 0.35;

/**
 * @~english
 * @brief Win rate threshold to stress komi value
 * @~japanese
 * @brief コミを厳しくする勝率の閾値
 */
constexpr double GREEN = 0.75;

/**
 * @~english
 * @brief Move count threshold to turn off LinearHandicap mode.
 * @~japanese
 * @brief LinearHandicapでコミの値を収束させる手数
 */
constexpr int LINEAR_THRESHOLD = 200;

/**
 * @~english
 * @brief Komi weight per handicap stone.
 * @~japanese
 * @brief 置き石1子あたりの重み
 */
constexpr int HANDICAP_WEIGHT = 8;


/**
 * @enum DYNAMIC_KOMI_MODE
 * @~english
 * @brief Komi adjustment mode.
 * @var DK_OFF
 * Stable komi mode.
 * @var DK_LINEAR
 * Mode to adjust komi based on the number of handicaps and moves.
 * @var DK_VALUE
 * Mode to adjust komi based on win rate.
 * @~japanese
 * @brief コミ変更モード
 * @var DK_OFF
 * コミを変更しないモード
 * @var DK_LINEAR
 * 置き石の数と着手数からコミを算出するモード
 * @var DK_VALUE
 * 勝率に基づいてコミを調整するモード
 */
enum DYNAMIC_KOMI_MODE {
  DK_OFF,     // Dynamic Komiなし
  DK_LINEAR,  // Linear Handicap
  DK_VALUE,   // Value Situational
};


//  置き石の個数の設定
void SetHandicapNum( const int num );

//  置き石の個数の設定(テスト対局用)
void SetConstHandicapNum( const int num );

//  Dynamic Komi
void DynamicKomi( const game_info_t *game, const uct_node_t *root, const int color );

#endif
