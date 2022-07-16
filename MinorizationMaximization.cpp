/**
 * @file MinorizationMaximization.cpp
 * @brief \~english Supervised learning using Minorization Maximization.
 *        \~japanese Minorization Maximizationを利用したBTモデルの教師あり学習
 * @author Yuki Kobayashi
 * @date 05/04/2022
 */
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

#include "board/GoBoard.hpp"
#include "common/Message.hpp"
#include "learn/LearningLog.hpp"
#include "learn/LearningUtility.hpp"
#include "learn/rating/BradleyTerryModel.hpp"
#include "learn/rating/MinorizationMaximization.hpp"
#include "sgf/SgfExtractor.hpp"
#include "pattern/Pattern.hpp"
#include "feature/SimulationFeature.hpp"
#include "util/Utility.hpp"


/// @~english
/// @brief Learning data output path.
/// @~japanese
/// @brief 学習結果の出力パス
static const char RESULT_PATH[] = "learning_result/md2";

/// @~english
/// @brief Index conversion for MD2 pattern.
/// @~japanese
/// @brief MD2パターンの添字変換
static std::vector<int> md2_index;

/// @~english
/// @brief Previous move distance features.
/// @~japanese
/// @brief 直前の着手からの着手距離の学習情報
static std::vector<std::vector<mm_t> > previous_distance;

/// @~english
/// @brief Features of 3x3 stones pattern.
/// @~japanese
/// @brief 近傍の配石パターンの学習情報(3x3)
static std::vector<std::vector<mm_t> > pat3;

/// @~english
/// @brief Features of MD2 stones pattern.
/// @~japanese
/// @brief 近傍の配石パターンの学習情報(MD2)
static std::vector<std::vector<mm_t> > md2;

/// @~english
/// @brief Capture features.
/// @~japanese
/// @brief トリの学習情報
static std::vector<std::vector<mm_t> > capture;

/// @~english
/// @brief Save extension features.
/// @~japanese
/// @brief アタリから逃げる手の学習情報
static std::vector<std::vector<mm_t> > save_extension;

/// @~english
/// @brief Atari features.
/// @~japanese
/// @brief アタリの学習情報
static std::vector<std::vector<mm_t> > atari;

/// @~english
/// @brief Extension features.
/// @~japanese
/// @brief ノビの学習情報
static std::vector<std::vector<mm_t> > extension;

/// @~english
/// @brief Dame features.
/// @~japanese
/// @brief ダメ詰めの学習情報
static std::vector<std::vector<mm_t> > dame;

/// @~english
/// @brief Connection features.
/// @~japanese
/// @brief ツギの学習情報
static std::vector<std::vector<mm_t> > connect;

/// @~english
/// @brief Throw in features.
/// @~japanese
/// @brief ホウリコミの学習情報
static std::vector<std::vector<mm_t> > throw_in;

/// @~english
/// @brief Array index for MD2 feature.
/// @~japanese
/// @brief MD2パターンの配列のインデックス
static std::vector<unsigned int> md2_list;

/// @~english
/// @brief Training target flags for MD2 feature.
/// @~japanese
/// @brief 学習対象のMD2パターン
static std::vector<bool> md2_target;

/// @~english
/// @brief Symmetrical patterns of 3x3 patterns.
/// @~japanese
/// @brief 3x3パターンの対称形
static std::vector<unsigned int> same_pat3;

/// @~english
/// @brief Symmetrical patterns of MD2 patterns.
/// @~japanese
/// @brief MD2パターンの対称形
static std::vector<unsigned int> same_md2;

/// @~english
/// @brief Minimum index for 3x3 patterns.
/// @~japanese
/// @brief 出現した3x3パターンのインデックス(対称形のうち最小のインデックスを保持)
static std::vector<unsigned int> pat3_appearance;

/// @~english
/// @brief Minimum index for MD2 patterns.
/// @~japanese
/// @brief 出現したMD2パターンのインデックス(対称系のうち最小のインデックスを保持)
static std::vector<unsigned int> md2_appearance;

