// Author(s): Aad Mathijssen, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lps_rewrite.cpp
/// \brief Contains a rewriter for the process part of an lps
/// \details This file contains a rewriter for the process part
/// of an lps. If a standard rewriter is applied to an lps, all
/// data terms in it are rewritten, including those in the rewrite
/// rules. This is not always desirable.

#include "boost.hpp" // precompiled headers

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"

#include "mcrl2/lps/specification.h"

using namespace mcrl2::data;

namespace mcrl2 {

namespace lps {

/// \brief Rewrite the linear process and the initial state of an lps
/// \details This routine rewrites all expressions in the init section 
/// of the given linear specification, as well as all data expressions
/// occuring in all summands of the linear process equation in the lps.

template <typename Rewriter>
specification rewrite_lps(const specification &spec, const Rewriter &r)
{ 
  // Rewrite the righthandsides of the assignments
  
  data_assignment_list initial_assignments=spec.initial_process().assignments();
  data_assignment_list new_initial_assignments;
  for(data_assignment_list::iterator i=initial_assignments.begin(); 
         i!=initial_assignments.end(); ++i)
  { new_initial_assignments=
         push_front(new_initial_assignments,
                data_assignment(i->lhs(),r(i->rhs()))); 
  }
  new_initial_assignments=atermpp::reverse(new_initial_assignments);

  // Rewrite the summands in the linear process.
  linear_process lpe=spec.process();

  summand_list summands=lpe.summands();
  summand_list new_summands;
  for(summand_list::iterator i=summands.begin();
           i!=summands.end(); ++i)
  { // Rewrite the condition.
    data_expression new_condition=r(i->condition());

    // Rewrite the actions.
    action_list new_actions, actions=i->actions();
    for(action_list::iterator j=actions.begin();
           j!=actions.end(); ++j)
    { data_expression_list new_arguments, arguments=j->arguments();
      for(data_expression_list::iterator k=arguments.begin();
            k!=arguments.end(); ++k)
      { new_arguments=push_front(new_arguments,r(*k));
      }
      new_arguments=reverse(new_arguments);
      new_actions=push_front(
                    new_actions,
                    action(
                      j->label(),
                      new_arguments));
    }
    new_actions=reverse(new_actions);
    
    // Rewrite the assignments in the next state of the summand.
    data_assignment_list new_assignments, assignments=i->assignments();
    for(data_assignment_list::iterator j=assignments.begin(); 
           j!=assignments.end(); ++j)
    { new_assignments=
           push_front(new_assignments,
                  data_assignment(j->lhs(),r(j->rhs()))); 
    }
    new_assignments=reverse(new_assignments);


    // Construct a new summand, with or without time.
    summand new_summand;
    if (i->has_time())
    { new_summand=summand(i->summation_variables(),
                          new_condition,
                          i->is_delta(),
                          new_actions, 
                          r(i->time()),
                          new_assignments);
    }
    else
    { new_summand=summand(i->summation_variables(),
                          new_condition,
                          i->is_delta(),
                          new_actions, 
                          new_assignments);
    }
    new_summands=push_front(new_summands,new_summand);
  }
  new_summands=reverse(new_summands);
  
  return specification(
                spec.data(),
                spec.action_labels(),
                linear_process(
                    lpe.free_variables(),
                    lpe.process_parameters(),
                    new_summands),
                process_initializer(
                        spec.initial_process().free_variables(),
                        new_initial_assignments));
}

} // end namespace lps
} // end namespace mcrl2
