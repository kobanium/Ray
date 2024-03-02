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
  std::atomic<int> colors[3];

  /**
   * @~english
   * @brief Clear territory counter.
   * @~japanese
   * @brief 統計情報のクリア
   */
  void
  clear()
  {
    for (int i = 0; i < 3; i++) {
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
    for (int i = 0; i < 3; i++) {
      colors[i] = v.colors[i].load();
    }
    return *this;
  }
};

#endif
