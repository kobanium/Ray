#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>
#include <cctype>
#include <iostream>

#include "DynamicKomi.h"
#include "Gtp.h"
#include "GoBoard.h"
#include "Nakade.h"
#include "UctSearch.h"
#include "UctRating.h"
#include "Message.h"
#include "Point.h"
#include "Rating.h"
#include "Simulation.h"
#include "ZobristHash.h"

using namespace std;

// gtpの出力用関数
static void GTP_response( const char *res, bool success );
// boardsizeコマンドを処理
static void GTP_boardsize( void );
// clearboardコマンドを処理
static void GTP_clearboard( void );
// nameコマンドを処理
static void GTP_name( void );
// protocolversionコマンドを処理
static void GTP_protocolversion( void );
// genmoveコマンドを処理
static void GTP_genmove( void );
// playコマンドを処理
static void GTP_play( void );
// knowncommandコマンドを処理
static void GTP_knowncommand( void );
// listcommandsコマンドを処理
static void GTP_listcommands( void );
// quitコマンドを処理
static void GTP_quit( void );
// komiコマンドを処理
static void GTP_komi( void );
// getkomiコマンドを処理
static void GTP_getkomi( void );
// finalscoreコマンドを処理
static void GTP_finalscore( void );
// timesettingsコマンドを処理
static void GTP_timesettings( void );
// timeleftコマンドを処理
static void GTP_timeleft( void );
// versionコマンドを処理
static void GTP_version( void );
// showboardコマンドを処理
static void GTP_showboard( void );
// kgs-genmove_cleanupコマンドを処理
static void GTP_kgs_genmove_cleanup( void );
// final_status_listコマンドを処理
static void GTP_final_status_list( void );
// set_free_handicapコマンドを処理
static void GTP_set_free_handicap( void );
// fixed_handicapコマンドを処理
static void GTP_fixed_handicap( void );


const GTP_command_t gtpcmd[GTP_COMMAND_NUM] = {
  { "quit",                GTP_quit                },
  { "protocol_version",    GTP_protocolversion     },
  { "name",                GTP_name                },
  { "version",             GTP_version             },
  { "boardsize",           GTP_boardsize           },
  { "clear_board",         GTP_clearboard          },
  { "komi",                GTP_komi                },
  { "get_komi",            GTP_getkomi             },
  { "play",                GTP_play                },
  { "fixed_handicap",      GTP_fixed_handicap      },
  { "place_free_handicap", GTP_fixed_handicap      },
  { "set_free_handicap",   GTP_set_free_handicap   },
  { "genmove",             GTP_genmove             }, 
  { "time_settings",       GTP_timesettings        },
  { "time_left",           GTP_timeleft            },
  { "final_score",         GTP_finalscore          },
  { "final_status_list",   GTP_final_status_list   },
  { "showboard",           GTP_showboard           },
  { "list_commands",       GTP_listcommands        },
  { "known_command",       GTP_knowncommand        },
  { "kgs-genmove_cleanup", GTP_kgs_genmove_cleanup },
};

char input[BUF_SIZE], input_copy[BUF_SIZE];
char *next_token;

char brank[] = "";
char err_command[] = "? unknown command";
char err_genmove[] = "gemmove color";
char err_play[] = "play color point";
char err_komi[] = "komi float";

int player_color = 0;

game_info_t *game;


///////////////////////
//  void GTP_main()  //
///////////////////////
void
GTP_main( void )
{
  game = AllocateGame();
  InitializeBoard(game);

  while (fgets(input, sizeof(input), stdin) != NULL) {
    char *command;
    bool nocommand = true;

    STRCPY(input_copy, BUF_SIZE, input);
    command = STRTOK(input, DELIM, &next_token);
    CHOMP(command);

    for (int i = 0; i < GTP_COMMAND_NUM; i++) {
      if (!strcmp(command, gtpcmd[i].command)) {
	StopPondering();
	(*gtpcmd[i].function)();
	nocommand = false;
	break;
      }
    }

    if (nocommand) {
      cout << err_command << endl << endl;
    }

    fflush(stdin);
    fflush(stdout);
  }
}


/////////////////////////////////////////////////
//  void GTP_response(char *res, int success)  //
/////////////////////////////////////////////////
static void
GTP_response( const char *res, bool success )
{
  if (success){
    cout << "= " << res << endl << endl;
  } else {
    if (res != NULL) {
      cerr << res << endl;
    }
    cout << "?" << endl << endl;
  }
}


