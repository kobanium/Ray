#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>

#include "board/Point.hpp"
#include "common/Message.hpp"
#include "feature/Ladder.hpp"
#include "feature/Nakade.hpp"
#include "feature/Semeai.hpp"
#include "pattern/PatternHash.hpp"
#include "mcts/UctRating.hpp"
#include "util/Utility.hpp"


// パス
static fm_t uct_pass[UCT_PASS_MAX];

// 戦術的特徴
static fm_t uct_capture[UCT_CAPTURE_MAX];
static fm_t uct_save_extension[UCT_SAVE_EXTENSION_MAX];
static fm_t uct_atari[UCT_ATARI_MAX];
static fm_t uct_extension[UCT_EXTENSION_MAX];
static fm_t uct_dame[UCT_DAME_MAX];
static fm_t uct_connect[UCT_CONNECT_MAX];
static fm_t uct_throw_in[UCT_THROW_IN_MAX];

// 1手前の着手からの距離
static fm_t uct_move_distance_1[MOVE_DISTANCE_MAX * 4];
// 2手前の着手からの距離
static fm_t uct_move_distance_2[MOVE_DISTANCE_MAX * 4];
// 3手前の着手からの距離
static fm_t uct_move_distance_3[MOVE_DISTANCE_MAX * 4];
// 4手前の着手からの距離
static fm_t uct_move_distance_4[MOVE_DISTANCE_MAX * 4];

// 盤上の位置
static fm_t uct_pos_id[POS_ID_MAX];

// パターン
static fm_t uct_pat3[PAT3_MAX];
static fm_t uct_md2[LARGE_PAT_MAX];
static fm_t uct_md3[LARGE_PAT_MAX];
static fm_t uct_md4[LARGE_PAT_MAX];
static fm_t uct_md5[LARGE_PAT_MAX];

static fm_t uct_ko_exist;

// オーナーのレート
double uct_owner[OWNER_MAX];
// クリティカリティのレート
double uct_criticality[CRITICALITY_MAX];

static index_hash_t md3_index[HASH_MAX];
static index_hash_t md4_index[HASH_MAX];
static index_hash_t md5_index[HASH_MAX];

static int md2_index[MD2_MAX];

double criticality_init = CRITICALITY_INIT;
double criticality_bias = CRITICALITY_BIAS;
double owner_bias = OWNER_BIAS;
double owner_k = OWNER_K;


//  γ読み込み
static void InputUCTParameter( void );
//  読み込み
static void InputBTFMParameter( const char *filename, fm_t *params, const int n );
//  読み込み Pat3
static void InputPat3( const char *filename, fm_t *params );
//  読み込み MD2
static void InputMD2( const char *filename, fm_t *params );
//  読み込み
static void InputLargePattern( const char *filename, fm_t *params, index_hash_t *index );


//////////////////////
//  γ値の初期設定  //
//////////////////////
void
InitializeUctRating()
{
  //  γ読み込み
  InputUCTParameter();
}




static double
Gamma( std::vector<fm_t*> &params )
{
  double gamma = 1.0;

  for (const fm_t *param : params) {
    gamma *= param->w;
  }
  
  return gamma;
}


static double
Theta( std::vector<fm_t*> &t, const int i, const int j )
{
  const double inv = 1.0 / static_cast<double>(BTFM_DIMENSION);
  double theta = 0.0;

  for (int k = 0; k < BTFM_DIMENSION; k++) {
    theta += t[i]->v[k] * t[j]->v[k];
  }

  return theta * inv;
}


