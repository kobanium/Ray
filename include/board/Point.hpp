#ifndef _POINT_HPP_
#define _POINT_HPP_

#include <string>

//////////////////
//  マクロ関数  //
//////////////////

#define GOGUI_X(pos) (gogui_x[CORRECT_X(pos)])               // GOGUIでのX座標の表記
#define GOGUI_Y(pos) (pure_board_size + 1 - CORRECT_Y(pos))  // GOGUIでのY座標の表記

////////////
//  定数  //
////////////

//  Y座標の文字
const char gogui_x[] = { 
  'I', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
  'U', 'V', 'W', 'X', 'Y', 'Z' 
};


////////////
//  関数  //
////////////

void LzIntegerToString( const int pos, char *cpos );

//  2次元表記から1次元表記へ変換  
int StringToInteger( const char *cpos );

//  1次元表記から2次元表記へ変換  
void IntegerToString( const int pos, char *cpos );

std::string ParsePoint( const int pos );

std::string ParseSgfPoint( const int x, const int y );

#endif
