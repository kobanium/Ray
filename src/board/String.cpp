/**
 * @file src/board/String.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Operation of stones string.
 * @~japanese
 * @brief 連の管理
 */
#include "board/GoBoard.hpp"
#include "board/String.hpp"
#include "board/ZobristHash.hpp"


// 呼吸点を連に追加
static int AddLiberty( string_t *string, const int pos, const int head );

// 隣接する連IDの追加
static void AddNeighbor( string_t *string, const int id, const int head );

// 連に石を追加
static void AddStoneToString( game_info_t *game, string_t *string, const int pos, const int head );

// 2つ以上の連の結合
static void MergeString( game_info_t *game, string_t *dst, string_t *src[3], const int n );

// 隣接する連IDの削除
static void RemoveNeighborString( string_t *string, const int id );

// 打ち上げた石の記録
static int AddCapture( int capture[], const int pos, const int head );


/**
 * @~english
 * @brief Make new string.
 * @param[in, out] game Board position data.
 * @param[in] pos Intersection.
 * @param[in] color Stone's color.
 * @~japanese
 * @brief 新しい連の作成
 * @param[in, out] game 局面情報
 * @param[in] pos 石を置いた座標
 * @param[in] color 石の色
 */
void
MakeString( game_info_t *game, const int pos, const int color )
{
  const char *board = game->board;
  const int other = GetOppositeColor(color);
  string_t *string = game->string;
  string_t *new_string;
  int *string_id = game->string_id;
  int id = 1, lib_add = 0, neighbor, neighbor4[4];

  // 未使用の連のインデックスを見つける
  while (string[id].flag) { id++; }

  // 新しく連のデータを格納する箇所を保持
  new_string = &game->string[id];

  // 連のデータの初期化
  std::fill_n(new_string->lib, STRING_LIB_MAX, 0);
  std::fill_n(new_string->neighbor, MAX_NEIGHBOR, 0);
  new_string->lib[0] = LIBERTY_END;
  new_string->neighbor[0] = NEIGHBOR_END;
  new_string->libs = 0;
  new_string->color = static_cast<char>(color);
  new_string->origin = pos;
  new_string->size = 1;
  new_string->neighbors = 0;
  game->string_id[pos] = id;
  game->string_next[pos] = STRING_END;

  // 上下左右の座標の導出
  GetNeighbor4(neighbor4, pos);

  // 新しく作成した連の上下左右の座標を確認
  // 空点ならば, 作成した連に呼吸点を追加する
  // 敵の連ならば, 隣接する連をお互いに追加する
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == S_EMPTY) {
      lib_add = AddLiberty(new_string, neighbor4[i], lib_add);
    } else if (board[neighbor4[i]] == other) {
      neighbor = string_id[neighbor4[i]];
      AddNeighbor(&string[neighbor], id, 0);
      AddNeighbor(&string[id], neighbor, 0);
    }
  }

  // 連の存在フラグをオンにする
  new_string->flag = true;
}


/**
 * @~english
 * @brief Add one stone to existing string.
 * @param[in, out] game Board position data.
 * @param[in, out] string String data.
 * @param[in] pos Intersection.
 * @param[in] head Starting index.
 * @~japanese
 * @brief 連に石を1つ追加
 * @param[in, out] game 局面情報
 * @param[in, out] string 石の追加する先の連
 * @param[in] pos 追加する石の座標
 * @param[in] head 開始点のインデックス
 */
static void
AddStoneToString( game_info_t *game, string_t *string, const int pos, const int head )
{
  int *string_next = game->string_next;
  int str_pos;

  if (pos == STRING_END) return;

  // 追加先の連の先頭の前ならば先頭に追加
  // そうでなければ挿入位置を探し出し追加
  if (string->origin > pos) {
    string_next[pos] = string->origin;
    string->origin = pos;
  } else {
    if (head != 0) {
      str_pos = head;
    } else {
      str_pos = string->origin;
    }
    while (string_next[str_pos] < pos){
      str_pos = string_next[str_pos];
    }
    string_next[pos] = string_next[str_pos];
    string_next[str_pos] = pos;
  }
  string->size++;
}


