#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <map>
#include <map>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include <cstring>

#include "board/GoBoard.hpp"
#include "board/Point.hpp"
#include "feature/UctFeature.hpp"
#include "learn/LearningLog.hpp"
#include "learn/LearningSettings.hpp"
#include "learn/LearningUtility.hpp"
#include "learn/BradleyTerryModel.hpp"
#include "sgf/SgfExtractor.hpp"
#include "util/Utility.hpp"


constexpr bool has_interaction = true;

static std::vector<int> pat3_index;
static std::vector<int> md2_index;
static std::vector<index_hash_t> md3_index;
static std::vector<index_hash_t> md4_index;
static std::vector<index_hash_t> md5_index;

static std::vector<btfm_t> pat3;
static std::vector<btfm_t> md2;
static std::vector<btfm_t> md3;
static std::vector<btfm_t> md4;
static std::vector<btfm_t> md5;
static std::vector<btfm_t> pos_id;
static std::vector<btfm_t> capture;
static std::vector<btfm_t> save_extension;
static std::vector<btfm_t> atari;
static std::vector<btfm_t> extension;
static std::vector<btfm_t> dame;
static std::vector<btfm_t> connect;
static std::vector<btfm_t> throw_in;
static std::vector<btfm_t> move_distance_1;
static std::vector<btfm_t> move_distance_2;
static std::vector<btfm_t> move_distance_3;
static std::vector<btfm_t> move_distance_4;
static std::vector<btfm_t> pass;
static std::vector<btfm_t> ko_exist;

static std::vector<unsigned int> md2_list;
static std::vector<unsigned int> md3_list;
static std::vector<unsigned int> md4_list;
static std::vector<unsigned int> md5_list;

static std::vector<bool> md2_target;
static std::vector<bool> md3_target;
static std::vector<bool> md4_target;
static std::vector<bool> md5_target;

static std::vector<unsigned int> same_pat3;

static std::atomic<int> all_moves;
static std::atomic<int> counter;

static double learning_rate = 0.01;
static std::atomic<int> prediction[PURE_BOARD_MAX + 1];
static double likelihood;
static std::mutex *mutex_terms;
static std::mutex mutex_likelihood;
static int thread_num = TRAIN_THREAD_NUM;
static std::map<int, double> lr_schedule;

static constexpr double minW = 0.00001;
static constexpr double minV = 0.00001;
static constexpr double lambda = 0.001;
static constexpr double lambdaV = lambda * 2;

static std::map<UCT_CAPTURE_FEATURE, std::string> capture_file_name;
static std::map<UCT_SAVE_EXTENSION_FEATURE, std::string> save_extension_file_name;
static std::map<UCT_ATARI_FEATURE, std::string> atari_file_name;
static std::map<UCT_EXTENSION_FEATURE, std::string> extension_file_name;
static std::map<UCT_DAME_FEATURE, std::string> dame_file_name;
static std::map<UCT_CONNECT_FEATURE, std::string> connect_file_name;
static std::map<UCT_THROW_IN_FEATURE, std::string> throw_in_file_name;


static void InitializeLearning( void );
static void InitializeOutputFileMap( void );
static void InitializeWeights( std::vector<btfm_t> &data, const int size, std::mt19937_64 &mt, std::normal_distribution<> &dist, int &data_id );
static void LearningSecondOrderBradleyTerryModel( void );
static void CorrectAllFeatures( game_info_t *game, int color, std::vector<btfm_t*> featureList[], std::map<int, btfm_t*> &featureMap );
static void ProcessPosition( game_info_t *game, const int expertMove, const int color, const int thread_id );
static double CalculateFirstTerm( const std::vector<btfm_t*> &t, const int n );
static double CalculateSecondTerm( const std::vector<btfm_t*> &t, const int i, const int j );
static void Output( void );
static void Backup( const int k );
static void OutputFMAdd( std::string filename, btfm_t &t );
static void LearningLoop( train_thread_arg_t *arg );
static void PlaybackGame( game_info_t *game, char *filename, int id );
static void CorrectTacticalFeature( std::vector<btfm_t*> feature_list[], std::map<int, btfm_t*> &feature_map, std::vector<btfm_t> &feature, const unsigned int tactical_features[], const UCT_ALL_FEATURE feature_index, const int pos );
static void CorrectMoveDistanceFeature( std::vector<btfm_t*> feature_list[], std::map<int, btfm_t*> &feature_map, std::vector<btfm_t> &move_distance_feature, const int pos, const int previous_move, const int shift );
static void PlaybackGameForEvaluation( game_info_t *game, char *filename );
static void EvaluateMovePrediction( game_info_t *game, const int expert_move, const int color );
static void TestingLoop( train_thread_arg_t *arg );


void
TrainBTModelWithFactorizationMachines( void )
{
  SetLearningLogFilePath();
  InitializeOutputFileMap();
  InitializeLearning();
  LearningSecondOrderBradleyTerryModel();
}


static void
InitializeWeights( std::vector<btfm_t> &data, const int size, std::mt19937_64 &mt, std::normal_distribution<> &dist, int &data_id )
{
  data.resize(size);

  for (btfm_t &datum : data) {
    datum.w = 1.0;
    for (int k = 0; k < FM_DIMENSION; k++) {
      datum.v[k] = exp(dist(mt));
    }
    datum.id = data_id++;
  }
}


