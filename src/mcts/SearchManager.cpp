/**
 * @file src/mcts/SearchManager.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Time management for search.
 * @~japanese
 * @brief 探索時間の管理
 */
#include <iostream>

#include "board/GoBoard.hpp"
#include "mcts/SearchManager.hpp"
#include "util/Utility.hpp"


/**
 * @~english
 * @brief Search setting.
 * @~japanese
 * @brief 探索の設定
 */
static SearchTimeStrategy search_setting = SearchTimeStrategy::ConstantTimeMode;

/**
 * @~english
 * @brief Thinking time per move.
 * @~japanese
 * @brief 1手あたりの思考時間
 */
static double const_thinking_time = CONST_TIME;

/**
 * @~english
 * @brief The number of playouts per move.
 * @~japanese
 * @brief 1手あたりのプレイアウト数
 */
static double playout = CONST_PLAYOUT;

/**
 * @~english
 * @brief Search time limit.
 * @~japanese
 * @brief 探索時間の上限
 */
static double time_limit = 0.0;

/**
 * @~english
 * @brief Search time extension flag.
 * @~japanese
 * @brief 試行時間を延長するかどうかのフラグ
 */
static bool extend_time = false;

/**
 * @~english
 * @brief Remaining time for time control setting.
 * @~japanese
 * @brief 持ち時間
 */
static double remaining_time[S_MAX];

/**
 * @~english
 * @brief Default remaining time.
 * @~japanese
 * @brief デフォルトの持ち時間
 */
static double default_remaining_time = ALL_THINKING_TIME;

/**
 * @~english
 * @brief Constant value for search time.
 * @~japanese
 * @brief 探索時間に関する定数
 */
static int time_c = TIME_C_19;

/**
 * @~english
 * @brief Constant value for search time.
 * @~japanese
 * @brief 探索時間に関する定数
 */
static int time_maxply = TIME_MAXPLY_19;

/**
 * @~english
 * @brief time_c change flag.
 * @~japanese
 * @brief time_cの変更有無フラグ
 */
static bool time_c_changed = false;

/**
 * @~english
 * @brief time_maxply change flag.
 * @~japanese
 * @brief time_maxplyの変更有無フラグ
 */
static bool time_maxply_changed = false;

/**
 * @~english
 * @brief Search interruption flag.
 * @~japanese
 * @brief 探索の中断フラグ
 */
static bool interruption_flag = true;

/**
 * @~english
 * @brief Timer.
 * @~japanese
 * @brief タイマ
 */
static ray_clock::time_point start_time;

/**
 * @~english
 * @brief Playout information.
 * @~japanese
 * @brief プレイアウト情報
 */
static po_info_t po_info;


/**
 * @~english
 * @brief Set search strategy.
 * @param[in] new_setting New search time settings.
 * @~japanese
 * @brief 探索時間のモードの設定
 * @param[in] new_setting 探索時間の設定
 */
void
SetSearchSetting( const SearchTimeStrategy &new_setting )
{
  search_setting = new_setting;
}


/**
 * @~english
 * @brief Set search interruption flag.
 * @param[in] flag Interruption flag.
 * @~japanese
 * @brief 探索中断有効化フラグの設定
 * @param[in] flag 探索中断有効化フラグ
 */
void
SetInterruptionFlag( const bool flag )
{
  interruption_flag = flag;
}


/**
 * @~english
 * @brief Set the number of playouts per move.
 * @param[in] po The number of playouts per move.
 * @~japanese
 * @brief 1手あたりのプレイアウト数の設定
 * @param[in] po 1手あたりのプレイアウト数の設定
 */
void
SetPlayout( const int po )
{
  playout = po;
}


/**
 * @~english
 * @brief Get search time limit.
 * @return Search time limit.
 * @~japanese
 * @brief 探索制限時間の取得
 * @return 探索制限時間
 */
double
GetTimeLimit( void )
{
  return time_limit;
}


/**
 * @~english
 * @brief Check search time extension mode.
 * @return Search time extension mode is enabled or not.
 * @~japanese
 * @brief 探索時間延長モードの有効化判定
 * @return 探索時間延長モードの有効化判定結果
 */
bool
IsTimeExtensionMode( void )
{
  return extend_time;
}


/**
 * @~english
 * @brief Check search setting is constant playout mode or not.
 * @return Search setting is constant playout mode or not.
 * @~japanese
 * @brief 1手あたりの固定探索回数の設定有効化判定
 * @return 1手あたりの固定探索回数の設定有効化判定結果
 */
bool
IsConstPlayoutMode( void )
{
  return search_setting == SearchTimeStrategy::ConstantPlayoutMode;
}