/**
 * @~english
 * @brief Process for add stone to string.
 * @param[in, out] game Board position data.
 * @param[in] pos Intersection.
 * @param[in] color Stone's color.
 * @param[in] id String ID.
 * @~japanese
 * @brief 連に石を追加
 * @param[in, out] game 局面情報
 * @param[in] pos 置いた石の座標
 * @param[in] color 置いた石の色
 * @param[in] id 石を追加する先の連ID
 */
void
AddStone( game_info_t *game, const int pos, const int color, const int id )
{
  const int other = GetOppositeColor(color);
  string_t *string = game->string;
  string_t *add_str;
  char *board = game->board;
  int *string_id = game->string_id;
  int lib_add = 0;
  int neighbor, neighbor4[4];

  // IDを更新
  string_id[pos] = id;

  // 追加先の連を取り出す
  add_str = &string[id];

  // 石を追加する
  AddStoneToString(game, add_str, pos, 0);

  // 上下左右の座標の導出
  GetNeighbor4(neighbor4, pos);

  // 空点なら呼吸点を追加し
  // 敵の石があれば隣接する敵連の情報を更新
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == S_EMPTY) {
      lib_add = AddLiberty(add_str, neighbor4[i], lib_add);
    } else if (board[neighbor4[i]] == other) {
      neighbor = string_id[neighbor4[i]];
      AddNeighbor(&string[neighbor], id, 0);
      AddNeighbor(&string[id], neighbor, 0);
    }
  }
}


/**
 * @~english
 * @brief String connection process.
 * @param[in, out] game Board position data.
 * @param[in] pos Intersection.
 * @param[in] color Stone's color.
 * @param[in] connection The number of string connection candidates.
 * @param[in] id String ID.
 * @~japanese
 * @brief 連同士の結合処理
 * @param[in, out] game 局面情報
 * @param[in] pos 置いた石の座標
 * @param[in] color 置いた石の色
 * @param[in] connection 隣接する連の候補の個数
 * @param[in] id 隣接する連の候補のID
 */
void
ConnectString( game_info_t *game, const int pos, const int color, const int connection, const int id[] )
{
  int min = id[0];
  string_t *string = game->string;
  string_t *str[3];
  int connections = 0;
  bool flag = true;

  // 接続する連の個数を厳密に確認
  for (int i = 1; i < connection; i++) {
    flag = true;
    for (int j = 0; j < i; j++) {
      if (id[j] == id[i]) {
        flag = false;
        break;
      }
    }
    if (flag) {
      if (min > id[i]) {
        str[connections] = &string[min];
        min = id[i];
      } else {
        str[connections] = &string[id[i]];
      }
      connections++;
    }
  }

  // 石を追加
  AddStone(game, pos, color, min);

  // 複数の連が接続するときの処理
  if (connections > 0) {
    MergeString(game, &game->string[min], str, connections);
  }
}


/**
 * @~english
 * @brief Connect strings.
 * @param[in, out] game Board position data.
 * @param[out] dst Connection destination string.
 * @param[in] src Connecting strings.
 * @param[in] n The number of connecting strings.
 * @~japanese
 * @brief 連を結合
 * @param[in, out] game
 * @param[out] dst 結合先の連
 * @param[in] src 結合させる連
 * @param[in] n 結合する連の個数
 */
static void
MergeString( game_info_t *game, string_t *dst, string_t *src[3], const int n )
{
  int tmp, pos, prev, neighbor;
  int *string_next = game->string_next;
  int *string_id = game->string_id;
  int id = string_id[dst->origin], rm_id;
  string_t *string = game->string;

  for (int i = 0; i < n; i++) {
    // 接続で消える連のID
    rm_id = string_id[src[i]->origin];

    // 呼吸点をマージ
    prev = 0;
    pos = src[i]->lib[0];
    while (pos != LIBERTY_END) {
      prev = AddLiberty(dst, pos, prev);
      pos = src[i]->lib[pos];
    }

    // 連のIDを更新
    prev = 0;
    pos = src[i]->origin;
    while (pos != STRING_END) {
      string_id[pos] = id;
      tmp = string_next[pos];
      AddStoneToString(game, dst, pos, prev);
      prev = pos;
      pos = tmp;
    }

    // 隣接する敵連の情報をマージ
    prev = 0;
    neighbor = src[i]->neighbor[0];
    while (neighbor != NEIGHBOR_END) {
      RemoveNeighborString(&string[neighbor], rm_id);
      AddNeighbor(dst, neighbor, prev);
      AddNeighbor(&string[neighbor], id, prev);
      prev = neighbor;
      neighbor = src[i]->neighbor[neighbor];
    }

    // 使用済みフラグをオフ
    src[i]->flag = false;
  }
}


