/**
 * @file src/feature/Ladder.cpp
 * @author Yuki Kobayashi
 * @~english
 * @brief Ladder checker.
 * @~japanese
 * @brief シチョウの確認
 */
#include <iostream>
#include <memory>

#include "board/Point.hpp"
#include "board/SearchBoard.hpp"
#include "common/Message.hpp"
#include "feature/Ladder.hpp"


/**
 * @~english
 * @brief Escapable ladder.
 * @~japanese
 * @brief 逃げられるシチョウ
 */
constexpr bool ALIVE = true;

/**
 * @~english
 * @brief Capturable ladder.
 * @~japanese
 * @brief 取られるシチョウ
 */
constexpr bool DEAD = false;


// シチョウ探索
static bool IsLadderCaptured( const int depth, search_game_info_t *game, const int ren_xy, const int turn_color );


/**
 * @~english
 * @brief Search capturable ladder.
 * @param[in] game Board position data.
 * @param[in] color Player's color.
 * @param[out] ladder_pos Judgment of ladder capturable intersections.
 * @~japanese
 * @brief シチョウで取られる手の探索
 * @param[in] game 局面情報
 * @param[in] color 手番の色
 * @param[out] ladder_pos シチョウで取られる箇所の判定
 */
void
LadderExtension( game_info_t *game, int color, bool *ladder_pos )
{
  const string_t *string = game->string;
  std::unique_ptr<search_game_info_t> search_game;
  bool checked[BOARD_MAX] = { false };

  for (int i = 0; i < MAX_STRING; i++) {
    if (!string[i].flag ||
        string[i].color != color) {
      continue;
    }
    // アタリから逃げる着手箇所
    int ladder = string[i].lib[0];

    bool flag = false;

    // アタリを逃げる手で未探索のものを確認
    if (!checked[ladder] && string[i].libs == 1) {
      if (!search_game)
        search_game.reset(new search_game_info_t(game));
      search_game_info_t *ladder_game = search_game.get();
      // 隣接する敵連を取って助かるかを確認
      int neighbor = string[i].neighbor[0];
      while (neighbor != NEIGHBOR_END && !flag) {
        if (string[neighbor].libs == 1) {
          if (IsLegal(game, string[neighbor].lib[0], color)) {
            PutStoneForSearch(ladder_game, string[neighbor].lib[0], color);
            if (IsLadderCaptured(0, ladder_game, string[i].origin, GetOppositeColor(color)) == DEAD) {
              if (string[i].size >= 2) {
                ladder_pos[string[neighbor].lib[0]] = true;
              }
            } else {
              flag = true;
            }
            Undo(ladder_game);
          }
        }
        neighbor = string[i].neighbor[neighbor];
      }

      // 取って助からない時は逃げてみる
      if (!flag) {
        if (IsLegal(game, ladder, color)) {
          PutStoneForSearch(ladder_game, ladder, color);
          if (string[i].size >= 2 &&
              IsLadderCaptured(0, ladder_game, ladder, GetOppositeColor(color)) == DEAD) {
            ladder_pos[ladder] = true;
          }
          Undo(ladder_game);
        }
      }
      checked[ladder] = true;
    }
  }
}


/**
 * @~english
 * @brief Ladder search.
 * @param[in] depth Search depth.
 * @param[in] game Fast board position data.
 * @param[in] ren_xy String coordinate.
 * @param[in] turn_color Player's color.
 * @return Ladder capturable flag.
 * @~japanese
 * @brief シチョウ探索
 * @param[in] depth 探索深さ
 * @param[in] game 局面情報
 * @param[in] ren_xy 逃げる連の座標
 * @param[in] turn_color 手番の色
 * @return シチョウで取られるか否かのフラグ
 */
static bool
IsLadderCaptured( const int depth, search_game_info_t *game, const int ren_xy, const int turn_color )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int str = game->string_id[ren_xy];
  int escape_color, capture_color, escape_xy, capture_xy, neighbor;
  bool result;
  
  if (depth >= 100 || game->moves >= MAX_RECORDS - 1) {
    return ALIVE;
  }

  if (board[ren_xy] == S_EMPTY) {
    return DEAD;
  } else if (string[str].libs >= 3) {
    return ALIVE;
  }

  escape_color = board[ren_xy];
  capture_color = GetOppositeColor(escape_color);

  if (turn_color == escape_color) {
    // 周囲の敵連が取れるか確認し,
    // 取れるなら取って探索を続ける
    neighbor = string[str].neighbor[0];
    while (neighbor != NEIGHBOR_END) {
      if (string[neighbor].libs == 1) {
        if (IsLegalForSearch(game, string[neighbor].lib[0], escape_color)) {
          PutStoneForSearch(game, string[neighbor].lib[0], escape_color);
          result = IsLadderCaptured(depth + 1, game, ren_xy, GetOppositeColor(turn_color));
          Undo(game);
          if (result == ALIVE) {
            return ALIVE;
          }
        }
      }
      neighbor = string[str].neighbor[neighbor];
    }
    
    // 逃げる手を打ってみて探索を続ける
    escape_xy = string[str].lib[0];
    while (escape_xy != LIBERTY_END) {
      if (IsLegalForSearch(game, escape_xy, escape_color)) {
        PutStoneForSearch(game, escape_xy, escape_color);
        result = IsLadderCaptured(depth + 1, game, ren_xy, GetOppositeColor(turn_color));
        Undo(game);
        if (result == ALIVE) {
          return ALIVE;
        }
      }
      escape_xy = string[str].lib[escape_xy];
    }
    return DEAD;
  } else {
    if (string[str].libs == 1) {
      return DEAD;
    }
    // 追いかける側なのでアタリにする手を打ってみる
    capture_xy = string[str].lib[0];
    while (capture_xy != LIBERTY_END) {
      if (IsLegalForSearch(game, capture_xy, capture_color)) {
        PutStoneForSearch(game, capture_xy, capture_color);
        result = IsLadderCaptured(depth + 1, game, ren_xy, GetOppositeColor(turn_color));
        Undo(game);
        if (result == DEAD) {
          return DEAD;
        }
      }
      capture_xy = string[str].lib[capture_xy];
    }
  }

  return ALIVE;
}


/**
 * @~english
 * @brief Check capturable ladder escape.
 * @param[in] game Board position data.
 * @param[in] pos Intersection coordinate.
 * @param[in] color Player's color.
 * @return Ladder capturable flag.
 * @~japanese
 * @brief 助からないシチョウを逃げる手か判定
 * @param[in] game 局面情報
 * @param[in] pos 確認する座標
 * @param[in] color 手番の色
 * @return シチョウで取られるか否かのフラグ
 */
bool
CheckLadderExtension( const game_info_t *game, const int color, const int pos )
{
  const char *board = game->board;
  const string_t *string = game->string;
  const int *string_id = game->string_id;

  if (board[pos] != color){
    return false;
  }

  const int id = string_id[pos];
  const int ladder = string[id].lib[0];

  if (string[id].libs == 1 &&
      IsLegal(game, ladder, color)) {
    std::unique_ptr<search_game_info_t> search_game(new search_game_info_t(game));
    search_game_info_t *ladder_game = search_game.get();
    PutStoneForSearch(ladder_game, ladder, color);
    if (IsLadderCaptured(0, ladder_game, ladder, GetOppositeColor(color)) == DEAD) {
      return true;
    } 
  }

  return false;
}
