#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "SearchBoard.h"

using namespace std;

////////////
//  関数  //
////////////

//  呼吸点の追加 
static int AddLiberty( string_t *string, const int pos, const int head );

//  隣接する敵連のIDの追加
static void AddNeighbor( string_t *string, const int id );

//  連に石を追加
static void AddStoneToString( search_game_info_t *game, string_t *string, const int pos);
//  石の追加  
static void AddStone( search_game_info_t *game, const int pos, const int color, const int id );

//  連の結合
static void ConnectString( search_game_info_t *game, const int pos, const int color, const int connection, const int id[] );

//  自殺手の判定
static bool IsSuicide( const search_game_info_t *game, const string_t *string, const int color, const int pos );

//  新たな連の作成
static void MakeString( search_game_info_t *game, const int pos, const int color );

//  呼吸点の結合
static void MergeLiberty( string_t *dst, string_t *src );

//  隣接する敵連のIDを結合
static void MergeNeighbor( string_t *string, string_t *dst, string_t *src, const int id, const int rm_id );

//  石の結合 
static void MergeStones( search_game_info_t *game, const int id, const int rm_id );

//  連の結合処理 
static void MergeString( search_game_info_t *game, string_t *dst, string_t *src[3], const int n );

//  変化のあった連の記録
static void RecordString( search_game_info_t *game, int id );

//  呼吸点を1つ除去
static void RemoveLiberty( search_game_info_t *game, string_t *string, const int pos );

//  隣接する敵連IDの除去
static void RemoveNeighborString( string_t *string, const int id );

//  連を取り除く処理
static int RemoveString( search_game_info_t *game, string_t *string );

//  連の復元 
static void RestoreChain( search_game_info_t *game, const int id, const int stone[], const int stones, const int color );


///////////////////
//  メモリの確保  //
///////////////////
search_game_info_t *
AllocateSearchGame( void )
{
  search_game_info_t *game;

  game = new search_game_info_t();
  memset(game, 0, sizeof(search_game_info_t));

  return game;
}


////////////////////
// 　メモリの開放  //
////////////////////
void
FreeSearchGame( search_game_info_t *game )
{
  if (game) delete game;
}


///////////////////////////////
//  局面のコピー(データ削減)  //
///////////////////////////////
void
CopyGameForSearch( search_game_info_t *dst, const game_info_t *src )
{
  memcpy(dst->record,      src->record,      sizeof(record_t) * MAX_RECORDS);
  memcpy(dst->prisoner,    src->prisoner,    sizeof(int) * S_MAX);
  memcpy(dst->board,       src->board,       sizeof(char) * BOARD_MAX);
  memcpy(dst->pat,         src->pat,         sizeof(pattern_t) * BOARD_MAX);
  memcpy(dst->string_id,   src->string_id,   sizeof(int) * STRING_POS_MAX);
  memcpy(dst->string_next, src->string_next, sizeof(int) * STRING_POS_MAX);
  memcpy(dst->candidates,  src->candidates,  sizeof(bool) * BOARD_MAX);

  memset(dst->stone,          0, sizeof(int) * MAX_RECORDS * 4 * PURE_BOARD_MAX);
  memset(dst->stones,         0, sizeof(int) * MAX_RECORDS * 4);
  memset(dst->strings,        0, sizeof(int) * MAX_RECORDS);
  memset(dst->strings_id,     0, sizeof(int) * MAX_RECORDS * 4);
  memset(dst->string_color,   0, sizeof(char) * MAX_RECORDS * 4);
  memset(dst->ko_move_record, 0, sizeof(int) * MAX_RECORDS);
  memset(dst->ko_pos_record,  0, sizeof(int) * MAX_RECORDS);
  
  for (int i = 0; i < MAX_STRING; i++) {
    if (src->string[i].flag) {
      memcpy(&dst->string[i], &src->string[i], sizeof(string_t));
    } else {
      dst->string[i].flag = false;
    }
  }

  dst->moves = src->moves;
  dst->ko_move = src->ko_move;
  dst->ko_pos = src->ko_pos;

  dst->ko_move_record[dst->moves] = dst->ko_move;
  dst->ko_pos_record[dst->moves] = dst->ko_pos;
}


