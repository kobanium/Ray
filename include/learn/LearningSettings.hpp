#ifndef _LEARNING_SETTINGS_HPP_
#define _LEARNING_SETTINGS_HPP_

// 学習ワーカスレッド数
constexpr int TRAIN_THREAD_NUM = 8;

// 学習対象のパターンの最大数
constexpr int TARGET_PATTERN_MAX = 500000;

// 学習する棋譜を格納したディレクトリのパス
constexpr char TRAIN_KIFU_PATH[] = "/home/user/SGF_Files/tygem-data/shuffle-tygem";

// 学習する棋譜の最初のインデックス
constexpr int TRAIN_KIFU_START_INDEX = 1;

// 学習する棋譜の最後のインデックス
constexpr int TRAIN_KIFU_LAST_INDEX = 30000;

// 正解率測定に使用する棋譜を格納したディレクトリのパス
constexpr char TEST_KIFU_PATH[] = "/home/user/SGF_Files/tygem-data/shuffle-tygem";

// 正解率を測定する際に使用する棋譜の最初のインデックス
constexpr int TEST_KIFU_START_INDEX = 40001;

// 正解率を測定する際に使用する棋譜の最後のインデックス
constexpr int TEST_KIFU_LAST_INDEX = 70000;

// 全ての特徴を更新するのに必要なステップ数
constexpr int MM_UPDATE_INTERVAL = 8;

// 学習ステップ数
constexpr int MM_UPDATE_STEPS = 30;

// 学習ループ実行回数
constexpr int MM_UPDATE_MAX = MM_UPDATE_INTERVAL * MM_UPDATE_STEPS;

// 学習ステップ数
constexpr int BTFM_UPDATE_STEPS = 100;

// 学習結果を格納するディレクトリ名
constexpr char LEARNING_RESULT_DIR_NAME[] = "learning_result";

// 学習対象配石パターンファイルを格納するディレクトリ名
constexpr char PATTERN_TARGET_DIR_NAME[] = "pattern-target";

// シミュレーション用パラメータのディレクトリ名
constexpr char SIMULATION_RESULT_DIR_NAME[] = "simulation";

// 木探索用パラメータのディレクトリ名
constexpr char TREE_RESULT_DIR_NAME[] = "uct";

// 正解率記録ファイル出力ディレクトリ名
constexpr char ACCURACY_LOG_DIR_NAME[] = "accuracy";

// 正解率を記録するファイル名
constexpr char ACCURACY_LOG_FILE_NAME[] = "accuracy.log";

// マンハッタン距離2の配石パターンの学習対象ファイル名
constexpr char MD2_TARGET_FILE_NAME[] = "MD2Target.txt";

// マンハッタン距離2の配石パターンの学習対象ファイル名
constexpr char MD3_TARGET_FILE_NAME[] = "MD3Target.txt";

// マンハッタン距離2の配石パターンの学習対象ファイル名
constexpr char MD4_TARGET_FILE_NAME[] = "MD4Target.txt";

// マンハッタン距離2の配石パターンの学習対象ファイル名
constexpr char MD5_TARGET_FILE_NAME[] = "MD5Target.txt";

#endif
