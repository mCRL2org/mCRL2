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
#include <set>
#include <string>

// #include "mcrl2/atermpp/set.h"

#include "enums.h"

class Cluster;
class LTS;
struct MarkRule;
class RGB_Color;
class State;
class Transition;

class MarkManager
{
  public:
    MarkManager();
    ~MarkManager();

    /* Mark rules */
    int createMarkRule(int param,bool neg,RGB_Color col, const std::set<std::string> &vals);
    void removeMarkRule(int mr);
    int getMarkRuleParam(int mr);
    bool getMarkRuleActivated(int mr);
    bool getMarkRuleNegated(int mr);
    RGB_Color getMarkRuleColor(int mr);
    std::set<std::string> getMarkRuleValues(int mr);
    void setMarkRuleData(int mr,int param,bool neg,RGB_Color col, const std::set<std::string> &vals);
    void setMarkRuleActivated(int mr,bool act);

    void setMatchStyle(MatchStyle ms);
    MatchStyle getMatchStyle();
    void setMatchStyleClusters(MatchStyle ms);
    MatchStyle getMatchStyleClusters();

    int getNumMarkedStates();
    int getNumMarkedTransitions();
    void setLTS(LTS* l,bool need_reset);
    void setMarkStyle(MarkStyle ms);
    MarkStyle getMarkStyle();
    void setActionMark(int l,bool b);
    void markClusters();

    bool isMarked(State* s);
    bool isMarked(Cluster* s);
    bool isMarked(Transition* t);

  private:
    std::vector< MarkRule* > mark_rules;
    std::vector< bool* > label_marks;
    std::vector< MarkRule* >::iterator first_free_mark_rule;
    MatchStyle match_style;
    MatchStyle match_style_clusters;
    MarkStyle mark_style;
    int num_marked_states_any;
    int num_marked_states_all;
    int num_marked_transitions;
    int num_active_mark_rules;
    LTS* lts;

    void activateMarkRule(int mr);
    void deactivateMarkRule(int mr);
    bool matchesRule(State* s,int mr);
    void recomputeMarkedStateNumbers();
    void reset();
};

#endif
