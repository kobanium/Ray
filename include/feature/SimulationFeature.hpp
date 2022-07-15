#ifndef _SIMULATION_FEATURE_HPP_
#define _SIMULATION_FEATURE_HPP_

#include <string>

#include "board/GoBoard.hpp"
#include "feature/FeatureUtility.hpp"


#define TF_INDEX(p, f) ((p) * ALL_MAX + (f))

enum ALL_FEATURE {
  CAPTURE,
  SAVE_EXTENSION,
  ATARI,
  EXTENSION,
  DAME,
  THROW_IN,
  ALL_MAX,
};


enum CAPTURE_FEATURE {
  SIM_CAPTURE_NONE,
  SIM_SELF_ATARI_SMALL,
  SIM_SELF_ATARI_NAKADE,
  SIM_SELF_ATARI_LARGE,
  SIM_CAPTURE,

  SIM_3POINT_CAPTURE_SMALL,
  SIM_2POINT_CAPTURE_SMALL,
  SIM_3POINT_CAPTURE_LARGE,
  SIM_2POINT_CAPTURE_LARGE,
  SIM_CAPTURE_AFTER_KO,

  SIM_SAVE_CAPTURE_SELF_ATARI,
  SIM_SAVE_CAPTURE_1_1,
  SIM_SAVE_CAPTURE_1_2,
  SIM_SAVE_CAPTURE_1_3,
  SIM_SAVE_CAPTURE_2_1,

  SIM_SAVE_CAPTURE_2_2,
  SIM_SAVE_CAPTURE_2_3,
  SIM_SAVE_CAPTURE_3_1,
  SIM_SAVE_CAPTURE_3_2,
  SIM_SAVE_CAPTURE_3_3,

  SIM_CAPTURE_MAX,
};


enum SAVE_EXTENSION_FEATURE {
  SIM_SAVE_EXTENSION_NONE,
  SIM_SAVE_EXTENSION_1,
  SIM_SAVE_EXTENSION_2,
  SIM_SAVE_EXTENSION_3,
  SIM_SAVE_EXTENSION_SAFELY_1,

  SIM_SAVE_EXTENSION_SAFELY_2,
  SIM_SAVE_EXTENSION_SAFELY_3,

  SIM_SAVE_EXTENSION_MAX,
};


enum ATARI_FEATURE {
  SIM_ATARI_NONE,
  SIM_ATARI,
  SIM_3POINT_ATARI_SMALL,
  SIM_2POINT_ATARI_SMALL,
  SIM_3POINT_ATARI_LARGE,

  SIM_2POINT_ATARI_LARGE,
  SIM_3POINT_C_ATARI_SMALL,
  SIM_2POINT_C_ATARI_SMALL,
  SIM_3POINT_C_ATARI_LARGE,
  SIM_2POINT_C_ATARI_LARGE,

  SIM_ATARI_MAX,
};

enum EXTENSION_FEATURE {
  SIM_EXTENSION_NONE,
  SIM_3POINT_EXTENSION,
  SIM_2POINT_EXTENSION,
  SIM_3POINT_EXTENSION_SAFELY,
  SIM_2POINT_EXTENSION_SAFELY,

  SIM_EXTENSION_MAX,
};

enum DAME_FEATURE {
  SIM_DAME_NONE,
  SIM_3POINT_DAME_SMALL,
  SIM_3POINT_DAME_LARGE,

  SIM_DAME_MAX,
};

enum THROW_IN_FEATURE {
  SIM_THROW_IN_NONE,
  SIM_THROW_IN_2,

  SIM_THROW_IN_MAX,
};



