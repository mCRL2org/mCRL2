// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file realelm.h
/// \brief Implements real time abstraction

#ifndef MCRL2_LPSREALELM_REALELM_H
#define MCRL2_LPSREALELM_REALELM_H

#include "mcrl2/lps/specification.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/rewriter.h"
#include "comp.h"


mcrl2::lps::specification realelm(mcrl2::lps::specification s, int max_iterations = 5, RewriteStrategy = GS_REWR_JITTY);

template <typename Term, typename MapContainer>
Term realelm_data_expression_map_replace(Term t, const MapContainer& replacements);

// Terms that must be protected are put in the atermpp set below.
static atermpp::set < data_expression > protective_set;

class real_representing_variable
{
  private:
    mcrl2::data::data_variable variable;
    mcrl2::data::data_expression lowerbound;
    mcrl2::data::data_expression upperbound;

  public:
    real_representing_variable
          ( mcrl2::data::data_variable v,
            mcrl2::data::data_expression lb,
            mcrl2::data::data_expression ub):
            variable(v), lowerbound(lb), upperbound(ub)
    {
      protective_set.insert(v);
      protective_set.insert(lb);
      protective_set.insert(ub);
    }

    mcrl2::data::data_variable get_variable() const
    { return variable;
    }

    mcrl2::data::data_expression get_lowerbound() const
    { return lowerbound;
    }

    mcrl2::data::data_expression get_upperbound() const
    { return upperbound;
    }
};

typedef atermpp::vector< real_representing_variable > context_type;

class summand_information
{

  private:
    mcrl2::lps::summand smd;
    atermpp::vector < mcrl2::data::data_expression > new_values_for_xi_variables;
    mcrl2::data::data_expression_list summand_real_conditions;
    atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression>  summand_real_nextstate_map;

  public:
    summand_information(
             const mcrl2::lps::summand s,
             atermpp::vector < mcrl2::data::data_expression > nv,
             mcrl2::data::data_expression_list src,
             atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression>  srnm
             ):
      smd(s),
      new_values_for_xi_variables(nv),
      summand_real_conditions(src),
      summand_real_nextstate_map(srnm)
    {}

    mcrl2::lps::summand get_summand() const
    { return smd;
    }

    atermpp::vector < mcrl2::data::data_expression > get_new_values_for_xi_variables() const
    { return new_values_for_xi_variables;
    }

    mcrl2::data::data_expression_list get_summand_real_conditions() const
    { return summand_real_conditions;
    }

    atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression> get_summand_real_nextstate_map() const
    { return summand_real_nextstate_map;
    }

    void add_a_new_next_state_argument(const context_type &context, const rewriter &r)
    { // TODO: normalize the inequalities.
      real_representing_variable new_xi_variable=context.back();
      data_expression substituted_lowerbound=r(realelm_data_expression_map_replace(new_xi_variable.get_lowerbound(),summand_real_nextstate_map));
      data_expression substituted_upperbound=r(realelm_data_expression_map_replace(new_xi_variable.get_upperbound(),summand_real_nextstate_map));
      // First check whether this new next state argument follows from an existing argument
      if (r(core::detail::gsMakeDataExprLT(substituted_lowerbound,substituted_upperbound))==true_())
      { new_values_for_xi_variables.push_back(smaller());
      }
      else if (r(core::detail::gsMakeDataExprEq(substituted_lowerbound,substituted_upperbound))==true_())
      { new_values_for_xi_variables.push_back(equal());
      }
      else if (r(core::detail::gsMakeDataExprLT(substituted_upperbound,substituted_lowerbound))==true_())
      { new_values_for_xi_variables.push_back(larger());
      }
      else 
      { bool success(false);
        // Second check whether this new next state argument follows from an existing argument
        for(context_type::const_reverse_iterator c=context.rbegin();
                     ((c!=context.rend()) && !success) ; ++c)
        { if ((substituted_lowerbound==c->get_lowerbound()) &&
              (substituted_upperbound==c->get_upperbound()))
          { new_values_for_xi_variables.push_back(c->get_variable());
            success=true;
          }
        }
        if (!success)
        { // Check whether this value can be set, immediatel, or whether other expressions can be set.
          new_values_for_xi_variables.push_back(mcrl2::data::data_expression());
        }
      }

      if (new_values_for_xi_variables.back()!=data_expression())
      { std::cerr << "New standard value for " << pp(context.back().get_variable()) << " is " << pp(new_values_for_xi_variables.back()) << "\n";
      }
      else
      { std::cerr << "New standard value for " << pp(context.back().get_variable()) << " is undefined.\n";
      }
      // At this point a data_expression has been pushed to the back of the new_values_for_xi_variables
      // Now it must be checked whether the newly added xi variable can lead to a standard value of
      // existing xi variables.
      
      for(atermpp::vector < mcrl2::data::data_expression >::iterator xi=new_values_for_xi_variables.begin();
                       (xi+1)!=new_values_for_xi_variables.end(); ++xi)
      { if (*xi==data_expression())
        { // Check something.
        }
      }
      assert(context.size()==new_values_for_xi_variables.size());
    }
};




#endif // MCRL2_LPSREALELM_REALELM_H
