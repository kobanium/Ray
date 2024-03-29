/**
 * @file src/learn/MinorizationMaximization.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Supervised learning using Minorization Maximization.
 * @~japanese
 * @brief Minorization Maximizationを利用したBTモデルの教師あり学習
 */
#include <algorithm>
#include <array>
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

#include "board/GoBoard.hpp"
#include "learn/BradleyTerryModel.hpp"
#include "learn/LearningLog.hpp"
#include "learn/LearningUtility.hpp"
#include "sgf/SgfExtractor.hpp"
#include "pattern/Pattern.hpp"
#include "feature/SimulationFeature.hpp"
#include "util/Utility.hpp"


/**
 * @~english
 * @brief Index conversion for 3x3 pattern.
 * @~japanese
 * @brief 3x3パターンの添字変換
 */
static std::vector<int> pat3_index;

/**
 * @~english
 * @brief Index conversion for MD2 pattern.
 * @~japanese
 * @brief MD2パターンの添字変換
 */
static std::vector<int> md2_index;

/**
 * @~english
 * @brief Previous move distance features.
 * @~japanese
 * @brief 直前の着手からの着手距離の学習情報
 */
static std::vector<std::vector<mm_t> > previous_distance;

/**
 * @~english
 * @brief Features of 3x3 stones pattern.
 * @~japanese
 * @brief 近傍の配石パターンの学習情報(3x3)
 */
static std::vector<std::vector<mm_t> > pat3;

/**
 * @~english
 * @brief Features of MD2 stones pattern.
 * @~japanese
 * @brief 近傍の配石パターンの学習情報(MD2)
 */
static std::vector<std::vector<mm_t> > md2;

/**
 * @~english
 * @brief Capture features.
 * @~japanese
 * @brief トリの学習情報
 */
static std::vector<std::vector<mm_t> > capture;

/**
 * @~english
 * @brief Save extension features.
 * @~japanese
 * @brief アタリから逃げる手の学習情報
 */
static std::vector<std::vector<mm_t> > save_extension;

/**
 * @~english
 * @brief Atari features.
 * @~japanese
 * @brief アタリの学習情報
 */
static std::vector<std::vector<mm_t> > atari;

/**
 * @~english
 * @brief Extension features.
 * @~japanese
 * @brief ノビの学習情報
 */
static std::vector<std::vector<mm_t> > extension;

/**
 * @~english
 * @brief Dame features.
 * @~japanese
 * @brief ダメ詰めの学習情報
 */
static std::vector<std::vector<mm_t> > dame;

/**
 * @~english
 * @brief Throw in features.
 * @~japanese
 * @brief ホウリコミの学習情報
 */
static std::vector<std::vector<mm_t> > throw_in;

/**
 * @~english
 * @brief Array index for MD2 feature.
 * @~japanese
 * @brief MD2パターンの配列のインデックス
 */
static std::vector<unsigned int> md2_list;

/**
 * @~english
 * @brief Training target flags for MD2 feature.
 * @~japanese
 * @brief 学習対象のMD2パターン
 */
static std::vector<bool> md2_target;

/**
 * @~english
 * @brief The number of all training positions.
 * @~japanese
 * @brief 学習局面数
 */
static std::atomic<int> all_moves;

/**
 * @~english
 * @brief Flag of first updating process.
 * @~japanese
 * @brief 初回更新時特別処理用のフラグ
 */
static bool first_flag = true;

/**
 * @~english
 * @brief Counter for accuracy calculation.
 * @~japanese
 * @brief 正解率算出用のカウンタ
 */
std::array<std::atomic<int>, PURE_BOARD_MAX> counter;


// 正解率を算出
static void CheckAccuracy( game_info_t *game, const std::string filename );

// Simgaのクリア
static void ClearSigma( std::vector<std::vector<mm_t> > &data );

// 着手予測精度の評価
static void EvaluateMovePrediction( const int steps );

// 戦術的特徴のGamma値を取得
static double GetTacticalFeatureGamma( std::vector<mm_t> &data, unsigned char *features, const int pos, const int feature_type );

// 戦術的特徴の打たれた回数を1だけ加算
static void IncrementTacticalFeatureCount( std::vector<mm_t> &data, const unsigned char *features, const int pos, const int feature_type );

// 学習情報の初期化
static void InitializeLearningData( std::vector<std::vector<mm_t> > &data, const int threads, const int data_size );

// 学習の初期設定
static void InitializeLearning( void );

// 全ての学習情報のSigmaのクリア
static void InitializeSigma( void );

// 1ステップ分の学習幅の計算
static void LearningWorker( train_thread_arg_t *arg );

// 特徴の抽出
static void SamplingFeatures( game_info_t *game, const int color, const int id );

// 学習対象の対局の再現
static void ReplayMatch( game_info_t *game, const std::string filename, const int id );

