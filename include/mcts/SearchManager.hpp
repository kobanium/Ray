#ifndef _SEARCH_MANAGER_HPP_
#define _SEARCH_MANAGER_HPP_

#include <atomic>

#include "board/GoBoard.hpp"
#include "mcts/MCTSNode.hpp"
#include "util/Utility.hpp"


constexpr double ALL_THINKING_TIME = 90.0;  // 持ち時間(デフォルト)
constexpr int CONST_PLAYOUT = 10000;        // 1手あたりのプレイアウト回数(デフォルト)
constexpr double CONST_TIME = 10.0;         // 1手あたりの思考時間(デフォルト)
constexpr int PLAYOUT_SPEED = 1000;         // 初期盤面におけるプレイアウト速度

// 思考時間の割り振り
constexpr int TIME_C_9 = 20;
constexpr int TIME_MAXPLY_9 = 0;
constexpr int TIME_C_13 = 30;
constexpr int TIME_MAXPLY_13 = 30;
constexpr int TIME_C_19 = 60;
constexpr int TIME_MAXPLY_19 = 80;


enum class SearchTimeStrategy {
  ConstantPlayoutMode,
  ConstantTimeMode,
  TimeControlMode,
  TimeControlWithByoYomiMode,
};  


struct po_info_t {
  int num;
  int halt;
  std::atomic<int> count;
};


// 消費時間の算出
double CalculateElapsedTime( void );

// 次のプレイアウト回数の設定
void CalculateNextPlayouts( const game_info_t *game, const int color, const double best_wp, const double finish_time, const int threads );

// 次の思考時間の上限値を算出
double CalculateNextTimeLimit( const int color, const int moves );

// 1秒あたりのプレイアウト回数を算出
double CalculatePlayoutSpeed( double consume_time, int threads );

// 消費した時間を減算
void ConsumeTime( const int color, const double finish_time );

// 思考時間をmultiplier倍に延長
void ExtendSearchTime( const double multiplier );

// 現在の探索回数の取得
int GetPoCount( void );

// 今回のプレイアウト回数の上限値の取得
int GetPoHalt( void );

// 次回のプレイアウト回数の上限値の取得
int GetPoNum( void );

// 残り時間の取得
double GetRemainingTime( const int color );

// 残りの探索回数の取得
int GetRestPoCount( void );

// 今回の思考時間の取得
double GetTimeLimit( void );

// 探索回数を1回増やす
void IncrementPoCount( void );

// 探索設定の初期化
void InitializeTimeSetting( void );

// 1手あたりのプレイアウト数固定のモードの判定
bool IsConstPlayoutMode( void );

// 探索の継続判定
bool IsSearchContinue( void );

// 思考時間を延長するモードの判定
bool IsTimeExtensionMode( void );

// 探索時間の超過判定
bool IsTimeOver( void );

// 探索回数をゼロクリア
void ResetPoCount( void );

// 既に実行している探索回数を設定
void SetPoCount( const int count );

// 1手あたりの思考時間の設定
void SetConstThinkingTime( const double time );

// サーバから送られてきた持ち時間の設定
void SetCurrentRemainingTime( const int color, const double time );

// 1手あたりのプレイアウト回数の設定
void SetPlayout( const int po );

// 探索回数の上限値の設定
void SetPoHalt( const int halt );

// 探索のモードの指定
void SetSearchSetting( const SearchTimeStrategy &new_setting );

// time_settingsコマンドによる設定
void SetTimeSettings( const int main_time, const int byoyomi, const int stones );

// 持ち時間の設定
void SetTime( const double time );

// 時間管理用のパラメータの設定
void SetTimeManagementParameter( void );

// 時間測定の開始
void StartTimer( void );

// time_cの再設定
void ResetTimeC( const int new_time_c );

// time_maxplyの設定
void ResetTimeMaxply( const int new_time_maxply );

bool CheckInterruption( const uct_node_t &root );

// 探索打ち切りの確認
void SetInterruptionFlag( const bool flag );

bool ExtendTime( const uct_node_t &root, const int moves );


#endif
