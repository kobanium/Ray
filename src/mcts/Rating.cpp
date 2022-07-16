#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <climits>
#include <algorithm>
#include <functional>
#include <string>

#include "board/Point.hpp"
#include "common/Message.hpp"
#include "feature/Nakade.hpp"
//#include "feature/Semeai.hpp"
#include "feature/SimulationFeature.hpp"
#include "mcts/Rating.hpp"
#include "util/Utility.hpp"


////////////////
//    変数    //
////////////////

// 3x3パターンのγ値
static float po_pat3[PAT3_MAX];
// MD2のパターンのγ値
static float po_md2[MD2_MAX];
// 3x3とMD2のパターンのγ値の積
static float po_pattern[MD2_MAX];

// 学習した着手距離の特徴 
static float po_neighbor_orig[PREVIOUS_DISTANCE_MAX];
// 補正した着手距離の特徴
static float po_previous_distance[PREVIOUS_DISTANCE_MAX];
// MD2パターンが届く範囲
static int neighbor[UPDATE_NUM];

// 戦術的特徴
static float sim_capture[SIM_CAPTURE_MAX];
static float sim_save_extension[SIM_SAVE_EXTENSION_MAX];
static float sim_atari[SIM_ATARI_MAX];
static float sim_extension[SIM_EXTENSION_MAX];
static float sim_dame[SIM_DAME_MAX];
static float sim_throw_in[SIM_THROW_IN_MAX];



// 着手距離2, 3のγ値の補正
static double neighbor_bias = NEIGHBOR_BIAS;
// 着手距離4のγ値の補正
static double jump_bias = JUMP_BIAS;


//////////////////
//  関数の宣言  //
//////////////////

static void Neighbor12( const int previous_move, int distance_2[], int distance_3[], int distance_4[] );

//  γ読み込み
static void LoadFeatureParameters( void );

static void LoadMD2Parameter( const char *filename, float params[] );

static void LoadParameter( const char *file, float params[], const int array_size );

/////////////////
// 近傍の設定  //
/////////////////
void
SetNeighbor( void )
{
  neighbor[ 0] = -2 * board_size;
  neighbor[ 1] = - board_size - 1;
  neighbor[ 2] = - board_size;
  neighbor[ 3] = - board_size + 1;
  neighbor[ 4] = -2;
  neighbor[ 5] = -1;
  neighbor[ 6] = 0;
  neighbor[ 7] = 1;
  neighbor[ 8] = 2;
  neighbor[ 9] = board_size - 1;
  neighbor[10] = board_size;
  neighbor[11] = board_size + 1;
  neighbor[12] = 2 * board_size;

  SetCrossPosition();
}

//////////////
//  初期化  //
//////////////
void
InitializeRating( void )
{
  // γ読み込み
  LoadFeatureParameters();
}

inline double
CalculateTacticalFeatures( unsigned char *features )
{
  double rate =
    sim_capture[features[CAPTURE]] *
    sim_save_extension[features[SAVE_EXTENSION]] *
    sim_atari[features[ATARI]] *
    sim_extension[features[EXTENSION]] *
    sim_dame[features[DAME]] *
    sim_throw_in[features[THROW_IN]];

  return rate;
}


//////////////////////
//  着手( rating )  // 
//////////////////////
int
RatingMove( game_info_t *game, int color, std::mt19937_64 &mt )
{
  long long *rate = game->rate[color - 1];
  long long *sum_rate_row = game->sum_rate_row[color - 1];
  long long *sum_rate = &game->sum_rate[color - 1];
  int y, pos;
  long long rand_num;

  // レートの部分更新
  PartialRating(game, color, sum_rate, sum_rate_row, rate);

  // 合法手を選択するまでループ
  while (true){
    if (*sum_rate == 0) return PASS;

    rand_num = (mt() % (*sum_rate)) + 1;

    // 縦方向の位置を求める
    y = board_start;
    while (rand_num > sum_rate_row[y]){
      rand_num -= sum_rate_row[y++];
    }

    // 横方向の位置を求める
    pos = POS(board_start, y);
    do{
      rand_num -= rate[pos];
      if (rand_num <= 0) break;
      pos++;
    } while (true);

    // 選ばれた手が合法手ならループを抜け出し
    // そうでなければその箇所のレートを0にし, 手を選びなおす
    if (IsLegalNotEye(game, pos, color)) {
      break;
    } else {
      *sum_rate -= rate[pos];
      sum_rate_row[y] -= rate[pos];
      rate[pos] = 0;
    }
  }

  return pos;
}


