/**
 * @file include/board/Constant.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Constant variable definition for game position.
 * @~japanese
 * @brief 局面を表現するために使う定数の定義
 */
#ifndef _CONSTANT_HPP_
#define _CONSTANT_HPP_

/**
 * @~english
 * @brief The size of board. Please change this value if you need optimize search speed and date size.
 * @~japanese
 * @brief 碁盤の大きさ. 基本的に19で良いが, データサイズと実行速度を最適化するときはサイズを変更する.
 */
constexpr int PURE_BOARD_SIZE = 19;

/**
 * @~english
 * @brief The width of outside a board. It must be 5.
 * @~japanese
 * @brief 盤外の幅. 値を変更してはならない
 */
constexpr int OB_SIZE = 5;

/**
 * @~english
 * @brief The size of board (including outside).
 * @~japanese
 * @brief 盤外を含めた碁盤の幅
 */
constexpr int BOARD_SIZE = PURE_BOARD_SIZE + 2 * OB_SIZE;

/**
 * @~english
 * @brief The number of intersections on a go board.
 * @~japanese
 * @brief 盤上の交点の数
 */
constexpr int PURE_BOARD_MAX = PURE_BOARD_SIZE * PURE_BOARD_SIZE;

/**
 * @~english
 * @brief The number of intersections on a board with out of board.
 * @~japanese
 * @brief 盤外を含めた時の碁盤の大きさ
 */
constexpr int BOARD_MAX = BOARD_SIZE * BOARD_SIZE;

/**
 * @~english
 * @brief Maximum number of strings.
 * @~japanese
 * @brief 連の最大数
 */
constexpr int MAX_STRING = PURE_BOARD_MAX * 4 / 5;

/**
 * @~english
 * @brief Maximum number of neighborhood strings.
 * @~japanese
 * @brief 隣接する相手の連の最大数
 */
constexpr int MAX_NEIGHBOR = MAX_STRING;

/**
 * @~english
 * @brief First coordinate of a board
 * @~japanese
 * @brief 碁盤の始点
 */
constexpr int BOARD_START = OB_SIZE;

/**
 * @~english
 * @brief Last coordinate of a board.
 * @~japanese
 * @brief 盤の終点
 */
constexpr int BOARD_END = PURE_BOARD_SIZE + OB_SIZE - 1;

/**
 * @~english
 * @brief Maximum number of a string's liberties.
 * @~japanese
 * @brief 1つの連が持つ呼吸点の最大数
 */
constexpr int STRING_LIB_MAX = BOARD_SIZE * (PURE_BOARD_SIZE + OB_SIZE);

/**
 * @~english
 * @brief Maximum nuber of a string's stone.
 * @~japanese
 * @brief 連が持ちうる座標の最大数
 */
constexpr int STRING_POS_MAX = BOARD_SIZE * (PURE_BOARD_SIZE + OB_SIZE);

/**
 * @~english
 * @brief Maxmum nuber of captured stones.
 * @~japanese
 * @brief 打ち上げる石の最大値
 */
constexpr int CAPTURE_MAX = BOARD_SIZE * (PURE_BOARD_SIZE + OB_SIZE);

/**
 * @~english
 * @brief Last index of captured stones record.
 * @~japanese
 * @brief 打ち上げた石の座標の終点を表す値
 */
constexpr int CAPTURE_END = CAPTURE_MAX - 1;

/**
 * @~english
 * @brief Value of a string's endpoint.
 * @~japanese
 * @brief 連の終端を表す値
 */
constexpr int STRING_END = STRING_POS_MAX - 1;

/**
 * @~english
 * @brief Value of a neighborhood string's endpoint.
 * @~japanese
 * @brief 隣接する敵の連の終端を表す値
 */
constexpr int NEIGHBOR_END = MAX_NEIGHBOR - 1;

/**
 * @~english
 * @brief Value of liberty's endpoint.
 * @~japanese
 * @brief 呼吸点の終端を表す値
 */
constexpr int LIBERTY_END = STRING_LIB_MAX - 1;

/**
 * @~english
 * @brief Maximum number of move history records.
 * @~japanese
 * @brief 記録する着手履歴の最大数
 */
constexpr int MAX_RECORDS = PURE_BOARD_MAX * 3;

/**
 * @~english
 * @brief Maximum number of move counts.
 * @~japanese
 * @brief 着手数の最大値
 */
constexpr int MAX_MOVES = MAX_RECORDS - 1;

/**
 * @~english
 * @brief Value of pass.
 * @~japanese
 * @brief パスを表す値
 */
constexpr int PASS = 0;

/**
 * @~english
 * @brief Value of resignation
 * @~japanese
 * @brief 投了を表す値
 */
constexpr int RESIGN = -1;

/**
 * @~english
 * @brief Default value of komi.
 * @~japanese
 * @brief デフォルトのコミの値
 */
constexpr double KOMI = 6.5;

#endif