double
CalculateMoveScoreWithBTFM( const game_info_t *game, const int pos, const unsigned int *tactical_features, const int distance_index )
{
  const int moves = game->moves;
  const int pm1 = (moves > 1) ? game->record[moves - 1].pos : PASS;
  const int pm2 = (moves > 2) ? game->record[moves - 2].pos : PASS;
  const int pm3 = (moves > 3) ? game->record[moves - 3].pos : PASS;
  const int pm4 = (moves > 4) ? game->record[moves - 4].pos : PASS;
  int dis = 0;
  std::vector<fm_t*> active_features;
  pattern_hash_t hash_pat;

  if (pos == PASS) {
    if (moves > 1 && pm1 == PASS) {
      active_features.push_back(&uct_pass[UCT_PASS_AFTER_PASS]);
    } else {
      active_features.push_back(&uct_pass[UCT_PASS_AFTER_MOVE]);
    }
  } else {
    if (pm1 != PASS) {
      dis = DIS(pos, pm1);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&uct_move_distance_1[dis + distance_index]);
    }

    if (pm2 != PASS) {
      dis = DIS(pos, pm2);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&uct_move_distance_2[dis + distance_index]);
    }

    if (pm3 != PASS) {
      dis = DIS(pos, pm3);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&uct_move_distance_3[dis + distance_index]);
    }

    if (pm4 != PASS) {
      dis = DIS(pos, pm4);
      if (dis >= MOVE_DISTANCE_MAX - 1) {
        dis = MOVE_DISTANCE_MAX - 1;
      }
      active_features.push_back(&uct_move_distance_4[dis + distance_index]);
    }

    const unsigned int *features = &tactical_features[pos * UCT_INDEX_MAX];

    if (features[UCT_CAPTURE_INDEX]        > 0) active_features.push_back(&uct_capture[features[UCT_CAPTURE_INDEX]]);
    if (features[UCT_SAVE_EXTENSION_INDEX] > 0) active_features.push_back(&uct_save_extension[features[UCT_SAVE_EXTENSION_INDEX]]);
    if (features[UCT_ATARI_INDEX]          > 0) active_features.push_back(&uct_atari[features[UCT_ATARI_INDEX]]);
    if (features[UCT_EXTENSION_INDEX]      > 0) active_features.push_back(&uct_extension[features[UCT_EXTENSION_INDEX]]);
    if (features[UCT_DAME_INDEX]           > 0) active_features.push_back(&uct_dame[features[UCT_DAME_INDEX]]);
    if (features[UCT_CONNECT_INDEX]        > 0) active_features.push_back(&uct_connect[features[UCT_CONNECT_INDEX]]);
    if (features[UCT_THROW_IN_INDEX]       > 0) active_features.push_back(&uct_throw_in[features[UCT_THROW_IN_INDEX]]);

    PatternHash(&game->pat[pos], &hash_pat);
    const int pat3_idx = Pat3(game->pat, pos);
    const int md2_idx = md2_index[MD2(game->pat, pos)];
    const int md3_idx = SearchIndex(md3_index, hash_pat.list[MD_3]);
    const int md4_idx = SearchIndex(md4_index, hash_pat.list[MD_4]);
    const int md5_idx = SearchIndex(md5_index, hash_pat.list[MD_5 + MD_MAX]);

    // 盤上の位置
    active_features.push_back(&uct_pos_id[board_pos_id[pos]]);

    // パターン
    if (md5_idx != -1) {
      active_features.push_back(&uct_md5[md5_idx]);
    } else if (md4_idx != -1) {
      active_features.push_back(&uct_md4[md4_idx]);
    } else if (md3_idx != -1) {
      active_features.push_back(&uct_md3[md3_idx]);
    } else if (md2_idx != -1) {
      active_features.push_back(&uct_md2[md2_idx]);
    } else {
      active_features.push_back(&uct_pat3[pat3_idx]);
    }
  }

  if (moves > 1 && game->ko_move == game->moves - 1) {
    active_features.push_back(&uct_ko_exist);
  }

  const int feature_num = active_features.size();
  const double gamma = Gamma(active_features);
  double theta = feature_num > 1 ? 1.0 : 0.0;

  for (int i = 0; i < feature_num - 1; i++) {
    for (int j = i + 1; j < feature_num; j++) {
      theta *= Theta(active_features, i, j);
    }
  }

  return gamma + theta;
}


//////////////////////////////////////////
//  着手予想の精度を確認するための関数  //
//////////////////////////////////////////
void
AnalyzeUctRating( game_info_t *game, int color, double rate[] )
{
  const int moves = game->moves;
  unsigned int tactical_features[BOARD_MAX * UCT_INDEX_MAX] = {0};  
  const int distance_index = CheckFeaturesForTree(game, color, tactical_features);

  CheckRemove2StonesForTree(game, color, tactical_features);
  if (moves > 2 && game->ko_move == moves - 2) {
    CheckCaptureAfterKoForTree(game, color, tactical_features);
    CheckKoConnectionForTree(game, tactical_features);
  } else if (moves > 3 && game->ko_move == moves - 3) {
    CheckKoRecaptureForTree(game, color, tactical_features);
  }

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (IsLegal(game, pos, color)) {
      CheckSelfAtariForTree(game, color, pos, tactical_features);
      CheckCaptureForTree(game, color, pos, tactical_features);
      CheckAtariForTree(game, color, pos, tactical_features);

      rate[i] = CalculateMoveScoreWithBTFM(game, pos, tactical_features, distance_index);
    } else {
      rate[i] = 0.0;
    }
  }
  rate[pure_board_max] = CalculateMoveScoreWithBTFM(game, PASS, tactical_features, distance_index);
}