/// @~english
/// @brief The number of appearance of 3x3 patterns.
/// @~japanese
/// @brief 3x3パターンの出現個数
static int pat3_appearance_num;

/// @~english
/// @brief The number of appearance of MD2 patterns.
/// @~japanese
/// @brief MD2パターンの出現個数
static int md2_appearance_num;

/// @~english
/// @brief The number of all training positions.
/// @~japanese
/// @brief 学習局面数
static std::atomic<int> all_moves;

/// @~english
/// @brief Flag of first updating process.
/// @~japanese
/// @brief 初回更新時特別処理用のフラグ
static bool first_flag = true;

/// @~english
/// @brief Mutex for 3x3 features.
/// @~japanese
/// @brief 3x3パターンのミューテックス
static std::mutex mutex_3x3;

/// @~english
/// @brief Mutex for MD2 features.
/// @~japanese
/// @brief MD2パターンのミューテックス
static std::mutex mutex_md2;

/// @~english
/// @brief Counter for accuracy calculation.
/// @~japanese
/// @brief 正解率算出用のカウンタ
std::array<std::atomic<int>, PURE_BOARD_MAX> counter;


// 正解率算出
static void CheckAccuracy( game_info_t *game, const std::string filename );

// Sigmaをクリア
static void ClearSigma( std::vector<std::vector<mm_t > > &data );

// 着手予測精度の評価
static void EvaluateMovePrediction( const int steps );

// 戦術的特徴のGammaの値の取得
static double GetTacticalFeatureGamma( std::vector<mm_t> &data, unsigned char *features, const int pos, const int feature_type );

// 戦術的特徴の打たれた回数を1加算
static void IncrementTacticalFeatureCount( std::vector<mm_t> &data, const unsigned char *features, const int pos, const int feature_type );

// 学習情報の初期化
static void InitializeLearningData( std::vector<std::vector<mm_t> > &data, const int threads, const int data_size );

// 学習の初期設定
static void InitializeLearning( const int threads );

// 全ての学習情報のSigmaをクリア
static void InitializeSigma( void );

// 1ステップ分の更新幅の計算
static void LearningWorker( train_thread_arg_t *arg );

// 特徴の抽出
static void SamplingFeatures( game_info_t *game, const int color, const int id, int &distance_index );

// 学習対象の対局の再現
static void ReplayMatch( game_info_t *game, const std::string filename, const int id );

// 全ての学習情報それぞれのSigmaの合計値を算出
static void SumSigma( const int thread );

// 1種類の学習情報のSigmaの合計値を算出
static void SumUpSigma( std::vector<std::vector<mm_t> > &data, const int threads );

// Cの値を加算
static void SumUpTacticalFeatureTeamRating( std::vector<mm_t> &data, unsigned char *features, const int pos, const int feature_type, const double gamma );

// 1種類の学習情報の打たれた回数の合計値を算出
static void SumUpWinCount( std::vector<std::vector<mm_t> > &data, const int threads );

// 全ての学習情報のそれぞれの打たれた回数の合計値を算出
static void SumWin( const int thread );

// 正解率算出用のワーカ
static void TestingWorker( train_thread_arg_t *targ );

// 学習情報のGammaの値を更新
static void UpdateGamma( std::vector<std::vector<mm_t> > &data );

// Gammaの値を更新
static void UpdateParameters( const int update );

// Sigmaを更新
static void UpdateSigma( std::vector<mm_t> &data, const double Ej );

// 学習経過の出力
static void OutputLearningProgress( const int update );

// パラメータの保存
static void OutputAllParameters( const int step );

static void MinorizationMaximization( const int threads );


/**
 * @~english
 * @brief Train by Minorization Maximization method.
 * @~japanese
 * @brief Minorization Maximization法の学習
 */
void
TrainBTModelByMinorizationMaximization( void )
{
  InitializeLearning(TRAIN_THREAD_NUM);
  MinorizationMaximization(TRAIN_THREAD_NUM);
}


