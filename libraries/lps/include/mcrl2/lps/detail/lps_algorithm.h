// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
#define MCRL2_LPS_DETAIL_LPS_ALGORITHM_H

#include <algorithm>
#include <set>
#include <vector>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/specification.h"  
#include "mcrl2/lps/detail/lps_rewriter.h"  
#include "mcrl2/lps/detail/remove_parameters.h"  

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Algorithm class for algorithms on linear process specifications.
  class lps_algorithm
  {
    protected:
      specification& spec;

    public:
      lps_algorithm(specification& s)
        : spec(s)
      {}

      /// \brief Removes formal parameters from the specification spec
      void remove_formal_parameters(const std::set<data::variable>& to_be_removed)
      {
        // TODO: make the algorithm in place, to avoid copying the specification
        spec = lps::detail::remove_parameters(spec, to_be_removed);
      }
      
      /// \brief Removes free variables from the specification spec
      void remove_free_variables(const std::set<data::variable>& to_be_removed)
      {
        // spec.free_variables().erase(to_be_removed.begin(), to_be_removed.end());       
      }
  };

  /// \brief Algorithm class for algorithms on linear process specifications
  /// that use a rewriter.
  class lps_rewriter_algorithm: public lps_algorithm
  {
    protected:
      data::rewriter R;

    public:
      lps_rewriter_algorithm(specification& spec, data::rewriter::strategy s = data::rewriter::jitty)
        : lps_algorithm(spec),
          R(spec.data(), s)
      {}

      /// \brief Rewrites the specification
      /// \brief sigma A substitution
      void rewrite()
      {
        lps::detail::make_lps_rewriter(R).rewrite(spec);
      }

      /// \brief Rewrites the specification with a substitution.
      /// \brief sigma A substitution
      template <typename Substitution>
      void rewrite(Substitution& sigma)
      {
        lps::detail::rewriter_adapter<data::rewriter, Substitution> Rsigma(R, sigma);
        lps::detail::make_lps_rewriter(Rsigma).rewrite(spec);
      }
  };
  
} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
