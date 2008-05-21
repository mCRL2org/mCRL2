#include "markmanager.h"
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
using namespace std;
using namespace Utils;

struct MarkRule {
  int param_index;
  bool is_activated;
  bool is_negated;
  RGB_Color color; // Colour asociated with this mark rule.
  vector< bool > value_set;
};

MarkManager::MarkManager() {
  lts = NULL;
  first_free_mark_rule = mark_rules.end();
  match_style = MATCH_ANY;
  mark_style = NO_MARKS;
  num_active_mark_rules = 0;
  num_marked_states_all = 0;
  num_marked_states_any = 0;
  num_marked_transitions = 0;
}

MarkManager::~MarkManager() {
  reset();
}

void MarkManager::reset() {
  //marked_states.clear();
  //unmarked_states.clear();
  //active_mark_rules.clear();
  for (unsigned int i = 0; i < mark_rules.size(); ++i) {
    delete mark_rules[i];
  }
  mark_rules.clear();
  for (unsigned int i = 0; i < label_marks.size(); ++i) {
    delete label_marks[i];
  }
  label_marks.clear();
  first_free_mark_rule = mark_rules.end();
  num_active_mark_rules = 0;
  num_marked_states_all = 0;
  num_marked_states_any = 0;
  num_marked_transitions = 0;
}

void MarkManager::setLTS(LTS *l,bool need_reset) {
  lts = l;
  if (need_reset) {
    reset();
    // set the label marks
    label_marks.assign(lts->getNumLabels(),static_cast<bool*>(NULL));
    State *s;
    Transition *t;
    int i;
    for (State_iterator si = lts->getStateIterator(); !si.is_end(); ++si) {
      s = *si;
      for (i = 0; i < s->getNumOutTransitions(); ++i) {
        t = s->getOutTransition(i);
        if (label_marks[t->getLabel()] == NULL) {
          label_marks[t->getLabel()] = new bool(false);
        }
        t->setMarkedPointer(label_marks[t->getLabel()]);
      }
      for (i = 0; i < s->getNumLoops(); ++i) {
        t = s->getLoop(i);
        if (label_marks[t->getLabel()] == NULL) {
          label_marks[t->getLabel()] = new bool(false);
        }
        t->setMarkedPointer(label_marks[t->getLabel()]);
      }
    }
    markClusters();
  } else {
    recomputeMarkedStateNumbers();
  }
}

int MarkManager::createMarkRule(int param,bool neg,Utils::RGB_Color col,
        std::vector<bool> &vals) {
  MarkRule *m = new MarkRule;
  m->param_index = param;
  m->is_activated = true;
  m->is_negated = neg;
  m->color = col;
  m->value_set = vals;
  int retval;
  if (first_free_mark_rule == mark_rules.end()) {
    retval = mark_rules.size();
    mark_rules.push_back(m);
    first_free_mark_rule = mark_rules.end();
  } else {
    *first_free_mark_rule = m;
    retval = first_free_mark_rule - mark_rules.begin();
    first_free_mark_rule = find(first_free_mark_rule,mark_rules.end(),
        static_cast<MarkRule*>(NULL));
  }
  activateMarkRule(retval);
  return retval;
}

void MarkManager::removeMarkRule(int mr) {
  if (mark_rules[mr]->is_activated) {
    deactivateMarkRule(mr);
  }
  if (mr < first_free_mark_rule - mark_rules.begin()) {
    first_free_mark_rule = mark_rules.begin() + mr;
  }
  delete mark_rules[mr];
  mark_rules[mr] = NULL;
}

int MarkManager::getMarkRuleParam(int mr) {
  return mark_rules[mr]->param_index;
}

bool MarkManager::getMarkRuleActivated(int mr) {
  return mark_rules[mr]->is_activated;
}

bool MarkManager::getMarkRuleNegated(int mr) {
  return mark_rules[mr]->is_negated;
}

RGB_Color MarkManager::getMarkRuleColor(int mr) {
  return mark_rules[mr]->color;
}

void MarkManager::getMarkRuleValues(int mr,vector<bool> &vals) {
  vals = mark_rules[mr]->value_set;
}

void MarkManager::setMarkRuleData(int mr,int param,bool neg,
    Utils::RGB_Color col,std::vector<bool> &vals) {
  bool changed = (param != mark_rules[mr]->param_index)
              || (neg != mark_rules[mr]->is_negated);
  unsigned int i = 0;
  while (i < vals.size() && !changed) {
    changed = (vals[i] != mark_rules[mr]->value_set[i]);
    ++i;
  }

  if (changed && mark_rules[mr]->is_activated) {
    deactivateMarkRule(mr);
  }
  mark_rules[mr]->param_index = param;
  mark_rules[mr]->is_negated = neg;
  mark_rules[mr]->color = col;
  mark_rules[mr]->value_set = vals;
  if (changed && mark_rules[mr]->is_activated) {
    activateMarkRule(mr);
  }
}

void MarkManager::setMarkRuleActivated(int mr,bool act) {
  if (act != mark_rules[mr]->is_activated) {
    if (act) {
      activateMarkRule(mr);
    } else {
      deactivateMarkRule(mr);
    }
    mark_rules[mr]->is_activated = act;
  }
}

int MarkManager::getNumMarkedStates() {
  if (mark_style == MARK_STATES) {
    if (match_style == MATCH_ALL) {
      return num_marked_states_all;
    } else {
      return num_marked_states_any;
    }
  }
  if (mark_style == MARK_DEADLOCKS) {
    return lts->getNumDeadlocks();
  }
  return 0;
}

int MarkManager::getNumMarkedTransitions() {
  if (mark_style == MARK_TRANSITIONS) {
    return num_marked_transitions;
  }
  return 0;
}

