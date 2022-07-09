#include <iostream>

#include "board/GoBoard.hpp"
#include "mcts/SearchManager.hpp"
#include "util/Utility.hpp"



static SearchTimeStrategy search_setting = SearchTimeStrategy::ConstantTimeMode;

static double const_thinking_time = CONST_TIME;

static double playout = CONST_PLAYOUT;

static double time_limit = 0.0;

static bool extend_time = false;

static double remaining_time[S_MAX];

static double default_remaining_time = ALL_THINKING_TIME;

static int time_c = TIME_C_19;

static int time_maxply = TIME_MAXPLY_19;

static bool time_c_changed = false;

static bool time_maxply_changed = false;

static bool interruption_flag = true;

static ray_clock::time_point start_time;

static po_info_t po_info;


void
SetSearchSetting( const SearchTimeStrategy &new_setting )
{
  search_setting = new_setting;
}


void
SetInterruptionFlag( const bool flag )
{
  interruption_flag = flag;
}

void
SetPlayout( const int po )
{
  playout = po;
}

double
GetTimeLimit( void )
{
  return time_limit;
}

bool
IsTimeExtensionMode( void )
{
  return extend_time;
}

bool
IsConstPlayoutMode( void )
{
  return search_setting == SearchTimeStrategy::ConstantPlayoutMode;
}

void
ExtendSearchTime( const double multiplier )
{
  po_info.halt = static_cast<int>(multiplier * po_info.halt);
  time_limit *= multiplier;
}

void
SetTime( const double time )
{
  default_remaining_time = time;
}

void
SetConstThinkingTime( const double time )
{
  const_thinking_time = time;
}

void
SetCurrentRemainingTime( const int color, const double time )
{
  remaining_time[color] = time;
}

double
GetRemainingTime( const int color )
{
  return remaining_time[color];
}


void
InitializeTimeSetting( void )
{
  for (int i = 0; i < S_MAX; i++) {
    remaining_time[i] = default_remaining_time;
  }

  switch (search_setting) {
    case SearchTimeStrategy::ConstantPlayoutMode:
      time_limit = 10000.0;
      po_info.num = playout;
      extend_time = false;
      break;
    case SearchTimeStrategy::ConstantTimeMode:
      time_limit = const_thinking_time;
      po_info.num = 10000000;
      extend_time = false;
      break;
    case SearchTimeStrategy::TimeControlMode:
    case SearchTimeStrategy::TimeControlWithByoYomiMode:
      time_limit = CalculateNextTimeLimit(S_EMPTY, -1);
      po_info.num = static_cast<int>(PLAYOUT_SPEED * time_limit);
      extend_time = true;
      break;
  }
}

void
SetTimeSettings( const int main_time, const int byoyomi, const int stones )
{
  if (search_setting == SearchTimeStrategy::ConstantPlayoutMode ||
      search_setting == SearchTimeStrategy::ConstantTimeMode ) {
    return ;
  }

  if (main_time == 0) {
    const_thinking_time = (double)byoyomi * 0.85;
    search_setting = SearchTimeStrategy::ConstantTimeMode;
    std::cerr << "Const Thinking Time Mode" << std::endl;
  } else {
    if (byoyomi == 0) {
      default_remaining_time = main_time;
      search_setting = SearchTimeStrategy::TimeControlMode;
      std::cerr << "Time Setting Mode" << std::endl;
    } else {
      default_remaining_time = main_time;
      const_thinking_time = (double)byoyomi / stones;
      search_setting = SearchTimeStrategy::TimeControlWithByoYomiMode;
      std::cerr << "Time Setting Mode (byoyomi)" << std::endl;
    }
  }
}



void
CalculateNextPlayouts( const game_info_t *game, const int color, const double best_wp, const double finish_time, const int threads )
{
  double po_per_sec;

  if (finish_time > 0.1) {
    po_per_sec = po_info.count / finish_time;
  } else {
    po_per_sec = PLAYOUT_SPEED * threads;
  }

  if (search_setting == SearchTimeStrategy::ConstantTimeMode) {
    if (best_wp > 0.95) {
      po_info.num = static_cast<int>(po_info.count / finish_time * const_thinking_time * 0.25);
    } else if (best_wp > 0.90) {
      po_info.num = static_cast<int>(po_info.count / finish_time * const_thinking_time * 0.50);
    } else {
      po_info.num = static_cast<int>(po_info.count / finish_time * const_thinking_time);

    }                     
  } else if (search_setting == SearchTimeStrategy::TimeControlMode ||
             search_setting == SearchTimeStrategy::TimeControlWithByoYomiMode) {
    ConsumeTime(color, finish_time);
    time_limit = CalculateNextTimeLimit(color, game->moves);
    if (search_setting == SearchTimeStrategy::TimeControlWithByoYomiMode &&
        time_limit < const_thinking_time) {
      time_limit = const_thinking_time * 0.85;
    }
    if (best_wp > 0.95) {
      time_limit *= 0.25;
    } else if (best_wp > 0.90) {
      time_limit *= 0.5;
    }
    po_info.num = static_cast<int>(po_per_sec * time_limit);
  }
}