static void
InitializeLearning( void )
{
  std::random_device rand;
  std::mt19937_64 mt(rand());
  std::normal_distribution<> dist(0.0, 0.1);
  int id = 0;

  lr_schedule[ 0] = 0.01;
  lr_schedule[ 5] = 0.005;
  lr_schedule[15] = 0.0025;
  lr_schedule[25] = 0.00125;
  lr_schedule[35] = 0.000625;
  lr_schedule[40] = -1.0;

  all_moves.store(0);
  counter.store(0);


  const std::string target_base_path = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    PATTERN_TARGET_DIR_NAME + PATH_SEPARATOR;
  const std::string md2_target_path(target_base_path + MD2_TARGET_FILE_NAME);
  const std::string md3_target_path(target_base_path + MD3_TARGET_FILE_NAME);
  const std::string md4_target_path(target_base_path + MD4_TARGET_FILE_NAME);
  const std::string md5_target_path(target_base_path + MD5_TARGET_FILE_NAME);

  InputMD2Target(md2_target_path, md2_index, md2_list, md2_target);
  InputLargePatternTarget(md3_target_path, md3_index, md3_list, md3_target);
  InputLargePatternTarget(md4_target_path, md4_index, md4_list, md4_target);
  InputLargePatternTarget(md5_target_path, md5_index, md5_list, md5_target);

  std::vector<std::string> n_patterns;

  n_patterns.push_back(std::to_string(md3_target.size()));
  n_patterns.push_back(std::to_string(md4_target.size()));
  n_patterns.push_back(std::to_string(md5_target.size()));

  InitializeWeights(move_distance_1, MOVE_DISTANCE_MAX * 4, mt, dist, id);
  InitializeWeights(move_distance_2, MOVE_DISTANCE_MAX * 4, mt, dist, id);
  InitializeWeights(move_distance_3, MOVE_DISTANCE_MAX * 4, mt, dist, id);
  InitializeWeights(move_distance_4, MOVE_DISTANCE_MAX * 4, mt, dist, id);

  InitializeWeights(pat3, PAT3_MAX, mt, dist, id);
  InitializeWeights(md2, md2_target.size(), mt, dist, id);
  InitializeWeights(md3, md3_target.size(), mt, dist, id);
  InitializeWeights(md4, md4_target.size(), mt, dist, id);
  InitializeWeights(md5, md5_target.size(), mt, dist, id);
  InitializeWeights(pos_id, POS_ID_MAX, mt, dist, id);
  InitializeWeights(capture, UCT_CAPTURE_MAX, mt, dist, id);
  InitializeWeights(save_extension, UCT_SAVE_EXTENSION_MAX, mt, dist, id);
  InitializeWeights(atari, UCT_ATARI_MAX, mt, dist, id);
  InitializeWeights(extension, UCT_EXTENSION_MAX, mt, dist, id);
  InitializeWeights(dame, UCT_DAME_MAX, mt, dist, id);
  InitializeWeights(connect, UCT_CONNECT_MAX, mt, dist, id);
  InitializeWeights(throw_in, UCT_THROW_IN_MAX, mt, dist, id);
  InitializeWeights(pass, UCT_PASS_MAX, mt, dist, id);
  InitializeWeights(ko_exist, 1, mt, dist, id);

  capture[0].w = 1.0;
  save_extension[0].w = 1.0;
  atari[0].w = 1.0;
  extension[0].w = 1.0;
  dame[0].w = 1.0;
  connect[0].w = 1.0;
  throw_in[0].w = 1.0;
  for (int i = 0; i < FM_DIMENSION; i++) {
    capture[0].v[i] = 1.0;
    save_extension[0].v[i] = 1.0;
    atari[0].v[i] = 1.0;
    extension[0].v[i]= 1.0;
    dame[0].v[i] = 1.0;
    connect[0].v[i] = 1.0;
    throw_in[0].v[i] = 1.0;
  }

  unsigned int transpose[16];
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

  mutex_terms = new std::mutex[id];
}


static double
CalculateFirstTerm( const std::vector<btfm_t*> &t, const int n )
{
  double gamma = 1.0;

  for (int i = 0; i < n; i++) {
    gamma *= t[i]->w;
  }

  return gamma;
}


static double
CalculateSecondTerm( const std::vector<btfm_t*> &t, const int i, const int j )
{
  double theta = 0.0;

  for (int k = 0; k < FM_DIMENSION; k++) {
    theta += t[i]->v[k] * t[j]->v[k];
  }
  return theta * (1.0 / static_cast<double>(FM_DIMENSION));
}




static double
CalculateMoveScore( game_info_t *game, const int pos, const unsigned int tactical_features[], const int distance_index )
{
  const int moves = game->moves;
  const int pm1 = (moves > 1) ? game->record[moves - 1].pos : PASS;
  const int pm2 = (moves > 2) ? game->record[moves - 2].pos : PASS;
  const int pm3 = (moves > 3) ? game->record[moves - 3].pos : PASS;
  const int pm4 = (moves > 4) ? game->record[moves - 4].pos : PASS;
  int dis = 0;
  std::vector<btfm_t*> active_features;
  pattern_hash_t hash_pat;

  if (pos == PASS) {
    if (moves > 1 && pm1 == PASS) {
      active_features.push_back(&pass[UCT_PASS_AFTER_PASS]);
    } else {
      active_features.push_back(&pass[UCT_PASS_AFTER_MOVE]);
    }
  } else {
    if (pm1 != PASS) {
      dis = DIS(pos, pm1);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&move_distance_1[dis + distance_index]);
    }
    if (pm2 != PASS) {
      dis = DIS(pos, pm2);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&move_distance_2[dis + distance_index]);
    }
    if (pm3 != PASS) {
      dis = DIS(pos, pm3);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&move_distance_3[dis + distance_index]);
    }
    if (pm4 != PASS) {
      dis = DIS(pos, pm4);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&move_distance_4[dis + distance_index]);
    }

    const unsigned int *features = &tactical_features[pos * UCT_INDEX_MAX];

    if (features[UCT_CAPTURE_INDEX]        > 0) active_features.push_back(&capture[features[UCT_CAPTURE_INDEX]]);
    if (features[UCT_SAVE_EXTENSION_INDEX] > 0) active_features.push_back(&save_extension[features[UCT_SAVE_EXTENSION_INDEX]]);
    if (features[UCT_ATARI_INDEX]          > 0) active_features.push_back(&atari[features[UCT_ATARI_INDEX]]);
    if (features[UCT_EXTENSION_INDEX]      > 0) active_features.push_back(&extension[features[UCT_EXTENSION_INDEX]]);
    if (features[UCT_DAME_INDEX]           > 0) active_features.push_back(&dame[features[UCT_DAME_INDEX]]);
    if (features[UCT_CONNECT_INDEX]        > 0) active_features.push_back(&connect[features[UCT_CONNECT_INDEX]]);
    if (features[UCT_THROW_IN_INDEX]       > 0) active_features.push_back(&throw_in[features[UCT_THROW_IN_INDEX]]);

    active_features.push_back(&pos_id[board_pos_id[pos]]);

    PatternHash(&game->pat[pos], &hash_pat);
    const int pat_3x3 = pat3_index[Pat3(game->pat, pos)];
    const int pat_md2 = md2_index[MD2(game->pat, pos)];
    const int md3_idx = SearchIndex(md3_index.data(), hash_pat.list[MD_3]);
    const int md4_idx = SearchIndex(md4_index.data(), hash_pat.list[MD_4]);
    const int md5_idx = SearchIndex(md5_index.data(), hash_pat.list[MD_5 + MD_MAX]);

    if (md5_idx != -1 && md5_target[md5_idx]) {
      active_features.push_back(&md5[md5_idx]);
    } else if (md4_idx != -1 && md4_target[md4_idx]) {
      active_features.push_back(&md4[md4_idx]);
    } else if (md3_idx != -1 && md3_target[md3_idx]) {
      active_features.push_back(&md3[md3_idx]);
    } else if (pat_md2 != 0 && md2_target[pat_md2]) {
      active_features.push_back(&md2[pat_md2]);
    } else {
      active_features.push_back(&pat3[pat_3x3]);
    }
  }

  if (moves > 1 && game->ko_move == game->moves - 1) {
    active_features.push_back(&ko_exist[0]);
  }

  const int feature_num = active_features.size();
  const double gamma = CalculateFirstTerm(active_features, feature_num);
  double theta = feature_num > 1 ? 1.0 : 0.0;

  for (int i = 0; i < feature_num - 1; i++) {
    for (int j = i + 1; j < feature_num; j++) {
      theta *= CalculateSecondTerm(active_features, i, j);
    }
  }

  return gamma + theta;
}




