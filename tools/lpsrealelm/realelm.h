// Author(s): Jan Friso Groote and Jeroen Keiren
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
#include "linear_inequalities.h"


mcrl2::lps::specification realelm(mcrl2::lps::specification s, int max_iterations = 5, RewriteStrategy = GS_REWR_JITTY);

template <typename Term, typename MapContainer>
Term realelm_data_expression_map_replace(Term t, const MapContainer& replacements);

// Terms that must be protected are put in the atermpp set below.
static atermpp::set < data_expression > protective_set;
static atermpp::set < data_expression_list > protective_list_set;
void normalize_pair(data_expression &,data_expression &,const rewriter &, const bool);

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

typedef std::vector< real_representing_variable > context_type;

class summand_information
{

  private:
    // the summand is put in an atermpp vector for protection. It only contains one element.
    atermpp::vector < mcrl2::lps::summand > smd;
    atermpp::vector < mcrl2::data::data_expression > new_values_for_xi_variables;
    vector < linear_inequality > summand_real_conditions;
    atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression>  summand_real_nextstate_map;
    // Variable below contains all combinations of nextstate_context_combinations that allow a
    // feasible solution, regarding the context variables that are relevant for this summand.
    vector < vector < linear_inequality > > nextstate_context_combinations;
    atermpp::vector < data_expression_list > nextstate_value_combinations;
    // context_type local_context;


  public:
    summand_information(
             const mcrl2::lps::summand s,
             vector < linear_inequality > src,
             atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression>  srnm
             ):
      smd(1,s),
      new_values_for_xi_variables(),
      summand_real_conditions(src),
      summand_real_nextstate_map(srnm),
      nextstate_context_combinations(1,vector < linear_inequality >()),
      nextstate_value_combinations(1,data_expression_list())
    {
    }

    mcrl2::lps::summand get_summand() const
    { return smd[0];
    }

    atermpp::vector < mcrl2::data::data_expression >::const_iterator get_new_values_for_xi_variables_begin() const
    {
      return new_values_for_xi_variables.begin();
    }

    atermpp::vector < mcrl2::data::data_expression >::const_iterator get_new_values_for_xi_variables_end() const
    {
      return new_values_for_xi_variables.end();
    }

    vector < linear_inequality >::const_iterator get_summand_real_conditions_begin() const
    { return summand_real_conditions.begin();
    }

    vector < linear_inequality >::const_iterator get_summand_real_conditions_end() const
    { return summand_real_conditions.end();
    }

    atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression> get_summand_real_nextstate_map() const
    { return summand_real_nextstate_map;
    }

    vector < vector < linear_inequality > > :: const_iterator nextstate_context_combinations_begin() const
    { return nextstate_context_combinations.begin();
    }

    vector < vector < linear_inequality > > :: const_iterator nextstate_context_combinations_end() const
    { return nextstate_context_combinations.end();
    }

    atermpp::vector < data_expression_list > :: const_iterator nextstate_value_combinations_begin() const
    { return nextstate_value_combinations.begin();
    }

    atermpp::vector < data_expression_list > :: const_iterator nextstate_value_combinations_end() const
    { return nextstate_value_combinations.end();
    }



    /// \brief Update the new_values_for_xi_variables given that a new xi variable
    ///        has been added to context.
    /// \details Assume the new variable in the context is xi, with as bounds t and u.
    ///          First check whether 1) t[x:=g(x)] and u[x:=g(x)] are smaller, equal or greater and
    ///          if so, store this value as the default value for xi in this summand.
    ///          Or 2) check whether t[:=g(x)] and u[x:=g(x)] form the bounds of another
    ///          variable xi' and substitute xi' for xi.
    ///          If neither 1) or 2) applies subsitute the default data_expression() as an
    ///          indication that no fixed value for xi can be used in this summand.
    ///          Secondly, check whether the bounds for xi imply the substituted bounds for
    ///          the other variables xi' and substitute xi for xi'.
    void add_a_new_next_state_argument(const context_type &context, const rewriter &r)
    {
      assert(context.size()==new_values_for_xi_variables.size()+1);
      real_representing_variable new_xi_variable=context.back();
      data_expression xi_t=new_xi_variable.get_lowerbound();
      data_expression xi_u=new_xi_variable.get_upperbound();
      data_expression substituted_lowerbound=realelm_data_expression_map_replace(xi_t,summand_real_nextstate_map);
      data_expression substituted_upperbound=realelm_data_expression_map_replace(xi_u,summand_real_nextstate_map);
      // normalize_pair(substituted_lowerbound,substituted_upperbound,r,false);
      linear_inequality e(substituted_lowerbound,substituted_upperbound,linear_inequality::less,r);
      e.typical_pair(substituted_lowerbound,substituted_upperbound,r);


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
        { // Check whether this value can be set, immediately, or whether other expressions can be set.
          new_values_for_xi_variables.push_back(mcrl2::data::data_expression());
        }
      }

