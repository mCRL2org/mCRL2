// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file markmanager.h
/// \brief Mark manager header file

#ifndef MARKMANAGER_H
#define MARKMANAGER_H
#include <vector>
#include "utils.h"

struct MarkRule;
class LTS;
class State;
class Cluster;
class Transition;

class MarkManager {
  public:
    MarkManager();
    ~MarkManager();

    /* Mark rules */
    int createMarkRule(int param,bool neg,Utils::RGB_Color col,
        std::vector<bool> &vals);
    void removeMarkRule(int mr);
    int getMarkRuleParam(int mr);
    bool getMarkRuleActivated(int mr);
    bool getMarkRuleNegated(int mr);
    Utils::RGB_Color getMarkRuleColor(int mr);
    void getMarkRuleValues(int mr,std::vector<bool> &vals);
    void setMarkRuleData(int mr,int param,bool neg,Utils::RGB_Color col,
        std::vector<bool> &vals);
    void setMarkRuleActivated(int mr,bool act);

    void setMatchStyle(Utils::MatchStyle ms);
    Utils::MatchStyle getMatchStyle();
    void setMatchStyleClusters(Utils::MatchStyle ms);
    Utils::MatchStyle getMatchStyleClusters();

    int getNumMarkedStates();
    int getNumMarkedTransitions();
    void setLTS(LTS *l,bool need_reset);
    void setMarkStyle(Utils::MarkStyle ms);
    Utils::MarkStyle getMarkStyle();
    void setActionMark(std::string label,bool b);
    void markClusters();

    bool isMarked(State *s);
    bool isMarked(Cluster *s);
    bool isMarked(Transition *t);

  private:
    std::vector< MarkRule* > mark_rules;
    std::vector< bool* > label_marks;
    std::vector< MarkRule* >::iterator first_free_mark_rule;
    Utils::MatchStyle match_style;
    Utils::MatchStyle match_style_clusters;
    Utils::MarkStyle mark_style;
    int num_marked_states_any;
    int num_marked_states_all;
    int num_marked_transitions;
    int num_active_mark_rules;
    LTS *lts;

    void activateMarkRule(int mr);
    void deactivateMarkRule(int mr);
    bool matchesRule(State *s,int mr);
    void recomputeMarkedStateNumbers();
    void reset();
};

#endif
