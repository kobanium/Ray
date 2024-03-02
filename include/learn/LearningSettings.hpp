/**
 * @file include/learn/LearningSettings.hpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Settings for learning.
 * @~japanese
 * @brief 学習用の設定
 */
#ifndef _LEARNING_SETTINGS_HPP_
#define _LEARNING_SETTINGS_HPP_

/**
 * @~english
 * @brief The number of training worker threads.
 * @~japanese
 * @brief 学習ワーカのスレッド数
 */
constexpr int TRAIN_THREAD_NUM = 8;

/**
 * @~english
 * @brief Maximum number of large patterns.
 * @~japanese
 * @brief 学習対象のパターンの最大数
 */
constexpr char TRAIN_KIFU_PATH[] = "/home/user/sgf";

/**
 * @~english
 * @brief First SGF file index for learning.
 * @~japanese
 * @brief 学習する棋譜の最初のインデックス
 */
constexpr int TRAIN_KIFU_START_INDEX = 1;

/**
 * @~english
 * @brief Last SGF file index for learning.
 * @~japanese
 * @brief 学習する棋譜の最後のインデックス
 */
constexpr int TRAIN_KIFU_LAST_INDEX = 30000;

/**
 * @~english
 * @brief Directory path to SGF files for accuracy calculation.
 * @~japanese
 * @brief 正解率を測定する際に使用する棋譜を格納したディレクトリへのパス
 */
constexpr char TEST_KIFU_PATH[] = "/home/user/sgf";

/**
 * @~english
 * @brief Fist SGF file index for accuracy calculation.
 * @~japanese
 * @brief 正解率を測定する際に使用する棋譜の最初のインデックス
 */
constexpr int TEST_KIFU_START_INDEX = 40001;

/**
 * @~english
 * @brief Last SGF file index for accuracy calculation.
 * @~japanese
 * @brief 正解率を測定する際に使用する棋譜の最後のインデックス
 */
constexpr int TEST_KIFU_LAST_INDEX = 70000;

/**
 * @~english
 * @brief Update interval of each independent features.
 * @~japanese
 * @brief 独立した特徴を更新する感覚
 */
constexpr int MM_UPDATE_INTERVAL = 8;

/**
 * @~english
 * @brief Update steps.
 * @~japanese
 * @brief 特徴の更新回数
 */
constexpr int MM_UPDATE_STEPS = 30;

/**
 * @~english
 * @brief Number of learning loop execution.
 * @~japanese
 * @brief 学習ループの実行回数
 */
constexpr int MM_UPDATE_MAX = MM_UPDATE_INTERVAL * MM_UPDATE_STEPS;

/**
 * @~english
 * @brief Number of learning loop execution.
 * @~japanese
 * @brief 学習ループの実行回数
 */
constexpr int BTFM_UPDATE_STEPS = 100;

/**
 * @~english
 * @brief Directory path to save learning results.
 * @~japanese
 * @brief 学習結果を格納するディレクトリ名
 */
constexpr char LEARNING_RESULT_DIR_NAME[] = "learning_result";

/**
 * @~english
 * @brief Directory path to save stone's pattern target files.
 * @~japanese
 * @brief 学習対象配石パターンファイルを格納するディレクトリ名
 */
constexpr char PATTERN_TARGET_DIR_NAME[] = "pattern-target";

/**
 * @~english
 * @brief Directory path to save parameter files for Monte-Carlo simulation.
 * @~japanese
 * @brief モンテカルロ・シミュレーション用パラメータのディレクトリ名
 */
constexpr char SIMULATION_RESULT_DIR_NAME[] = "simulation";

/**
 * @~english
 * @brief Directory path to save parameter files for tree search.
 * @~japanese
 * @brief 木探索用パラメータのディレクトリ名
 */
constexpr char TREE_RESULT_DIR_NAME[] = "uct";

/**
 * @~english
 * @brief Accuracy logging directory name.
 * @~japanese
 * @brief 正解率を記録するファイルを保存するディレクトリ
 */
constexpr char ACCURACY_LOG_DIR_NAME[] = "accuracy";

/**
 * @~english
 * @brief Accuracy logging file.
 * @~japanese
 * @brief 正解率を記録するファイル名
 */
constexpr char ACCURACY_LOG_FILE_NAME[] = "accuracy.log";

/**
 * @~english
 * @brief MD2 pattern targets file name.
 * @~japanese
 * @brief マンハッタン距離2の配石パターンの学習対象ファイル名
 */
constexpr char MD2_TARGET_FILE_NAME[] = "MD2Target.txt";

/**
 * @~english
 * @brief MD3 pattern targets file name.
 * @~japanese
 * @brief マンハッタン距離3の配石パターンの学習対象ファイル名
 */
constexpr char MD3_TARGET_FILE_NAME[] = "MD3Target.txt";

/**
 * @~english
 * @brief MD4 pattern targets file name.
 * @~japanese
 * @brief マンハッタン距離4の配石パターンの学習対象ファイル名
 */
constexpr char MD4_TARGET_FILE_NAME[] = "MD4Target.txt";

/**
 * @~english
 * @brief MD5 pattern targets file name.
 * @~japanese
 * @brief マンハッタン距離5の配石パターンの学習対象ファイル名
 */
constexpr char MD5_TARGET_FILE_NAME[] = "MD5Target.txt";

#endif