/**
 * @~english
 * @brief Exted search time.
 * @param[in] multiplier Time limit multiplier.
 * @~japanese
 * @brief 探索時間の延長
 * @param[in] multiplier 探索時間延長の倍数
 */
void
ExtendSearchTime( const double multiplier )
{
  po_info.halt = static_cast<int>(multiplier * po_info.halt);
  time_limit *= multiplier;
}


/**
 * @~english
 * @brief Set default remaining time.
 * @param[in] time Defalut remaining time.
 * @~japanese
 * @brief 持ち時間の設定
 * @param[in] time 持ち時間
 */
void
SetTime( const double time )
{
  default_remaining_time = time;
}


/**
 * @~english
 * @brief Set constant thinking time per move.
 * @param[in] time Constant thinking time.
 * @~japanese
 * @brief 1手あたりの思考時間の設定
 * @param[in] time 1手あたりの思考時間
 */
void
SetConstThinkingTime( const double time )
{
  const_thinking_time = time;
}


/**
 * @~english
 * @brief Set current remaining time.
 * @param[in] color Player's color.
 * @param[in] time Current remaining time.
 * @~japanese
 * @brief 残り時間の設定
 * @param[in] color 手番の色
 * @param[in] time 残り時間
 */
void
SetCurrentRemainingTime( const int color, const double time )
{
  remaining_time[color] = time;
}


/**
 * @~english
 * @brief Get current remaining time.
 * @param[in] color Player's color.
 * @return Current remaining time.
 * @~japanese
 * @brief 残り時間の取得
 * @param[in] color 手番の色
 * @return 現在の残り時間
 */
double
GetRemainingTime( const int color )
{
  return remaining_time[color];
}


/**
 * @~english
 * @brief Initialize Rremaining time and search setting.
 * @~japanese
 * @brief 持ち時間の初期化
 */
void
InitializeTimeSetting( void )
{
  for (int i = 0; i < S_MAX; i++) {
    remaining_time[i] = default_remaining_time;
  }

  switch (search_setting) {
    case SearchTimeStrategy::ConstantPlayoutMode:
      time_limit = 10000.0;
      po_info.num = playout;
      extend_time = false;
      break;
    case SearchTimeStrategy::ConstantTimeMode:
      time_limit = const_thinking_time;
      po_info.num = 10000000;
      extend_time = false;
      break;
    case SearchTimeStrategy::TimeControlMode:
    case SearchTimeStrategy::TimeControlWithByoYomiMode:
      time_limit = CalculateNextTimeLimit(S_EMPTY, -1);
      po_info.num = static_cast<int>(PLAYOUT_SPEED * time_limit);
      extend_time = true;
      break;
  }
}


/**
 * @~english
 * @brief Set time settings.
 * @param[in] main_time Remaining time.
 * @param[in] byoyomi Byo-yomi
 * @param[in] stones The number of stones to add byo-yomi to remaining time.
 * @~japanese
 * @brief 現在の持ち時間の設定
 * @param[in] main_time 持ち時間
 * @param[in] byoyomi 秒読み
 * @param[in] stones 秒読みを加算する手数
 */
void
SetTimeSettings( const int main_time, const int byoyomi, const int stones )
{
  if (search_setting == SearchTimeStrategy::ConstantPlayoutMode ||
      search_setting == SearchTimeStrategy::ConstantTimeMode ) {
    return ;
  }

  if (main_time == 0) {
    const_thinking_time = static_cast<double>(byoyomi) * 0.85;
    search_setting = SearchTimeStrategy::ConstantTimeMode;
    std::cerr << "Const Thinking Time Mode" << std::endl;
  } else {
    if (byoyomi == 0) {
      default_remaining_time = main_time;
      search_setting = SearchTimeStrategy::TimeControlMode;
      std::cerr << "Time Setting Mode" << std::endl;
    } else {
      default_remaining_time = main_time;
      const_thinking_time = static_cast<double>(byoyomi) / stones;
      search_setting = SearchTimeStrategy::TimeControlWithByoYomiMode;
      std::cerr << "Time Setting Mode (byoyomi)" << std::endl;
    }
  }
}


/**
 * @~english
 * @brief Set the number of playouts for a current move.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[in] best_wp Winning rate of a selected move.
 * @param[in] finish_time Time consumption (seconds).
 * @param[in] threads The number of search worker threads.
 * @~japanese
 * @brief 現在の探索のプレイアウト回数の設定
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 * @param[in] best_wp 選んだ手の勝率
 * @param[in] finish_time 消費した時間
 * @param[in] threads 探索ワーカスレッド数
 */