static void
UpdateFirstTermAdam( btfm_t *feature, const double alpha, const double lambda, const double minW, const int id )
{
  const double beta = 0.9;
  const double gamma = 0.999;
  const double epsilon = 1e-6;

  const double w = feature->w;
  const double grad = feature->grad_w[id];
  const double weight_decay = w - 1.0;
  const double gradSq = grad * grad;

  const double first_moment_w = beta * feature->first_moment_w + (1.0 - beta) * grad;
  const double second_moment_w = gamma * feature->second_moment_w + (1.0 - gamma) * gradSq;
  feature->first_moment_w = first_moment_w;
  feature->second_moment_w = second_moment_w;

  const double first_hat = first_moment_w / (1.0 - beta);
  const double second_hat = second_moment_w / (1.0 - gamma);

  const double newParam = w + alpha * (first_hat / (sqrt(second_hat) + epsilon) - lambda * weight_decay);

  feature->w = std::max(minW, newParam);
  feature->grad_w[id] = 0.0;
}



static void
UpdateSecondTermAdam( btfm_t *feature, const double alpha, const double lambda, const double minV, const int id )
{
  const double beta = 0.9;
  const double gamma = 0.999;
  const double epsilon = 1e-6;

  for (int f = 0; f < FM_DIMENSION; f++) {
    const double v = feature->v[f];
    const double grad = feature->grad_v[id][f];
    const double gradSq = grad * grad;
    const double weight_decay = v - 1.0;
    const double first_moment_v = beta * feature->first_moment_v[f] + (1.0 - beta) * grad;
    const double second_moment_v = gamma * feature->second_moment_v[f] + (1.0 - gamma) * gradSq;
    feature->first_moment_v[f] = first_moment_v;
    feature->second_moment_v[f] = second_moment_v;

    const double first_hat = first_moment_v / (1.0 - beta);
    const double second_hat = second_moment_v / (1.0 - gamma);
    const double newParam = v + alpha * (first_hat / (sqrt(second_hat) + epsilon) - lambda * weight_decay);

    feature->v[f] = std::max(minV, newParam);
    feature->grad_v[id][f] = 0.0;
  }
}