//////////////////
//  合法手判定  //
//////////////////
bool
IsLegalForSearch( const search_game_info_t *game, const int pos, const int color )
{
  // 既に石がある
  if (game->board[pos] != S_EMPTY) {
    return false;
  }

  // 自殺手である
  if (nb4_empty[Pat3(game->pat, pos)] == 0 &&
      IsSuicide(game, game->string, color, pos)) {
    return false;
  }

  // 劫である
  if (game->ko_pos == pos &&
      game->ko_move == (game->moves - 1)) {
    return false;
  }

  return true;
}


///////////////////////////
//  変化のあった連の記録  //
///////////////////////////
static void
RecordString( search_game_info_t *game, int id )
{
  const int moves = game->moves;  
  const string_t *string = game->string;
  const int *string_next = game->string_next;
  const int strings = game->strings[moves];
  int pos, i = 0;

  pos = string[id].origin;
  while (pos != STRING_END) {
    game->stone[moves][game->strings[moves]][i++] = pos;
    pos = string_next[pos];
  }

  game->string_color[moves][strings] = string[id].color;
  game->stones[moves][strings] = string[id].size;
  game->strings_id[moves][strings] = id;
  game->strings[moves]++;
}


////////////////
//  石を置く  //
////////////////
void
PutStoneForSearch( search_game_info_t *game, const int pos, const int color )
{
  int *string_id = game->string_id;
  char *board = game->board;
  string_t *string = game->string;
  int other = FLIP_COLOR(color);
  int connection = 0;
  int connect[4] = { 0 };
  int prisoner = 0;
  int neighbor[4];

  // 着手箇所と色を記録
  if (game->moves < MAX_RECORDS) {
    game->record[game->moves].color = color;
    game->record[game->moves].pos = pos;
    game->strings[game->moves] = 0;
  }

  // 着手がパスなら手数を進めて終了
  if (pos == PASS) {
    game->moves++;
    return;
  }

  // 石を置く
  board[pos] = (char)color;

  // 候補手から除外
  game->candidates[pos] = false;

  // パターンの更新(MD5)
  UpdateMD2Stone(game->pat, color, pos);

  // 着手点の上下左右の座標を導出
  GetNeighbor4(neighbor, pos);

  // 着手箇所の上下左右の確認
  // 自分の連があれば, その連の呼吸点を1つ減らし, 接続候補に入れる
  // 敵の連であれば, その連の呼吸点を1つ減らし, 呼吸点が0になったら取り除く
  for (int i = 0; i < 4; i++) {
    if (board[neighbor[i]] == color) {
      RemoveLiberty(game, &string[string_id[neighbor[i]]], pos);
      connect[connection++] = string_id[neighbor[i]];
    } else if (board[neighbor[i]] == other) {
      RemoveLiberty(game, &string[string_id[neighbor[i]]], pos);
      if (string[string_id[neighbor[i]]].libs == 0) {
	RecordString(game, string_id[neighbor[i]]);
	prisoner += RemoveString(game, &string[string_id[neighbor[i]]]);
      }
    }
  }

  // 打ち上げた石をアゲハマに追加
  game->prisoner[color] += prisoner;

  // 接続候補がなければ, 新しい連を作成して, 劫かどうかの確認をする
  // 接続候補が1つならば, その連に石を追加する
  // 接続候補が2つ以上ならば, 連同士を繋ぎ合わせて, 石を追加する
  if (connection == 0) {
    MakeString(game, pos, color);
    if (prisoner == 1 &&
	string[string_id[pos]].libs == 1) {
      game->ko_move = game->moves;
      game->ko_pos = string[string_id[pos]].lib[0];
    }
  } else if (connection == 1) {
    RecordString(game, connect[0]);
    AddStone(game, pos, color, connect[0]);
  } else {
    ConnectString(game, pos, color, connection, connect);
  }


  // 手数を1つだけ進める
  game->moves++;
  game->ko_move_record[game->moves] = game->ko_move;
  game->ko_pos_record[game->moves] = game->ko_pos;

}


///////////////////
//  呼吸点の追加  //
///////////////////
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
  string->lib[lib] = (short)pos;

  // 呼吸点の数を1つ増やす
  string->libs++;

  // 追加した呼吸点の座標を返す
  return pos;
}


