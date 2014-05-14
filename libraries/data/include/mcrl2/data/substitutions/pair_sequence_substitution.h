// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/pair_sequence_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_PAIR_SEQUENCE_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_PAIR_SEQUENCE_SUBSTITUTION_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

/// \brief Generic substitution function. The substitution is stored as a sequence
/// of pairs of variables and expressions.
template <typename Container>
struct pair_sequence_substitution: public std::unary_function<typename Container::value_type::first_type, typename Container::value_type::second_type>
{
  /// \brief type used to represent variables
  typedef typename Container::value_type::first_type variable_type;

  /// \brief type used to represent expressions
  typedef typename Container::value_type::second_type expression_type;

  const Container& container;

  pair_sequence_substitution(const Container& container_)
    : container(container_)
  {}

  const expression_type& operator()(const variable_type& v) const
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      if (i->first == v)
      {
        return i->second;
      }
    }
    return v;
  }

  template <typename Expression>
  expression_type operator()(const Expression&) const
  {
    throw mcrl2::runtime_error("data::pair_sequence_substitution::operator(const Expression&) is a deprecated interface!");
    return data::undefined_data_expression();
  }
};

/// \brief Utility function for creating a pair_sequence_substitution.
template <typename Container>
pair_sequence_substitution<Container>
make_pair_sequence_substitution(const Container& c)
{
  return pair_sequence_substitution<Container>(c);
}

template <typename Container>
bool is_simple_substitution(const pair_sequence_substitution<Container>& sigma)
{
  for (auto i = sigma.container.begin(); i != sigma.container.end(); ++i)
  {
    if (!is_simple_substitution(i->first, i->second))
    {
      return false;
    }
  }
  return true;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_PAIR_SEQUENCE_SUBSTITUTION_H
