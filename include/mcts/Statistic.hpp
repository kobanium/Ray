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
