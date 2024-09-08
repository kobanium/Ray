/**
 * @file include/mcts/AnalysisData.hpp
 * @brief \~english Data manager for cgos_genmove-analyze.
 *        \~japanese CGOS読み筋表示用のデータ管理
 */
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


/**
 * @class PrincipalVariationData
 * @~english
 * @brief Data management class for principal variation.
 * @~japanese
 * @brief 読み筋管理クラス
 */
class PrincipalVariationData {
private:
  /**
   * @~english
   * @brief Coordinate of a move.
   * @~japanese
   * @brief 着手の座標
   */
  int move;

  /**
   * @~english
   * @brief Winning ratio of Monte-Carlo simulations.
   * @~japanese
   * @brief モンテカルロ・シミュレーションの勝率
   */
  double playout_win_rate;

  /**
   * @~english
   * @brief Policy (Prior value).
   * @~japanese
   * @brief 着手評価値
   */
  double prior;

  /**
   * @~english
   * @brief The number of visits.
   * @~japanese
   * @brief 探索回数
   */
  int visits;

  /**
   * @~english
   * @brief Upper length limit of PV list.
   * @~japanese
   * @brief PVリストの長さ上限
   */
  int pv_depth_limit;

  /**
   * @~english
   * @brief List of principal variation.
   * @~japanese
   * @brief PVリスト
   */
  std::vector<std::string> pv;

public:
  /**
   * @~english
   * @brief Constructor of PrincipalVariationData class.
   * @param[in] root Root node of a current position.
   * @param[in] child_index Index of a selected child node.
   * @param[in] depth_limit Depth limiation for principal variation.
   * @~japanese
   * @brief PrincipalVariationDataクラスのコンストラクタ
   * @param[in] root 現局面のルートノード
   * @param[in] child_index 選択した手のインデックス
   * @param[in] depth_limit PVの深さ制限
   */
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

  /**
   * @~english
   * @brief Comparison operator.
   * @param[in] data Right hand value.
   * @return Comparison result
   * @~japanese
   * @brief 比較演算子
   * @param[in] data 右辺値
   * @return 比較結果
   */
  bool
  operator>( const PrincipalVariationData &data ) const
  {
    return visits > data.visits;
  }

  /**
   * @~english
   * @brief Get JSON-formatted string.
   * @return JSON-formated string.
   * @~japanese
   * @brief JSON形式の文字列を取得する
   * @return JSON形式の文字列
   */
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


/**
 * @class CgosAnalyzeData
 * @~english
 * @brief Data management class for CGOS.
 * @~japanese
 * @brief CGOS用の読み筋管理クラス
 */
class CgosAnalyzeData {
private:
  /**
   * @~english
   * @brief The number of a current position's visits.
   * @~japanese
   * @brief 現局面の探索回数
   */
  const int visits;

  /**
   * @~english
   * @brief Winning ration of a current position.
   * @~japanese
   * @brief 現局面の勝率
   */
  double win_rate;

  /**
   * @~english
   * @brief Data of principal variations.
   * @~japanese
   * @brief 読み筋の情報
   */
  std::vector<PrincipalVariationData> pv_data;

  /**
   * @~english
   * @brief Charactors for ownership.
   * @~japanese
   * @brief Ownershipを表現するための文字列
   */
  const char owner_char[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+";

  /**
   * @~english
   * @brief Decoded ownership data.
   * @~japanese
   * @brief デコードしたOwnershipのデータ
   */
  std::string ownership;

  /**
   * @~english
   * @brief Comment.
   * @~japanese
   * @brief コメント
   */
  std::string comment;

public:
  /**
   * @~english
   * @brief Constructor of CgosAnalyzeData class.
   * @param[in] root Root node.
   * @param[in] color Player's color.
   * @~japanese
   * @brief CgosAnalyzeDataクラスのコンストラクタ
   * @param[in] root ルート
   * @param[in] color 手番の色
   */
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
      win_rate = static_cast<double>(root.win) / root.move_count;

      const child_node_t *children = root.child;

      for (int i = 0; i < root.child_num; i++) {
        if (children[i].move_count > 0) {
          pv_data.push_back(PrincipalVariationData(root, i));
        }
      }
      std::sort(pv_data.begin(), pv_data.end(), std::greater<PrincipalVariationData>());
      ownership = "";
      for (int i = 0; i < pure_board_max; i++) {
        const double owner = static_cast<double>(root.ownership[onboard_pos[i]]) / root.move_count;
        const int owner_index = static_cast<int>(owner * 62);
        ownership += owner_char[owner_index];
      }
      comment = u8"Ray selected the next move based on Monte-Carlo tree search.";
    }

  }

  /**
   * @~english
   * @brief Get JSON-formatted string.
   * @return JSON-formated string.
   * @~japanese
   * @brief JSON形式の文字列を取得する
   * @return JSON形式の文字列
   */
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
