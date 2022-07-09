#ifndef _SIMULATION_FEATURE_HPP_
#define _SIMULATION_FEATURE_HPP_

#include <string>

#include "board/GoBoard.hpp"
#include "feature/FeatureUtility.hpp"


enum FEATURE1{
  F_SAVE_CAPTURE1_1,
  F_SAVE_CAPTURE1_2,
  F_SAVE_CAPTURE1_3,
  F_SAVE_CAPTURE2_1,
  F_SAVE_CAPTURE2_2,
  F_SAVE_CAPTURE2_3,
  F_SAVE_CAPTURE3_1,
  F_SAVE_CAPTURE3_2,
  F_SAVE_CAPTURE3_3,
  F_SAVE_CAPTURE_SELF_ATARI,
  F_2POINT_CAPTURE_SMALL,
  F_2POINT_CAPTURE_LARGE,
  F_3POINT_CAPTURE_SMALL,
  F_3POINT_CAPTURE_LARGE,
  F_CAPTURE,
  F_CAPTURE_AFTER_KO,
  F_SAVE_EXTENSION_SAFELY1,
  F_SAVE_EXTENSION_SAFELY2,
  F_SAVE_EXTENSION_SAFELY3,
  F_SAVE_EXTENSION1,
  F_SAVE_EXTENSION2,
  F_SAVE_EXTENSION3,
  F_MAX1,
};

enum FEATURE2 {
  F_SELF_ATARI_SMALL,
  F_SELF_ATARI_NAKADE,
  F_SELF_ATARI_LARGE,
  F_ATARI,
  F_2POINT_ATARI_SMALL,
  F_2POINT_ATARI_LARGE,
  F_2POINT_C_ATARI_SMALL,
  F_2POINT_C_ATARI_LARGE,
  F_3POINT_ATARI_SMALL,
  F_3POINT_ATARI_LARGE,
  F_3POINT_C_ATARI_SMALL,
  F_3POINT_C_ATARI_LARGE,
  F_3POINT_DAME_SMALL,
  F_3POINT_DAME_LARGE,
  F_2POINT_EXTENSION_SAFELY,
  F_2POINT_EXTENSION,
  F_3POINT_EXTENSION_SAFELY,
  F_3POINT_EXTENSION,
  F_THROW_IN_2,
  F_MAX2,
};


const std::string po_features_name[F_MAX1 + F_MAX2] = {
  "SAVE_CAPTURE1_1         ",
  "SAVE_CAPTURE1_2         ",
  "SAVE_CAPTURE1_3         ",
  "SAVE_CAPTURE2_1         ",
  "SAVE_CAPTURE2_2         ",
  "SAVE_CAPTURE2_3         ",
  "SAVE_CAPTURE3_1         ",
  "SAVE_CAPTURE3_2         ",
  "SAVE_CAPTURE3_3         ",
  "SAVE_CAPTURE_SELF_ATARI ",
  "2POINT_CAPTURE_SMALL    ",
  "2POINT_CAPTURE_LARGE    ",
  "3POINT_CAPTURE_SMALL    ",
  "3POINT_CAPTURE_LARGE    ",
  "CAPTURE                 ",
  "CAPTURE_AFTER_KO        ",
  "SAVE_EXTENSION_SAFELY1  ",
  "SAVE_EXTENSION_SAFELY2  ",
  "SAVE_EXTENSION_SAFELY3  ",
  "SAVE_EXTENSION1         ",
  "SAVE_EXTENSION2         ",
  "SAVE_EXTENSION3         ",
  "SELF_ATARI_SMALL        ",
  "SELF_ATARI_NAKADE       ",
  "SELF_ATARI_LARGE        ",
  "ATARI                   ",
  "2POINT_ATARI_SMALL      ",
  "2POINT_ATARI_LARGE      ",
  "2POINT_C_ATARI_SMALL    ",
  "2POINT_C_ATARI_LARGE    ",
  "3POINT_ATARI_SMALL      ",
  "3POINT_ATARI_LARGE      ",
  "3POINT_C_ATARI_SMALL    ",
  "3POINT_C_ATARI_LARGE    ",
  "3POINT_DAME_SMALL       ",
  "3POINT_DAME_LARGE       ",
  "2POINT_EXTENSION_SAFELY ",
  "2POINT_EXTENSION        ",
  "3POINT_EXTENSION_SAFELY ",
  "3POINT_EXTENSION        ",
  "THROW_IN_2              ",
};


const int TACTICAL_FEATURE_MAX = F_MAX1 + F_MAX2;
const int PREVIOUS_DISTANCE_MAX = 3;

const int PO_TACTICALS_MAX1 = (1 << F_MAX1);
const int PO_TACTICALS_MAX2 = (1 << F_MAX2);



void SetCrossPosition( void );


void CheckFeaturesForSimulation( game_info_t *game, const int color, int *update, int *update_num );

void CheckCaptureAfterKoForSimulation( game_info_t *game, const int color, int *update, int *update_num );

bool CheckSelfAtariForSimulation( game_info_t *game, const int color, const int pos );


void CheckCaptureAndAtariForSimulation( game_info_t *game, const int color, const int pos );


void CheckRemove2StonesForSimulation( game_info_t *game, const int color, int *update, int *update_num );


#endif

