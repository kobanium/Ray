#include <iostream>

#include "GoBoard.h"
#include "Point.h"
#include "Seki.h"
#include "Semeai.h"

using namespace std;



//////////////////
//  繧サ繧ュ縺ョ蛻、螳 //
//////////////////
void
CheckSeki( game_info_t *game, bool seki[] )
{
  int i, j, k, pos, id;
  char *board = game->board;
  int *string_id = game->string_id;
  string_t *string = game->string;
  bool seki_candidate[BOARD_MAX] = {false};
  int lib1, lib2;
  int lib1_id[4], lib2_id[4], lib1_ids, lib2_ids;
  int neighbor1_lib, neighbor2_lib;
  int neighbor4[4];
  bool already_checked;

  // 蜿梧婿縺瑚㌕蟾ア繧「繧ソ繝ェ縺ォ縺ェ縺」縺ヲ縺ｋ蠎ァ讓吶ｒ謚ス蜃コ
  for (i = 0; i < pure_board_max; i++) {
    pos = onboard_pos[i];
    if (IsSelfAtari(game, S_BLACK, pos) &&
	IsSelfAtari(game, S_WHITE, pos)) {
      seki_candidate[pos] = true;
    }
  }

  for (i = 0; i < MAX_STRING; i++) {
    // 騾」縺悟ュ伜惠縺励↑縺 
    // 縺セ縺溘騾」縺ョ蜻シ蜷ク轤ケ謨ー縺蛟九〒縺ェ縺代ｌ縺ー谺。繧定ェソ縺ケ繧    if (!string[i].flag || string[i].libs != 2) continue;

    // 騾」縺ョ螟ァ縺阪＆縺莉・荳翫↑繧峨す繝溘Η繝ャ繝シ繧キ繝ァ繝ウ縺ァ
    // 閾ェ蟾ア繧「繧ソ繝ェ繧呈遠縺溘↑縺縺ァ谺。繧定ェソ縺ケ繧    if (string[i].size >= 6) continue;

    lib1 = string[i].lib[0];
    lib2 = string[i].lib[lib1];
    // 騾」縺ョ謖√▽蜻シ蜷ク轤ケ縺悟縺ォ繧サ繧ュ縺ョ蛟呵」    if (seki_candidate[lib1] &&
	seki_candidate[lib2]) {
      // 蜻シ蜷ク轤ケ1縺ョ蜻ィ蝗イ縺ョ騾」縺ョID繧貞叙繧雁縺      GetNeighbor4(neighbor4, lib1);
      lib1_ids = 0;
      for (j = 0; j < 4; j++) {
	if (board[neighbor4[j]] == S_BLACK ||
	    board[neighbor4[j]] == S_WHITE) {
	  id = string_id[neighbor4[j]];
	  if (id != i) {
	    already_checked = false;
	    for (k = 0; k < lib1_ids; k++) {
	      if (lib1_id[k] == id) {
		already_checked = true;
		break;
	      }
	    }
	    if (!already_checked) {
	      lib1_id[lib1_ids++] = id;
	    }
	  }
	}
      }
      // 蜻シ蜷ク轤ケ2縺ョ蜻ィ蝗イ縺ョ騾」縺ョID繧貞叙繧雁縺      GetNeighbor4(neighbor4, lib2);
      lib2_ids = 0;
      for (j = 0; j < 4; j++) {
	if (board[neighbor4[j]] == S_BLACK ||
	    board[neighbor4[j]] == S_WHITE) {
	  id = string_id[neighbor4[j]];
	  if (id != i) {
	    already_checked = false;
	    for (k = 0; k < lib2_ids; k++) {
	      if (lib2_id[k] == id) {
		already_checked = true;
		break;
	      }
	    }
	    if (!already_checked) {
	      lib2_id[lib2_ids++] = id;
	    }
	  }
	}
      }

      if (lib1_ids == 1 && lib2_ids == 1) {
	neighbor1_lib = string[lib1_id[0]].lib[0];
	if (neighbor1_lib == lib1 ||
	    neighbor1_lib == lib2) {
	  neighbor1_lib = string[lib1_id[0]].lib[neighbor1_lib];
	}
	neighbor2_lib = string[lib2_id[0]].lib[0];
	if (neighbor2_lib == lib1 ||
	    neighbor2_lib == lib2) {
	  neighbor2_lib = string[lib2_id[0]].lib[neighbor2_lib];
	}
	if (neighbor1_lib == neighbor2_lib) {
	  if (eye_condition[Pat3(game->pat, neighbor1_lib)] != E_NOT_EYE) {
	    seki[lib1] = seki[lib2] = true;
	    seki[neighbor1_lib] = true;
	  }
	} else if (eye_condition[Pat3(game->pat, neighbor1_lib)] == E_COMPLETE_HALF_EYE &&
		   eye_condition[Pat3(game->pat, neighbor2_lib)] == E_COMPLETE_HALF_EYE) {
	  int tmp_id1 = 0, tmp_id2 = 0;
	  GetNeighbor4(neighbor4, neighbor1_lib);
	  for (j = 0; j < 4; j++) {
	    if (board[neighbor4[j]] == S_BLACK ||
		board[neighbor4[j]] == S_WHITE) {
	      id = string_id[neighbor4[j]];
	      if (id != lib1_id[0] &&
		  id != lib2_id[0] &&
		  id != tmp_id1) {
		tmp_id1 = id;
	      }
	    }
	  }
	  GetNeighbor4(neighbor4, neighbor2_lib);
	  for (j = 0; j < 4; j++) {
	    if (board[neighbor4[j]] == S_BLACK ||
		board[neighbor4[j]] == S_WHITE) {
	      id = string_id[neighbor4[j]];	      
	      if (id != lib1_id[0] &&
		  id != lib2_id[0] &&
		  id != tmp_id2) {
		tmp_id2 = id;
	      }
	    }
	  }
	  if (tmp_id1 == tmp_id2) {
	    seki[lib1] = seki[lib2] = true;
	    seki[neighbor1_lib] = seki[neighbor2_lib] = true;	    
	  }
	}
      }
    }
  }
}