/////////////////////////////
//  隣接する敵連のIDの追加  //
/////////////////////////////
static void
AddNeighbor( string_t *string, const int id )
{
  int neighbor = 0;

  // 既に追加されている場合は何もしない
  if (string->neighbor[id] != 0) return;

  // 追加場所を見つけるまで進める
  while (string->neighbor[neighbor] < id) {
    neighbor = string->neighbor[neighbor];
  }

  // 隣接する連IDを追加する
  string->neighbor[id] = string->neighbor[neighbor];
  string->neighbor[neighbor] = (short)id;

  // 隣接する連の数を1つ増やす
  string->neighbors++;
}


///////////////////
//  連に石を追加  //
///////////////////
static void
AddStoneToString( search_game_info_t *game, string_t *string, const int pos )
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
    str_pos = string->origin;
    while (string_next[str_pos] < pos) {
      str_pos = string_next[str_pos];
    }
    string_next[pos] = string_next[str_pos];
    string_next[str_pos] = pos;
  }
  string->size++;
}


////////////////
//  石の追加  //
////////////////
static void
AddStone( search_game_info_t *game, const int pos, const int color, const int id )
{
  const char *board = game->board;
  string_t *string = game->string;
  string_t *add_str;
  int *string_id = game->string_id;
  int lib_add = 0;
  int other = FLIP_COLOR(color);
  int neighbor, neighbor4[4];

  // IDを更新
  string_id[pos] = id;

  // 追加先の連を取り出す
  add_str = &string[id];

  // 石を追加する
  AddStoneToString(game, add_str, pos);

  // 上下左右の座標の導出
  GetNeighbor4(neighbor4, pos);

  // 空点なら呼吸点を追加し
  // 敵の石があれば隣接する敵連の情報を更新
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == S_EMPTY) {
      lib_add = AddLiberty(add_str, neighbor4[i], lib_add);
    } else if (board[neighbor4[i]] == other) {
      neighbor = string_id[neighbor4[i]];
      AddNeighbor(&string[neighbor], id);
      AddNeighbor(&string[id], neighbor);
    }
  }
}


////////////////
//  連の結合  //
////////////////
static void
ConnectString( search_game_info_t *game, const int pos, const int color, const int connection, const int id[] )
{
  int min = id[0];
  int ids[4];
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
      ids[connections] = id[i];
      connections++;
    }
  }

  ids[connections] = id[0];

  for (int i = 0; i <= connections; i++) {
    RecordString(game, ids[i]);
  }

  // 石を追加
  AddStone(game, pos, color, min);

  // 複数の連が接続するときの処理
  if (connections > 0) {
    MergeString(game, &game->string[min], str, connections);
  }
}


////////////////////
//  自殺手の判定  //
///////////////////
static bool
IsSuicide( const search_game_info_t *game, const string_t *string, const int color, const int pos )
{
  const char *board = game->board;
  const int *string_id = game->string_id;
  const int other = FLIP_COLOR(color);
  int neighbor4[4];

  GetNeighbor4(neighbor4, pos);

  // 隣接するの石についての判定
  // 隣接する石が相手でも、その石を含む連の呼吸点が1の時は合法手
  // 隣接する石が自分で、その石を含む連の呼吸点が2以上の時は合法手
  for (int i = 0; i < 4; i++) {
    if (board[neighbor4[i]] == other &&
	string[string_id[neighbor4[i]]].libs == 1) {
      return false;
    } else if (board[neighbor4[i]] == color &&
	     string[string_id[neighbor4[i]]].libs > 1) {
      return false;
    }
  }

  return true;
}


/////////////////////
//  新たな連の作成  //
/////////////////////
static void
MakeString( search_game_info_t *game, const int pos, const int color )
{
  string_t *string = game->string;
  string_t *new_string;
  char *board = game->board;
  int *string_id = game->string_id;
  int id = 1;
  int lib_add = 0;
  int other = FLIP_COLOR(color);
  int neighbor, neighbor4[4];

  // 未使用の連のインデックスを見つける
  while (string[id].flag) { id++; }

  // 新しく連のデータを格納する箇所を保持
  new_string = &game->string[id];

  // 連のデータの初期化
  fill_n(new_string->lib, STRING_LIB_MAX, 0);
  fill_n(new_string->neighbor, MAX_NEIGHBOR, 0);
  new_string->color = (char)color;
  new_string->lib[0] = LIBERTY_END;
  new_string->neighbor[0] = NEIGHBOR_END;
  new_string->libs = 0;
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
      AddNeighbor(&string[neighbor], id);
      AddNeighbor(&string[id], neighbor);
    }
  }

  // 連の存在フラグをオンにする
  new_string->flag = true;
}


