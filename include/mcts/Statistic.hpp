#ifndef _STATISTIC_HPP_
#define _STATISTIC_HPP_

#include <atomic>

struct statistic_t {
  std::atomic<int> colors[3];  // その箇所を領地にした回数

  void clear() {
    for (int i = 0; i < 3; i++) {
      colors[i] = 0;
    }
  }

  statistic_t& operator=( const statistic_t& v ) {
    for (int i = 0; i < 3; i++) {
      colors[i] = v.colors[i].load();
    }
    return *this;
  }
};

#endif