static void
ProcessPosition( game_info_t *game, const int expertMove, const int color, const int thread_id )
{
  std::map<int, btfm_t*> featureMap;
  std::vector<btfm_t*> featureList[BOARD_MAX];
  const double alpha = learning_rate;
  const double alphaV = alpha;

  CorrectAllFeatures(game, color, featureList, featureMap);

  std::vector<double> scoreList;
  double totalScore = 0.0, gammaTable[BOARD_MAX], thetaTable[BOARD_MAX];


  // 1次の項と2次の項のスコアを求める
  for (int p = 0; p < PURE_BOARD_MAX; p++) {
    const int pos = onboard_pos[p];
    if (!IsLegal(game, pos, color)) {
      continue;
    }

    std::vector<btfm_t*>& feature = featureList[pos];
    const int size = feature.size();

    // 1次のスコアの導出
    double score = CalculateFirstTerm(feature, size);

    gammaTable[pos] = score;

    // 2次のスコアの導出
    if (has_interaction) {
      double theta = 1.0;
      for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
          theta *= CalculateSecondTerm(feature, i, j);
        }
      }
      thetaTable[pos] = theta;
      score += theta;
    }

    totalScore += score;
    scoreList.push_back(score);
  }

  // パス
  {
    std::vector<btfm_t*>& feature = featureList[PASS];
    const int size = feature.size();

    // 1次のスコアの導出
    double score = CalculateFirstTerm(feature, size);

    gammaTable[PASS] = score;

    // 2次のスコアの導出
    if (has_interaction && size > 1) {
      double theta = 1.0;
      for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
          theta *= CalculateSecondTerm(feature, i, j);
        }
      }
      thetaTable[PASS] = theta;
      score += theta;
    } else {
      thetaTable[PASS] = 0;
    }

    totalScore += score;
    scoreList.push_back(score);
  }

  double expertScore = gammaTable[expertMove];
  if (has_interaction) {
    expertScore += thetaTable[expertMove];
  }

  const double invTotal = 1.0 / totalScore;
  const double invTotalN = 1.0 / (totalScore * static_cast<double>(FM_DIMENSION));

  // 全ての着手に共通する項の計算
  for (int p = 0; p < PURE_BOARD_MAX; p++) {
    const int pos = onboard_pos[p];
    if (!IsLegal(game, pos, color)) {
      continue;
    }

    std::vector<btfm_t*>& feature = featureList[pos];
    const int size = feature.size();

    // 1次の項の勾配
    for (int i = 0; i < size; i++) {
      feature[i]->grad_w[thread_id] -= gammaTable[pos] * invTotal / feature[i]->w;
    }
    // 2次の項の勾配
    if (has_interaction) {
      const double thetaP = thetaTable[pos];
      for (int i = 0; i < size; i++) {
        for (int k = 0; k < FM_DIMENSION; k++) {
          double sum = 0;
          for (int j = 0; j < size; j++) {
            if (i == j) continue;
            sum += feature[j]->v[k] / CalculateSecondTerm(feature, i, j);
          }
          feature[i]->grad_v[thread_id][k] -= thetaP * sum * invTotalN;
        }
      }
    }
  }

  // パス
  {
    std::vector<btfm_t*>& feature = featureList[PASS];
    const int size = feature.size();

    // 1次の項の勾配
    for (int i = 0; i < size; i++) {
      feature[i]->grad_w[thread_id] -= gammaTable[PASS] * invTotal / feature[i]->w;
    }
    // 2次の項の勾配
    if (has_interaction) {
      const double thetaP = thetaTable[PASS];
      for (int i = 0; i < size; i++) {
        for (int k = 0; k < FM_DIMENSION; k++) {
          double sum = 0;
          for (int j = 0; j < size; j++) {
            if (i == j) continue;
            sum += feature[j]->v[k] / CalculateSecondTerm(feature, i, j);
          }
          feature[i]->grad_v[thread_id][k] -= thetaP * sum * invTotalN;
        }
      }
    }
  }

  // 教師データの着手のみにある項の計算
  {
    std::vector<btfm_t*>& feature = featureList[expertMove];
    const int size = feature.size();
    const double thetaP = thetaTable[expertMove];
    const double invExpertScoreN = 1 / (expertScore * FM_DIMENSION);
    const double deltaT = gammaTable[expertMove] / expertScore;

    for (int i = 0; i < size; i++) {
      feature[i]->grad_w[thread_id] += deltaT / feature[i]->w;
    }

    if (has_interaction && size > 1) {
      for (int i = 0; i < size; i++) {
        for (int k = 0; k < FM_DIMENSION; k++) {
          double sum = 0;
          for (int j = 0; j < size; j++) {
            if (i == j) continue;
            sum += feature[j]->v[k] / CalculateSecondTerm(feature, i, j);
          }
          feature[i]->grad_v[thread_id][k] += thetaP * sum * invExpertScoreN;
        }
      }
    }
  }

  for (auto &x : featureMap) {
    btfm_t *feature = x.second;
    mutex_terms[feature->id].lock();
    UpdateFirstTermAdam(feature, alpha, lambda, minW, thread_id);
    if (has_interaction) {
      UpdateSecondTermAdam(feature, alphaV, lambdaV, minV, thread_id);
    }
    mutex_terms[feature->id].unlock();
  }

  std::sort(scoreList.begin(), scoreList.end(), std::greater<double>());

  for (unsigned int i = 0; i < scoreList.size(); i++) {
    if (expertScore >= scoreList[i]) {
      prediction[i].fetch_add(1);
      break;
    }
  }

  const double p = expertScore * invTotal;
  if (p > 0) {
    mutex_likelihood.lock();
    likelihood += std::log(p);
    mutex_likelihood.unlock();
  } else {
    std::cerr << "Error likelihood : " << p << std::endl;
  }

  counter.fetch_add(1);
  all_moves.fetch_add(1);
}

static void
LearningSecondOrderBradleyTerryModel( void  )
{
  train_thread_arg_t targ[TRAIN_THREAD_NUM];
  std::thread *handle[TRAIN_THREAD_NUM];

  for (int i = 0; i < thread_num; i++) {
    targ[i].id = i;
  }
  
  for (int step = 0; step <= BTFM_UPDATE_STEPS; step++){
    for (int i = 0; i < thread_num; i++) {
      targ[i].step = step;
    }
    
    const std::chrono::system_clock::time_point train_start = std::chrono::system_clock::now();

    if (lr_schedule.count(step) != 0 &&
        lr_schedule[step] != learning_rate) {
      if (lr_schedule[step] < 0) {
        break;
      }
      learning_rate = lr_schedule[step];
    }

    for (int i = 0; i < thread_num; i++) {
      handle[i] = new std::thread(LearningLoop, &targ[i]);
    }

    for (int i = 0; i < thread_num; i++) {
      handle[i]->join();
      delete handle[i];
    }

    const std::chrono::system_clock::time_point train_end = std::chrono::system_clock::now();
    const double train_elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(train_end - train_start).count() * 0.001;
    const double train_accuracy = static_cast<double>(prediction[0].load()) / counter.load();

    // 学習のログファイル出力
    OutputLearningLogFile(step, all_moves.load(), train_elapsed_time, train_accuracy, true);

    std::cerr << "Train data accuracy : " << train_accuracy << std::endl;

    // 算出結果を消去
    for (int i = 0; i < PURE_BOARD_MAX + 1; i++) {
      prediction[i].store(0);
    }
    likelihood = 0.0;
    counter.store(0);

    // Output parameter files.
    Output();

    // Validation phase.
    const std::chrono::system_clock::time_point test_start = std::chrono::system_clock::now();
    for (int i = 0; i < thread_num; i++) {
      handle[i] = new std::thread(TestingLoop, &targ[i]);
    }

    for (int i = 0; i < thread_num; i++) {
      handle[i]->join();
      delete handle[i];
    }

    const std::chrono::system_clock::time_point test_end = std::chrono::system_clock::now();
    const double test_elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(test_end - test_start).count() * 0.001;
    const double test_accuracy = static_cast<double>(prediction[0].load()) / counter.load();
    OutputLearningLogFile(step, counter.load(), test_elapsed_time, test_accuracy, false);

    std::cerr << "Test data accuracy : " << test_accuracy << std::endl;

    for (int i = 0; i < PURE_BOARD_MAX + 1; i++) {
      prediction[i].store(0);
    }
    counter.store(0);

    Backup(step);

    all_moves.store(0);
  }
}