/**
 * @~english
 * @brief Initialize learning settings.
 * @param[in] threads The number of training worker threads.
 * @~japanese
 * @brief 学習の初期設定
 * @param[in] threads 学習ワーカスレッド数
 */
static void
InitializeLearning( const int threads )
{
  const std::string md2_target_path("./learning_result/analyze/MD2Target.txt");

  InputMD2Target(md2_target_path, md2_index, md2_list, md2_target);

  InitializeLearningData(previous_distance, threads, PREVIOUS_DISTANCE_MAX * 3);
  InitializeLearningData(pat3, threads, PAT3_MAX);
  InitializeLearningData(md2, threads, md2_target.size());
  InitializeLearningData(capture, threads, SIM_CAPTURE_MAX);
  InitializeLearningData(save_extension, threads, SIM_SAVE_EXTENSION_MAX);
  InitializeLearningData(atari, threads, SIM_ATARI_MAX);
  InitializeLearningData(extension, threads, SIM_EXTENSION_MAX);
  InitializeLearningData(dame, threads, SIM_DAME_MAX);
  InitializeLearningData(connect, threads, SIM_CONNECT_MAX);
  InitializeLearningData(throw_in, threads, SIM_THROW_IN_MAX);

  same_pat3 = std::vector<unsigned int>(PAT3_MAX, 0);
  pat3_appearance = std::vector<unsigned int>(PAT3_MAX, 0);
  pat3_appearance_num = 1;

  same_md2 = std::vector<unsigned int>(md2_target.size(), 0);
  md2_appearance = std::vector<unsigned int>(md2_target.size(), 0);
  md2_appearance_num = 1;
}


/**
 * @~english
 * @brief Train with Minorization-Maximization algorithm.
 * @param[in] threads The number of training worker threads.
 * @~japanese
 * @brief Minorization Maximization法による学習
 * @param[in] threads 学習ワーカスレッド数
 */