////////////////////////////
//  12近傍の座標を求める  //
////////////////////////////
static void
Neighbor12( const int previous_move, int distance_2[], int distance_3[], int distance_4[] )
{
  // 着手距離2の座標
  distance_2[0] = previous_move + neighbor[ 2];
  distance_2[1] = previous_move + neighbor[ 5];
  distance_2[2] = previous_move + neighbor[ 7];
  distance_2[3] = previous_move + neighbor[10];

  // 着手距離3の座標
  distance_3[0] = previous_move + neighbor[ 1];
  distance_3[1] = previous_move + neighbor[ 3];
  distance_3[2] = previous_move + neighbor[ 9];
  distance_3[3] = previous_move + neighbor[11];

  // 着手距離4の座標
  distance_4[0] = previous_move + neighbor[ 0];
  distance_4[1] = previous_move + neighbor[ 4];
  distance_4[2] = previous_move + neighbor[ 8];
  distance_4[3] = previous_move + neighbor[12];
}


//////////////////////////////
//  直前の着手の周辺の更新  //
//////////////////////////////
void
NeighborUpdate( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate, int *update, bool *flag, int index )
{
  int pos;
  double gamma, bias[4];
  bool self_atari_flag;

  bias[0] = bias[1] = bias[2] = bias[3] = 1.0;

  // 盤端での特殊処理
  if (index == 1) {
    pos = game->record[game->moves - 1].pos;
    if ((border_dis_x[pos] == 1 && border_dis_y[pos] == 2) ||
        (border_dis_x[pos] == 2 && border_dis_y[pos] == 1)) {
      for (int i = 0; i < 4; i++) {
        if ((border_dis_x[update[i]] == 1 && border_dis_y[update[i]] == 2) ||
            (border_dis_x[update[i]] == 2 && border_dis_y[update[i]] == 1)) {
          bias[i] = 1000.0;
          break;
        }
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    pos = update[i];
    if (game->candidates[pos]){
      if (flag[pos] && bias[i] == 1.0) continue;
      self_atari_flag = CheckSelfAtariForSimulation(game, color, pos);

      // 元あったレートを消去
      *sum_rate -= rate[pos];
      sum_rate_row[board_y[pos]] -= rate[pos];

      if (!self_atari_flag){
        rate[pos] = 0;
      } else {
        CheckCaptureAndAtariForSimulation(game, color, pos);

        gamma = po_pattern[MD2(game->pat, pos)] * po_previous_distance[index];
        gamma *= CalculateTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
        gamma *= bias[i];
        rate[pos] = (long long)(gamma) + 1;

        // 新たに計算したレートを代入
        *sum_rate += rate[pos];
        sum_rate_row[board_y[pos]] += rate[pos];
      }

      ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
    }
    flag[pos] = true;
  }
}


//////////////////////////
//  ナカデの急所の更新  //
//////////////////////////
void
NakadeUpdate( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate, int *nakade_pos, int nakade_num, bool *flag, int pm1 )
{
  int pos, dis;
  double gamma;
  bool self_atari_flag;

  for (int i = 0; i < nakade_num; i++) {
    pos = nakade_pos[i];
    if (pos != NOT_NAKADE && game->candidates[pos]){
      self_atari_flag = CheckSelfAtariForSimulation(game, color, pos);

      // 元あったレートを消去
      *sum_rate -= rate[pos];
      sum_rate_row[board_y[pos]] -= rate[pos];

      if (!self_atari_flag) {
        rate[pos] = 0;
      } else {
        CheckCaptureAndAtariForSimulation(game, color, pos);
        dis = DIS(pm1, pos);
        if (dis < 5) {
          gamma = 10000.0 * po_previous_distance[dis - 2];
        } else {
          gamma = 10000.0;
        }
        gamma *= po_pattern[MD2(game->pat, pos)];
        gamma *= CalculateTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
        rate[pos] = (long long)(gamma) + 1;
        // 新たに計算したレートを代入      
        *sum_rate += rate[pos];
        sum_rate_row[board_y[pos]] += rate[pos];     
      }

      ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
      flag[pos] = true;
    }
  }
}


////////////////////
//  レートの更新  //
////////////////////
void
OtherUpdate( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate, int update_num, int *update, bool *flag )
{
  int pos;
  double gamma;
  bool self_atari_flag;

  for (int i = 0; i < update_num; i++) {
    pos = update[i];
    if (flag[pos]) continue;

    if (game->candidates[pos]) {
      self_atari_flag = CheckSelfAtariForSimulation(game, color, pos);

      // 元あったレートを消去
      *sum_rate -= rate[pos];
      sum_rate_row[board_y[pos]] -= rate[pos];

      // パターン、戦術的特徴、距離のγ値
      if (!self_atari_flag) {
        rate[pos] = 0;
      } else {
        CheckCaptureAndAtariForSimulation(game, color, pos);
        gamma = po_pattern[MD2(game->pat, pos)];
        gamma *= CalculateTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
        rate[pos] = (long long)(gamma) + 1;

        // 新たに計算したレートを代入
        *sum_rate += rate[pos];
        sum_rate_row[board_y[pos]] += rate[pos];
      }

      ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
    }
    // 更新済みフラグを立てる
    flag[pos] = true;
  }
}


/////////////////////////////////
//  MD2パターンの範囲内の更新  //
/////////////////////////////////
void
Neighbor12Update( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate, int update_num, int *update, bool *flag )
{
  int pos;
  double gamma;
  bool self_atari_flag;

  for (int i = 0; i < update_num; i++) {
    for (int j = 0; j < UPDATE_NUM; j++) {
      pos = update[i] + neighbor[j];
      if (flag[pos]) continue;

      if (game->candidates[pos]) {
        self_atari_flag = CheckSelfAtariForSimulation(game, color, pos);

        // 元あったレートを消去
        *sum_rate -= rate[pos];
        sum_rate_row[board_y[pos]] -= rate[pos];

        // パターン、戦術的特徴、距離のγ値
        if (!self_atari_flag){
          rate[pos] = 0;
        } else {
          CheckCaptureAndAtariForSimulation(game, color, pos);
          gamma = po_pattern[MD2(game->pat, pos)];
          gamma *= CalculateTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
          rate[pos] = (long long)(gamma) + 1;

          // 新たに計算したレートを代入
          *sum_rate += rate[pos];
          sum_rate_row[board_y[pos]] += rate[pos];
        }

        ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
      }
      // 更新済みフラグを立てる
      flag[pos] = true;
    }
  }
}


////////////////
//  部分更新  //
////////////////
void
PartialRating( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate )
{
  const int other = GetOppositeColor(color);
  int pm1 = PASS, pm2 = PASS, pm3 = PASS;
  int distance_2[4], distance_3[4], distance_4[4];
  bool flag[BOARD_MAX] = { false };  
  int *update_pos = game->update_pos[color];
  int *update_num = &game->update_num[color];
  int nakade_pos[4] = { 0 };
  int nakade_num = 0;
  int prev_feature = game->update_num[color];
  int prev_feature_pos[PURE_BOARD_MAX];

  for (int i = 0; i < prev_feature; i++){
    prev_feature_pos[i] = update_pos[i];
  }

  *update_num = 0;

  pm1 = game->record[game->moves - 1].pos;
  if (game->moves > 2) pm2 = game->record[game->moves - 2].pos;
  if (game->moves > 3) pm3 = game->record[game->moves - 3].pos;

  if (game->ko_move == game->moves - 2){
    CheckCaptureAfterKoForSimulation(game, color, update_pos, update_num);
  }

  if (pm1 != PASS) {
    Neighbor12(pm1, distance_2, distance_3, distance_4);
    CheckFeaturesForSimulation(game, color, update_pos, update_num);
    CheckRemove2StonesForSimulation(game, color, update_pos, update_num);

    SearchNakade(game, &nakade_num, nakade_pos);
    NakadeUpdate(game, color, sum_rate, sum_rate_row, rate, nakade_pos, nakade_num, flag, pm1);
    // 着手距離2の更新
    NeighborUpdate(game, color, sum_rate, sum_rate_row, rate, distance_2, flag, 0);
    // 着手距離3の更新
    NeighborUpdate(game, color, sum_rate, sum_rate_row, rate, distance_3, flag, 1);
    // 着手距離4の更新
    NeighborUpdate(game, color, sum_rate, sum_rate_row, rate, distance_4, flag, 2);

  }

  // 2手前の着手の12近傍の更新
  if (pm2 != PASS) Neighbor12Update(game, color, sum_rate, sum_rate_row, rate, 1, &pm2, flag);
  // 3手前の着手の12近傍の更新
  if (pm3 != PASS) Neighbor12Update(game, color, sum_rate, sum_rate_row, rate, 1, &pm3, flag);

  // 以前の着手で戦術的特徴が現れた箇所の更新
  OtherUpdate(game, color, sum_rate, sum_rate_row, rate, prev_feature, prev_feature_pos, flag);
  // 最近の自分の着手の時に戦術的特徴が現れた箇所の更新
  OtherUpdate(game, color, sum_rate, sum_rate_row, rate, game->update_num[color], game->update_pos[color], flag);
  // 最近の相手の着手の時に戦術的特徴が現れた箇所の更新
  OtherUpdate(game, color, sum_rate, sum_rate_row, rate, game->update_num[other], game->update_pos[other], flag);
  // 自分の着手で石を打ち上げた箇所のとその周囲の更新
  Neighbor12Update(game, color, sum_rate, sum_rate_row, rate, game->capture_num[color], game->capture_pos[color], flag);
  // 相手の着手で石を打ち上げられた箇所とその周囲の更新
  Neighbor12Update(game, color, sum_rate, sum_rate_row, rate, game->capture_num[other], game->capture_pos[other], flag);

}


////////////////////
//  レーティング  //
////////////////////
void
Rating( game_info_t *game, int color, long long *sum_rate, long long *sum_rate_row, long long *rate )
{
  int pos, dis, pm1 = PASS, update_num = 0;
  int update_pos[PURE_BOARD_MAX];  
  double gamma;
  bool self_atari_flag;

  pm1 = game->record[game->moves - 1].pos;

  for (int i = 0; i < pure_board_max; i++) {
    pos = onboard_pos[i];
    ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
  }
  
  CheckFeaturesForSimulation(game, color, update_pos, &update_num);
  if (game->ko_move == game->moves - 2) {
    CheckCaptureAfterKoForSimulation(game, color, update_pos, &update_num);
  }

  for (int i = 0; i < pure_board_max; i++) {
    pos = onboard_pos[i];
    if (game->candidates[pos] && IsLegalNotEye(game, pos, color)) {
      self_atari_flag = CheckSelfAtariForSimulation(game, color, pos);
      CheckCaptureAndAtariForSimulation(game, color, pos);

      if (!self_atari_flag) {
        rate[pos] = 0;
      } else {
        gamma = po_pattern[MD2(game->pat, pos)];
        gamma *= CalculateTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
        if (pm1 != PASS) {
          dis = DIS(pos, pm1);
          if (dis < 5) {
            gamma *= po_previous_distance[dis - 2];
          }
        }
        rate[pos] = (long long)(gamma) + 1;
      }

      *sum_rate += rate[pos];
      sum_rate_row[board_y[pos]] += rate[pos];

      ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
    }
  }
}


/////////////////
//  γ読み込み  //
/////////////////
static void
LoadFeatureParameters( void )
{
  std::string po_parameters_path = GetWorkingDirectory() + PATH_SEPARATOR + "sim_params" + PATH_SEPARATOR;
  std::string path;

  // 直前の着手からの距離の読み込み
  path = po_parameters_path + "PreviousDistance.txt";
  LoadParameter(path.c_str(), po_neighbor_orig, PREVIOUS_DISTANCE_MAX);

  // 直前の着手からの距離のγを補正して出力
  for (int i = 0; i < PREVIOUS_DISTANCE_MAX - 1; i++) {
    po_previous_distance[i] = (float)(po_neighbor_orig[i] * neighbor_bias);
  }
  po_previous_distance[2] = (float)(po_neighbor_orig[2] * jump_bias);

  // 戦術的特徴の読み込み
  path = po_parameters_path + "CaptureFeature.txt";
  LoadParameter(path.c_str(), sim_capture, SIM_CAPTURE_MAX);

  path = po_parameters_path + "SaveExtensionFeature.txt";
  LoadParameter(path.c_str(), sim_save_extension, SIM_SAVE_EXTENSION_MAX);

  path = po_parameters_path + "AtariFeature.txt";
  LoadParameter(path.c_str(), sim_atari, SIM_ATARI_MAX);

  path = po_parameters_path + "ExtensionFeature.txt";
  LoadParameter(path.c_str(), sim_extension, SIM_EXTENSION_MAX);

  path = po_parameters_path + "DameFeature.txt";
  LoadParameter(path.c_str(), sim_dame, SIM_DAME_MAX);

  path = po_parameters_path + "ThrowInFeature.txt";
  LoadParameter(path.c_str(), sim_throw_in, SIM_THROW_IN_MAX);
  
  
  // 3x3のパターンの読み込み
  path = po_parameters_path + "Pat3.txt";
  LoadParameter(path.c_str(), po_pat3, PAT3_MAX);

  // マンハッタン距離2のパターンの読み込み
  path = po_parameters_path + "MD2.txt";
  LoadMD2Parameter(path.c_str(), po_md2);

  // 3x3とMD2のパターンをまとめる
  for (int i = 0; i < MD2_MAX; i++){
    if (po_md2[i] != 1.0) {
      po_pattern[i] = static_cast<float>(po_md2[i] * 10.0);
    } else {
      po_pattern[i] = static_cast<float>(po_pat3[(i & 0xffff)] * 10.0);
    }
    //po_pattern[i] = (float)(po_md2[i] * po_pat3[i & 0xFFFF] * 100.0);
  }
}



static void
LoadParameter( const char *filename, float params[], const int array_size )
{
  FILE *fp;
#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf_s(fp, "%e", &params[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
  #else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf(fp, "%e", &params[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
  }
  #endif
  fclose(fp);
}



///////////////////////////////
//  パラメータ読み込み(MD2)  //
///////////////////////////////
static void
LoadMD2Parameter( const char *filename, float params[] )
{
  FILE *fp;
  int index;
  float rate;
  unsigned int transp[16];

  for (int i = 0; i < MD2_MAX; i++) params[i] = 1.0;

#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  while (fscanf_s(fp, "%d%e", &index, &rate) != EOF) {
    MD2Transpose16(static_cast<unsigned int>(index), transp);
    for (int i = 0; i < 16; i++) {
      params[transp[i]] = rate;
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  while (fscanf(fp, "%d%e", &index, &rate) != EOF) {
    MD2Transpose16(static_cast<unsigned int>(index), transp);
    for (int i = 0; i < 16; i++) {
      params[transp[i]] = rate;
    }
  }
#endif
  fclose(fp);
}


void
AnalyzePoRating( game_info_t *game, int color, double rate[] )
{
  const int moves = game->moves;
  const int pm1 = game->record[moves - 1].pos;
  int pos;
  float gamma;
  int update_pos[BOARD_MAX], update_num = 0;  
  
  for (int i = 0; i < pure_board_max; i++) {
    pos = onboard_pos[i];
    ClearTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
  }
  
  CheckFeaturesForSimulation(game, color, update_pos, &update_num);
  CheckRemove2StonesForSimulation(game, color, update_pos, &update_num);
  if (game->ko_move == moves - 2) {
    CheckCaptureAfterKoForSimulation(game, color, update_pos, &update_num);
  }
  
  for (int i = 0; i < pure_board_max; i++) {
    pos = onboard_pos[i];
    
    if (!IsLegal(game, pos, color)) {
      rate[i] = 0;
      continue;
    }
    
    CheckSelfAtariForSimulation(game, color, pos);
    CheckCaptureAndAtariForSimulation(game, color, pos);
    
    gamma = 1.0;
    
    if (pm1 != PASS) {
      if (DIS(pos, pm1) == 2) {
        gamma *= po_previous_distance[0];
      } else if (DIS(pos, pm1) == 3) {
        gamma *= po_previous_distance[1];
      } else if (DIS(pos, pm1) == 4) {
        gamma *= po_previous_distance[2];
      }
    }
    
    gamma *= CalculateTacticalFeatures(&game->tactical_features[pos * ALL_MAX]);
    gamma *= po_pattern[MD2(game->pat, pos)];
    
    rate[i] = (long long int)gamma + 1;
  }
}
