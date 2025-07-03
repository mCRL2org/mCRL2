// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/default_expression_generator.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DEFAULT_EXPRESSION_GENERATOR_H
#define MCRL2_DATA_DEFAULT_EXPRESSION_GENERATOR_H

#include "mcrl2/data/representative_generator.h"

namespace mcrl2::data {

/// \brief Expression generator that caches values.
class default_expression_generator
{
  protected:
    data::representative_generator generator;
    std::map<sort_expression, data_expression> default_values;

  public:
    explicit default_expression_generator(const data_specification& dataspec)
      : generator(dataspec)
    {}

    const data_expression& operator()(const sort_expression& x)
    {
      auto i = default_values.find(x);
      if (i == default_values.end())
      {
        data_expression d = generator(x);
        if (!d.defined())
        {
          throw mcrl2::runtime_error("could not find a default value for sort " + data::pp(x));
        }
        i = default_values.insert(std::make_pair(x, d)).first;
      }
      return i->second;
    }
};

} // namespace mcrl2::data



#endif // MCRL2_DATA_DEFAULT_EXPRESSION_GENERATOR_H
