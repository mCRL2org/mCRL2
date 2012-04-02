// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef MCRL2_LTS_DETAIL_EXPLORATION_STRATEGY_H
#define MCRL2_LTS_DETAIL_EXPLORATION_STRATEGY_H


namespace mcrl2
{
namespace lts
{

enum exploration_strategy { es_none,
                            es_breadth,
                            es_depth,
                            es_random,
                            es_value_prioritized,
                            es_value_random_prioritized
                          };

inline
exploration_strategy parse_exploration_strategy(const std::string& s)
{
  if (s=="b" || s=="breadth")
  {
    return es_breadth;
  }
  if (s=="d" || s=="depth")
  {
    return es_depth;
  }
  if (s=="r" || s=="random")
  {
    return es_random;
  }
  if (s=="p" || s=="prioritized")
  {
    return es_value_prioritized;
  }
  if (s=="q" || s=="rprioritized")
  {
    return es_value_random_prioritized;
  }
  return es_none;
}

inline
std::string print_exploration_strategy(const exploration_strategy es)
{
  switch (es)
  {
    case es_breadth:
      return "breadth";
    case es_depth:
      return "depth";
    case es_random:
      return "random";
    case es_value_prioritized:
      return "prioritized";
    case es_value_random_prioritized:
      return "rprioritized";
    default:
      throw mcrl2::runtime_error("unknown exploration strategy");
  }
}

inline
std::istream& operator>>(std::istream& is, exploration_strategy& strat)
{
  try
  {
    std::string s;
    is >> s;
    strat = parse_exploration_strategy(s);
  }
  catch (mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const exploration_strategy strat)
{
  os << print_exploration_strategy(strat);
  return os;
}

inline std::string description(const exploration_strategy strat)
{
  switch (strat)
  {
    case es_breadth:
      return "breadth-first search";
    case es_depth:
      return "depth-first search";
    case es_random:
      return "random simulation. Out of all next states one is chosen at random independently of whether this state has already been observed. Consequently, random simultation only terminates when a deadlocked state is encountered.";
    case es_value_prioritized:
      return "prioritize single actions on its first argument being of sort Nat where only those actions with the lowest value for this parameter are selected. E.g. if there are actions a(3) and b(4), a(3) remains and b(4) is skipped. Actions without a first parameter of sort Nat and multactions with more than one action are always chosen (option is experimental)";
    case es_value_random_prioritized:
      return "prioritize actions on its first argument being of sort Nat (see option --prioritized), and randomly select one of these to obtain a prioritized random simulation (option is experimental)";
    default:
      throw mcrl2::runtime_error("unknown exploration_strategy");
  }
}

} // namespace lts
} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_EXPLORATION_STRATEGY_H
