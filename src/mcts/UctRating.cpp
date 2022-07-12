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


// w_0
static double weight_zero;
// 戦術的特徴のレート
static latent_factor_t uct_tactical_features[UCT_TACTICAL_FEATURE_MAX];
// 盤上の位置のレート
static latent_factor_t uct_pos_id[POS_ID_MAX];
// パスのレート
static latent_factor_t uct_pass[UCT_PASS_MAX];
// 直前の着手からの距離のレート
static latent_factor_t uct_move_distance_1[MOVE_DISTANCE_MAX];
// 2手前の着手からの距離のレート
static latent_factor_t uct_move_distance_2[MOVE_DISTANCE_MAX];
// 3x3パターンのレート
static latent_factor_t uct_pat3[PAT3_LIMIT];
// マンハッタン距離2のパターンのレート
static latent_factor_t uct_md2[MD2_LIMIT];
// マンハッタン距離3のパターンのレート
static latent_factor_t uct_md3[LARGE_PAT_MAX];
// マンハッタン距離4のパターンのレート
static latent_factor_t uct_md4[LARGE_PAT_MAX];
// マンハッタン距離5のパターンのレート
static latent_factor_t uct_md5[LARGE_PAT_MAX];
// オーナーのレート
double uct_owner[OWNER_MAX];
// クリティカリティのレート
double uct_criticality[CRITICALITY_MAX];

index_hash_t md3_index[HASH_MAX];
index_hash_t md4_index[HASH_MAX];
index_hash_t md5_index[HASH_MAX];

static int pat3_index[PAT3_MAX];
static int md2_index[MD2_MAX];

double criticality_init = CRITICALITY_INIT;
double criticality_bias = CRITICALITY_BIAS;
double owner_bias = OWNER_BIAS;
double owner_k = OWNER_K;

unsigned long long atari_mask, capture_mask;

//  γ読み込み
static void InputUCTParameter( void );
//  読み込み 
static void InputLatentFactor( const char *filename, latent_factor_t *lf, int n );
//  読み込み Pat3
static void InputPat3( const char *filename, latent_factor_t *lf );
//  読み込み MD2
static void InputMD2( const char *filename, latent_factor_t *lf );
//  読み込み
static void InputLargePattern( const char *filename, latent_factor_t *lf, index_hash_t *pat_index );


//////////////////////
//  γ値の初期設定  //
//////////////////////
void
InitializeUctRating()
{
  //  γ読み込み
  InputUCTParameter();

  for (int i = UCT_SAVE_CAPTURE_1_1; i <= UCT_SEMEAI_CAPTURE; i++) {
    capture_mask |= bit_mask[i];
  }

  for (int i = UCT_ATARI; i <= UCT_3POINT_C_ATARI_L_L; i++) {
    atari_mask |= bit_mask[i];
  }
}


double
CalculateLFRScore( game_info_t *game, int pos, int index[3], uct_features_t *uct_features )
{
  const int moves = game->moves;
  const pattern_t *pat = game->pat;
  int pm1 = PASS, pm2 = PASS, dis1 = -1, dis2 = -1, feature_num = 0;
  double score = weight_zero, tmp_score;
  unsigned long long *tactical_features1 = uct_features->tactical_features1;
  unsigned int pat3, md2;
  latent_factor_t *all_feature[UCT_TACTICAL_FEATURE_MAX + 6];

  if (moves > 1) pm1 = game->record[moves - 1].pos;
  if (moves > 2) pm2 = game->record[moves - 2].pos;

  // パスの時の分岐
  if (pos == PASS) {
    if (moves > 1 && pm1 == PASS) {
      score += uct_pass[UCT_PASS_AFTER_PASS].w;
    } else {
      score += uct_pass[UCT_PASS_AFTER_MOVE].w;
    }
    return score;
  }

  if (moves > 1 && pm1 != PASS) {
    dis1 = DIS(pm1, pos);
    if (dis1 >= MOVE_DISTANCE_MAX - 1) {
      dis1 = MOVE_DISTANCE_MAX - 1;
    }
  }

  if (moves > 2 && pm2 != PASS) {
    dis2 = DIS(pm2, pos);
    if (dis2 >= MOVE_DISTANCE_MAX - 1) {
      dis2 = MOVE_DISTANCE_MAX - 1;
    }
  }

  pat3 = pat3_index[Pat3(pat, pos)];
  md2 = md2_index[MD2(pat, pos)];

  // 特徴を
  for (int i = 0; i < UCT_TACTICAL_FEATURE_MAX; i++) {
    if ((tactical_features1[pos] & bit_mask[i]) != 0) {
      all_feature[feature_num++] = &uct_tactical_features[i];
    }
  }
  // 盤上の位置
  all_feature[feature_num++] = &uct_pos_id[board_pos_id[pos]];
  // 1手前からの距離
  if (dis1 != -1) {
    all_feature[feature_num++] = &uct_move_distance_1[dis1];
  }
  // 2手前からの距離
  if (dis2 != -1) {
    all_feature[feature_num++] = &uct_move_distance_2[dis2];
  }
  // パターン
  if (index[2] != -1) {
    all_feature[feature_num++] = &uct_md5[index[2]];
  } else if (index[1] != -1) {
    all_feature[feature_num++] = &uct_md4[index[1]];
  } else if (index[0] != -1) {
    all_feature[feature_num++] = &uct_md3[index[0]];
  } else if (uct_md2[md2].w != 0.0) {
    all_feature[feature_num++] = &uct_md2[md2];
  } else {
    all_feature[feature_num++] = &uct_pat3[pat3];
  }

  // wの足し算
  for (int i = 0; i < feature_num; i++) {
    score += all_feature[i]->w;
  }

  // vの計算
  for (int f = 0; f < LFR_DIMENSION; f++) {
    for (int i = 0; i < feature_num; i++) {
      tmp_score = 0.0;
      for (int j = i + 1; j < feature_num; j++) {
        tmp_score += all_feature[j]->v[f];
      }
      score += tmp_score * all_feature[i]->v[f];
    }
  }

  return score;
}


