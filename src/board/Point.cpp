#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <iostream>

#include "board/GoBoard.hpp"
#include "board/Point.hpp"


constexpr char pass[] = "PASS";
constexpr char resign[] = "resign";


////////////////////////////////////
//  2次元表記から1次元表記へ変換  //
////////////////////////////////////
int
StringToInteger( const char *cpos )
{
  char alphabet;
  int x, y, pos;

  if (strcmp(cpos, "pass") == 0 || 
      strcmp(cpos, "PASS") == 0){
    pos = PASS;
  } else {
    alphabet = (char)toupper(cpos[0]);
    x = 0;
    for (int i = 1; i <= pure_board_size; i++) {
      if (gogui_x[i] == alphabet) {
        x = i;
      }
    }
    y = pure_board_size - atoi(&cpos[1]) + 1;
    pos = POS(x + (OB_SIZE - 1), y + (OB_SIZE - 1));
  }

  return pos;
}


////////////////////////////////////
//  1次元表記から2次元表記へ変換  //
////////////////////////////////////
void
IntegerToString( const int pos, char *cpos )
{
  int x, y;

  if (pos == PASS) {
#if defined (_WIN32)
    sprintf_s(cpos, 5, "%s", pass);
#else
    snprintf(cpos, 5, "%s", pass);
#endif
  } else if (pos == RESIGN) {
#if defined (_WIN32)
    sprintf_s(cpos, 7, "%s", resign);
#else
    snprintf(cpos, 7, "%s", resign);
#endif
  } else {
    x = X(pos) - (OB_SIZE - 1);
    y = pure_board_size - (Y(pos) - OB_SIZE);
    cpos[0] = gogui_x[x];
    if (y / 10 == 0) {
      cpos[1] = (char)('0' + y % 10);
      cpos[2] = '\0';
    } else {
      cpos[1] = (char)('0' + y / 10);
      cpos[2] = (char)('0' + y % 10);
      cpos[3] = '\0';
    }
  }
}


std::string
ParsePoint( const int pos )
{
  if (pos == PASS) {
    return std::string("PASS");
  } else if (pos == RESIGN) {
    return std::string("RESIGN");
  } else {
    return std::string(1, GOGUI_X(pos)) + std::to_string(GOGUI_Y(pos));
  }
}


std::string
ParseSgfPoint( const int x, const int y )
{
  std::string sgf_pos;

  if ((x ==  0 && y ==  0) ||
      (x == -1 && y == -1)) {
    sgf_pos = std::string("tt");
  } else {
    sgf_pos  = static_cast<char>(x - 1 + 'a');
    sgf_pos += static_cast<char>(y - 1 + 'a');
  }

  return sgf_pos;
}