/////////////////////////////
//　 void GTP_boardsize()  //
/////////////////////////////
static void
GTP_boardsize( void )
{
  char *command;
  int size;
  char buf[1024];
  
  command = STRTOK(NULL, DELIM, &next_token);

#if defined (_WIN32)
  sscanf_s(command, "%d", &size);
  sprintf_s(buf, 1024, " ");
#else
  sscanf(command, "%d", &size);
  snprintf(buf, 1024, " ");
#endif

  if (pure_board_size != size &&
      size <= PURE_BOARD_SIZE && size > 0) {
    SetBoardSize(size);
    SetParameter();
    SetNeighbor();
    InitializeNakadeHash();
  }

  FreeGame(game);
  game = AllocateGame();
  InitializeBoard(game);
  InitializeSearchSetting();
  InitializeUctHash();

  GTP_response(brank, true);
}
  

/////////////////////////////
//  void GTP_clearboard()  //
/////////////////////////////
static void
GTP_clearboard( void )
{
  player_color = 0;
  SetHandicapNum(0);
  FreeGame(game);
  game = AllocateGame();
  InitializeBoard(game);
  InitializeSearchSetting();
  InitializeUctHash();

  GTP_response(brank, true);
}
  

///////////////////////
//  void GTP_name()  //
///////////////////////
static void
GTP_name( void )
{
  GTP_response(PROGRAM_NAME, true);
}


//////////////////////////////////
//  void GTP_protocolversion()  //
//////////////////////////////////
static void
GTP_protocolversion( void )
{
  GTP_response(PROTOCOL_VERSION, true);
}


//////////////////////////
//  void GTP_genmove()  //
//////////////////////////
static void
GTP_genmove( void )
{
  char *command;
  char c;
  char pos[10];
  int color;
  int point = PASS;
  
  command = STRTOK(input_copy, DELIM, &next_token);
  
  CHOMP(command);

  command = STRTOK(NULL, DELIM, &next_token);
  if (command == NULL){
    GTP_response(err_genmove, true);
    return;
  }
  CHOMP(command);
  c = (char)tolower((int)command[0]);
  if (c == 'w') {
    color = S_WHITE;
  } else if (c == 'b') {
    color = S_BLACK;
  } else {
    GTP_response(err_genmove, true);
    return;
  }

  player_color = color;
  
  point = UctSearchGenmove(game, color);
  if (point != RESIGN) {
    PutStone(game, point, color);
  }
  
  IntegerToString(point, pos);
  
  GTP_response(pos, true);

  UctSearchPondering(game, FLIP_COLOR(color));
}


///////////////////////
//  void GTP_play()  //
///////////////////////
static void
GTP_play( void )
{
  char *command;
  char c;
  int color, pos = 0;
  
  command = STRTOK(input_copy, DELIM, &next_token);

  command = STRTOK(NULL, DELIM, &next_token);
  if (command == NULL){
    GTP_response(err_play, false);
    return;
  }
  CHOMP(command);
  c = (char)tolower((int)command[0]);
  if (c == 'w') {
    color = S_WHITE;
  } else{
    color = S_BLACK;
  }

  command = STRTOK(NULL, DELIM, &next_token);
  
  CHOMP(command);
  if (command == NULL){
    GTP_response(err_play, false);
    return;
  } else {
    pos = StringToInteger(command);
  }

  if (pos != RESIGN) {
    PutStone(game, pos, color);
  }
  
  GTP_response(brank, true);
}
 

/////////////////////////////
// void GTP_knowncommand() //
/////////////////////////////
static void
GTP_knowncommand( void )
{
  char *command;
  
  command = STRTOK(NULL, DELIM, &next_token);
  
  if (command == NULL){
    GTP_response("known_command command", false);
    return;
  }
  CHOMP(command);
  for (int i = 0; i < GTP_COMMAND_NUM; i++){
    if (!strcmp(command, gtpcmd[i].command)) {
      GTP_response("true", true);
      return;
    }
  }
  GTP_response("false", false);
}
 
 
///////////////////////////////
//  void GTP_listcommands()  //
///////////////////////////////
static void
GTP_listcommands( void )
{
  char list[2048];
  int i;

  i = 0;
  list[i++] = '\n';
  for (int j = 0; j < GTP_COMMAND_NUM; j++) {
    for (unsigned int k = 0; k < strlen(gtpcmd[j].command); k++){
      list[i++] = gtpcmd[j].command[k];
    }
    list[i++] = '\n';
  }
  list[i++] = '\0';

  GTP_response(list, true);
}
 
 
//////////////////////
// void GTP_quit()  //
//////////////////////
static void
GTP_quit( void )
{
  FinalizeUctSearch();
  GTP_response(brank, true);
  exit(0);
}
 
 
///////////////////////
//  void GTP_komi()  //
///////////////////////
static void
GTP_komi( void )
{
  char* c_komi;
  
  c_komi = STRTOK(NULL, DELIM, &next_token);

  if (c_komi != NULL) {
    SetKomi(atof(c_komi));
    PrintKomiValue();
    GTP_response(brank, true);
  } else {
    GTP_response(err_komi, false);
  }
}
 