//////////////////////////////////////////
//  着手予想の精度を確認するための関数  //
//////////////////////////////////////////
void
AnalyzeUctRating( game_info_t *game, int color, double rate[] )
{
  const int moves = game->moves;
  int pat_index[3];
  pattern_hash_t hash_pat;
  uct_features_t uct_features;

  memset(&uct_features, 0, sizeof(uct_features_t));

  CheckFeaturesForTree(game, color, &uct_features);
  CheckRemove2StonesForTree(game, color, &uct_features);
  CheckRemove3StonesForTree(game, color, &uct_features);
  if (game->ko_move == moves - 2) {
    CheckCaptureAfterKoForTree(game, color, &uct_features);
    CheckKoConnectionForTree(game, &uct_features);
  }

  for (int i = 0; i < pure_board_max; i++) {
    const int pos = onboard_pos[i];
    if (!game->candidates[pos] || !IsLegal(game, pos, color)) {
      rate[i] = 0;
      continue;
    }

    CheckSelfAtariForTree(game, color, pos, &uct_features);
    CheckSnapBackForTree(game, color, pos, &uct_features);
    if ((uct_features.tactical_features1[pos] & capture_mask)== 0) CheckCaptureForTree(game, color, pos, &uct_features);
    if ((uct_features.tactical_features1[pos] & atari_mask) == 0) CheckAtariForTree(game, color, pos, &uct_features);
    CheckDoubleKeimaForTree(game, color, pos, &uct_features);
    CheckKeimaTsukekoshiForTree(game, color, pos, &uct_features);

    //  Pattern
    PatternHash(&game->pat[pos], &hash_pat);
    pat_index[0] = SearchIndex(md3_index, hash_pat.list[MD_3]);
    pat_index[1] = SearchIndex(md4_index, hash_pat.list[MD_4]);
    pat_index[2] = SearchIndex(md5_index, hash_pat.list[MD_5 + MD_MAX]);

    rate[i] = CalculateLFRScore(game, pos, pat_index, &uct_features);
  }
}

//////////////////
//  γ読み込み  //
//////////////////
void
InputUCTParameter(void)
{
  std::string uct_parameters_path = GetWorkingDirectory() + PATH_SEPARATOR + "uct_params" + PATH_SEPARATOR;
  std::string path;

  path = uct_parameters_path + "WeightZero.txt";

  //  W_0
  InputTxtDBL(path.c_str(), &weight_zero, 1);

  //  戦術的特徴
  path = uct_parameters_path + "TacticalFeature.txt";
  InputLatentFactor(path.c_str(), uct_tactical_features, UCT_TACTICAL_FEATURE_MAX);

  // 盤上の位置
  path = uct_parameters_path + "PosID.txt";
  InputLatentFactor(path.c_str(), uct_pos_id, POS_ID_MAX);

  // パス
  path = uct_parameters_path + "Pass.txt";
  InputLatentFactor(path.c_str(), uct_pass, UCT_PASS_MAX);

  //  直前の手との距離
  path = uct_parameters_path + "MoveDistance1.txt";
  InputLatentFactor(path.c_str(), uct_move_distance_1, MOVE_DISTANCE_MAX);

  //  2手前の手との距離
  path = uct_parameters_path + "MoveDistance2.txt";
  InputLatentFactor(path.c_str(), uct_move_distance_2, MOVE_DISTANCE_MAX);

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
    uct_criticality[i] = exp(criticality_bias * i) - 1;
  }
}