/**
 * @~english
 * @brief Process to add a liberty.
 * @param[in, out] string String will be updated.
 * @param[in] pos Liberty coordinate.
 * @param[in] head Search index.
 * @return Liberty intersection. 
 * @~japanese
 * @brief 呼吸点の追加
 * @param[in, out] string 呼吸点を追加する対象の連
 * @param[in] pos 追加する呼吸点の座標
 * @param[in] head 探索対象の先頭のインデックス
 * @return 追加した呼吸点の座標
 */
static int
AddLiberty( string_t *string, const int pos, const int head )
{
  int lib;

  // 既に追加されている場合は何もしない
  if (string->lib[pos] != 0) return pos;

  // 探索対象の先頭のインデックスを代入
  lib = head;

  // 追加する場所を見つけるまで進める
  while (string->lib[lib] < pos) {
    lib = string->lib[lib];
  }

  // 呼吸点の座標を追加する
  string->lib[pos] = string->lib[lib];
  string->lib[lib] = static_cast<short>(pos);

  // 呼吸点の数を1つ増やす
  string->libs++;

  // 追加した呼吸点の座標を返す
  return pos;
}


/**
 * @~english
 * @brief Remove a liberty.
 * @param[in, out] game Board position data.
 * @param[in, out] string String will be removed a liberty.
 * @param[in] pos Liberty intersection.
 * @~japanese
 * @brief 呼吸点の除去
 * @param[in, out] game 局面情報
 * @param[in, out] string 呼吸点を取り除く対象の連
 * @param[in] pos 取り除かれる呼吸点の座標
 */
void
RemoveLiberty( game_info_t *game, string_t *string, const int pos )
{
  int lib = 0;

  // 既に取り除かれている場合は何もしない
  if (string->lib[pos] == 0) return;

  // 取り除く呼吸点の座標を見つけるまで進める
  while (string->lib[lib] != pos) {
    lib = string->lib[lib];
  }

  // 呼吸点の座標の情報を取り除く
  string->lib[lib] = string->lib[string->lib[lib]];
  string->lib[pos] = static_cast<short>(0);

  // 連の呼吸点の数を1つ減らす
  string->libs--;

  // 呼吸点が1つならば, その連の呼吸点を候補手に追加
  if (string->libs == 1) {
    game->candidates[string->lib[0]] = true;
  }
}


/**
 * @~english
 * @brief Remove a liberty for Monte-Carlo simulation.
 * @param[in, out] game Board position data.
 * @param[in, out] string String will be removed a liberty.
 * @param[in] pos Liberty intersection.
 * @param[in] color Player's color.
 * @~japanese
 * @brief 呼吸点の除去 (モンテカルロ・シミュレーション用)
 * @param[in, out] game 局面情報
 * @param[in, out] string 呼吸点を取り除く対象の連
 * @param[in] pos 取り除かれる呼吸点の座標
 * @param[in] color 手番の色
 */
void
PoRemoveLiberty( game_info_t *game, string_t *string, const int pos, const int color )
{
  int lib = 0;

  // 既に取り除かれている場合は何もしない
  if (string->lib[pos] == 0) return;

  // 取り除く呼吸点の座標を見つけるまで進める
  while (string->lib[lib] != pos) {
    lib = string->lib[lib];
  }

  // 呼吸点の座標の情報を取り除く
  string->lib[lib] = string->lib[string->lib[lib]];
  string->lib[pos] = 0;

  // 呼吸点の数を1つ減らす
  string->libs--;

  // 連の呼吸点の数を確認
  // 呼吸点が1つならば, その呼吸点を候補手に戻して, レートの更新対象に加える
  // 呼吸点が2つならば, レートの更新対象に加える
  if (string->libs == 1) {
    game->candidates[string->lib[0]] = true;
    game->update_pos[color][game->update_num[color]++] = string->lib[0];
    game->seki[string->lib[0]] = false;
  }
}