void MarkManager::setActionMark(std::string label,bool b) {
  int l = lts->getLabelIndex(label);
  *(label_marks[l]) = b;
  num_marked_transitions = 0;
  for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end(); ++ci) {
    num_marked_transitions += (**ci).setActionMark(l,b);
  }
}

void MarkManager::setMatchStyle(MatchStyle ms) {
  if (match_style != ms) {
    changeMatchStyle(ms);
    match_style = ms;
  }
}

MatchStyle MarkManager::getMatchStyle() {
  return match_style;
}

void MarkManager::setMarkStyle(MarkStyle ms) {
  mark_style = ms;
}

MarkStyle MarkManager::getMarkStyle() {
  return mark_style;
}

void MarkManager::markClusters() {
  vector<bool> rules_matched;
  vector<int> state_rules;
  int num_all,num_any;
  num_marked_states_all = 0;
  num_marked_states_any = 0;
  Cluster *c;
  State *s;
  for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end(); ++ci) {
    c = *ci;
    num_all = 0;
    num_any = 0;
    rules_matched.assign(mark_rules.size(),false);
    for (int i = 0; i < c->getNumStates(); ++i) {
      s = c->getState(i);
      s->getMatchedRules(state_rules);
      if (state_rules.size() > 0) {
        ++num_any;
      }
      if (state_rules.size() == (unsigned int)(num_active_mark_rules)) {
        ++num_all;
      }
      for (unsigned int j = 0; j < state_rules.size(); ++j) {
        rules_matched[state_rules[j]] = true;
      }
    }
    for (unsigned int j = 0; j < rules_matched.size(); ++j) {
      if (rules_matched[j]) {
        c->addMatchedRule(j);
      }
    }
    c->setNumMarkedStatesAll(num_all);
    c->setNumMarkedStatesAny(num_any);
    num_marked_states_all += num_all;
    num_marked_states_any += num_any;
  }
}

void MarkManager::activateMarkRule(int mr) {
  Cluster *c;
  State *s;
  bool add_to_c;
  int num_all,num_any;
  for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end(); ++ci) {
    c = *ci;
    // indicates whether this rule also has to be added to c's matched rules
    add_to_c = false;
    num_all = c->getNumMarkedStatesAll();
    num_any = c->getNumMarkedStatesAny();
    for (int i = 0; i < c->getNumStates(); ++i) {
      s = c->getState(i);
      if (matchesRule(s,mr)) {
        if (s->getNumMatchedRules() == 0) {
          // state did not match any rule yet, so increase num_any counters
          ++num_any;
          ++num_marked_states_any;
        }
        // add this rule to the matched rules of s
        s->addMatchedRule(mr);
        add_to_c = true;
      } else {
        if (s->getNumMatchedRules() == num_active_mark_rules) {
          // state matched all rules up until now, so decrease num_all counters
          --num_all;
          --num_marked_states_all;
        }
      }
    }
    if (add_to_c) {
      c->addMatchedRule(mr);
    }
    c->setNumMarkedStatesAll(num_all);
    c->setNumMarkedStatesAny(num_any);
  }
  ++num_active_mark_rules;
}

void MarkManager::deactivateMarkRule(int mr) {
  Cluster *c;
  State *s;
  int num_all,num_any;
  for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end(); ++ci) {
    c = *ci;
    num_all = c->getNumMarkedStatesAll();
    num_any = c->getNumMarkedStatesAny();
    for (int i = 0; i < c->getNumStates(); ++i) {
      s = c->getState(i);
      if (s->removeMatchedRule(mr)) {
        if (s->getNumMatchedRules() == 0) {
          --num_any;
          --num_marked_states_any;
        }
      } else {
        if (s->getNumMatchedRules() == num_active_mark_rules-1) {
          ++num_all;
          ++num_marked_states_all;
        }
      }
    }
    c->removeMatchedRule(mr);
    c->setNumMarkedStatesAll(num_all);
    c->setNumMarkedStatesAny(num_any);
  }
  --num_active_mark_rules;
}

void MarkManager::changeMatchStyle(MatchStyle ms) {
}

bool MarkManager::matchesRule(State *s,int mr) {
  MarkRule *rule = mark_rules[mr];
  bool retval = rule->value_set[s->getParameterValue(rule->param_index)];
  if (!rule->is_negated) {
    return retval;
  } else {
    return !retval;
  }
}

bool MarkManager::isMarked(State* s) {
  if (mark_style == MARK_STATES) {
    if (match_style == MATCH_ALL) {
      return (s->getNumMatchedRules() == num_active_mark_rules);
    } else {
      return (s->getNumMatchedRules() > 0);
    }
  }
  if (mark_style == MARK_DEADLOCKS) {
    return s->isDeadlock();
  }
  return false;
}

bool MarkManager::isMarked(Cluster* c) {
  if (c == NULL) return false;
  switch (mark_style) {
    case MARK_STATES:
      if (match_style == MATCH_ALL) {
        return (c->getNumMarkedStatesAll() > 0);
      } else {
        return (c->getNumMarkedStatesAny() > 0);
      }
    case MARK_DEADLOCKS:
      return c->hasDeadlock();
    case MARK_TRANSITIONS:
      return c->hasMarkedTransition();
    default:
      return false;
  }
}

bool MarkManager::isMarked(Transition* t) {
  return mark_style == MARK_TRANSITIONS && t->isMarked();
}

void MarkManager::recomputeMarkedStateNumbers() {
  num_marked_states_all = 0;
  num_marked_states_any = 0;
  Cluster *c;
  for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end(); ++ci) {
    c = *ci;
    num_marked_states_all += c->getNumMarkedStatesAll();
    num_marked_states_any += c->getNumMarkedStatesAny();
  }
}