///////////////////////////
//  γ読み込み 着手距離  //
///////////////////////////
static void
InputLatentFactor( const char *filename, latent_factor_t *lf, int n )
{
  FILE *fp;

#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < n; i++) {
    if (fscanf_s(fp, "%lf", &lf[i].w) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    for (int j = 0; j < LFR_DIMENSION; j++) {
      if (fscanf_s(fp, "%lf", &lf[i].v[j]) == EOF) {
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
    if (fscanf(fp, "%lf", &lf[i].w) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    for (int j = 0; j < LFR_DIMENSION; j++) {
      if (fscanf(fp, "%lf", &lf[i].v[j]) == EOF) {
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
InputPat3( const char *filename, latent_factor_t *lf )
{
  FILE *fp;
  int idx = 0;
  double weight;
  unsigned int pat3_transp16[16];

  for (unsigned int pat3 = 0; pat3 < (unsigned int)PAT3_MAX; pat3++) {
    pat3_index[pat3] = -1;
  }

  for (unsigned int pat3 = 0; pat3 < (unsigned int)PAT3_MAX; pat3++) {
    if (pat3_index[pat3] == -1) {
      Pat3Transpose16(pat3, pat3_transp16);
      for (int i = 0; i < 16; i++) {
        pat3_index[pat3_transp16[i]] = idx;
      }
      idx++;
    }
  }

#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  for (unsigned int pat3 = 0; pat3 < (unsigned int)PAT3_MAX; pat3++) {
    if (fscanf_s(fp, "%lf", &weight) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    idx = pat3_index[pat3];
    lf[idx].w = weight;   
    for (int i = 0; i < LFR_DIMENSION; i++) {
      if (fscanf_s(fp, "%lf", &lf[idx].v[i]) == EOF) {
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
  for (unsigned int pat3 = 0; pat3 < (unsigned int)PAT3_MAX; pat3++) {
    if (fscanf(fp, "%lf", &weight) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
    idx = pat3_index[pat3];
    lf[idx].w = weight;
    for (int i = 0; i < LFR_DIMENSION; i++) {
      if (fscanf(fp, "%lf", &lf[idx].v[i]) == EOF) {
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
InputMD2( const char *filename, latent_factor_t *lf )
{
  FILE *fp;
  int index, idx = 0;
  double weight;
  unsigned int md2_transp16[16];

  for (unsigned int md2 = 0; md2 < (unsigned int)MD2_MAX; md2++) {
    md2_index[md2] = -1;
  }

  for (unsigned int md2 = 0; md2 < (unsigned int)MD2_MAX; md2++) {
    if (md2_index[md2] == -1) {
      MD2Transpose16(md2, md2_transp16);
      for (int i = 0; i < 16; i++) {
        md2_index[md2_transp16[i]] = idx;
      }
      idx++;
    }
  }

#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  while (fscanf_s(fp, "%d%lf", &index, &weight) != EOF) {
    idx = md2_index[index];
    lf[idx].w = weight;
    for (int i = 0; i < LFR_DIMENSION; i++) {
      if (fscanf_s(fp, "%lf", &lf[idx].v[i]) == EOF) {
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
  while (fscanf(fp, "%d%lf", &index, &weight) != EOF) {
    idx = md2_index[index];
    lf[idx].w = weight;
    for (int i = 0; i < LFR_DIMENSION; i++) {
      if (fscanf(fp, "%lf", &lf[idx].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
  }
#endif
  fclose(fp);
}


//  読み込み
static void 
InputLargePattern( const char *filename, latent_factor_t *lf, index_hash_t *pat_index )
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
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  while (fscanf_s(fp, "%d%llu%lf", &index, &hash, &weight) != EOF) {
    pat_index[index].hash = hash;
    pat_index[index].index = idx;
    lf[idx].w = weight;
    for (int i = 0; i < LFR_DIMENSION; i++) {
      if (fscanf_s(fp, "%lf", &lf[idx].v[i]) == EOF) {
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
    lf[idx].w = weight;
    for (int i = 0; i < LFR_DIMENSION; i++) {
      if (fscanf(fp, "%lf", &lf[idx].v[i]) == EOF) {
        std::cerr << "Read Error : " << filename << std::endl;
        exit(1);
      }
    }
    idx++;
  }
#endif
  fclose(fp);
}