// 全ての学習情報のそれぞれのSigmaの合計値を算出
static void SumSigma( const int threads );

// 1つの学習情報のSigmaの合計値を算出
static void SumUpSigma( std::vector<std::vector<mm_t> > &data, const int threads );

// Cの値を加算
static void SumUpTacticalFeatureTeamRating( std::vector<mm_t> &data, unsigned char *features, const int pos, const int feature_type, const double gamma );

// 1種類の学習情報の打たれた回数の合計値を算出
static void SumUpWinCount( std::vector<std::vector<mm_t> > &data, const int threads );

// 全ての学習情報のそれぞれの打たれた回数の合計値を算出
static void SumWin( const int threads );

// 正解率算出のワーカ
static void TestingWorker( train_thread_arg_t *targ );

// 学習情報のGammaの値を更新
static void UpdateGamma( std::vector<std::vector<mm_t> > &data );

// Gammaの値を更新
static void UpdateParameters( const int update_steps );

// Sigmaの更新
static void UpdateSigma( std::vector<mm_t> &data, const double Ej );

// 学習経過の出力
static void OutputLearningProgress( const int update_steps );

// パラメータの出力
static void OutputAllParameters( const int step );

// Minorization Maximization法の学習
static void MinorizationMaximization( void );


/**
 * @~english
 * @brief Train by Minorization Maximization method.
 * @~japanese
 * @brief Minorization Maximization法の学習
 */
void
TrainBTModelByMinorizationMaximization( void )
{
  SetLearningLogFilePath();
  InitializeLearning();
  MinorizationMaximization();
}


/**
 * @~english
 * @brief Initialize learning settings.
 * @~japanese
 * @brief 学習の初期設定
 */
static void
InitializeLearning( void )
{
  const std::string md2_target_path = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    PATTERN_TARGET_DIR_NAME + PATH_SEPARATOR +
    MD2_TARGET_FILE_NAME;
  unsigned int transpose[16];

  InputMD2Target(md2_target_path, md2_index, md2_list, md2_target);

  InitializeLearningData(previous_distance, TRAIN_THREAD_NUM, PREVIOUS_DISTANCE_MAX);
  InitializeLearningData(pat3, TRAIN_THREAD_NUM, PAT3_MAX);
  InitializeLearningData(md2, TRAIN_THREAD_NUM, md2_target.size());
  InitializeLearningData(capture, TRAIN_THREAD_NUM, SIM_CAPTURE_MAX);
  InitializeLearningData(save_extension, TRAIN_THREAD_NUM, SIM_SAVE_EXTENSION_MAX);
  InitializeLearningData(atari, TRAIN_THREAD_NUM, SIM_ATARI_MAX);
  InitializeLearningData(extension, TRAIN_THREAD_NUM, SIM_EXTENSION_MAX);
  InitializeLearningData(dame, TRAIN_THREAD_NUM, SIM_DAME_MAX);
  InitializeLearningData(throw_in, TRAIN_THREAD_NUM, SIM_THROW_IN_MAX);

  pat3_index = std::vector<int>(PAT3_MAX, -1);
  for (unsigned int pat_3x3 = 0; pat_3x3 < static_cast<unsigned int>(PAT3_MAX); pat_3x3++) {
    if (pat3_index[pat_3x3] == -1) {
      Pat3Transpose16(pat_3x3, transpose);
      const int tc = GetUniquePattern(transpose, 16);
      for (int i = 0; i < tc; i++) {
        pat3_index[transpose[i]] = pat_3x3;
      }
    }
  }
}


/**
 * @~english
 * @brief Train with Minorization-Maximization algorithm.
 * @~japanese
 * @brief Minorization Maximization法による学習
 */
static void
MinorizationMaximization( void )
{
  train_thread_arg_t targ[TRAIN_THREAD_NUM];
  std::thread *handle[TRAIN_THREAD_NUM];

  for (int step = 0; step <= MM_UPDATE_MAX; step++) {
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    InitializeSigma();

    for (int i = 0; i < TRAIN_THREAD_NUM; i++) {
      targ[i].id = i;
      targ[i].step = step;
      handle[i] = new std::thread(LearningWorker, &targ[i]);
    }

    for (int i = 0; i < TRAIN_THREAD_NUM; i++) {
      handle[i]->join();
      delete handle[i];
    }

    if (first_flag) {
      SumWin(TRAIN_THREAD_NUM);
    }
    SumSigma(TRAIN_THREAD_NUM);

    UpdateParameters(step - 1);

    OutputLearningProgress(step - 1);
    
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    const double elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 0.001;

    OutputLearningLogFile(step, all_moves.load(), elapsed_time);

    if (step % MM_UPDATE_INTERVAL == 0 || step == MM_UPDATE_MAX) {
      OutputAllParameters(step);
      EvaluateMovePrediction(step);
    }

    first_flag = false;
  }
}