void
CalculateNextPlayouts( const game_info_t *game, const int color, const double best_wp, const double finish_time, const int threads )
{
  double po_per_sec;

  if (finish_time > 0.1) {
    po_per_sec = po_info.count / finish_time;
  } else {
    po_per_sec = PLAYOUT_SPEED * threads;
  }

  if (search_setting == SearchTimeStrategy::ConstantTimeMode) {
    if (best_wp > 0.95) {
      po_info.num = static_cast<int>(po_info.count / finish_time * const_thinking_time * 0.25);
    } else if (best_wp > 0.90) {
      po_info.num = static_cast<int>(po_info.count / finish_time * const_thinking_time * 0.50);
    } else {
      po_info.num = static_cast<int>(po_info.count / finish_time * const_thinking_time);

    }                     
  } else if (search_setting == SearchTimeStrategy::TimeControlMode ||
             search_setting == SearchTimeStrategy::TimeControlWithByoYomiMode) {
    ConsumeTime(color, finish_time);
    time_limit = CalculateNextTimeLimit(color, game->moves);
    if (search_setting == SearchTimeStrategy::TimeControlWithByoYomiMode &&
        time_limit < const_thinking_time) {
      time_limit = const_thinking_time * 0.85;
    }
    if (best_wp > 0.95) {
      time_limit *= 0.25;
    } else if (best_wp > 0.90) {
      time_limit *= 0.5;
    }
    po_info.num = static_cast<int>(po_per_sec * time_limit);
  }
}


/**
 * @~english
 * @brief Set constant values for time constrol setting.
 * @~japanese
 * @brief 時間管理の定数の設定
 */
void
SetTimeManagementParameter( void )
{
  if (!time_c_changed) {
    if (pure_board_size < 11) {
      time_c = TIME_C_9;
    } else if (pure_board_size < 16) {
      time_c = TIME_C_13;
    } else {
      time_c = TIME_C_19;
    }
  }

  if (!time_maxply_changed) {
    if (pure_board_size < 11) {
      time_maxply = TIME_MAXPLY_9;
    } else if (pure_board_size < 16) {
      time_maxply = TIME_MAXPLY_13;
    } else {
      time_maxply = TIME_MAXPLY_19;
    }
  }
}


/**
 * @~english
 * @brief Calculate remaining time.
 * @param[in] color Player's color.
 * @param[in] finish_time Search time consumption.
 * @~japanese
 * @brief 消費時間の減算
 * @param[in] color 手番の色
 * @param[in] finish_time 探索にかかった時間
 */
void ConsumeTime( const int color, const double finish_time )
{
  remaining_time[color] -= finish_time;
}


/**
 * @~english
 * @brief Calculate time limit.
 * @param[in] color Player's color.
 * @param[in] moves The number of moves.
 * @return Time limit for next search.
 * @~japanese
 * @brief 探索時間の上限の算出
 * @param[in] color 手番の色
 * @param[in] moves 現在の手数
 * @return 次の探索時間制限
 */
double
CalculateNextTimeLimit( const int color, const int moves )
{
  return remaining_time[color] / (time_c + ((time_maxply - (moves + 1) > 0) ? time_maxply - (moves + 1) : 0));
}


/**
 * @~english
 * @brief Set parameter for time control.
 * @param[in] new_time_c Parameter for time control.
 * @~japanese
 * @brief 探索時間配分用パラメータの設定
 * @param[in] new_time_c 探索時間配分用パラメータ
 */
void
ResetTimeC( const int new_time_c )
{
  time_c = new_time_c;
  time_c_changed = true;
}


/**
 * @~english
 * @brief Set parameter for time control.
 * @param[in] new_time_maxply Parameter for time control.
 * @~japanese
 * @brief 探索時間配分用パラメータの設定
 * @param[in] new_time_maxply 探索時間配分用パラメータ
 */
void
ResetTimeMaxply( const int new_time_maxply )
{
  time_maxply = new_time_maxply;
  time_maxply_changed = true;
}


/**
 * @~english
 * @brief Calculate search speed for time management.
 * @param[in] consume_time Time consumption of current move
 * @param[in] threads The number of search threads.
 * @~japanese
 * @brief 時間管理のための探索速度の算出
 * @param[in] consume_time 現在の探索にかかった時間
 * @param[in] threads 探索ワーカスレッド数
 */
double
CalculatePlayoutSpeed( const double consume_time, const int threads )
{
  if (consume_time < 0.001) {
    return PLAYOUT_SPEED * threads;
  } else {
    return po_info.count / consume_time;
  }
}


/**
 * @~english
 * @brief Start timer.
 * @~japanese
 * @brief 時間計測の開始
 */
void
StartTimer( void )
{
  start_time = ray_clock::now();
}