static void
LearningLoop( train_thread_arg_t *targ )
{
  char filename[1024];
  game_info_t *game = AllocateGame();
  game_info_t *init_game = AllocateGame();

  InitializeBoard(init_game);

  for (int i = TRAIN_KIFU_START_INDEX; i <= TRAIN_KIFU_LAST_INDEX; i++) {
    if (i % thread_num == targ->id) {
      sprintf(filename, "%s/%d.sgf", TRAIN_KIFU_PATH, i);
      if (i % 1000 == 0) {
        fprintf(stderr, "%d.sgf\n", i);
        fflush(stderr);
      }
      CopyGame(game, init_game);
      PlaybackGame(game, filename, targ->id);
    }
  }

  FreeGame(game);
  FreeGame(init_game);
}


static void
TestingLoop( train_thread_arg_t *targ )
{
  char filename[1024];
  game_info_t *game = AllocateGame();
  game_info_t *init_game = AllocateGame();

  InitializeBoard(init_game);

  for (int i = TEST_KIFU_START_INDEX; i <= TEST_KIFU_LAST_INDEX; i++) {
    if (i % thread_num == targ->id) {
      sprintf(filename, "%s/%d.sgf", TEST_KIFU_PATH, i);
      if (i % 1000 == 0) {
        fprintf(stderr, "Test %d.sgf\n", i);
        fflush(stderr);
      }
      CopyGame(game, init_game);
      PlaybackGameForEvaluation(game, filename);
    }
  }
  
  FreeGame(game);
  FreeGame(init_game);
}

static void
PlaybackGame( game_info_t *game, char *filename, int id )
{
  SGF_record_t kifu;
  int color = S_BLACK;

  // 棋譜の読み込み
  ExtractKifu(filename, &kifu);

  // 終局するまでループ
  for (int i = 0; i < kifu.moves; i++) {
    // 着手位置を入力
    const int pos = GetKifuMove(&kifu, i);

    // 特徴の抽出
    ProcessPosition(game, pos, color, id);

    // 碁盤に石を置く
    PutStone(game, pos, color);

    // 手番交代
    color = GetOppositeColor(color);
  }
}


static void
PlaybackGameForEvaluation( game_info_t *game, char *filename )
{
  SGF_record_t kifu;
  int color = S_BLACK;

  ExtractKifu(filename, &kifu);

  for (int i = 0; i < kifu.moves; i++) {
    const int pos = GetKifuMove(&kifu, i);

    EvaluateMovePrediction(game, pos, color);

    PutStone(game, pos, color);

    color = GetOppositeColor(color);
  }
}


static void
CorrectAllFeatures( game_info_t *game, int color, std::vector<btfm_t*> featureList[], std::map<int, btfm_t*> &featureMap )
{
  const int moves = game->moves;
  const int pm1 = game->record[moves - 1].pos;
  const int pm2 = (moves > 2) ? game->record[moves - 2].pos : PASS;
  const int pm3 = (moves > 3) ? game->record[moves - 3].pos : PASS;
  const int pm4 = (moves > 4) ? game->record[moves - 4].pos : PASS;
  unsigned int tactical_features[BOARD_MAX * UCT_INDEX_MAX] = {0};
  int id = 0;
  pattern_t *pat = game->pat;
  pattern_hash_t hash_pat;

  const int distance_index = CheckFeaturesForTree( game, color, tactical_features );
  CheckRemove2StonesForTree(game, color, tactical_features);

  if (moves > 2 && game->ko_move == moves - 2) {
    CheckCaptureAfterKoForTree(game, color, tactical_features);
    CheckKoConnectionForTree(game, tactical_features);
  } else if (moves > 3 && game->ko_move == moves - 3) {
    CheckKoRecaptureForTree(game, color, tactical_features);
  }

  // パス
  if (moves > 1 && pm1 == PASS) {
    featureList[PASS].push_back(&pass[UCT_PASS_AFTER_PASS]);
    id = pass[UCT_PASS_AFTER_PASS].id;
    featureMap[id] = &pass[UCT_PASS_AFTER_PASS];
  } else {
    featureList[PASS].push_back(&pass[UCT_PASS_AFTER_MOVE]);
    id = pass[UCT_PASS_AFTER_MOVE].id;
    featureMap[id] = &pass[UCT_PASS_AFTER_MOVE];
  }

  if (moves > 1 && game->ko_move == moves - 1) {
    featureList[PASS].push_back(&ko_exist[0]);
    id = ko_exist[0].id;
    featureMap[id] = &ko_exist[0];
  }

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (IsLegal(game, pos, color)) {
      CheckSelfAtariForTree(game, color, pos, tactical_features);
      CheckCaptureForTree(game, color, pos, tactical_features);
      CheckAtariForTree(game, color, pos, tactical_features);

      if (moves > 1 && game->ko_move == moves - 1) {
        featureList[pos].push_back(&ko_exist[0]);
      }

      CorrectTacticalFeature(featureList, featureMap, capture,        tactical_features, UCT_CAPTURE_INDEX,        pos);
      CorrectTacticalFeature(featureList, featureMap, save_extension, tactical_features, UCT_SAVE_EXTENSION_INDEX, pos);
      CorrectTacticalFeature(featureList, featureMap, atari,          tactical_features, UCT_ATARI_INDEX,          pos);
      CorrectTacticalFeature(featureList, featureMap, extension,      tactical_features, UCT_EXTENSION_INDEX,      pos);
      CorrectTacticalFeature(featureList, featureMap, dame,           tactical_features, UCT_DAME_INDEX,           pos);
      CorrectTacticalFeature(featureList, featureMap, connect,        tactical_features, UCT_CONNECT_INDEX,        pos);
      CorrectTacticalFeature(featureList, featureMap, throw_in,       tactical_features, UCT_THROW_IN_INDEX,       pos);

      CorrectMoveDistanceFeature(featureList, featureMap, move_distance_1, pos, pm1, distance_index);
      CorrectMoveDistanceFeature(featureList, featureMap, move_distance_2, pos, pm2, distance_index);
      CorrectMoveDistanceFeature(featureList, featureMap, move_distance_3, pos, pm3, distance_index);
      CorrectMoveDistanceFeature(featureList, featureMap, move_distance_4, pos, pm4, distance_index);

      featureList[pos].push_back(&pos_id[board_pos_id[pos]]);
      id = pos_id[board_pos_id[pos]].id;
      if (!featureMap.count(id)) {
        featureMap[id] = &pos_id[board_pos_id[pos]];
      }

      PatternHash(&game->pat[pos], &hash_pat);
      const int pat_3x3 = pat3_index[Pat3(pat, pos)];
      const int pat_md2 = md2_index[MD2(pat, pos)];
      const int md3_idx = SearchIndex(md3_index.data(), hash_pat.list[MD_3]);
      const int md4_idx = SearchIndex(md4_index.data(), hash_pat.list[MD_4]);
      const int md5_idx = SearchIndex(md5_index.data(), hash_pat.list[MD_5 + MD_MAX]);

      if (md5_idx != -1 && md5_target[md5_idx]) {
        featureList[pos].push_back(&md5[md5_idx]);
        id = md5[md5_idx].id;
        if (!featureMap.count(id)) {
          featureMap[id] = &md5[md5_idx];
        }
      } else if (md4_idx != -1 && md4_target[md4_idx]) {
        featureList[pos].push_back(&md4[md4_idx]);
        id = md4[md4_idx].id;
        if (!featureMap.count(id)) {
          featureMap[id] = &md4[md4_idx];
        }
      } else if (md3_idx != -1 && md3_target[md3_idx]) {
        featureList[pos].push_back(&md3[md3_idx]);
        id = md3[md3_idx].id;
        if (!featureMap.count(id)) {
          featureMap[id] = &md3[md3_idx];
        }
      } else if (pat_md2 != 0 && md2_target[pat_md2]) {
        featureList[pos].push_back(&md2[pat_md2]);
        id = md2[pat_md2].id;
        if (!featureMap.count(id)) {
          featureMap[id] = &md2[pat_md2];
        }
      } else {
        featureList[pos].push_back(&pat3[pat_3x3]);
        id = pat3[pat_3x3].id;
        if (!featureMap.count(id)) {
          featureMap[id] = &pat3[pat_3x3];
        }
      }
    }
  }
}


