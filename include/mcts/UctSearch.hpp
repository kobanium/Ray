/**
 * @file include/mcts/UctSearch.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Monte-Carlo tree search with upper confidence bound.
 * @~japanese
 * @brief UCBを利用したモンテカルロ木探索
 */
#ifndef _UCT_SEARCH_HPP_
#define _UCT_SEARCH_HPP_

#include <atomic>
#include <random>

#include "board/GoBoard.hpp"
#include "board/ZobristHash.hpp"
#include "mcts/MCTSNode.hpp"
#include "mcts/SearchManager.hpp"
#include "mcts/Statistic.hpp"


/**
 * @~english
 * @brief Maximum number of worker threads.
 * @~japanese
 * @brief 使用するスレッド数の最大値
 */
constexpr int THREAD_MAX = 64;

/**
 * @~english
 * @brief Update interval for calculating criticality and ownership.
 * @~japanese
 * @brief CriticalityとOwnerを計算する間隔
 */
constexpr int CRITICALITY_INTERVAL = 100;

/**
 * @~english
 * @brief Progressive widening parameter.
 * @~japanese
 * @brief Progressive Wideningの指数パラメータ
 */
constexpr double PROGRESSIVE_WIDENING = 1.8;

/**
 * @~english
 * @brief Search count threshold for expanding a leaf node (9x9).
 * @~japanese
 * @brief ノード展開の閾値 (9路)
 */
constexpr int EXPAND_THRESHOLD_9  = 20;

/**
 * @~english
 * @brief Search count threshold for expanding a leaf node (13x13).
 * @~japanese
 * @brief ノード展開の閾値 (13路)
 */
constexpr int EXPAND_THRESHOLD_13 = 25;

/**
 * @~english
 * @brief Search count threshold for expanding a leaf node (19x19).
 * @~japanese
 * @brief ノード展開の閾値 (19路)
 */
constexpr int EXPAND_THRESHOLD_19 = 40;


/**
 * @struct thread_arg_t
 * @~english
 * @brief Arguments for MCTS worker thread.
 * @~japanese
 * @brief MCTSワーカスレッド用の引数
 */
struct thread_arg_t {
  /**
   * @~english
   * @brief Root position for MCTS.
   * @~japanese
   * @brief 探索開始局面
   */
  game_info_t *game;

  /**
   * @~english
   * @brief Search worker thread ID.
   * @~japanese
   * @brief 探索スレッドID
   */
  int thread_id;

  /**
   * @~english
   * @brief Player's color at root position.
   * @~japanese
   * @brief 探索開始局面の手番
   */
  int color;

  /**
   * @~english
   * @brief Display interval for lz-analyze.
   * @~japanese
   * @brief lz-analyzeコマンドの表示間隔
   */
  int lz_analysis_cs;
};


/**
 * @struct rate_order_t
 * @~english
 * @brief Data for move reordering.
 * @~japanese
 * @brief 着手の並び替えのためのデータ
 */
struct rate_order_t {
  /**
   * @~english
   * @brief UCT node index.
   * @~japanese
   * @brief ノードのインデックス
   */
  int index;

  /**
   * @~english
   * @brief Move rate.
   * @~japanese
   * @brief 着手のレート値
   */
  double rate;
};


// UCTのノード
extern uct_node_t *uct_node;

// 現在のルートのインデックス
extern int current_root;

// 予測読みの有効化フラグ
extern bool pondering_mode;


// 予測読みを止める
void StopPondering( void );

// 予測読みのモードの設定
void SetPonderingMode( const bool flag );

// 使用するスレッド数の指定
void SetThread( const int new_threads );

// パラメータの設定
void SetParameter( void );

// UCT探索の初期設定
void InitializeUctSearch( void ); 

// 探索設定の初期化
void InitializeSearchSetting( void );

// UCT探索による着手生成
int UctSearchGenmove( game_info_t *game, int color, int lz_analysis_cs );

// 予測よみ
void UctSearchPondering( game_info_t *game, int color, int lz_analysis_cs );

// UCT探索による着手生成
int UctAnalyze( game_info_t *game, int color );

// 領地になる確率をdestにコピーする
void OwnerCopy( int *dest );

// Criticaltyをdestに
void CopyCriticality( double *dest );

void CopyStatistic( statistic_t *dest );

// UCT探索による着手生成(Clean Upモード)
int UctSearchGenmoveCleanUp( game_info_t *game, int color );

// 探索の再利用の設定
void SetReuseSubtree( bool flag );

// 指定したインデックスのノードを取得
uct_node_t& GetNode( const int index );

// ルートノードを取得
uct_node_t& GetRootNode( void );

#endif