/**
 * @~english
 * @brief Tranining worker.
 * @param[in] targ Arguments for training worker.
 * @~japanese
 * @brief 学習ワーカ
 * @param[in] targ 学習ワーカ用の引数
 */
static void
LearningWorker( train_thread_arg_t *targ )
{
  game_info_t *game = AllocateGame();
  game_info_t *init_game = AllocateGame();

  InitializeBoard(init_game);

  for (int i = TRAIN_KIFU_START_INDEX; i <= TRAIN_KIFU_LAST_INDEX; i++) {
    if (i % TRAIN_THREAD_NUM == targ->id) {
      std::ostringstream oss;

      oss << TRAIN_KIFU_PATH << PATH_SEPARATOR << i << ".sgf";
      if (targ->id == 0) {
        std::cerr << "Step " << targ->step << " : " << oss.str() << std::endl;
      }
      CopyGame(game, init_game);
      ReplayMatch(game, oss.str(), targ->id);
    }
  }

  FreeGame(game);
  FreeGame(init_game);
}


/**
 * @~english
 * @brief Replay training data.
 * @param[in] game Position data.
 * @param[in] filename Training file name.
 * @param[in] id Thread ID.
 * @~japanese
 * @brief 学習する棋譜の再生
 * @param[in] game 局面のデータ
 * @param[in] filename 学習する棋譜のファイル名
 * @param[in] id スレッドID
 */
static void
ReplayMatch( game_info_t *game, const std::string filename, const int id )
{
  pattern_t *pat = game->pat;
  int color = S_BLACK;
  SGF_record_t kifu;
  unsigned char *tactical_features = game->tactical_features;

  ExtractKifu(filename.c_str(), &kifu);

  for (int i = 0; i < kifu.moves; i++) {
    const int pos = GetKifuMove(&kifu, i);

    if (first_flag) {
      if (pos != PASS) {
        // 戦術的特徴
        IncrementTacticalFeatureCount(capture[id], tactical_features, pos, CAPTURE);
        IncrementTacticalFeatureCount(save_extension[id], tactical_features, pos, SAVE_EXTENSION);
        IncrementTacticalFeatureCount(atari[id], tactical_features, pos, ATARI);
        IncrementTacticalFeatureCount(extension[id], tactical_features, pos, EXTENSION);
        IncrementTacticalFeatureCount(dame[id], tactical_features, pos, DAME);
        IncrementTacticalFeatureCount(throw_in[id], tactical_features, pos, THROW_IN);
        

        if (game->moves > 1 &&
            game->record[game->moves - 1].pos != PASS) {
          const int distance = DIS(game->record[game->moves - 1].pos, pos);

          if (distance == 2) {
            previous_distance[id][0].w++;
          } else if (distance == 3) {
            previous_distance[id][1].w++;
          } else if (distance == 4) {
            previous_distance[id][2].w++;
          }
        }

        const int pat_md2 = md2_index[MD2(pat, pos)];
        const int pat_3x3 = pat3_index[Pat3(pat, pos)];
        if (md2_target[pat_md2]) {
          md2[id][pat_md2].w++;
        } else {
          pat3[id][pat_3x3].w++;
        }
      }
      
      all_moves++;
    }
    PutStone(game, pos, color);

    color = GetOppositeColor(color);
    
    SamplingFeatures(game, color, id);
  }
}


/**
 * @~english
 * @brief Correct features.
 * @param[in] game Position data.
 * @param[in] color Player's color.
 * @param[in] id Thread ID.
 * @~japanese
 * @brief 学習する特徴の抽出
 * @param[in] game 局面のデータ
 * @param[in] color 手番の色
 * @param[in] id スレッドID
 */