static void
MinorizationMaximization( const int threads )
{
  train_thread_arg_t targ[threads];
  std::thread *handle[threads];

  for (int step = 0; step <= UPDATE_INTERVAL * UPDATE_STEPS; step++) {
    //  タイマー起動
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // 初期化
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
      SumWin(threads);
    }
    SumSigma(threads);

    // Gammaの更新
    UpdateParameters(step - 1);

    // 学習の経過を出力
    OutputLearningProgress(step - 1);
    
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    const double elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 0.001;

    OutputLearningLogFile(step, all_moves.load(), elapsed_time);

    if (step % UPDATE_INTERVAL == 0 || step == UPDATE_INTERVAL * UPDATE_STEPS) {
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
        PrintConsoleMessage("Step " + std::to_string(targ->step) + " : " + oss.str() + "\n", ray::log::LOG_INFO);
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
  unsigned int transpose[16];
  unsigned int pat_md2, pat_3x3;
  SGF_record_t kifu;
  unsigned char *tactical_features = game->tactical_features;
  int distance_index = 0;

  // 棋譜の読み込み 
  ExtractKifu(filename.c_str(), &kifu);

  // 終局するまでループ
  for (int i = 0; i < kifu.moves; i++){
    // 着手位置を入力
    const int pos = GetKifuMove(&kifu, i);

    if (first_flag) {
      // 打たれた手の数
      if (pos != PASS) {
        // 戦術的特徴
        IncrementTacticalFeatureCount(capture[id], tactical_features, pos, CAPTURE);
        IncrementTacticalFeatureCount(save_extension[id], tactical_features, pos, SAVE_EXTENSION);
        IncrementTacticalFeatureCount(atari[id], tactical_features, pos, ATARI);
        IncrementTacticalFeatureCount(extension[id], tactical_features, pos, EXTENSION);
        IncrementTacticalFeatureCount(dame[id], tactical_features, pos, DAME);
        IncrementTacticalFeatureCount(connect[id], tactical_features, pos, CONNECT);
        IncrementTacticalFeatureCount(throw_in[id], tactical_features, pos, THROW_IN);

        // 直前からの着手距離
        if (game->moves > 1 &&
            game->record[game->moves - 1].pos != PASS) {
          const int dis = DIS(game->record[game->moves - 1].pos, pos);
          if (dis == 2) {
            previous_distance[id][0 + distance_index].w++;
          } else if (dis == 3) {
            previous_distance[id][1 + distance_index].w++;
          } else if (dis == 4) {
            previous_distance[id][2 + distance_index].w++;
          }
        }

        // 近傍の配石パターン
        pat_md2 = MD2(pat, pos);
        pat_3x3 = Pat3(pat, pos);
        if (md2_target[md2_index[pat_md2]]) {
          // md2
          MD2Transpose16(pat_md2, transpose);
          const int tc = GetUniquePattern(transpose, 16);
          for (int j = 0; j < tc; j++) {
            md2[id][md2_index[transpose[j]]].w++;
          }
        } else {
          // 3x3
          Pat3Transpose16(pat_3x3, transpose);
          const int tc = GetUniquePattern(transpose, 16);
          for (int j = 0; j < tc; j++) {
            pat3[id][transpose[j]].w++;
          }
        }
      }
        
      // 局面数を数える
      all_moves++;
    }
    
    // 碁盤に石を置く
    PutStone( game, pos, color );

    // 手番交代
    color = GetOppositeColor(color);
    
    // 特徴の抽出
    SamplingFeatures( game, color, id, distance_index );
  }
}


/**
 * @~english
 * @brief Correct features.
 * @param[in] game Position data.
 * @param[in] color Player's color.
 * @param[in] id Thread ID.
 * @param[in] distance_index Previous move distance.
* @~japanese
 * @brief 学習する特徴の抽出
 * @param[in] game 局面のデータ
 * @param[in] color 手番の色
 * @param[in] id スレッドID
 * @param[in] distance_index 直前の着手からの距離
 */
static void
SamplingFeatures( game_info_t *game, const int color, const int id, int &distance_index )
{
  const int previous_move = game->record[game->moves - 1].pos;
  pattern_t *pat = game->pat;
  double gamma, Ej = 0;
  unsigned char *tactical_features = game->tactical_features;
  unsigned int pat_3x3, pat_md2, minpat, transpose[16];
  unsigned int check_pat3[PURE_BOARD_MAX], check_md2[PURE_BOARD_MAX];
  int cpat3 = 0, cmd2 = 0, update_num = 0, update_pos[PURE_BOARD_MAX];

  distance_index = 0;

  std::fill_n(check_pat3, PURE_BOARD_MAX, 0);
  std::fill_n(check_md2, PURE_BOARD_MAX, 0);
  
  for (int i = 0; i < PURE_BOARD_MAX; i++) {
    const int pos = onboard_pos[i];
    ClearTacticalFeatures(&tactical_features[pos * ALL_MAX]);
  }
  
  distance_index = CheckFeaturesForSimulation(game, color, update_pos, &update_num);
  CheckRemove2StonesForSimulation(game, color, update_pos, &update_num);
  if (game->ko_move == game->moves - 2) {
    CheckCaptureAfterKoForSimulation(game, color, update_pos, &update_num);
  }

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (IsLegalNotEye( game, pos, color )) {
      CheckCaptureAndAtariForSimulation(game, color, pos);
      CheckSelfAtariForSimulation(game, color, pos);

      // 戦術的特徴
      gamma  = GetTacticalFeatureGamma(capture[id], tactical_features, pos, CAPTURE);
      gamma *= GetTacticalFeatureGamma(save_extension[id], tactical_features, pos, SAVE_EXTENSION);
      gamma *= GetTacticalFeatureGamma(atari[id], tactical_features, pos, ATARI);
      gamma *= GetTacticalFeatureGamma(extension[id], tactical_features, pos, EXTENSION);
      gamma *= GetTacticalFeatureGamma(dame[id], tactical_features, pos, DAME);
      gamma *= GetTacticalFeatureGamma(connect[id], tactical_features, pos, CONNECT);
      gamma *= GetTacticalFeatureGamma(throw_in[id], tactical_features, pos, THROW_IN);

      // 直前からの着手距離
      if (game->moves > 1 && previous_move != PASS){
        const int dis = DIS(previous_move, pos);
        if (dis == 2) {
          gamma *= previous_distance[id][0 + distance_index].gamma;
        } else if (dis == 3) {
          gamma *= previous_distance[id][1 + distance_index].gamma;
        } else if (dis == 4) {
          gamma *= previous_distance[id][2 + distance_index].gamma;
        }
      }

      // 配石パターン
      pat_md2 = MD2(pat, pos);
      pat_3x3 = Pat3(pat, pos);
      if (md2_target[md2_index[pat_md2]]) {
        gamma *= md2[id][md2_index[pat_md2]].gamma;
      } else {
        gamma *= pat3[id][pat_3x3].gamma;
      }
      
      // チームの仲間の強さ

      // 戦術的特徴
      SumUpTacticalFeatureTeamRating(capture[id], tactical_features, pos, CAPTURE, gamma);
      SumUpTacticalFeatureTeamRating(save_extension[id], tactical_features, pos, SAVE_EXTENSION, gamma);
      SumUpTacticalFeatureTeamRating(atari[id], tactical_features, pos, ATARI, gamma);
      SumUpTacticalFeatureTeamRating(extension[id], tactical_features, pos, EXTENSION, gamma);
      SumUpTacticalFeatureTeamRating(dame[id], tactical_features, pos, DAME, gamma);
      SumUpTacticalFeatureTeamRating(connect[id], tactical_features, pos, CONNECT, gamma);
      SumUpTacticalFeatureTeamRating(throw_in[id], tactical_features, pos, THROW_IN, gamma);

      // 直前からの着手距離
      if (game->moves > 1 && previous_move != PASS) {
        const int dis = DIS(previous_move, pos);
        if (dis == 2) {
          previous_distance[id][0 + distance_index].c += gamma;
        } else if (dis == 3) {
          previous_distance[id][1 + distance_index].c += gamma;
        } else if (dis == 4) {
          previous_distance[id][2 + distance_index].c += gamma;
        }
      }

      // 近傍の配石パターン
      if (first_flag) {
        if (md2_target[md2_index[pat_md2]]) {
          mutex_md2.lock();
        } else {
          mutex_3x3.lock();
        }

        if (md2_target[md2_index[pat_md2]]) {
          // md2
          MD2Transpose16(pat_md2, transpose);
          const int tc = GetUniquePattern(transpose, 16);
          minpat = transpose[0];
          for (int j = 0; j < tc; j++){
            if (transpose[j] < minpat) minpat = transpose[j];
          }
          if (same_md2[md2_index[pat_md2]] == 0 && pat_md2 != 0) {
            md2_appearance[md2_appearance_num++] = minpat;
            for (int j = 0; j < tc; j++) same_md2[md2_index[transpose[j]]] = minpat;
          }
          const unsigned int index = md2_index[minpat];
          md2[id][index].c += gamma;
          check_md2[cmd2++] = index;
        } else {
          // 3x3
          Pat3Transpose16(pat_3x3, transpose);
          const int tc = GetUniquePattern(transpose, 16);
          minpat = transpose[0];
          for (int j = 0; j < tc; j++) {
            if(transpose[j] < minpat) minpat = transpose[j];
          }
          if (same_pat3[pat_3x3] == 0 && pat_3x3 != 0) {
            pat3_appearance[pat3_appearance_num++] = minpat;
            for (int j = 0; j < tc; j++) same_pat3[transpose[j]] = minpat;
          }
          pat3[id][minpat].c += gamma;
          check_pat3[cpat3++] = minpat;
        }

        if (md2_target[md2_index[pat_md2]]) {
          mutex_md2.unlock();
        } else {
          mutex_3x3.unlock();
        }
      } else {
        if (md2_target[md2_index[pat_md2]]) {
          // md2
          const unsigned int index = md2_index[same_md2[md2_index[pat_md2]]];
          md2[id][index].c += gamma;
          check_md2[cmd2++] = index;
        } else {
          // 3x3
          const unsigned int index = same_pat3[pat_3x3];
          pat3[id][index].c += gamma;
          check_pat3[cpat3++] = index;
        }
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
  UpdateSigma(connect[id], Ej);
  UpdateSigma(throw_in[id], Ej);
  UpdateSigma(previous_distance[id], Ej);

  // 3x3
  cpat3 = GetUniquePattern(check_pat3, cpat3);
  for (int i = 0; i < cpat3; i++) {
    const unsigned int index = check_pat3[i];
    pat3[id][index].sigma += (pat3[id][index].c / pat3[id][index].gamma) / Ej;
    pat3[id][index].c = 0.0;
  }

  // md2
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
 * @param[in] update The number of update steps.
 * @~japanese
 * @brief パラメータの更新
 * @param[in] update 更新回数
 */
void
UpdateParameters( const int update )
{
  unsigned int transpose[16];

  if (update == -1 || update % UPDATE_INTERVAL == 0) {
    // 3x3パターン
    for (int i = 0; i < pat3_appearance_num; i++) {
      unsigned int tmp = pat3_appearance[i];
      if (pat3[0][tmp].sigma > 0) {
        pat3[0][tmp].gamma = (pat3[0][tmp].w + 1) / (pat3[0][tmp].sigma + 2.0 / (1.0 + pat3[0][tmp].gamma));
        Pat3Transpose16(tmp, transpose);
        const int tc = GetUniquePattern(transpose, 16);
        for (int j = 0; j < tc; j++) {
          pat3[0][transpose[j]].gamma = pat3[0][tmp].gamma;
        }
      }
    }

    const std::size_t data_size = pat3[0].size();
    for (std::vector<mm_t> &vec : pat3) {
      for (std::size_t i = 0; i < data_size; i++) {
        vec[i].gamma = pat3[0][i].gamma;
      }
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 0) {
    // MD2パターン
    for (int i = 0; i < md2_appearance_num; i++) {
      unsigned int tmp = md2_index[md2_appearance[i]];
      if (md2[0][tmp].sigma > 0) {
        md2[0][tmp].gamma = (md2[0][tmp].w + 1) / (md2[0][tmp].sigma + 2.0 / (1.0 + md2[0][tmp].gamma));
        MD2Transpose16(md2_appearance[i], transpose);
        const int tc = GetUniquePattern(transpose, 16);
        for (int j = 0; j < tc; j++) {
          md2[0][md2_index[transpose[j]]].gamma = md2[0][tmp].gamma;
        }
      }
    }
    const std::size_t data_size = md2[0].size();
    for (std::vector<mm_t> &vec : md2) {
      for (std::size_t i = 0; i < data_size; i++) {
        vec[i].gamma = md2[0][i].gamma;
      }
    }
  }
  
  if (update == -1 || update % UPDATE_INTERVAL == 1) UpdateGamma(previous_distance);
  if (update == -1 || update % UPDATE_INTERVAL == 2) UpdateGamma(capture);
  if (update == -1 || update % UPDATE_INTERVAL == 3) UpdateGamma(save_extension);
  if (update == -1 || update % UPDATE_INTERVAL == 4) UpdateGamma(atari);
  if (update == -1 || update % UPDATE_INTERVAL == 5) UpdateGamma(extension);
  if (update == -1 || update % UPDATE_INTERVAL == 6) UpdateGamma(dame);
  if (update == -1 || update % UPDATE_INTERVAL == 7) UpdateGamma(connect);
  if (update == -1 || update % UPDATE_INTERVAL == 8) UpdateGamma(throw_in);
}


/**
 * @~english
 * @brief Output learning process.
 * @param[in] update The number of update steps.
 * @~japanese
 * @brief 学習の経過情報の出力
 * @param[in] update 更新回数
 */
static void
OutputLearningProgress( const int update )
{
  const std::string directory("./learning_result/md2/");

  if (update == -1 || update % UPDATE_INTERVAL == 0) {
    OutputGamma(directory + "Pat3.txt", pat3[0]);
    OutputGamma(directory + "MD2.txt", md2[0], md2_list);
  }
  
  if (update == -1 || update % UPDATE_INTERVAL == 1) {
    for (int i = 0; i < PREVIOUS_DISTANCE_MAX * 3; i++) {
      const int distance_index = i / PREVIOUS_DISTANCE_MAX;
      const int distance = i % PREVIOUS_DISTANCE_MAX + 2;
      std::string filename = std::string("PreviousDistance_") + std::to_string(distance_index) + "_" + std::to_string(distance);
      OutputGammaAdditionMode(directory + filename + ".txt", previous_distance[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 2) {
    for (int i = 1; i < SIM_CAPTURE_MAX; i++) {
      std::string filename = TrimRightSpace(sim_capture_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", capture[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 3) {
    for (int i = 1; i < SIM_SAVE_EXTENSION_MAX; i++) {
      std::string filename = TrimRightSpace(sim_save_extension_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", save_extension[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 4) {
    for (int i = 1; i < SIM_ATARI_MAX; i++) {
      std::string filename = TrimRightSpace(sim_atari_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", atari[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 5) {
    for (int i = 1; i < SIM_EXTENSION_MAX; i++) {
      std::string filename = TrimRightSpace(sim_extension_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", extension[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 6) {
    for (int i = 1; i < SIM_DAME_MAX; i++) {
      std::string filename = TrimRightSpace(sim_dame_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", dame[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 7) {
    for (int i = 1; i < SIM_CONNECT_MAX; i++) {
      std::string filename = TrimRightSpace(sim_connect_name[i]);
      OutputGammaAdditionMode(directory + filename + ".txt", connect[0][i]);
    }
  }

  if (update == -1 || update % UPDATE_INTERVAL == 8) {
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
  char path[1024];

  sprintf(path, "mkdir %s/result%d", RESULT_PATH, step);
  system(path);

  // 戦術的特徴
  sprintf(path, "%s/result%d/CaptureFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), capture[0]);
  sprintf(path, "%s/result%d/SaveExtensionFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), save_extension[0]);
  sprintf(path, "%s/result%d/AtariFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), atari[0]);
  sprintf(path, "%s/result%d/ExtensionFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), extension[0]);
  sprintf(path, "%s/result%d/DameFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), dame[0]);
  sprintf(path, "%s/result%d/ConnectFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), connect[0]);
  sprintf(path, "%s/result%d/ThrowInFeature.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), throw_in[0]);

  // 直前の手との距離
  sprintf(path, "%s/result%d/PreviousDistance.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), previous_distance[0]);

  // 3x3
  sprintf(path, "%s/result%d/Pat3.txt", RESULT_PATH, step);
  OutputGamma(std::string(path), pat3[0]);

  // md2
  sprintf(path, "%s/result%d/MD2.txt", RESULT_PATH, step);
  //OutputGamma(std::string(path), md2[0], md2_list, md2_target_num);
  OutputGamma(std::string(path), md2[0], md2_list);

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
  const std::string accuracy_log_directory("./learning_result/accuracy/");
  const std::string accuracy_filename = accuracy_log_directory + "result" + std::to_string(steps) + ".txt";
  const std::string progress_filename = accuracy_log_directory + ACCURACY_LOG_FILE_NAME;
  std::ofstream accuracy_ofs(accuracy_filename);
  std::ofstream progress_ofs(progress_filename, std::ios::app);
  train_thread_arg_t targ[TRAIN_THREAD_NUM];
  std::thread *handle[TRAIN_THREAD_NUM];
  int count_sum = 0;

  if (!accuracy_ofs) {
    std::ostringstream oss;
    oss << "Cannot open \"" << accuracy_filename << "\"\n";
    PrintConsoleMessage(oss.str(), ray::log::LOG_WARNING);
    return;
  }

  if (!progress_ofs) {
    std::ostringstream oss;
    oss << "Cannot open \"" << progress_filename << "\"\n";
    PrintConsoleMessage(oss.str(), ray::log::LOG_WARNING);
  }

    // カウンタの初期化
  for (std::atomic<int> &count : counter) {
    count.store(0);
  }

  // 正解率測定のワーカ起動
  for (int i = 0; i < TRAIN_THREAD_NUM; i++) {
    targ[i].id = i;
    handle[i] = new std::thread(TestingWorker, &targ[i]);
  }

  // 各スレッド終了
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
      
      oss << TEST_KIFU_PATH << "/" << i << ".sgf";
      if (targ->id == 0) {
        PrintConsoleMessage("Test : " + oss.str() + "\n", ray::log::LOG_INFO);
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
  unsigned int pat_3x3, pat_md2;

  ExtractKifu(filename.c_str(), &kifu);

  for (int i = 0; i < kifu.moves; i++) {
    const int expert_move = GetKifuMove(&kifu, i);

    // パスの時は何もしない
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

    // 戦術的特徴の初期化
    for (int j = 0; j < pure_board_max; j++) {
      const int pos = onboard_pos[j];
      ClearTacticalFeatures(&tactical_features[pos * ALL_MAX]);
    }

    update_num = 0;
    
    const int distance_index = CheckFeaturesForSimulation(game, color, update_pos, &update_num);
    CheckRemove2StonesForSimulation(game, color, update_pos, &update_num);
    if (game->ko_move == game->moves - 2) {
      CheckCaptureAfterKoForSimulation(game, color, update_pos, &update_num);
    }

    for (int j = 0; j < pure_board_max; j++) {
      const int pos = onboard_pos[j];
      double gamma = 0.0;
      if (IsLegalNotEye( game, pos, color )) {
        CheckCaptureAndAtariForSimulation(game, color, pos);
        CheckSelfAtariForSimulation(game, color, pos);
        
        // 戦術的特徴
        gamma  = GetTacticalFeatureGamma(capture[0], tactical_features, pos, CAPTURE);
        gamma *= GetTacticalFeatureGamma(save_extension[0], tactical_features, pos, SAVE_EXTENSION);
        gamma *= GetTacticalFeatureGamma(atari[0], tactical_features, pos, ATARI);
        gamma *= GetTacticalFeatureGamma(extension[0], tactical_features, pos, EXTENSION);
        gamma *= GetTacticalFeatureGamma(dame[0], tactical_features, pos, DAME);
        gamma *= GetTacticalFeatureGamma(connect[0], tactical_features, pos, CONNECT);
        gamma *= GetTacticalFeatureGamma(throw_in[0], tactical_features, pos, THROW_IN);

        // 直前からの着手距離
        if (game->moves > 1 && previous_move != PASS){
          const int dis = DIS(previous_move, pos);
          if (dis == 2) {
            gamma *= previous_distance[0][0 + distance_index].gamma;
          } else if (dis == 3) {
            gamma *= previous_distance[0][1 + distance_index].gamma;
          } else if (dis == 4) {
            gamma *= previous_distance[0][2 + distance_index].gamma;
          }
        }

        // 配石パターン
        pat_md2 = MD2(pat, pos);
        pat_3x3 = Pat3(pat, pos);
        if (md2_target[md2_index[pat_md2]]) {
          gamma *= md2[0][md2_index[pat_md2]].gamma;
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
  ClearSigma(connect);
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
ClearSigma( std::vector<std::vector<mm_t > > &data )
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
  SumUpWinCount(connect, threads);
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
SumUpWinCount( std::vector<std::vector<mm_t > > &data, const int threads )
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
  SumUpSigma(connect, threads);
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
SumUpSigma( std::vector<std::vector<mm_t > > &data, const int threads )
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
    if (datum.sigma > 0) {
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