////////////////////
//  呼吸点の結合  //
////////////////////
static void
MergeLiberty( string_t *dst, string_t *src )
{
  int dst_lib = 0, src_lib = 0;

  while (src_lib != LIBERTY_END) {
    // 既に呼吸点に含まれていれば次の座標へ
    if (dst->lib[src_lib] == 0) {
      // 該当する場所を見つけるまで進める
      while (dst->lib[dst_lib] < src_lib) {
	dst_lib = dst->lib[dst_lib];
      }
      // 呼吸点の座標を挿入する
      dst->lib[src_lib] = dst->lib[dst_lib];
      dst->lib[dst_lib] = src_lib;
      // 呼吸点を1つ増やす
      dst->libs++;
    }
    src_lib = src->lib[src_lib];
  }
}


////////////////
//  石の結合  //
////////////////
static void
MergeStones( search_game_info_t *game, const int id, const int rm_id )
{
  string_t *string = game->string;
  int *string_next = game->string_next;
  int *string_id = game->string_id;
  int dst_pos = string[id].origin, src_pos = string[rm_id].origin;
  int pos;

  // 結合元srcの連の始点が結合先dstの連の始点よりも
  // 前にあるときの特殊処理
  if (dst_pos > src_pos) {
    // srcの次の座標を保持
    pos = string_next[src_pos];
    // 連の繋がりの更新
    string_next[src_pos] = dst_pos;
    // 連のIDの更新
    string_id[src_pos] = id;
    // 連の始点を更新
    string[id].origin = src_pos;
    // 探索開始点を更新
    dst_pos = string[id].origin;
    // 1つ先の座標を代入
    src_pos = pos;
  }

  while (src_pos != STRING_END) {
    string_id[src_pos] = id;
    pos = string_next[src_pos];
    // 該当する場所を見つけるまで進める
    while (string_next[dst_pos] < src_pos) {
      dst_pos = string_next[dst_pos];
    }
    // 呼吸点の座標を挿入する
    string_next[src_pos] = string_next[dst_pos];
    string_next[dst_pos] = src_pos;
    // 次の座標へ移動
    src_pos = pos;
  }

  string[id].size += string[rm_id].size;
}


///////////////////
//  連の結合処理  //
///////////////////
static void
MergeString( search_game_info_t *game, string_t *dst, string_t *src[3], const int n )
{
  int *string_id = game->string_id;
  int id = string_id[dst->origin], rm_id;
  string_t *string = game->string;

  for (int i = 0; i < n; i++) {
    // 接続で消える連のID
    rm_id = string_id[src[i]->origin];
    // 呼吸点をマージ
    MergeLiberty(dst, src[i]);
    // 連の石の繋がりと連のIDを更新
    MergeStones(game, id, rm_id);
    // 隣接する敵連の情報をマージ
    MergeNeighbor(string, dst, src[i], id, rm_id);
    // 使用済みフラグをオフ
    src[i]->flag = false;
  }
}


//////////////////////
//  呼吸点を1つ除去  //
//////////////////////
static void
RemoveLiberty( search_game_info_t *game, string_t *string, const int pos )
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
  string->lib[pos] = (short)0;

  // 連の呼吸点の数を1つ減らす
  string->libs--;

  // 呼吸点が1つならば, その連の呼吸点を候補手に追加
  if (string->libs == 1) {
    game->candidates[string->lib[0]] = true;
  }
}


/////////////////////////////
//  隣接する敵連のIDを結合  //
/////////////////////////////
static void
MergeNeighbor( string_t *string, string_t *dst, string_t *src, const int id, const int rm_id )
{
  int src_neighbor = 0, dst_neighbor = 0;
  int neighbor = src->neighbor[0];

  while (src_neighbor != NEIGHBOR_END) {
    // 既に隣接する敵連のIDが含まれていれば次の座標へ
    if (dst->neighbor[src_neighbor] == 0) {
      // 該当する場所を見つけるまで進める
      while (dst->neighbor[dst_neighbor] < src_neighbor) {
	dst_neighbor = dst->neighbor[dst_neighbor];
      }
      // 隣接する敵連のIDを挿入する
      dst->neighbor[src_neighbor] = dst->neighbor[dst_neighbor];
      dst->neighbor[dst_neighbor] = src_neighbor;
      // 隣接する敵連の個数を1つ増やす
      dst->neighbors++;
    }
    src_neighbor = src->neighbor[src_neighbor];
  }

  // 元あった連srcに隣接する敵連の
  // 隣接情報からrm_idを除去
  while (neighbor != NEIGHBOR_END) {
    RemoveNeighborString(&string[neighbor], rm_id);
    AddNeighbor(&string[neighbor], id);
    neighbor = src->neighbor[neighbor];
  }
}