static void
SamplingFeatures( game_info_t *game, const int color, const int id )
{
  const int previous_move = game->record[game->moves - 1].pos;
  pattern_t *pat = game->pat;
  double gamma, Ej = 0;
  unsigned char *tactical_features = game->tactical_features;
  unsigned int check_pat3[PURE_BOARD_MAX], check_md2[PURE_BOARD_MAX];
  int cpat3 = 0, cmd2 = 0, update_num = 0, update_pos[PURE_BOARD_MAX];
  
  std::fill_n(check_pat3, PURE_BOARD_MAX, 0);
  std::fill_n(check_md2, PURE_BOARD_MAX, 0);
  
  for (int i = 0; i < PURE_BOARD_MAX; i++) {
    const int pos = onboard_pos[i];
    ClearTacticalFeatures(&tactical_features[pos * ALL_MAX]);
  }
  
  CheckFeaturesForSimulation(game, color, update_pos, update_num);
  CheckRemove2StonesForSimulation(game, color, update_pos, update_num);
  if (game->ko_move == game->moves - 2) {
    CheckCaptureAfterKoForSimulation(game, color, update_pos, update_num);
  }

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (IsLegalNotEye(game, pos, color)) {
      CheckSelfAtariForSimulation(game, color, pos);
      CheckCaptureAndAtariForSimulation(game, color, pos);

      // 戦術的特徴
      gamma  = GetTacticalFeatureGamma(capture[id], tactical_features, pos, CAPTURE);
      gamma *= GetTacticalFeatureGamma(save_extension[id], tactical_features, pos, SAVE_EXTENSION);
      gamma *= GetTacticalFeatureGamma(atari[id], tactical_features, pos, ATARI);
      gamma *= GetTacticalFeatureGamma(extension[id], tactical_features, pos, EXTENSION);
      gamma *= GetTacticalFeatureGamma(dame[id], tactical_features, pos, DAME);
      gamma *= GetTacticalFeatureGamma(throw_in[id], tactical_features, pos, THROW_IN);

      // 直前からの着手距離
      if (game->moves > 1 && previous_move != PASS) {
        const int dis = DIS(previous_move, pos);
        if (dis == 2) {
          gamma *= previous_distance[id][0].gamma;
        } else if (dis == 3) {
          gamma *= previous_distance[id][1].gamma;
        } else if (dis == 4) {
          gamma *= previous_distance[id][2].gamma;
        }
      }

      // 配石パターン
      const int pat_md2 = md2_index[MD2(pat, pos)];
      const int pat_3x3 = pat3_index[Pat3(pat, pos)];
      if (md2_target[pat_md2]) {
        gamma *= md2[id][pat_md2].gamma;
      } else {
        gamma *= pat3[id][pat_3x3].gamma;
      }

      // チームの仲間の強さ
      SumUpTacticalFeatureTeamRating(capture[id], tactical_features, pos, CAPTURE, gamma);
      SumUpTacticalFeatureTeamRating(save_extension[id], tactical_features, pos, SAVE_EXTENSION, gamma);
      SumUpTacticalFeatureTeamRating(atari[id], tactical_features, pos, ATARI, gamma);
      SumUpTacticalFeatureTeamRating(extension[id], tactical_features, pos, EXTENSION, gamma);
      SumUpTacticalFeatureTeamRating(dame[id], tactical_features, pos, DAME, gamma);
      SumUpTacticalFeatureTeamRating(throw_in[id], tactical_features, pos, THROW_IN, gamma);

      // 直前からの着手距離
      if (game->moves > 1 && previous_move != PASS) {
        const int dis = DIS(previous_move, pos);
        if (dis == 2) {
          previous_distance[id][0].c += gamma;
        } else if (dis == 3) {
          previous_distance[id][1].c += gamma;
        } else if (dis == 4) {
          previous_distance[id][2].c += gamma;
        }
      }

      if (md2_target[pat_md2]) {
        // md2
        md2[id][pat_md2].c += gamma;
        check_md2[cmd2++] = pat_md2;
      } else {
        // 3x3
        pat3[id][pat_3x3].c += gamma;
        check_pat3[cpat3++] = pat_3x3;
      }
      // 全員の強さ
      Ej += gamma;
    }
  }
  
  UpdateSigma(capture[id], Ej);
  UpdateSigma(save_extension[id], Ej);
  UpdateSigma(atari[id], Ej);
  UpdateSigma(extension[id], Ej);
  UpdateSigma(dame[id], Ej);
  UpdateSigma(throw_in[id], Ej);
  UpdateSigma(previous_distance[id], Ej);
  
  cpat3 = GetUniquePattern(check_pat3, cpat3);
  for (int i = 0; i < cpat3; i++) {
    const unsigned int index = check_pat3[i];
    pat3[id][index].sigma += (pat3[id][index].c / pat3[id][index].gamma) / Ej;
    pat3[id][index].c = 0.0;
  }

  cmd2 = GetUniquePattern(check_md2, cmd2);
  for (int i = 0; i < cmd2; i++) {
    const unsigned int index = check_md2[i];
    md2[id][index].sigma += (md2[id][index].c / md2[id][index].gamma) / Ej;
    md2[id][index].c = 0.0;
  }
}


/**
 * @~english
 * @brief Update feature parameters.
 * @param[in] update_steps The number of update steps.
 * @~japanese
 * @brief パラメータの更新
 * @param[in] update_steps 更新回数
 */
void
UpdateParameters( const int update_steps )
{
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 0) {
    // 3x3パターン
    UpdateGamma(pat3);

    for (int i = 0; i < PAT3_MAX; i++) {
      pat3[0][i].gamma = pat3[0][pat3_index[i]].gamma;
    }
    const std::size_t data_size = pat3[0].size();
    for (std::vector<mm_t> &vec : pat3) {
      for (std::size_t i = 0; i < data_size; i++) {
        vec[i].gamma = pat3[0][i].gamma;
      }
    }
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 0) UpdateGamma(md2);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 1) UpdateGamma(previous_distance);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 2) UpdateGamma(capture);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 3) UpdateGamma(save_extension);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 4) UpdateGamma(atari);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 5) UpdateGamma(extension);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 6) UpdateGamma(dame);
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 7) UpdateGamma(throw_in);
}


