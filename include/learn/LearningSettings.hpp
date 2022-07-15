#ifndef _LEARNING_SETTINGS_HPP_
#define _LEARNING_SETTINGS_HPP_

// 学習ワーカスレッド数
constexpr int TRAIN_THREAD_NUM = 1;

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

// 正解率を記録するファイルパス
constexpr char ACCURACY_LOG_PATH[] = "learning_result/accuracy/accuracy.log";

// 正解率を記録するファイル名
constexpr char ACCURACY_LOG_FILE_NAME[] = "accuracy.log";

#endif
