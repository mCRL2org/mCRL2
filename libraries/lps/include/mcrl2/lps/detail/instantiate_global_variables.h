// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/instantiate_global_variables.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
#define MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H

#include "mcrl2/data/representative_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/substitute.h"
#include "mcrl2/lps/remove.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  inline
  void instantiate_global_variables(specification& spec)
  {
      data::mutable_associative_container_substitution<> sigma;
      data::representative_generator default_expression_generator(spec.data());
      std::set<data::variable> to_be_removed;
      const atermpp::set<data::variable>& v = spec.global_variables();
      for (atermpp::set<data::variable>::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        data::data_expression d = default_expression_generator(i->sort());
        if (d == data::data_expression())
        {
          throw mcrl2::runtime_error("Error in lps::instantiate_global_variables: could not instantiate " + pp(*i));
        }
        sigma[*i] = d;
        to_be_removed.insert(*i);
      }
      lps::substitute_free_variables(spec.process(), sigma);
      spec.initial_process() = lps::substitute_free_variables(spec.initial_process(), sigma);
      spec.action_labels() = lps::substitute_free_variables(spec.action_labels(), sigma);
      spec.global_variables().clear();
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_INSTANTIATE_GLOBAL_VARIABLES_H