      if (new_values_for_xi_variables.back()!=data_expression())
      { // std::cerr << "New standard value for " << pp(context.back().get_variable()) << " is " << pp(new_values_for_xi_variables.back()) << ")\n";
      }
      else
      { // std::cerr << "New standard value for " << pp(context.back().get_variable()) << " is undefined.\n";
      }
      // At this point a data_expression has been pushed to the back of the new_values_for_xi_variables
      // Now it must be checked whether the newly added xi variable can lead to a standard value of
      // existing xi variables.

      context_type::const_iterator c=context.begin();
      for(atermpp::vector < mcrl2::data::data_expression >::iterator cxi=new_values_for_xi_variables.begin();
                       (cxi+1)!=new_values_for_xi_variables.end(); ++cxi, ++c)
      { if (*cxi==data_expression())
        { // Check whether lowerbound,upperbound for xi imply the substituted, normalized lowerbounds for
          // c->get_variable().
          data_expression cxi_t=c->get_lowerbound();
          data_expression cxi_u=c->get_upperbound();
          data_expression substituted_cxi_t=realelm_data_expression_map_replace(cxi_t,summand_real_nextstate_map);
          data_expression substituted_cxi_u=realelm_data_expression_map_replace(cxi_u,summand_real_nextstate_map);
          linear_inequality e(substituted_cxi_t,substituted_cxi_u,linear_inequality::less,r);
          e.typical_pair(substituted_cxi_t,substituted_cxi_u,r);
          if ((substituted_cxi_t==xi_t) && (substituted_cxi_u==xi_u))
          { *cxi=new_xi_variable.get_variable();
          }
        }
      }

#ifndef NDEBUG
      // sanity check
      for(atermpp::vector<mcrl2::data::data_expression>::const_iterator i = new_values_for_xi_variables.begin();
          i != new_values_for_xi_variables.end(); ++i)
      {
        assert(mcrl2::data::is_data_expression(*i) || *i == mcrl2::data::data_expression());
      }
      assert(context.size()==new_values_for_xi_variables.size());
#endif

      // Update the nextstate_context_combinations if new_values_for_xi_variables for this summand
      // does not get a concrete variable for this context variable.

      if (new_values_for_xi_variables.back()==data_expression())
      { data_expression t=realelm_data_expression_map_replace(
                             context.back().get_lowerbound(),
                             summand_real_nextstate_map);
        data_expression u=realelm_data_expression_map_replace(
                             context.back().get_upperbound(),
                             summand_real_nextstate_map);

        vector < vector < linear_inequality > > new_nextstate_context_combinations;
        atermpp::vector < data_expression_list > new_nextstate_value_combinations;
        atermpp::vector < data_expression_list >::const_iterator j=nextstate_value_combinations.begin();
        for(vector < vector < linear_inequality > >::iterator i=nextstate_context_combinations.begin();
                    i!=nextstate_context_combinations.end(); ++i,j++)
        { vector < linear_inequality > vec_lin_eq;
          vec_lin_eq.swap(*i);
          unsigned int old_size=vec_lin_eq.size();
          vector < linear_inequality > new_condition_list;
          vec_lin_eq.push_back(linear_inequality(t,u,linear_inequality::equal,r));
          remove_redundant_inequalities(
                    vec_lin_eq,
                    new_condition_list,
                    r);
          // std::cerr << "New conditions list 1 " << pp_vector(new_condition_list) << "\n";
          // if (!is_inconsistent(new_condition_list,r))
          if (new_condition_list.empty() || !new_condition_list.front().is_false())
          { new_nextstate_context_combinations.push_back(new_condition_list);
            new_nextstate_value_combinations.push_back(push_front(*j,data_expression(equal())));
          }

          vec_lin_eq[old_size]=linear_inequality(t,u,linear_inequality::less,r);
          new_condition_list.clear();
          remove_redundant_inequalities(
                    vec_lin_eq,
                    new_condition_list,
                    r);
          // std::cerr << "New conditions list 2 " << pp_vector(new_condition_list) << "\n";
          // if (!is_inconsistent(new_condition_list,r))
          if (new_condition_list.empty() || !new_condition_list.front().is_false())
          { new_nextstate_context_combinations.push_back(new_condition_list);
            new_nextstate_value_combinations.push_back(push_front(*j,data_expression(smaller())));
          }

          vec_lin_eq[old_size]=linear_inequality(u,t,linear_inequality::less,r);
          new_condition_list.clear();
          remove_redundant_inequalities(
                    vec_lin_eq,
                    new_condition_list,
                    r);
          // std::cerr << "New conditions list 3 " << pp_vector(new_condition_list) << "\n";
          // if (!is_inconsistent(new_condition_list,r))
          if (new_condition_list.empty() || !new_condition_list.front().is_false())
          { new_nextstate_context_combinations.push_back(new_condition_list);
            new_nextstate_value_combinations.push_back(push_front(*j,data_expression(larger())));
          }

        }
        nextstate_value_combinations.swap(new_nextstate_value_combinations);
        nextstate_context_combinations.swap(new_nextstate_context_combinations);
      }
    }
};




#endif // MCRL2_LPSREALELM_REALELM_H