/**
 * @~english
 * @brief Output learning process.
 * @param[in] update_steps The number of update steps.
 * @~japanese
 * @brief 学習の経過情報の出力
 * @param[in] update_steps 更新回数
 */
static void
OutputLearningProgress( const int update_steps )
{
  const std::string directory = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    SIMULATION_RESULT_DIR_NAME + PATH_SEPARATOR;

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 0) {
    OutputGamma(directory + "Pat3.txt", pat3[0]);
    OutputGamma(directory + "MD2.txt", md2[0], md2_list);
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 1) {
    for (int i = 0; i < PREVIOUS_DISTANCE_MAX; i++) {
      std::string filename = std::string("PreviousDistance_") + std::to_string(i + 2) + ".txt";
      OutputGammaAdditionMode(directory + filename, previous_distance[0][i]);
    }
  }

  // 戦術的特徴
  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 2) {
    for (int i = 1; i < SIM_CAPTURE_MAX; i++) {
      std::string filename = TrimRightSpace(sim_capture_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", capture[0][i]);
    }
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 3) {
    for (int i = 1; i < SIM_SAVE_EXTENSION_MAX; i++) {
      std::string filename = TrimRightSpace(sim_save_extension_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", save_extension[0][i]);
    }
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 4) {
    for (int i = 1; i < SIM_ATARI_MAX; i++) {
      std::string filename = TrimRightSpace(sim_atari_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", atari[0][i]);
    }
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 5) {
    for (int i = 1; i < SIM_EXTENSION_MAX; i++) {
      std::string filename = TrimRightSpace(sim_extension_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", extension[0][i]);
    }
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 6) {
    for (int i = 1; i < SIM_DAME_MAX; i++) {
      std::string filename = TrimRightSpace(sim_dame_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", dame[0][i]);
    }
  }

  if (update_steps == -1 || update_steps % MM_UPDATE_INTERVAL == 7) {
    for (int i = 1; i < SIM_THROW_IN_MAX; i++) {
      std::string filename = TrimRightSpace(sim_throw_in_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", throw_in[0][i]);
    }
  }
}


/**
 * @~english
 * @brief Save all feature's parameters.
 * @param[in] step The number of update steps.
 * @~japanese
 * @brief 全てのパラメータの保存
 * @param[in] step 更新回数
 */
static void
OutputAllParameters( const int step )
{
  const std::string result_directory = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    SIMULATION_RESULT_DIR_NAME + PATH_SEPARATOR +
    "result" + std::to_string(step) + PATH_SEPARATOR;
  const std::string cmd = "mkdir " + result_directory;
  std::string path;

  system(cmd.c_str());

  // 戦術的特徴
  path = result_directory + "CaptureFeature.txt";
  OutputGamma(path, capture[0]);
  path = result_directory + "SaveExtensionFeature.txt";
  OutputGamma(path, save_extension[0]);
  path = result_directory + "AtariFeature.txt";
  OutputGamma(path, atari[0]);
  path = result_directory + "ExtensionFeature.txt";
  OutputGamma(path, extension[0]);
  path = result_directory + "DameFeature.txt";
  OutputGamma(path, dame[0]);
  path = result_directory + "ThrowInFeature.txt";
  OutputGamma(path, throw_in[0]);

  // 直前からの着手距離
  path = result_directory + "PreviousDistance.txt";
  OutputGamma(path, previous_distance[0]);

  // 3x3
  path = result_directory + "Pat3.txt";
  OutputGamma(path, pat3[0]);

  // md2
  path = result_directory + "MD2.txt";
  OutputGamma(path, md2[0], md2_list);
}


/**
 * @~english
 * @brief Calculate accuracy of move predictions.
 * @param[in] steps The number of update steps.
 * @~japanese
 * @brief 着手予測正解率の算出
 * @param[in] steps 更新回数
 */
static void
EvaluateMovePrediction( const int steps )
{
  const std::string accuracy_log_directory = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    ACCURACY_LOG_DIR_NAME + PATH_SEPARATOR;
  const std::string accuracy_filename = accuracy_log_directory + "result" + std::to_string(steps) + ".txt";
  const std::string progress_filename = accuracy_log_directory + ACCURACY_LOG_FILE_NAME;
  std::ofstream accuracy_ofs(accuracy_filename);
  std::ofstream progress_ofs(progress_filename, std::ios::app);
  train_thread_arg_t targ[TRAIN_THREAD_NUM];
  std::thread *handle[TRAIN_THREAD_NUM];
  int count_sum = 0;

  if (!accuracy_ofs) {
    std::cerr << "Cannot open \"" << accuracy_filename << "\"" << std::endl;
    return;
  }

  if (!progress_ofs) {
    std::cerr << "Cannot open \"" << progress_filename << "\"" << std::endl;
    return;
  }

  for (std::atomic<int> &count : counter) {
    count.store(0);
  }

  for (int i = 0; i < TRAIN_THREAD_NUM; i++) {
    targ[i].id = i;
    handle[i] = new std::thread(TestingWorker, &targ[i]);
  }

  for (int i = 0; i < TRAIN_THREAD_NUM; i++) {
    handle[i]->join();
    delete handle[i];
  }

  for (int i = 0; i < pure_board_max; i++) {
    count_sum += counter[i].load();
  }

  for (int i = 0, correct = 0; i < pure_board_max; i++) {
    correct += counter[i];
    accuracy_ofs << static_cast<double>(correct) / count_sum << "\n";
  }
  progress_ofs << steps << " " << static_cast<double>(counter[0].load()) / count_sum << "\n";
}


/**
 * @~english
 * @brief Testing worker.
 * @param[in] targ Arguments for testing worker.
 * @~japanese
 * @brief テスト実行ワーカ
 * @param[in] targ テスト実行ワーカ用の引数
 */
static void
TestingWorker( train_thread_arg_t *targ )
{
  game_info_t *game = AllocateGame();
  game_info_t *init_game = AllocateGame();

  InitializeBoard(init_game);

  for (int i = TEST_KIFU_START_INDEX; i <= TEST_KIFU_LAST_INDEX; i++) {
    if (i % TRAIN_THREAD_NUM == targ->id) {
      std::ostringstream oss;

      oss << TEST_KIFU_PATH << PATH_SEPARATOR << i << ".sgf";
      if (targ->id == 0) {
        std::cerr << "Test : " << oss.str() << std::endl;
      }
      CopyGame(game, init_game);
      CheckAccuracy(game, oss.str());
    }
  }
}


/**
 * @~english
 * @brief Calculate accuracy for a file.
 * @param[in] game Position data.
 * @param[in] filename File name for calculating accuracy.
 * @~japanese
 * @brief 1つのファイルの正解率の集計
 * @param[in] game 局面のデータ
 * @param[in] filename 正解率計測用の棋譜ファイル名
 */
static void
CheckAccuracy( game_info_t *game, const std::string filename )
{
  pattern_t *pat = game->pat;
  unsigned char *tactical_features = game->tactical_features;
  SGF_record_t kifu;
  std::array<std::pair<double, int>, PURE_BOARD_MAX> score_list;
  int color = S_BLACK;
  int update_pos[PURE_BOARD_MAX], update_num = 0;
  int previous_move = PASS;

  ExtractKifu(filename.c_str(), &kifu);

  for (int i = 0; i < kifu.moves; i++) {
    const int expert_move = GetKifuMove(&kifu, i);

    if (expert_move == PASS) {
      PutStone(game, expert_move, color);
      color = GetOppositeColor(color);
      previous_move = expert_move;
      continue;
    }

    for (std::pair<double, int> &score : score_list) {
      score.first = 0.0;
      score.second = 0;
    }

    for (int j = 0; j < pure_board_max; j++) {
      const int pos = onboard_pos[j];
      ClearTacticalFeatures(&tactical_features[pos * ALL_MAX]);
    }

    update_num = 0;

    CheckFeaturesForSimulation(game, color, update_pos, update_num);
    CheckRemove2StonesForSimulation(game, color, update_pos, update_num);
    if (game->ko_move == game->moves - 2) {
      CheckCaptureAfterKoForSimulation(game, color, update_pos, update_num);
    }

    for (int j = 0; j < pure_board_max; j++) {
      const int pos = onboard_pos[j];
      double gamma = 0.0;
      if (IsLegalNotEye(game, pos, color)) {
        CheckCaptureAndAtariForSimulation(game, color, pos);
        CheckSelfAtariForSimulation(game, color, pos);

        gamma  = GetTacticalFeatureGamma(capture[0], tactical_features, pos, CAPTURE);
        gamma *= GetTacticalFeatureGamma(save_extension[0], tactical_features, pos, SAVE_EXTENSION);
        gamma *= GetTacticalFeatureGamma(atari[0], tactical_features, pos, ATARI);
        gamma *= GetTacticalFeatureGamma(extension[0], tactical_features, pos, EXTENSION);
        gamma *= GetTacticalFeatureGamma(dame[0], tactical_features, pos, DAME);
        gamma *= GetTacticalFeatureGamma(throw_in[0], tactical_features, pos, THROW_IN);

        if (game->moves > 1 && previous_move != PASS) {
          const int dis = DIS(previous_move, pos);
          if (dis == 2) {
            gamma *= previous_distance[0][0].gamma;
          } else if (dis == 3) {
            gamma *= previous_distance[0][1].gamma;
          } else if (dis == 4) {
            gamma *= previous_distance[0][2].gamma;
          }
        }

        const int pat_md2 = md2_index[MD2(pat, pos)];
        const int pat_3x3 = pat3_index[Pat3(pat, pos)];
        if (md2_target[pat_md2]) {
          gamma *= md2[0][pat_md2].gamma;
        } else {
          gamma *= pat3[0][pat_3x3].gamma;
        }
      }
      score_list[j].first = gamma;
      score_list[j].second = pos;
    }

    std::sort(score_list.begin(), score_list.end(), std::greater<std::pair<double, int> >());

    int skip = 0;
    
    for (int j = 0; j < pure_board_max; j++) {
      if (j != 0 &&
          score_list[j].first == score_list[j - 1].first) {
        skip++;
      } else {
        skip = 0;
      }
      if (score_list[j].second == expert_move) {
        counter[j - skip]++;
        break;
      }
    }
    
    PutStone(game, expert_move, color);
    color = GetOppositeColor(color);
    previous_move = expert_move;
  }
}


/**
 * @~english
 * @brief Initialize feature data.
 * @param[in, out] data Feature data.
 * @param[in] threads The number of training workers.
 * @param[in] data_size The number of feature data.
 * @~japanese
 * @brief 特徴データの初期化
 * @param[in, out] data 学習する特徴のデータ
 * @param[in] threads 学習ワーカスレッド数
 * @param[in] data_size 特徴データの個数
 */
static void
InitializeLearningData( std::vector<std::vector<mm_t> > &data, const int threads, const int data_size )
{
  data.resize(threads);

  for (std::vector<mm_t> &vec : data) {
    vec.resize(data_size);
    for (mm_t &datum : vec) {
      datum.gamma = 1.0;
      datum.w = 0;
    }
  }
}


/**
 * @~english
 * @brief Initialize sigma value for all features..
 * @~japanese
 * @brief 全ての特徴のシグマ値の初期化
 */
static void
InitializeSigma( void )
{
  ClearSigma(previous_distance);
  ClearSigma(pat3);
  ClearSigma(md2);
  ClearSigma(capture);
  ClearSigma(save_extension);
  ClearSigma(atari);
  ClearSigma(extension);
  ClearSigma(dame);
  ClearSigma(throw_in);
}


/**
 * @~english
 * @brief Initialize sigma value.
 * @param[in, out] data Feature data.
 * @~japanese
 * @brief シグマ値の初期化
 * @param[in, out] data 学習する特徴のデータ
 */
static void
ClearSigma( std::vector<std::vector<mm_t> > &data )
{
  for (std::vector<mm_t> &vec : data) {
    for (mm_t &datum : vec) {
      datum.sigma = 0.0;
    }
  }
}


/**
 * @~english
 * @brief Sum up all feature's played count.
 * @param[in] threads The number of training worker threads.
 * @~japanese
 * @brief 全ての特徴の打たれた回数の算出
 * @param[in] threads 学習ワーカスレッド数
 */
static void
SumWin( const int threads )
{
  SumUpWinCount(previous_distance, threads);
  SumUpWinCount(pat3, threads);
  SumUpWinCount(md2, threads);
  SumUpWinCount(capture, threads);
  SumUpWinCount(save_extension, threads);
  SumUpWinCount(atari, threads);
  SumUpWinCount(extension, threads);
  SumUpWinCount(dame, threads);
  SumUpWinCount(throw_in, threads);
}


/**
 * @~english
 * @brief Sum up feature's played count.
 * @param[in, out] data Feature data.
 * @param[in] threads The number of training worker threads.
 * @~japanese
 * @brief 特徴を持つ手が打たれた回数の集計
 * @param[in, out] data 学習する特徴のデータ
 * @param[in] threads 学習ワーカスレッド数
 */
static void
SumUpWinCount( std::vector<std::vector<mm_t> > &data, const int threads )
{
  const std::size_t data_size = data[0].size();

  for (int i = 1; i < threads; i++) {
    for (size_t j = 0; j < data_size; j++) {
      data[0][j].w += data[i][j].w;
    }
  }
}


/**
 * @~english
 * @brief Sum up sigma values for all features.
 * @param[in] threads The number of training worker threads.
 * @~japanese
 * @brief 全特徴のシグマ値の合計値の算出
 * @param[in] threads 学習ワーカスレッド数
 */
static void
SumSigma( const int threads )
{
  SumUpSigma(previous_distance, threads);
  SumUpSigma(pat3, threads);
  SumUpSigma(md2, threads);
  SumUpSigma(capture, threads);
  SumUpSigma(save_extension, threads);
  SumUpSigma(atari, threads);
  SumUpSigma(extension, threads);
  SumUpSigma(dame, threads);
  SumUpSigma(throw_in, threads);
}


/**
 * @~english
 * @brief Sum up sigma values.
 * @param[in, out] data Feature data.
 * @param[in] threads The number of training worker threads.
 * @~japanese
 * @brief シグマ値の合計値の計算
 * @param[in, out] data 特徴データ
 * @param[in] threads 学習ワーカスレッド数
 */
static void
SumUpSigma( std::vector<std::vector<mm_t> > &data, const int threads )
{
  const std::size_t data_size = data[0].size();

  for (int i = 1; i < threads; i++) {
    for (std::size_t j = 0; j < data_size; j++) {
      data[0][j].sigma += data[i][j].sigma;
    }
  }
}


/**
 * @~english
 * @brief Update gamma values.
 * @param[in, out] data Feature data.
 * @~japanese
 * @brief ガンマ値の更新
 * @param[in, out] data 特徴データ
 */
static void
UpdateGamma( std::vector<std::vector<mm_t> > &data )
{
  const std::size_t size = data[0].size();

  for (mm_t &datum : data[0]) {
    if (datum.sigma > 0.0) {
      datum.gamma = (datum.w + 1) / (datum.sigma + 2.0 / (1.0 + datum.gamma));
    }
  }

  for (std::vector<mm_t> &vec : data) {
    for (size_t i = 0; i < size; i++) {
      vec[i].gamma = data[0][i].gamma;
    }
  }
}


/**
 * @~english
 * @brief Add appearance count of tactical feature.
 * @param[out] data Feature data for tactical features.
 * @param[in] features Tactical features.
 * @param[in] pos Point of intersections.
 * @param[in] feature_type Tactical feature type.
 * @~japanese
 * @brief 戦術的特徴の出現回数の加算
 * @param[out] data 戦術的特徴の学習データ
 * @param[in] features 戦術的特徴
 * @param[in] pos 交点の座標
 * @param[in] feature_type 戦術的特徴の種別
 */
static void
IncrementTacticalFeatureCount( std::vector<mm_t> &data, const unsigned char *features, const int pos, const int feature_type )
{
  if (features[TF_INDEX(pos, feature_type)] > 0) {
    data[features[TF_INDEX(pos, feature_type)]].w++;
  }
}


/**
 * @~english
 * @brief Return gamma value of tactical feature.
 * @param[in] data Feature data for tactical features.
 * @param[in] features Tactical features.
 * @param[in] pos Point of intersections.
 * @param[in] feature_type Tactical feature type.
 * @return Tactical feature's gamma value.
 * @~japanese
 * @brief 戦術的特徴のガンマ値の取得
 * @param[in] data 戦術的特徴の学習データ
 * @param[in] features 戦術的特徴
 * @param[in] pos 交点の座標
 * @param[in] feature_type 戦術的特徴の種別
 * @return 戦術的特徴のガンマ値
 */
static double
GetTacticalFeatureGamma( std::vector<mm_t> &data, unsigned char *features, const int pos, const int feature_type )
{
  if (features[TF_INDEX(pos, feature_type)] > 0) {
    return data[features[TF_INDEX(pos, feature_type)]].gamma;
  } else {
    return 1.0;
  }
}


/**
 * @~english
 * @brief Sum up team rating for tactical features.
 * @param[out] data Feature data for tactical features.
 * @param[in] features Tactical features.
 * @param[in] pos Point of intersections.
 * @param[in] feature_type Tactical feature type.
 * @param[in] gamma Gamma value.
 * @~japanese
 * @brief チームのガンマの加算
 * @param[out] data 戦術的特徴の学習データ
 * @param[in] features 戦術的特徴
 * @param[in] pos 交点の座標
 * @param[in] feature_type 戦術的特徴の種別
 * @param[in] gamma ガンマ値
 */
static void
SumUpTacticalFeatureTeamRating( std::vector<mm_t> &data, unsigned char *features, const int pos, const int feature_type, const double gamma )
{
  if (features[TF_INDEX(pos, feature_type)] > 0) {
    data[features[TF_INDEX(pos, feature_type)]].c += gamma;
  }
}


/**
 * @~english
 * @brief Update sigma value for feature data.
 * @param[in, out] data Feature data.
 * @param[in] Ej Value of E_j
 * @~japanese
 * @brief 学習情報のシグマ値の更新
 * @param[in, out] data 学習情報データ
 * @param[in] Ej E_jの値
 */
static void
UpdateSigma( std::vector<mm_t> &data, const double Ej )
{
  for (mm_t &datum : data) {
    datum.sigma += (datum.c / datum.gamma) / Ej;
    datum.c = 0;
  }
}
