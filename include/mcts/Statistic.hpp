/**
 * @file include/mcts/Statistic.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Statistic information of Monte-Carlo simulation.
 * @~japanese
 * @brief モンテカルロ・シミュレーションの統計情報
 */
#ifndef _STATISTIC_HPP_
#define _STATISTIC_HPP_

#include <atomic>


/**
 * @enum StatisticInformation
 * @~english
 * @brief Statistic information data index assignment.
 * @var Empty
 * The number of times occupied by empty at the end of the game.
 * @var Black
 * The number of times occupied by black player at the end of the game.
 * @var White
 * The number of times occupied by white player at the end of the game.
 * @var Win
 * The number of times occupied by winner at the end of the game.
 * @var Max
 * Sentinel.
 * @~japanese
 * @brief 統計情報のインデックスの割当
 * @var Empty
 * 終局時に空点だった回数
 * @var Black
 * 終局時に黒が占有していた回数
 * @var White
 * 終局時に白が占有していた回数
 * @var Win
 * 勝った手番が占有していた回数
 * @var Max
 * 番兵
 */
enum class StatisticInformation : int {
  Empty = 0,
  Black,
  White,
  Win,
  Max,
};


/**
 * @struct statistic_t
 * @~english
 * @brief Statistic information
 * @~japanese
 * @brief 統計情報
 */
struct statistic_t {
  /**
   * @~english
   * @brief Territory countter.
   * @~japanese
   * @brief 領地にした回数
   */
  std::atomic<int> colors[static_cast<int>(StatisticInformation::Max)];

  /**
   * @~english
   * @brief Clear territory counter.
   * @~japanese
   * @brief 統計情報のクリア
   */
  void
  clear( void )
  {
    for (int i = 0; i < static_cast<int>(StatisticInformation::Max); i++) {
      colors[i] = 0;
    }
  }

  /**
   * @~english
   * @brief Assignment operator.
   * @param[in] v Right hand value.
   * @return Value after assignment.
   * @~japanese
   * @brief 代入オペレータ
   * @param[in] v
   * @return 代入した値
   */
  statistic_t&
  operator=( const statistic_t& v )
  {
    for (int i = 0; i < static_cast<int>(StatisticInformation::Max); i++) {
      colors[i] = v.colors[i].load();
    }
    return *this;
  }
};

#endif