static void
EvaluateMovePrediction( game_info_t *game, const int expert_move, const int color )
{
  const int moves = game->moves;
  std::vector<std::pair<double, int> > score_list;
  unsigned int tactical_features[BOARD_MAX * UCT_INDEX_MAX] = {0};
  const int distance_index = CheckFeaturesForTree(game, color, tactical_features);
  CheckRemove2StonesForTree(game, color, tactical_features);

  if (moves > 2 && game->ko_move == moves - 2) {
    CheckCaptureAfterKoForTree(game, color, tactical_features);
    CheckKoConnectionForTree(game, tactical_features);
  } else if (moves > 3 && game->ko_move == moves - 3) {
    CheckKoRecaptureForTree(game, color, tactical_features);
  }


  score_list.push_back({ CalculateMoveScore(game, PASS, tactical_features, distance_index), PASS });

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (IsLegal(game, pos, color)) {
      CheckSelfAtariForTree(game, color, pos, tactical_features);
      CheckCaptureForTree(game, color, pos, tactical_features);
      CheckAtariForTree(game, color, pos, tactical_features);

      score_list.push_back({ CalculateMoveScore(game, pos, tactical_features, distance_index), pos });
    } else {
      score_list.push_back({ 0.0, pos });
    }
  }

  std::sort(score_list.begin(), score_list.end(), std::greater<std::pair<double, int> >());

  int skip = 0;

  for (int j = 0; j < pure_board_max + 1; j++) {
    if (j != 0 &&
        score_list[j].first == score_list[j - 1].first) {
      skip++;
    } else {
      skip = 0;
    }
    if (score_list[j].second == expert_move) {
      prediction[j]++;
    }
  }
  counter++;
}




static void
CorrectTacticalFeature( std::vector<btfm_t*> feature_list[],
                        std::map<int, btfm_t*> &feature_map,
                        std::vector<btfm_t> &feature,
                        const unsigned int tactical_features[],
                        const UCT_ALL_FEATURE feature_index,
                        const int pos )
{
  if (tactical_features[UctFeatureIndex(pos, feature_index)] > 0) {
    feature_list[pos].push_back(&feature[tactical_features[UctFeatureIndex(pos, feature_index)]]);
    const int id = feature[tactical_features[UctFeatureIndex(pos, feature_index)]].id;
    if (!feature_map.count(id)) {
      feature_map[id] = &feature[tactical_features[UctFeatureIndex(pos, feature_index)]];
    }
  }
}



static void
CorrectMoveDistanceFeature( std::vector<btfm_t*> feature_list[],
                            std::map<int, btfm_t*> &feature_map,
                            std::vector<btfm_t> &move_distance_feature,
                            const int pos,
                            const int previous_move,
                            const int shift )
{
  if (previous_move != PASS) {
    int distance = DIS(previous_move, pos);
    if (distance > MOVE_DISTANCE_MAX - 1) {
      distance = MOVE_DISTANCE_MAX - 1;
    }
    distance += shift;
    feature_list[pos].push_back(&move_distance_feature[distance]);
    const int id = move_distance_feature[distance].id;
    if (!feature_map.count(id)) {
      feature_map[id] = &move_distance_feature[distance];
    }
  }
}