/**
 * @~english
 * @brief Remove string.
 * @param[in, out] game Board position data.
 * @param[in, out] string String must be removed.
 * @param[in] color Player's color.
 * @return The number of removed stones.
 * @~japanese
 * @brief 連の除去の処理
 * @param[in, out] game 局面情報
 * @param[in, out] string 取り除く対象の連
 * @param[in] color 手番の色
 * @return 打ち上げた石の個数
 */
int
RemoveString( game_info_t *game, string_t *string, const int color )
{
  string_t *str = game->string;
  int *string_next = game->string_next;
  int *string_id = game->string_id;
  int pos = string->origin, next;
  char *board = game->board;
  bool *candidates = game->candidates;
  int neighbor, rm_id = string_id[string->origin];
  int removed_color = board[pos];
  int *capture_pos = game->capture_pos[color];
  int *capture_num = &game->capture_num[color];

  do {
    // 空点に戻す
    board[pos] = S_EMPTY;

    // 候補手に追加する
    candidates[pos] = true;

    // 打ち上げた石の記録
    capture_pos[(*capture_num)++] = pos;

    // パターンの更新
    UpdatePatternEmpty(game->pat, pos);

    game->current_hash ^= hash_bit[pos][removed_color];
    game->positional_hash ^= hash_bit[pos][removed_color];

    // 上下左右を確認する
    // 隣接する連があれば呼吸点を追加する
    if (str[string_id[NORTH(pos)]].flag) AddLiberty(&str[string_id[NORTH(pos)]], pos, 0);
    if (str[string_id[ WEST(pos)]].flag) AddLiberty(&str[string_id[ WEST(pos)]], pos, 0);
    if (str[string_id[ EAST(pos)]].flag) AddLiberty(&str[string_id[ EAST(pos)]], pos, 0);
    if (str[string_id[SOUTH(pos)]].flag) AddLiberty(&str[string_id[SOUTH(pos)]], pos, 0);

    // 連を構成する次の石の座標を記録
    next = string_next[pos];

    // 連の構成要素から取り除き,
    // 石を取り除いた箇所の連IDを元に戻す
    string_next[pos] = 0;
    string_id[pos] = 0;

    // 連を構成する次の石の座標に移動
    pos = next;
  } while (pos != STRING_END);

  // 取り除いた連に隣接する連から隣接情報を取り除く
  neighbor = string->neighbor[0];
  while (neighbor != NEIGHBOR_END) {
    RemoveNeighborString(&str[neighbor], rm_id);
    neighbor = string->neighbor[neighbor];
  }

  // 連の存在フラグをオフ
  string->flag = false;

  // 打ち上げた石の数を返す
  return string->size;
}


/**
 * @~english
 * @brief Remove string for Monte-Carlo simulation.
 * @param[in, out] game Board postion data.
 * @param[in, out] string String must be removed.
 * @param[in] color Player's color.
 * @return The number of removed stones.
 * @~japanese
 * @brief 連の除去の処理 (モンテカルロ・シミュレーション用)
 * @param[in, out] game 局面情報
 * @param[in, out] string 取り除く対象の連
 * @param[in] color 手番の色
 * @return 打ち上げた石の個数
 */