///////////////////////////
//  隣接する敵連IDの除去  //
///////////////////////////
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


///////////////////////
//  連を取り除く処理  //
///////////////////////
static int
RemoveString( search_game_info_t *game, string_t *string )
{
  string_t *str = game->string;
  int *string_next = game->string_next;
  int *string_id = game->string_id;
  int pos = string->origin, next;
  char *board = game->board;
  bool *candidates = game->candidates;
  int neighbor, rm_id = string_id[string->origin];

  do {
    // 空点に戻す
    board[pos] = S_EMPTY;

    // 候補手に追加する
    candidates[pos] = true;

    // パターンの更新
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


////////////////
//  連の復元  //
////////////////
static void
RestoreChain( search_game_info_t *game, const int id, const int stone[], const int stones, const int color )
{
  string_t *string = game->string;
  string_t *new_string;
  char *board = game->board;
  int *string_id = game->string_id;
  int lib_add = 0;
  const int other = FLIP_COLOR(color);
  int neighbor, neighbor4[4];
  int pos;

  // 新しい連のポインタ
  new_string = &game->string[id];

  // 連の初期化
  fill_n(new_string->lib, STRING_LIB_MAX, 0);
  fill_n(new_string->neighbor, MAX_NEIGHBOR, 0);
  new_string->color = (char)color;
  new_string->lib[0] = LIBERTY_END;
  new_string->neighbor[0] = NEIGHBOR_END;
  new_string->libs = 0;
  new_string->origin = stone[0];
  new_string->size = stones;
  new_string->neighbors = 0;

  for (int i = 0; i < stones; i++) {
    pos = stone[i];
    board[pos] = (char)color;
    game->string_id[pos] = id;
    UpdateMD2Stone(game->pat, color, pos); 
  }

  for (int i = 0; i < stones - 1; i++) {
    game->string_next[stone[i]] = stone[i + 1];
  }
  game->string_next[stone[stones - 1]] = STRING_END;

  for (int i = 0; i < stones; i++) {
    pos = stone[i];
    // 上下左右の座標の導出
    GetNeighbor4(neighbor4, pos);
    // 新しく作成した連の上下左右の座標を確認
    // 空点ならば, 作成した連に呼吸点を追加する
    // 敵の連ならば, 隣接する連をお互いに追加する
    for (int j = 0; j < 4; j++) {
      if (board[neighbor4[j]] == S_EMPTY) {
	AddLiberty(new_string, neighbor4[j], lib_add);
      } else if (board[neighbor4[j]] == other) {
	neighbor = string_id[neighbor4[j]];
	RemoveLiberty(game, &string[neighbor], pos);	
	AddNeighbor(&string[neighbor], id);
	AddNeighbor(&string[id], neighbor);	
      }
    }
  }

  // 連の存在フラグをオンにする
  new_string->flag = true;
}


////////////////
//  一手戻す  //
////////////////
void
Undo( search_game_info_t *game )
{
  const int pm_count = game->moves - 1;
  const int previous_move = game->record[pm_count].pos;
  const int played_color = game->record[pm_count].color;
  const int opponent_color = FLIP_COLOR(played_color);
  string_t *string = game->string;
  int *string_id = game->string_id;

  // 連を取り除く
  RemoveString(game, &string[string_id[previous_move]]);

  // 連を1手前の状態に戻す
  for (int i = 0; i < game->strings[pm_count]; i++) {
    if (game->string_color[pm_count][i] == opponent_color) {
      game->prisoner[played_color] -= game->stones[pm_count][i];
    }
    RestoreChain(game, game->strings_id[pm_count][i], game->stone[pm_count][i], game->stones[pm_count][i], game->string_color[pm_count][i]);
    game->stones[pm_count][i] = 0;
  }

  game->strings[pm_count] = 0;

  game->ko_move = game->ko_move_record[pm_count];
  game->ko_pos = game->ko_pos_record[pm_count];

  game->moves--;
}

