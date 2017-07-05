// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_pcrl.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_PCRL_H
#define MCRL2_PROCESS_ALPHABET_PCRL_H

#include "mcrl2/process/alphabet_operations.h"
#include "mcrl2/process/detail/pcrl_equation_cache.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

/// \brief Traverser that computes the alphabet of pCRL process expressions
struct alphabet_pcrl_traverser: public process_expression_traverser<alphabet_pcrl_traverser>
{
  typedef process_expression_traverser<alphabet_pcrl_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  const std::map<process_identifier, multi_action_name_set>& pcrl_equation_cache;
  multi_action_name_set result;

  alphabet_pcrl_traverser(const std::map<process_identifier, multi_action_name_set>& pcrl_equation_cache_)
    : pcrl_equation_cache(pcrl_equation_cache_)
  {}

  void error(const process::process_expression& x) const
  {
    throw mcrl2::runtime_error("encountered a non-pCRL expression " + process::pp(x) + " in the context of a pCRL expression");
  }

  void leave(const process::action& x)
  {
    multi_action_name alpha;
    alpha.insert(x.label().name());
    result.insert(alpha);
  }

  void leave(const process::tau& /* x */)
  {
    result.insert(multi_action_name());
  }

  void leave(const process::process_instance& x)
  {
    auto i = pcrl_equation_cache.find(x.identifier());
    if (i == pcrl_equation_cache.end())
    {
      error(x);
    }
    else
    {
      const multi_action_name_set& A = i->second;
      result.insert(A.begin(), A.end());
    }
  }

  void leave(const process::process_instance_assignment& x)
  {
    auto i = pcrl_equation_cache.find(x.identifier());
    if (i == pcrl_equation_cache.end())
    {
      error(x);
    }
    else
    {
      const multi_action_name_set& A = i->second;
      result.insert(A.begin(), A.end());
    }
  }

  void leave(const process::block& x)
  {
    error(x);
  }

  void leave(const process::hide& x)
  {
    error(x);
  }

  void leave(const process::rename& x)
  {
    error(x);
  }

  void leave(const process::comm& x)
  {
    error(x);
  }

  void leave(const process::allow& x)
  {
    error(x);
  }

  void leave(const process::sync& x)
  {
    error(x);
  }

  void leave(const process::merge& x)
  {
    error(x);
  }

  void leave(const process::left_merge& x)
  {
    error(x);
  }
};

} // namespace detail

/// \brief Computes the alphabet of a pCRL expression x, using a pCRL equation cache
inline
multi_action_name_set alphabet_pcrl(const process_expression& x, const std::map<process_identifier, multi_action_name_set>& pcrl_equation_cache)
{
  detail::alphabet_pcrl_traverser f(pcrl_equation_cache);
  f.apply(x);
  return f.result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_PCRL_H
