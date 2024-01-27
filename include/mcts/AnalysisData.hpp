#ifndef _ANALYSIS_DATA_HPP_
#define _ANALYSIS_DATA_HPP_

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "mcts/MCTSNode.hpp"
#include "mcts/UctSearch.hpp"
#include "util/Utility.hpp"


class PrincipalVariationData {
private:
  int move;
  double playout_win_rate;
  double prior;
  int visits;
  int pv_depth_limit;
  std::vector<std::string> pv;

public:
  PrincipalVariationData( const uct_node_t &root, const int child_index, const int depth_limit = 100 )
    : move(root.child[child_index].pos), playout_win_rate(CalculateWinningRate(root.child[child_index])),
      prior(root.child[child_index].rate), visits(root.child[child_index].move_count.load()), pv_depth_limit(depth_limit)
  {
    const child_node_t &child = root.child[child_index];

    pv.push_back(ParsePoint(child.pos));

    int index = child.index;

    while (pv.size() < pv_depth_limit && index != NOT_EXPANDED) {
      const uct_node_t &node = GetNode(index);
      const child_node_t *children = node.child;
      const int num_children = node.child_num;

      if (node.move_count == 0) {
        break;
      }

      int max_count = children[PASS_INDEX].move_count;
      int max_index = PASS_INDEX;

      for (int i = 1; i < num_children; i++) {
        if (children[i].move_count > max_count) {
          max_count = children[i].move_count;
          max_index = i;
        }
      }

      pv.push_back(ParsePoint(children[max_index].pos));
      index= children[max_index].index;
    }
  }

  bool
  operator>( const PrincipalVariationData &data ) const
  {
    return visits > data.visits;
  }

  std::string
  GetJsonData( void ) const
  {
    std::ostringstream oss;

    oss << "{ ";
    oss << "\"move\": \"" << ParsePoint(move) << "\",";
    oss << "\"winrate\": " << std::fixed << std::setprecision(4) << playout_win_rate << ",";
    oss << "\"prior\": " << std::fixed << std::setprecision(6) << prior << ",";
    oss << "\"pv\" : \"" << JoinStrings(pv, " ") << "\",";
    oss << "\"visits\" : " << visits;
    oss << "}";

    return oss.str();
  }
};


class CgosAnalyzeData {
private:
  const int visits;
  double win_rate;
  std::vector<PrincipalVariationData> pv_data;
  const char owner_char[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+";
  std::string ownership;
  std::string comment;

public:
  CgosAnalyzeData( const uct_node_t &root, const int color )
    : visits(root.move_count)
  {
    if (root.move_count == 0) {
      win_rate = 0.5;
      ownership = "";
      for (int i = 0; i < pure_board_max; i++) {
        ownership += owner_char[31];
      }
      comment = u8"Ray selected pass immediately.";
    } else {
      win_rate = root.win / root.move_count;

      const child_node_t *children = root.child;

      for (int i = 0; i < root.child_num; i++) {
        if (children[i].move_count > 0) {
          pv_data.push_back(PrincipalVariationData(root, i));
        }
      }
      std::sort(pv_data.begin(), pv_data.end(), std::greater<PrincipalVariationData>());
      ownership = "";
      for (int i = 0; i < pure_board_max; i++) {
        const double owner = root.ownership[onboard_pos[i]] / root.move_count;
        const int owner_index = static_cast<int>(owner * 62);
        ownership += owner_char[owner_index];
      }
      comment = u8"This is the result of Monte-Carlo tree search.";
    }

  }


  std::string
  GetJsonData( void ) const
  {
    std::ostringstream oss;
    std::vector<std::string> pv_str;

    for (const PrincipalVariationData &datum : pv_data) {
      pv_str.push_back(datum.GetJsonData());
    }
    oss << "{ ";
    oss << "\"winrate\": " << std::fixed << std::setprecision(4) << win_rate << ",";
    oss << "\"visits\": " << visits << ", ";
    if (!pv_str.empty()) {
      oss << "\"moves\": [" << JoinStrings(pv_str, ",") << "],";
    }
    oss << "\"ownership\": \"" << ownership << "\",";
    oss << "\"comment\" : \"" << comment << "\"";
    oss << "}";

    return oss.str();
  }

};


#endif