//////////////////////////
//  void GTP_getkomi()  //
//////////////////////////
static void
GTP_getkomi( void )
{
  char buf[256];
  
#if defined(_WIN32)
  sprintf_s(buf, 4, "%lf", komi[0]);
#else
  snprintf(buf, 4, "%lf", komi[0]);
#endif
  GTP_response(buf, true);
}


/////////////////////////////
//  void GTP_finalscore()  //
/////////////////////////////
static void
GTP_finalscore( void )
{
  char buf[10];
  double score = 0;
  
  score = UctAnalyze(game, S_BLACK) - komi[0];

#if defined(_WIN32)  
  if (score > 0) {
    sprintf_s(buf, 10, "B+%.1lf", score);
  } else {
    sprintf_s(buf, 10, "W+%.1lf", abs(score));
  }
#else
  if (score > 0) {
    snprintf(buf, 10, "B+%.1lf", score);
  } else {
    snprintf(buf, 10, "W+%.1lf", abs(score));
  }
#endif

  GTP_response(buf, true);
}
 

///////////////////////////////
//  void GTP_timesettings()  //
///////////////////////////////
static void
GTP_timesettings( void )
{
  char *str1, *str2, *str3;
  double main_time, byoyomi, stone;

  str1 = STRTOK(NULL, DELIM, &next_token);
  str2 = STRTOK(NULL, DELIM, &next_token);
  str3 = STRTOK(NULL, DELIM, &next_token);

  main_time = atoi(str1);
  byoyomi = atoi(str2);
  stone = atoi(str3);

  cerr << main_time << "," << byoyomi << "," << stone << endl;

  SetTimeSettings(main_time, byoyomi, stone);
  InitializeSearchSetting();
  
  GTP_response(brank, true);
}


///////////////////////////
//  void GTP_timeleft()  //
///////////////////////////
static void
GTP_timeleft( void )
{
  char *str1, *str2;

  str1 = STRTOK(NULL, DELIM, &next_token);
  str2 = STRTOK(NULL, DELIM, &next_token);
  
  if (str1[0] == 'B' || str1[0] == 'b'){
    remaining_time[S_BLACK] = atof(str2);
  } else if (str1[0] == 'W' || str1[0] == 'w'){
    remaining_time[S_WHITE] = atof(str2);
  }
  
  fprintf(stderr, "%f\n", remaining_time[S_BLACK]);
  fprintf(stderr, "%f\n", remaining_time[S_WHITE]);
  GTP_response(brank, true);
}


//////////////////////////
//  void GTP_version()  //
//////////////////////////
static void
GTP_version( void )
{
  GTP_response(PROGRAM_VERSION, true);
}
 
 
////////////////////////////
//  void GTP_showboard()  //
////////////////////////////
static void
GTP_showboard( void )
{
  PrintBoard(game);
  GTP_response(brank, true);
}


/////////////////////////////////
//  void GTP_fixed_handicap()  //
/////////////////////////////////
static void
GTP_fixed_handicap( void )
{
  char *command;
  int num;
  char buf[1024];
  char pos[5];
  int handicap[9];
  const int place_index[8][9] = {
    {2, 6},
    {0, 2, 6},
    {0, 2, 6, 8},
    {0, 2, 4, 6, 8},
    {0, 2, 3, 5, 6, 8},
    {0, 2, 3, 4, 5, 6, 8},
    {0, 1, 2, 3, 5, 6, 7, 8},
    {0, 1, 2, 3, 4, 5, 6, 7, 8},
  };
  
  command = STRTOK(NULL, DELIM, &next_token);
  
#if defined (_WIN32)
  sscanf_s(command, "%d", &num);
  sprintf_s(buf, 1024, " ");
#else
  sscanf(command, "%d", &num);
  snprintf(buf, 1024, " ");
#endif

  if (num < 2 || 9 < num) {
    GTP_response(brank, false);
    return ;
  }

  handicap[0] = POS(board_start +  3, board_start +  3);
  handicap[1] = POS(board_start +  9, board_start +  3);
  handicap[2] = POS(board_start + 15, board_start +  3);
  handicap[3] = POS(board_start +  3, board_start +  9);
  handicap[4] = POS(board_start +  9, board_start +  9);
  handicap[5] = POS(board_start + 15, board_start +  9);
  handicap[6] = POS(board_start +  3, board_start + 15);
  handicap[7] = POS(board_start +  9, board_start + 15);
  handicap[8] = POS(board_start + 15, board_start + 15);

  for (int i = 0; i < num; i++) {
    PutStone(game, handicap[place_index[num - 2][i]], S_BLACK);
#if defined (_WIN32)
    sprintf_s(pos, 5, "%c%d ", GOGUI_X(handicap[place_index[num - 2][i]]), GOGUI_Y(handicap[place_index[num - 2][i]]));
    strcat_s(buf, 1024, pos);
#else
    snprintf(pos, 5, "%c%d ", GOGUI_X(handicap[place_index[num - 2][i]]), GOGUI_Y(handicap[place_index[num - 2][i]]));
    strncat(buf, pos, 5);
#endif
  }
  
  SetKomi(0.5);
  SetHandicapNum(num);
  GTP_response(buf, true);
}


