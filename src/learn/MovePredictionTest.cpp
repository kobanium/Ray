/**
 * @file src/learn/MovePredictionTest.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Accuracy calculator for move prediction.
 * @~japanese
 * @brief 着手予測の正解率計算
 */
#include <algorithm>
#include <array>
#include <atomic>
#include <functional>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <thread>

#include "board/GoBoard.hpp"
#include "board/Point.hpp"
#include "sgf/SgfExtractor.hpp"
#include "mcts/UctRating.hpp"
#include "mcts/Rating.hpp"
#include "util/Utility.hpp"


/**
 * @~english
 * @brief The number of appearance count.
 * @~japanese
 * @brief 出現回数
 */
static std::array<std::atomic<int>, PURE_BOARD_MAX + 1> appearance;

/**
 * @~english
 * @brief First index.
 * @~japanese
 * @brief 最初のインデックス
 */
static constexpr int KIFU_START = 40001;

/**
 * @~english
 * @brief Last index.
 * @~japanese
 * @brief 最後のインデックス
 */
static constexpr int KIFU_END = 70000;

/**
 * @~english
 * @brief Directory path to SGF files.
 * @~japanese
 * @brief SGFファイルを格納しているディレクトリへのパス
 */
static constexpr char KIFU_PATH[] = "/home/usr/sgf";

/**
 * @~english
 * @brief The number of testing worker threads.
 * @~japanese
 * @brief テストワーカスレッド数
 */
static constexpr int TEST_WORKER_THREADS = 8;


// 着手予測の精度測定
static void CheckMovePrediction( game_info_t *game, const char *filename );

// 正解率測定ワーカ
static void PredictionTestWorker( const int thread_id );


/**
 * @~english
 * @brief Evaluate move prediction accuracy.
 * @~japanese
 * @brief 着手予測精度の評価
 */
void
EvaluateMovePrediction( void )
{
  std::thread *workers[TEST_WORKER_THREADS];

  for (int i = 0; i < TEST_WORKER_THREADS; i++) {
    workers[i] = new std::thread(PredictionTestWorker, i);
  }

  int sum = 0;

  for (int i = 0; i < TEST_WORKER_THREADS; i++) {
    workers[i]->join();
    delete workers[i];
  }
  

  for (int i = 0; i < pure_board_max + 1; i++) {
    sum += appearance[i];
  }

  for (int i = 0, part = 0; i < 10; i++) {
    part += appearance[i];
    std::cout << i + 1 << " " << static_cast<double>(part) / sum << std::endl;
    std::cerr << i + 1 << " " << appearance[i] << std::endl;
  }
}


/**
 * @~english
 * @brief Testing move prediction worker
 * @param[in] thread_id Worker thread ID.
 * @~japanese
 * @brief 正解率測定ワーカ
 * @param[in] thread_id スレッドID
 */
static void
PredictionTestWorker( const int thread_id )
{
  game_info_t *game = AllocateGame();
  game_info_t *init_game = AllocateGame();

  InitializeBoard(init_game);
  
  for (int i = KIFU_START; i <= KIFU_END; i++) {
    if (i % TEST_WORKER_THREADS == thread_id) {
      CopyGame(game, init_game);

      const std::string filename = KIFU_PATH + PATH_SEPARATOR + std::to_string(i) + ".sgf";

      if (i % 1000 == 0) {
        std::cerr << filename << std::endl;
      }
      CheckMovePrediction(game, filename.c_str());
    }
  }

  FreeGame(game);
  FreeGame(init_game);
}


/**
 * @~english
 * @brief Check move prediction.
 * @param[in] game Board position data.
 * @param[in] filename SGF filename.
 * @~japanese
 * @brief 終局まで着手予測を集計
 * @param[in] game 局面情報
 * @param[in] filename SGFファイル名
 */
static void
CheckMovePrediction( game_info_t *game, const char *filename )
{
  int color = S_BLACK;
  SGF_record_t kifu;
  double rate[PURE_BOARD_MAX + 1];

  ExtractKifu(filename, &kifu);

  for (int i = 0; i < kifu.moves; i++) {
    const int expert_move = GetKifuMove(&kifu, i);
    std::vector<std::pair<double, int> > score_data;

    AnalyzeUctRating(game, color, rate);
    //AnalyzePoRating(game, color, rate);

    score_data.push_back({ rate[pure_board_max], PASS });
    for (int j = 0; j < pure_board_max; j++) {
      const int pos = onboard_pos[j];
      score_data.push_back({ rate[j], pos });
    }
    std::sort(score_data.begin(), score_data.end(), std::greater<std::pair<double, int> >());

    int skip = 0;
    bool find = false;
    
    for (int j = 0; j <= pure_board_max; j++) {
      if (j != 0 &&
          score_data[j].first == score_data[j - 1].first) {
        skip++;
      } else {
        skip = 0;
      }
      if (score_data[j].second == expert_move) {
        appearance[j - skip]++;
        find = true;
      }
    }

    if (!find) {
      std::cerr << filename << " " << i << std::endl;
    }
    
    PutStone(game, expert_move, color);
    color = GetOppositeColor(color);
  }
}
