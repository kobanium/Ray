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



static std::array<std::atomic<int>, PURE_BOARD_MAX + 1> appearance;
static constexpr int KIFU_START = 40001;
static constexpr int KIFU_END = 70000;
static constexpr char *KIFU_PATH = "/home/yuki/SGF_Files/tygem-data/shuffle-tygem";
static constexpr int TEST_WORKER_THREADS = 8;

static void CheckMovePrediction( game_info_t *game, const char *filename );

static void PredictionTestWorker( const int thread_id );


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


static void
PredictionTestWorker( const int thread_id )
{
  game_info_t *game = AllocateGame();
  game_info_t *init_game = AllocateGame();
  char filename[256];

  InitializeBoard(init_game);
  
  for (int i = KIFU_START; i <= KIFU_END; i++) {
    if (i % TEST_WORKER_THREADS == thread_id) {
      CopyGame(game, init_game);
      sprintf(filename, "%s/%d.sgf", KIFU_PATH, i);
      if (i % 1000 == 0) {
        std::cerr << filename << std::endl;
      }
      CheckMovePrediction(game, filename);
    }
  }

  FreeGame(game);
  FreeGame(init_game);
}


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