////////////////////////////////////
//  void GTP_set_free_handicap()  //
////////////////////////////////////
static void
GTP_set_free_handicap( void )
{
  char *command;
  int pos, num = 0;
  
  while (1){
    command = STRTOK(NULL, DELIM, &next_token);

    if (command == NULL){
      SetHandicapNum(num);
      SetKomi(0.5);
      GTP_response(brank, true);
      return;
    }
    
    pos = StringToInteger(command);
    
    if (pos > 0 && pos < board_max && IsLegal(game, pos, S_BLACK)) {
      PutStone(game, pos, S_BLACK);
      num++;
    }
  }
}


////////////////////////////////////
//  void GTP_final_status_list()  //
////////////////////////////////////
static void
GTP_final_status_list( void )
{
  char dead[2048] = { 0 };
  char pos[5];
  int owner[BOARD_MAX]; 
  char *command;
  
  OwnerCopy(owner);
  
  command = STRTOK(NULL, DELIM, &next_token);

  CHOMP(command);
  
  if (!strcmp(command, "dead")){
    for (int y = board_start; y <= board_end; y++) {
      for (int x = board_start; x <= board_end; x++) {
	if ((game->board[POS(x, y)] == player_color && owner[POS(x, y)] <= 30) ||
	    (game->board[POS(x, y)] == FLIP_COLOR(player_color) && owner[POS(x, y)] >= 70)) {
#if defined (_WIN32)
	  sprintf_s(pos, 5, "%c%d ", GOGUI_X(POS(x, y)), GOGUI_Y(POS(x, y)));
	  strcat_s(dead, 2048, pos);
#else
	  snprintf(pos, 5, "%c%d ", GOGUI_X(POS(x, y)), GOGUI_Y(POS(x, y)));
	  strncat(dead, pos, 5);
#endif
	}
      }
    }
  } else if (!strcmp(command, "alive")){
    for (int y = board_start; y <= board_end; y++) {
      for (int x = board_start; x <= board_end; x++) {
	if ((game->board[POS(x, y)] == player_color && owner[POS(x, y)] >= 70) ||
	    (game->board[POS(x, y)] == FLIP_COLOR(player_color) && owner[POS(x, y)] <= 30)) {
#if defined (_WIN32)
	  sprintf_s(pos, 5, "%c%d ", GOGUI_X(POS(x, y)), GOGUI_Y(POS(x, y)));
	  strcat_s(dead, 2048, pos);
#else
	  snprintf(pos, 5, "%c%d ", GOGUI_X(POS(x, y)), GOGUI_Y(POS(x, y)));
	  strncat(dead, pos, 5);
#endif
	}
      }
    }
  }
  
  GTP_response(dead, true);
}


//////////////////////////////////////
//  void GTP_kgs_genmove_cleanup()  //
//////////////////////////////////////
static void
GTP_kgs_genmove_cleanup( void )
{
  char *command;
  char c;
  char pos[10];
  int color;
  int point = PASS;
  
  command = STRTOK(input_copy, DELIM, &next_token);
  
  CHOMP(command);
  if (!strcmp("genmove_black", command)) {
    color = S_BLACK;
  } else if (!strcmp("genmove_white", command)) {
    color = S_WHITE;
  } else {
    command = STRTOK(NULL, DELIM, &next_token);
    if (command == NULL){
      GTP_response(err_genmove, false);
      return;
    }
    CHOMP(command);
    c = (char)tolower((int)command[0]);
    if (c == 'w') {
      color = S_WHITE;
    } else if (c == 'b') {
      color = S_BLACK;
    } else {
      GTP_response(err_genmove, false);
      return;
    }
  }
  
  player_color = color;
  
  point = UctSearchGenmoveCleanUp(game, color);
  if (point != RESIGN) {
    PutStone(game, point, color);
  }
  
  IntegerToString(point, pos);
  
  GTP_response(pos, true);
}
 