void
SetTimeManagementParameter( void )
{
  if (!time_c_changed) {
    if (pure_board_size < 11) {
      time_c = TIME_C_9;
    } else if (pure_board_size < 16) {
      time_c = TIME_C_13;
    } else {
      time_c = TIME_C_19;
    }
  }

  if (!time_maxply_changed) {
    if (pure_board_size < 11) {
      time_maxply = TIME_MAXPLY_9;
    } else if (pure_board_size < 16) {
      time_maxply = TIME_MAXPLY_13;
    } else {
      time_maxply = TIME_MAXPLY_19;
    }
  }
}


void ConsumeTime( const int color, const double finish_time )
{
  remaining_time[color] -= finish_time;
}


double
CalculateNextTimeLimit( const int color, const int moves )
{
  return remaining_time[color] / (time_c + ((time_maxply - (moves + 1) > 0) ? time_maxply - (moves + 1) : 0));
}


void
ResetTimeC( const int new_time_c )
{
  time_c = new_time_c;
  time_c_changed = true;
}

void
ResetTimeMaxply( const int new_time_maxply )
{
  time_maxply = new_time_maxply;
  time_maxply_changed = true;
}


double
CalculatePlayoutSpeed( const double consume_time, const int threads )
{
  if (consume_time < 0.001) {
    return PLAYOUT_SPEED * threads;
  } else {
    return po_info.count / consume_time;
  }
}


void
StartTimer( void )
{
  start_time = ray_clock::now();
}


double
CalculateElapsedTime( void )
{
  return GetSpendTime(start_time);
}


bool
IsTimeOver( void )
{
  return CalculateElapsedTime() > GetTimeLimit();
}
  

bool
IsSearchContinue( void )
{
  return po_info.count < po_info.halt;
}


void
ResetPoCount( void )
{
  po_info.count = 0;
}


int
GetPoCount( void )
{
  return po_info.count;
}


int
GetPoHalt( void )
{
  return po_info.halt;
}

int
GetPoNum( void )
{
  return po_info.num;
}

int
GetRestPoCount( void )
{
  return po_info.halt - po_info.count;
}



void
SetPoHalt( const int halt )
{
  po_info.halt = halt;
}


void
IncrementPoCount( void )
{
  atomic_fetch_add(&po_info.count , 1);
}



bool
CheckInterruption( const uct_node_t &root )
{
  int max = 0, second = 0;
  const int child_num = root.child_num;
  const int rest = GetRestPoCount();
  const child_node_t *child = root.child;

  if (!interruption_flag) {
    return false;
  }

  if (!IsConstPlayoutMode() &&
      CalculateElapsedTime() * 10.0 < GetTimeLimit()) {
    return false;
  }

  for (int i = 0; i < child_num; i++) {
    if (child[i].move_count > max) {
      second = max;
      max = child[i].move_count;
    } else if (child[i].move_count > second) {
      second = child[i].move_count;
    }
  }

  if (max - second > rest) {
    return true;
  } else {
    return false;
  }
}


bool
ExtendTime( const uct_node_t &root, const int moves )
{
  const int child_num = root.child_num;
  const child_node_t *child = root.child;
  int max = 0, second = 0;

  if (!IsTimeExtensionMode() ||
      moves < pure_board_size * 3 - 17) {
    return false;
  }
  
  const double root_wp = static_cast<double>(root.win) / root.move_count;

  if (root_wp > 0.80) {
    return false;
  }

  for (int i = 0; i < child_num; i++) {
    if (child[i].move_count > max) {
      second = max;
      max = child[i].move_count;
    } else if (child[i].move_count > second) {
      second = child[i].move_count;
    }
  }
  if (max < second * 1.2) {
    return true;
  } else {
    return false;
  }
}