const std::string sim_capture_name[SIM_CAPTURE_MAX] = {
  "SIM_CAPTURE_NONE",
  "SIM_SELF_ATARI_SMALL",
  "SIM_SELF_ATARI_NAKADE",
  "SIM_SELF_ATARI_LARGE",
  "SIM_CAPTURE",
  "SIM_3POINT_CAPTURE_SMALL",
  "SIM_2POINT_CAPTURE_SMALL",
  "SIM_3POINT_CAPTURE_LARGE",
  "SIM_2POINT_CAPTURE_LARGE",
  "SIM_CAPTURE_AFTER_KO",
  "SIM_SAVE_CAPTURE_SELF_ATARI",
  "SIM_SAVE_CAPTURE_1_1",
  "SIM_SAVE_CAPTURE_1_2",
  "SIM_SAVE_CAPTURE_1_3",
  "SIM_SAVE_CAPTURE_2_1",
  "SIM_SAVE_CAPTURE_2_2",
  "SIM_SAVE_CAPTURE_2_3",
  "SIM_SAVE_CAPTURE_3_1",
  "SIM_SAVE_CAPTURE_3_2",
  "SIM_SAVE_CAPTURE_3_3",
};

const std::string sim_save_extension_name[SIM_SAVE_EXTENSION_MAX] = {
  "SIM_SAVE_EXTENSION_NONE",
  "SIM_SAVE_EXTENSION_1",
  "SIM_SAVE_EXTENSION_2",
  "SIM_SAVE_EXTENSION_3",
  "SIM_SAVE_EXTENSION_SAFELY_1",
  "SIM_SAVE_EXTENSION_SAFELY_2",
  "SIM_SAVE_EXTENSION_SAFELY_3",
};

const std::string sim_atari_name[SIM_ATARI_MAX] = {
  "SIM_ATARI_NONE",
  "SIM_ATARI",
  "SIM_3POINT_ATARI_SMALL",
  "SIM_2POINT_ATARI_SMALL",
  "SIM_3POINT_ATARI_LARGE",
  "SIM_2POINT_ATARI_LARGE",
  "SIM_3POINT_C_ATARI_SMALL",
  "SIM_2POINT_C_ATARI_SMALL",
  "SIM_3POINT_C_ATARI_LARGE",
  "SIM_2POINT_C_ATARI_LARGE",
};

const std::string sim_extension_name[SIM_EXTENSION_MAX] = {
  "SIM_EXTENSION_NONE",
  "SIM_3POINT_EXTENSION",
  "SIM_2POINT_EXTENSION",
  "SIM_3POINT_EXTENSION_SAFELY",
  "SIM_2POINT_EXTENSION_SAFELY",
};

const std::string sim_dame_name[SIM_DAME_MAX] = {
  "SIM_DAME_NONE",
  "SIM_3POINT_DAME_SMALL",
  "SIM_3POINT_DAME_LARGE",
};

const std::string sim_throw_in_name[SIM_THROW_IN_MAX] = {
  "SIM_THROW_IN_NONE",
  "SIM_THROW_IN_2",
};




const int SIM_TACTICAL_FEATURES = (SIM_CAPTURE_MAX - 1) +
  (SIM_SAVE_EXTENSION_MAX - 1) + (SIM_ATARI_MAX - 1) +
  (SIM_EXTENSION_MAX - 1) + (SIM_DAME_MAX - 1) + (SIM_THROW_IN_MAX - 1);


const int PREVIOUS_DISTANCE_MAX = 3;



inline void
ClearTacticalFeatures( unsigned char *features )
{
  features[CAPTURE] = 0;
  features[SAVE_EXTENSION] = 0;
  features[ATARI] = 0;
  features[EXTENSION] = 0;
  features[DAME] = 0;
  features[THROW_IN] = 0;
}


inline void
CompareSwapFeature( unsigned char *tactical_features, const int pos, const int type, const unsigned int new_feature )
{
  if (tactical_features[TF_INDEX(pos, type)] < new_feature) {
    tactical_features[TF_INDEX(pos, type)] = new_feature;
  }
}



void SetCrossPosition( void );

void CheckFeaturesForSimulation( game_info_t *game, const int color, int *update, int *update_num );

void CheckCaptureAfterKoForSimulation( game_info_t *game, const int color, int *update, int *update_num );

bool CheckSelfAtariForSimulation( game_info_t *game, const int color, const int pos );

void CheckCaptureAndAtariForSimulation( game_info_t *game, const int color, const int pos );

void CheckRemove2StonesForSimulation( game_info_t *game, const int color, int *update, int *update_num );

#endif