//////////////////
//  γ読み込み  //
//////////////////
void
InputUCTParameter(void)
{
  std::string uct_parameters_path = GetWorkingDirectory() + PATH_SEPARATOR + "uct_params" + PATH_SEPARATOR;
  std::string path;

  // 劫があるときの特徴
  path = uct_parameters_path + "KoExist.txt";
  InputBTFMParameter(path.c_str(), &uct_ko_exist, 1);

  // パス
  path = uct_parameters_path + "Pass.txt";
  InputBTFMParameter(path.c_str(), uct_pass, UCT_PASS_MAX);

  // 戦術的特徴
  path = uct_parameters_path + "CaptureFeature.txt";
  InputBTFMParameter(path.c_str(), uct_capture, UCT_CAPTURE_MAX);
  path = uct_parameters_path + "SaveExtensionFeature.txt";
  InputBTFMParameter(path.c_str(), uct_save_extension, UCT_SAVE_EXTENSION_MAX);
  path = uct_parameters_path + "AtariFeature.txt";
  InputBTFMParameter(path.c_str(), uct_atari, UCT_ATARI_MAX);
  path = uct_parameters_path + "ExtensionFeature.txt";
  InputBTFMParameter(path.c_str(), uct_extension, UCT_EXTENSION_MAX);
  path = uct_parameters_path + "DameFeature.txt";
  InputBTFMParameter(path.c_str(), uct_dame, UCT_DAME_MAX);
  path = uct_parameters_path + "ConnectionFeature.txt";
  InputBTFMParameter(path.c_str(), uct_connect, UCT_CONNECT_MAX);
  path = uct_parameters_path + "ThrowInFeature.txt";
  InputBTFMParameter(path.c_str(), uct_throw_in, UCT_THROW_IN_MAX);

  /*
  for (int i = 0; i < UCT_CAPTURE_MAX; i++) {
    fm_t &feature = uct_capture[i];
    std::cerr << feature.w;
    for (int j = 0; j < BTFM_DIMENSION; j++) {
      std::cerr << " " << feature.v[j];
    }
    std::cerr << std::fixed << std::endl;
  }
  */

  // 盤上の位置
  path = uct_parameters_path + "PosID.txt";
  InputBTFMParameter(path.c_str(), uct_pos_id, POS_ID_MAX);

  // 着手距離
  path = uct_parameters_path + "MoveDistance1.txt";
  InputBTFMParameter(path.c_str(), uct_move_distance_1, MOVE_DISTANCE_MAX * 4);
  path = uct_parameters_path + "MoveDistance2.txt";
  InputBTFMParameter(path.c_str(), uct_move_distance_2, MOVE_DISTANCE_MAX * 4);
  path = uct_parameters_path + "MoveDistance3.txt";
  InputBTFMParameter(path.c_str(), uct_move_distance_3, MOVE_DISTANCE_MAX * 4);
  path = uct_parameters_path + "MoveDistance4.txt";
  InputBTFMParameter(path.c_str(), uct_move_distance_4, MOVE_DISTANCE_MAX * 4);

  //  3x3パターン
  path = uct_parameters_path + "Pat3.txt";
  InputPat3(path.c_str(), uct_pat3);
  //  マンハッタン距離2のパターン
  path = uct_parameters_path + "MD2.txt";
  InputMD2(path.c_str(), uct_md2);
  //  マンハッタン距離3のパターン
  path = uct_parameters_path + "MD3.txt";
  InputLargePattern(path.c_str(), uct_md3, md3_index);
  //  マンハッタン距離4のパターン
  path = uct_parameters_path + "MD4.txt";
  InputLargePattern(path.c_str(), uct_md4, md4_index);
  //  マンハッタン距離5のパターン
  path = uct_parameters_path + "MD5.txt";
  InputLargePattern(path.c_str(), uct_md5, md5_index);

  //  Owner
  for (int i = 0; i < OWNER_MAX; i++) {
    uct_owner[i] = owner_k * exp(-pow(i - 5, 2) / owner_bias);
  }

  //  Criticality
  for (int i = 0; i < CRITICALITY_MAX; i++) {
    uct_criticality[i] = exp(criticality_bias * i);
  }
}