static void
Output( void )
{
  const std::string output_directory = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    TREE_RESULT_DIR_NAME + PATH_SEPARATOR;

  for (unsigned int i = 0; i < static_cast<unsigned int>(PAT3_MAX); i++) {
    pat3[i].w = pat3[pat3_index[i]].w;
    for (int j = 0; j < FM_DIMENSION; j++) {
      pat3[i].v[j] = pat3[pat3_index[i]].v[j];
    }
  }
  
  // パス
  OutputBTFMParameter(output_directory + "Pass.txt", pass);

  // 戦術的特徴
  OutputBTFMParameter(output_directory + "CaptureFeature.txt", capture);
  OutputBTFMParameter(output_directory + "SaveExtensionFeature.txt", save_extension);
  OutputBTFMParameter(output_directory + "AtariFeature.txt", atari);
  OutputBTFMParameter(output_directory + "ExtensionFeature.txt", extension);
  OutputBTFMParameter(output_directory + "DameFeature.txt", dame);
  OutputBTFMParameter(output_directory + "ConnectFeature.txt", connect);
  OutputBTFMParameter(output_directory + "ThrowInFeature.txt", throw_in);

  //  直前の手との距離
  OutputBTFMParameter(output_directory + "MoveDistance1.txt", move_distance_1);
  OutputBTFMParameter(output_directory + "MoveDistance2.txt", move_distance_2);
  OutputBTFMParameter(output_directory + "MoveDistance3.txt", move_distance_3);
  OutputBTFMParameter(output_directory + "MoveDistance4.txt", move_distance_4);

  //  盤上の位置
  OutputBTFMParameter(output_directory + "PosID.txt", pos_id);

  //  近傍の配石パターン(3x3, md2, md3, md4, md5)
  OutputBTFMParameter(output_directory + "Pat3.txt", pat3);
  OutputBTFMParameter(output_directory + "MD2.txt", md2, md2_list, md2_target);
  OutputBTFMParameter(output_directory + "MD3.txt", md3, md3_list, md3_index, md3_target);
  OutputBTFMParameter(output_directory + "MD4.txt", md4, md4_list, md4_index, md4_target);
  OutputBTFMParameter(output_directory + "MD5.txt", md5, md5_list, md5_index, md5_target);

  // Capture
  for (const auto &item : capture_file_name) {
    OutputFMAdd(output_directory + item.second, capture[item.first]);
  }

  // SaveExtension
  for (const auto &item : save_extension_file_name) {
    OutputFMAdd(output_directory + item.second, save_extension[item.first]);
  }

  //  Atari
  for (const auto &item : atari_file_name) {
    OutputFMAdd(output_directory + item.second, atari[item.first]);
  }

  //  Dame
  for (const auto &item : dame_file_name) {
    OutputFMAdd(output_directory + item.second, dame[item.first]);
  }

  //  Extension
  for (const auto &item : extension_file_name) {
    OutputFMAdd(output_directory + item.second, extension[item.first]);
  }

  // Connect
  for (const auto &item : connect_file_name) {
    OutputFMAdd(output_directory + item.second, connect[item.first]);
  }

  //  Throw in
  for (const auto &item : throw_in_file_name) {
    OutputFMAdd(output_directory + item.second, throw_in[item.first]);
  }

  //  Ko existence
  OutputFMAdd(output_directory + "KoExist.txt", ko_exist[0]);
}



static void
Backup( const int k )
{
  const std::string output_directory = GetWorkingDirectory() + PATH_SEPARATOR +
    LEARNING_RESULT_DIR_NAME + PATH_SEPARATOR +
    TREE_RESULT_DIR_NAME + PATH_SEPARATOR +
    "result" + std::to_string(k) + PATH_SEPARATOR;
  const std::string cmd = "mkdir " + output_directory;
  
  system(cmd.c_str());

  //  パス
  OutputBTFMParameter(output_directory + "Pass.txt", pass);

  //  劫の存在
  OutputBTFMParameter(output_directory + "KoExist.txt", ko_exist);

  //  戦術的特徴
  OutputBTFMParameter(output_directory + "CaptureFeature.txt", capture);
  OutputBTFMParameter(output_directory + "SaveExtensionFeature.txt", save_extension);
  OutputBTFMParameter(output_directory + "AtariFeature.txt", atari);
  OutputBTFMParameter(output_directory + "ExtensionFeature.txt", extension);
  OutputBTFMParameter(output_directory + "DameFeature.txt", dame);
  OutputBTFMParameter(output_directory + "ConnectionFeature.txt", connect);
  OutputBTFMParameter(output_directory + "ThrowInFeature.txt", throw_in);

  //  盤上の位置
  OutputBTFMParameter(output_directory + "PosID.txt", pos_id);

  //  直前の手との距離
  OutputBTFMParameter(output_directory + "MoveDistance1.txt", move_distance_1);
  OutputBTFMParameter(output_directory + "MoveDistance2.txt", move_distance_2);
  OutputBTFMParameter(output_directory + "MoveDistance3.txt", move_distance_3);
  OutputBTFMParameter(output_directory + "MoveDistance4.txt", move_distance_4);

  //  近傍の配石パターン(3x3, md2, md3, md4, md5)
  OutputBTFMParameter(output_directory + "Pat3.txt", pat3);
  OutputBTFMParameter(output_directory + "MD2.txt", md2, md2_list, md2_target);
  OutputBTFMParameter(output_directory + "MD3.txt", md3, md3_list, md3_index, md3_target);
  OutputBTFMParameter(output_directory + "MD4.txt", md4, md4_list, md4_index, md4_target);
  OutputBTFMParameter(output_directory + "MD5.txt", md5, md5_list, md5_index, md5_target);
}


static void
OutputFMAdd( std::string filename, btfm_t &t )
{
  std::ofstream ofs(filename, std::ios::app);

  if (ofs.fail()) {
    std::cerr << "cannot open " << filename << std::endl;
    return;
  }

  ofs << std::scientific << t.w;
  for (int j = 0; j < FM_DIMENSION; j++) {
    ofs << " " << std::scientific << t.v[j];
  }
  ofs << std::endl;
}


