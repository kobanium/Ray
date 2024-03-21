/**
 * @file include/util/Command.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Implementation of command line options.
 * @~japanese
 * @brief CLIの実装
 */
#ifndef _COMMAND_HPP_
#define _COMMAND_HPP_

/**
 * @enum COMMAND
 * @~english
 * @brief Definitions for command line options.
 * @var COMMAND_PLAYOUT
 * Specifying the number of playouts per move.
 * @var COMMAND_TIME
 * Specifying total remaining time (second).
 * @var COMMAND_SIZE
 * Specifying go board size.
 * @var COMMAND_CONST_TIME
 * Specifying searching time per move (second).
 * @var COMMAND_THREAD
 * Specifying the number of search thread.
 * @var COMMAND_KOMI
 * Specifying komi value.
 * @var COMMAND_HANDICAP
 * Specifying the number of handicap stones.
 * @var COMMAND_REUSE_SUBTREE
 * Activating reusing sub tree.
 * @var COMMAND_PONDERING
 * Activating pondering.
 * @var COMMAND_TREE_SIZE
 * Specifying the number of tree nodes.
 * @var COMMAND_NO_DEBUG
 * Suppressing debug messages.
 * @var COMMAND_SUPERKO
 * Activating positional super-ko rule.
 * @var COMMAND_RESIGN_THRESHOLD
 * Specifying winning ratio of resignation.
 * @var COMMAND_CGOS_MODE
 * Activating all capturing dead stones mode.
 * @var COMMAND_MAX
 * Sentinel.
 * @~japanese
 * @brief コマンドラインオプションの種別
 * @var COMMAND_PLAYOUT
 * 1手あたりの探索回数の指定
 * @var COMMAND_TIME
 * 持ち時間の指定
 * @var COMMAND_SIZE
 * 碁盤のサイズの指定
 * @var COMMAND_CONST_TIME
 * 1手あたりの思考時間の指定
 * @var COMMAND_THREAD
 * 探索スレッド数の指定
 * @var COMMAND_KOMI
 * コミの指定
 * @var COMMAND_HANDICAP
 * 置き石の個数の指定 (テスト対局用)
 * @var COMMAND_REUSE_SUBTREE
 * 探索結果の再利用の有効化
 * @var COMMAND_PONDERING
 * 予測読みの有効化
 * @var COMMAND_TREE_SIZE
 * 探索木のノード数の指定
 * @var COMMAND_NO_DEBUG
 * デバッグメッセージの出力抑制の有効化
 * @var COMMAND_SUPERKO
 * 超劫の有効化
 * @var COMMAND_RESIGN_THRESHOLD
 * 投了する勝率の閾値の指定
 * @var COMMAND_CGOS_MODE
 * 全ての石を打ち上げるモードの有効化
 * @var COMMAND_MAX
 * 番兵
 */
enum COMMAND {
  COMMAND_PLAYOUT,
  COMMAND_TIME,
  COMMAND_SIZE,
  COMMAND_CONST_TIME,
  COMMAND_THREAD,
  COMMAND_KOMI,
  COMMAND_HANDICAP,
  COMMAND_REUSE_SUBTREE,
  COMMAND_PONDERING,
  COMMAND_TREE_SIZE,
  COMMAND_NO_DEBUG,
  COMMAND_SUPERKO,
  COMMAND_RESIGN_THRESHOLD,
  COMMAND_CGOS_MODE,
  COMMAND_MAX,
};


// コマンドライン引数のコマンドの解析
void AnalyzeCommand( int argc, char **argv );

#endif