//////////////////////////
//  パラメータ読み込み  //
//////////////////////////
static void
InputBTFMParameter( const char *filename, fm_t params[], const int n )
{
  FILE *fp;

#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < n; i++) {
    if (fscanf_s(fp, "%lf", &params[i].w) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    for (int j = 0; j < BTFM_DIMENSION; j++) {
      if (fscanf_s(fp, "%lf", &params[i].v[j]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < n; i++) {
    if (fscanf(fp, "%lf", &params[i].w) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    for (int j = 0; j < BTFM_DIMENSION; j++) {
      if (fscanf(fp, "%lf", &params[i].v[j]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
  }
#endif
  fclose(fp);
}



//////////////////////
//  γ読み込み Pat3  //
//////////////////////
static void
InputPat3( const char *filename, fm_t params[] )
{
  FILE *fp;
  double weight;

#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  for (unsigned int pat3 = 0; pat3 < static_cast<unsigned int>(PAT3_MAX); pat3++) {
    if (fscanf_s(fp, "%lf", &weight) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    params[pat3].w = weight;   
    for (int i = 0; i < BTFM_DIMENSION; i++) {
      if (fscanf_s(fp, "%lf", &params[pat3].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  for (unsigned int pat3 = 0; pat3 < static_cast<unsigned int>(PAT3_MAX); pat3++) {
    if (fscanf(fp, "%lf", &weight) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    params[pat3].w = weight;
    for (int i = 0; i < BTFM_DIMENSION; i++) {
      if (fscanf(fp, "%lf", &params[pat3].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
  }
#endif
  fclose(fp);
}


//////////////////////
//  γ読み込み MD2  //
//////////////////////
static void
InputMD2( const char *filename, fm_t params[] )
{
  FILE *fp;
  int index, counter = 0;
  unsigned int md2_transp16[16];

  for (unsigned int md2 = 0; md2 < static_cast<unsigned int>(MD2_MAX); md2++) {
    md2_index[md2] = -1;
  }
#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  while (fscanf_s(fp, "%d%lf", &index, &weight) != EOF) {
    idx = md2_index[index];
    params[idx].w = weight;
    for (int i = 0; i < BTFM_DIMENSION; i++) {
      if (fscanf_s(fp, "%lf", &params[idx].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  while (fscanf(fp, "%d%lf", &index, &params[counter].w) != EOF) {
    for (int i = 0; i < BTFM_DIMENSION; i++) {
      if (fscanf(fp, "%lf", &params[counter].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
    MD2Transpose16(index, md2_transp16);

    for (int i = 0; i < 16; i++) {
      const unsigned int idx = md2_transp16[i];
      md2_index[idx] = counter;
    }
    counter++;
  }
#endif
  fclose(fp);
}


///////////////////////////////////
//  パラメータ読み込み パターン  //
///////////////////////////////////
static void 
InputLargePattern( const char *filename, fm_t params[], index_hash_t pat_index[] )
{
  FILE *fp;
  int index, idx = 0;
  unsigned long long hash;
  double weight;

  for (int i = 0; i < HASH_MAX; i++) {
    pat_index[i].hash = 0;
    pat_index[i].index = -1;
  }

#if defined (_WIN32)
  errno_t err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  while (fscanf_s(fp, "%d%llu%lf", &index, &hash, &weight) != EOF) {
    pat_index[index].hash = hash;
    pat_index[index].index = idx;
    params[idx].w = weight;
    for (int i = 0; i < BTFM_DIMENSION; i++) {
      if (fscanf_s(fp, "%lf", &params[idx].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
    idx++;
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  while (fscanf(fp, "%d%llu%lf", &index, &hash, &weight) != EOF) {
    pat_index[index].hash = hash;
    pat_index[index].index = idx;
    params[idx].w = weight;
    for (int i = 0; i < BTFM_DIMENSION; i++) {
      if (fscanf(fp, "%lf", &params[idx].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
    idx++;
  }
#endif
  fclose(fp);
}