static void
InitializeOutputFileMap( void )
{
  capture_file_name.insert(std::make_pair(UCT_SELF_ATARI_SMALL,          "SelfAtariSmall.txt"        ));
  capture_file_name.insert(std::make_pair(UCT_SELF_ATARI_LARGE,          "SelfAtariLarge.txt"        ));
  capture_file_name.insert(std::make_pair(UCT_SELF_ATARI_NAKADE,         "SelfAtariNakade.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_CAPTURE,                   "Capture.txt"               ));
  capture_file_name.insert(std::make_pair(UCT_SEMEAI_CAPTURE,            "SemeaiCapture.txt"         ));
  capture_file_name.insert(std::make_pair(UCT_KO_RECAPTURE,              "KoRecapture.txt"           ));
  capture_file_name.insert(std::make_pair(UCT_CAPTURE_AFTER_KO,          "CaptureAfterKo.txt"        ));
  capture_file_name.insert(std::make_pair(UCT_3POINT_CAPTURE_S_S,        "3PointCapture_S_S.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_2POINT_CAPTURE_S_S,        "2PointCapture_S_S.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_3POINT_CAPTURE_S_L,        "3PointCapture_S_L.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_2POINT_CAPTURE_S_L,        "2PointCapture_S_L.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_3POINT_CAPTURE_L_S,        "3PointCapture_L_S.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_2POINT_CAPTURE_L_S,        "2PointCapture_L_S.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_3POINT_CAPTURE_L_L,        "3PointCapture_L_L.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_2POINT_CAPTURE_L_L,        "2PointCapture_L_L.txt"     ));
  capture_file_name.insert(std::make_pair(UCT_SEMEAI_CAPTURE_SELF_ATARI, "SemeaiCaptureSelfAtari.txt"));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_1_1,          "SaveCapture_1_1.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_1_2,          "SaveCapture_1_2.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_1_3,          "SaveCapture_1_3.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_2_1,          "SaveCapture_2_1.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_2_2,          "SaveCapture_2_2.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_2_3,          "SaveCapture_2_3.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_3_1,          "SaveCapture_3_1.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_3_2,          "SaveCapture_3_2.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_3_3,          "SaveCapture_3_3.txt"       ));
  capture_file_name.insert(std::make_pair(UCT_SAVE_CAPTURE_SELF_ATARI,   "SaveCaptureSelfAtari.txt"  ));

  save_extension_file_name.insert(std::make_pair(UCT_SAVE_EXTENSION_1, "SaveExtension_1.txt"));
  save_extension_file_name.insert(std::make_pair(UCT_SAVE_EXTENSION_2, "SaveExtension_2.txt"));
  save_extension_file_name.insert(std::make_pair(UCT_SAVE_EXTENSION_3, "SaveExtension_3.txt"));
  save_extension_file_name.insert(std::make_pair(UCT_LADDER_EXTENSION, "LadderExtension.txt"));

  atari_file_name.insert(std::make_pair(UCT_ATARI,              "Atari.txt"                    ));
  atari_file_name.insert(std::make_pair(UCT_3POINT_ATARI_S_S,   "3PointAtari_S_S.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_2POINT_ATARI_S_S,   "2PointAtari_S_S.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_3POINT_ATARI_S_L,   "3PointAtari_S_L.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_2POINT_ATARI_S_L,   "2PointAtari_S_L.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_3POINT_ATARI_L_S,   "3PointAtari_L_S.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_2POINT_ATARI_L_S,   "2PointAtari_L_S.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_3POINT_ATARI_L_L,   "3PointAtari_L_L.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_2POINT_ATARI_L_L,   "2PointAtari_L_L.txt"          ));
  atari_file_name.insert(std::make_pair(UCT_3POINT_C_ATARI_S_S, "3PointCapturableAtari_S_S.txt"));
  atari_file_name.insert(std::make_pair(UCT_2POINT_C_ATARI_S_S, "2PointCapturableAtari_S_S.txt"));
  atari_file_name.insert(std::make_pair(UCT_3POINT_C_ATARI_S_L, "3PointCapturableAtari_S_L.txt"));
  atari_file_name.insert(std::make_pair(UCT_2POINT_C_ATARI_S_L, "2PointCapturableAtari_S_L.txt"));
  atari_file_name.insert(std::make_pair(UCT_3POINT_C_ATARI_L_S, "3PointCapturableAtari_L_S.txt"));
  atari_file_name.insert(std::make_pair(UCT_2POINT_C_ATARI_L_S, "2PointCapturableAtari_L_S.txt"));
  atari_file_name.insert(std::make_pair(UCT_3POINT_C_ATARI_L_L, "3PointCapturableAtari_L_L.txt"));
  atari_file_name.insert(std::make_pair(UCT_2POINT_C_ATARI_L_L, "2PointCapturableAtari_L_L.txt"));

  dame_file_name.insert(std::make_pair(UCT_3POINT_DAME_S_S, "3PointDame_S_S.txt"));
  dame_file_name.insert(std::make_pair(UCT_3POINT_DAME_S_L, "3PointDame_S_L.txt"));
  dame_file_name.insert(std::make_pair(UCT_3POINT_DAME_L_S, "3PointDame_L_S.txt"));
  dame_file_name.insert(std::make_pair(UCT_3POINT_DAME_L_L, "3PointDame_L_L.txt"));

  extension_file_name.insert(std::make_pair(UCT_2POINT_EXTENSION_DECREASE, "2PointExtensionDecreaseLarge.txt"));
  extension_file_name.insert(std::make_pair(UCT_2POINT_EXTENSION_EVEN,     "2PointExtensionEvenLarge.txt"    ));
  extension_file_name.insert(std::make_pair(UCT_2POINT_EXTENSION_INCREASE, "2PointExtensionIncreaseLarge.txt"));
  extension_file_name.insert(std::make_pair(UCT_3POINT_EXTENSION_DECREASE, "3PointExtensionDecreaseSmall.txt"));
  extension_file_name.insert(std::make_pair(UCT_3POINT_EXTENSION_EVEN,     "3PointExtensionEvenSmall.txt"    ));
  extension_file_name.insert(std::make_pair(UCT_3POINT_EXTENSION_INCREASE, "3PointExtensionIncreaseSmall.txt"));

  connect_file_name.insert(std::make_pair(UCT_KO_CONNECTION, "KoConnection.txt"));

  throw_in_file_name.insert(std::make_pair(UCT_THROW_IN_2, "Remove2Stones.txt"));
}