int
PoRemoveString( game_info_t *game, string_t *string, const int color )
{
  string_t *str = game->string;
  int *string_next = game->string_next;
  int *string_id = game->string_id;
  int pos = string->origin, next;
  char *board = game->board;
  bool *candidates = game->candidates;
  int neighbor, rm_id = string_id[string->origin];
  int *capture_pos = game->capture_pos[color];
  int *capture_num = &game->capture_num[color];
  int *update_pos = game->update_pos[color];
  int *update_num = &game->update_num[color];
  int lib;

  // 隣接する連の呼吸点を更新の対象に加える
  neighbor = string->neighbor[0];
  while (neighbor != NEIGHBOR_END) {
    if (str[neighbor].libs < 3) {
      lib = str[neighbor].lib[0];
      while (lib != LIBERTY_END) {
        update_pos[(*update_num)++] = lib;
        game->seki[lib] = false;
        lib = str[neighbor].lib[lib];
      }
    }
    neighbor = string->neighbor[neighbor];
  }

  do {
    // 空点に戻す
    board[pos] = S_EMPTY;
    // 候補手に追加する
    candidates[pos] = true;

    // レーティング更新対象に追加
    capture_pos[(*capture_num)++] = pos;

    // 3x3のパターンの更新
    UpdateMD2Empty(game->pat, pos);

    // 上下左右を確認する
    // 隣接する連があれば呼吸点を追加する
    if (str[string_id[NORTH(pos)]].flag) AddLiberty(&str[string_id[NORTH(pos)]], pos, 0);
    if (str[string_id[ WEST(pos)]].flag) AddLiberty(&str[string_id[ WEST(pos)]], pos, 0);
    if (str[string_id[ EAST(pos)]].flag) AddLiberty(&str[string_id[ EAST(pos)]], pos, 0);
    if (str[string_id[SOUTH(pos)]].flag) AddLiberty(&str[string_id[SOUTH(pos)]], pos, 0);

    // 連を構成する次の石の座標を記録
    next = string_next[pos];

    // 連の構成要素から取り除き,
    // 石を取り除いた箇所の連IDを元に戻す
    string_next[pos] = 0;
    string_id[pos] = 0;

    // 連を構成する次の石へ移動
    pos = next;
  } while (pos != STRING_END);

  // 取り除いた連に隣接する連から隣接情報を取り除く
  neighbor = string->neighbor[0];
  while (neighbor != NEIGHBOR_END) {
    RemoveNeighborString(&str[neighbor], rm_id);
    neighbor = string->neighbor[neighbor];
  }

  // 連の存在フラグをオフ
  string->flag = false;

  // 打ち上げた石の個数を返す
  return string->size;
}


/**
 * @~english
 * @brief Add neighbor string ID with duplication check.
 * @param[in, out] string String must be updated.
 * @param[in] id Neighbor string ID.
 * @param[in] head Starting index.
 * @~japanese
 * @brief 隣接する連IDの追加(重複確認)
 * @param[in, out] string 隣接情報を追加する連
 * @param[in] id 追加する連ID
 * @param[in] head 開始点のインデックス
 */
static void
AddNeighbor( string_t *string, const int id, const int head )
{
  int neighbor = 0;

  // 既に追加されている場合は何もしない
  if (string->neighbor[id] != 0) return;

  // 探索対象の先頭のインデックスを代入
  neighbor = head;

  // 追加場所を見つけるまで進める
  while (string->neighbor[neighbor] < id) {
    neighbor = string->neighbor[neighbor];
  }

  // 隣接する連IDを追加する
  string->neighbor[id] = string->neighbor[neighbor];
  string->neighbor[neighbor] = static_cast<short>(id);

  // 隣接する連の数を1つ増やす
  string->neighbors++;
}


/**
 * @~english
 * @brief Remove neighbor string ID.
 * @param[in, out] string String must be updated.
 * @param[in] id Remove string ID.
 * @~japanese
 * @brief 隣接する連IDの除去
 * @param[in, out] string 隣接する連IDを取り除く対象の連
 * @param[in] id 取り除く連ID
 */
static void
RemoveNeighborString( string_t *string, const int id )
{
  int neighbor = 0;

  // 既に除外されていれば何もしない
  if (string->neighbor[id] == 0) return;

  // 取り除く隣接する連IDを見つけるまで進める
  while (string->neighbor[neighbor] != id) {
    neighbor = string->neighbor[neighbor];
  }

  // 隣接する連IDを取り除く
  string->neighbor[neighbor] = string->neighbor[string->neighbor[neighbor]];
  string->neighbor[id] = 0;

  // 隣接する連の数を1つ減らす
  string->neighbors--;
}


/**
 * @~english
 * @brief Update captured stones information.
 * @param[out] capture Captured stones information.
 * @param[in] pos Update liberty intersection.
 * @param[in] head Search index.
 * @return
 * @~japanese
 * @brief 取られた石の座標の追加処理
 * @param[out] capture 着手によって取られた石の座標の記録
 * @param[in] pos 追加する呼吸点の座標
 * @param[in] head 探索対象の先頭のインデックス
 * @return
 */
static int
AddCapture( int capture[], const int pos, const int head )
{
  int cap = head;;

  if (capture[pos] != 0) return pos;

  while (capture[cap] < pos) {
    cap = capture[cap];
  }

  capture[pos] = capture[cap];
  capture[cap] = pos;

  return pos;
}