/**
 * @~english
 * @brief Calculate search time eplasion.
 * @~japanese
 * @brief 消費時間の算出
 */
double
CalculateElapsedTime( void )
{
  return GetSpendTime(start_time);
}


/**
 * @~english
 * @brief Check time over.
 * @return Time is over or not.
 * @~japanese
 * @brief 探索時間制限の確認
 * @return 探索時間制限超過フラグ
 */
bool
IsTimeOver( void )
{
  return CalculateElapsedTime() > GetTimeLimit();
}


/**
 * @~english
 * @brief Check search count limit.
 * @return Search count is over threshold or not.
 * @~japanese
 * @brief 探索回数制限の確認
 * @return 探索回数制限超過フラグ
 */
bool
IsSearchContinue( void )
{
  return po_info.count < po_info.halt;
}


/**
 * @~english
 * @brief Clear search count.
 * @~japanese
 * @brief 探索回数の初期化
 */
void
ResetPoCount( void )
{
  po_info.count = 0;
}


/**
 * @~english
 * @brief Get search count.
 * @return Search count.
 * @~japanese
 * @brief 探索回数の取得
 * @return 探索回数
 */
int
GetPoCount( void )
{
  return po_info.count;
}


/**
 * @~english
 * @brief Get search count threshold.
 * @return Search count threshold.
 * @~japanese
 * @brief 探索回数の閾値の取得
 * @return 探索回数の閾値
 */
int
GetPoHalt( void )
{
  return po_info.halt;
}


/**
 * @~english
 * @brief Get the number of playouts for next move.
 * @brief The number of playouts for next move.
 * @~japanese
 * @brief 次の探索回数の取得
 * @return 次の探索回数
 */
int
GetPoNum( void )
{
  return po_info.num;
}


/**
 * @~english
 * @brief Get remaining search count.
 * @return Remaining search count.
 * @~japanese
 * @brief 残りの探索回数の取得
 * @return 探索回数の取得
 */
int
GetRestPoCount( void )
{
  return po_info.halt - po_info.count;
}


/**
 * @~english
 * @brief Set search count threshold.
 * @param[in] halt Search count threshold.
 * @~japanese
 * @brief 探索回数閾値の設定
 * @param[in] halt 探索回数の設定
 */
void
SetPoHalt( const int halt )
{
  po_info.halt = halt;
}


/**
 * @~english
 * @brief Increment search count.
 * @~japanese
 * @brief 探索回数を1回増やす
 */
void
IncrementPoCount( void )
{
  atomic_fetch_add(&po_info.count , 1);
}


/**
 * @~english
 * @brief Check search interruption condition.
 * @param[in] root Root node.
 * @return Search must be interruped or not.
 * @~japanese
 * @brief 探索を中断するか確認
 * @param[in] root ルートの情報
 * @return 探索中断フラグ
 */
bool
CheckInterruption( const uct_node_t &root )
{
  int max = 0, second = 0;
  const int child_num = root.child_num;
  const int rest = GetRestPoCount();
  const child_node_t *child = root.child;

  if (!interruption_flag) {
    return false;
  }

  if (!IsConstPlayoutMode() &&
      CalculateElapsedTime() * 10.0 < GetTimeLimit()) {
    return false;
  }

  for (int i = 0; i < child_num; i++) {
    if (child[i].move_count > max) {
      second = max;
      max = child[i].move_count;
    } else if (child[i].move_count > second) {
      second = child[i].move_count;
    }
  }

  if (max - second > rest) {
    return true;
  } else {
    return false;
  }
}


/**
 * @~english
 * @brief Check search time extension condition.
 * @param[in] root Root node.
 * @param[in] moves The number of moves.
 * @return Search time must be extended or not.
 * @~japanese
 * @brief 探索時間延長の確認
 * @param[in] root ルートの情報
 * @param[in] moves 現在の手数
 * @return 探索時間延長フラグ
 */
bool
ExtendTime( const uct_node_t &root, const int moves )
{
  const int child_num = root.child_num;
  const child_node_t *child = root.child;
  int max = 0, second = 0;

  if (!IsTimeExtensionMode() ||
      moves < pure_board_size * 3 - 17) {
    return false;
  }
  
  const double root_wp = static_cast<double>(root.win) / root.move_count;

  if (root_wp > 0.80) {
    return false;
  }

  for (int i = 0; i < child_num; i++) {
    if (child[i].move_count > max) {
      second = max;
      max = child[i].move_count;
    } else if (child[i].move_count > second) {
      second = child[i].move_count;
    }
  }
  if (max < second * 1.2) {
    return true;
  } else {
    return false;
  }
}
