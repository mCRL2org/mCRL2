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
// #include "mcrl2/data/rewrite.h"
#include "mcrl2/data/rewriter.h"
#include "comp.h"
#include "linear_inequalities.h"



mcrl2::lps::specification realelm(mcrl2::lps::specification s,
                                  int max_iterations = 5,
                                  const rewriter &r = rewriter());

template <typename Term, typename MapContainer>
Term realelm_data_expression_map_replace(Term t, const MapContainer& replacements);

void normalize_pair(data_expression &,data_expression &,const rewriter &, const bool);

class real_representing_variable
{
  private:
    mcrl2::data::variable variable;
    mcrl2::data::data_expression lowerbound;
    mcrl2::data::data_expression upperbound;

  public:
    real_representing_variable
          ( mcrl2::data::variable v,
            mcrl2::data::data_expression lb,
            mcrl2::data::data_expression ub)
    {
      variable.protect();
      lowerbound.protect();
      upperbound.protect();
      variable=v;
      lowerbound=lb;
      upperbound=ub;
    }


/*  The code below gives rise to garbage collection problems. */
    ~real_representing_variable()
    {
      variable.unprotect();
      lowerbound.unprotect();
      upperbound.unprotect();
    }

    real_representing_variable & operator = (const real_representing_variable & other)
    {
      variable=other.variable;
      lowerbound=other.lowerbound;
      upperbound=other.upperbound;
      return *this;
    }

    real_representing_variable(const real_representing_variable &other)
    {
      variable.protect();
      lowerbound.protect();
      upperbound.protect();
      variable=other.variable;
      lowerbound=other.lowerbound;
      upperbound=other.upperbound;

    }

    mcrl2::data::variable get_variable() const
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
    mcrl2::lps::summand smd;
    variable_list real_summation_variables;
    variable_list non_real_summation_variables;
    atermpp::vector < mcrl2::data::data_expression > new_values_for_xi_variables;
    std::vector < linear_inequality > summand_real_conditions;
    atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression>  summand_real_nextstate_map;
    // Variable below contains all combinations of nextstate_context_combinations that allow a
    // feasible solution, regarding the context variables that are relevant for this summand.
    std::vector < std::vector < linear_inequality > > nextstate_context_combinations;
    // vector < vector < linear_inequality > > residual_inequalities; // These are inequalities equal to
                                                                   // xi conditions which are used in the current
                                                                   // summand to be generated, but not added
                                                                   // to the context combinations. Adding them
                                                                   // is only necessary, if variables overlap.
    atermpp::vector < data_expression_list > nextstate_value_combinations;
    // context_type local_context;


  public:
    summand_information(
             const mcrl2::lps::summand s,
             variable_list rsv,
             variable_list nrsv,
             std::vector < linear_inequality > src,
             atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression>  srnm
             ):
      smd(s),
      real_summation_variables(rsv),
      non_real_summation_variables(nrsv),
      new_values_for_xi_variables(),
      summand_real_conditions(src),
      summand_real_nextstate_map(srnm),
      // Remove the variables in src that do not occur in nextstate via
      // fourier motzkin, because they are not needed for further calculations.
      // vector < linear_inequality >() reduced_src;
      // fourier_motzkin(src,vars.begin(),vars().end(),reduced_src);
      // nextstate_context_combinations(1,reduced_src),
      nextstate_context_combinations(1,src),
      // residual_inequalities(1,vector < linear_inequality >()),
      nextstate_value_combinations(1,data_expression_list())
    { smd.protect();
      real_summation_variables.protect();
      non_real_summation_variables.protect();
      // std::cerr << "NEW REAL SUMMATION VARIABLES " << pp(rsv) << "\n";
    }

    summand_information(const summand_information &s)
    { smd.protect();
      real_summation_variables.protect();
      non_real_summation_variables.protect();
      smd=s.smd;
      real_summation_variables=s.real_summation_variables;
      non_real_summation_variables=s.non_real_summation_variables;
      new_values_for_xi_variables=s.new_values_for_xi_variables;
      summand_real_conditions=s.summand_real_conditions;
      summand_real_nextstate_map=s.summand_real_nextstate_map;
      nextstate_context_combinations=s.nextstate_context_combinations;
      // residual_inequalities=s.residual_inequalities;
      nextstate_value_combinations=s.nextstate_value_combinations;
    }

    ~summand_information()
    { smd.unprotect();
      real_summation_variables.unprotect();
      non_real_summation_variables.unprotect();

    }

    mcrl2::lps::summand get_summand() const
    { return smd;
    }

    variable_list get_real_summation_variables() const
    { return real_summation_variables;
    }

    variable_list get_non_real_summation_variables() const
    { return non_real_summation_variables;
    }

    atermpp::vector < mcrl2::data::data_expression >::const_iterator get_new_values_for_xi_variables_begin() const
    {
      return new_values_for_xi_variables.begin();
    }

    atermpp::vector < mcrl2::data::data_expression >::const_iterator get_new_values_for_xi_variables_end() const
    {
      return new_values_for_xi_variables.end();
    }

    std::vector < linear_inequality >::const_iterator get_summand_real_conditions_begin() const
    { return summand_real_conditions.begin();
    }

    std::vector < linear_inequality >::const_iterator get_summand_real_conditions_end() const
    { return summand_real_conditions.end();
    }

    atermpp::map<mcrl2::data::data_expression, mcrl2::data::data_expression> get_summand_real_nextstate_map() const
    { return summand_real_nextstate_map;
    }

    std::vector < std::vector < linear_inequality > > :: const_iterator nextstate_context_combinations_begin() const
    { return nextstate_context_combinations.begin();
    }

    std::vector < std::vector < linear_inequality > > :: const_iterator nextstate_context_combinations_end() const
    { return nextstate_context_combinations.end();
    }

    /* vector < vector < linear_inequality > > :: const_iterator residual_inequalities_begin() const
    { return residual_inequalities.begin();
    }

    vector < vector < linear_inequality > > :: const_iterator residual_inequalities_end() const
    { return residual_inequalities.end();
    } */

    atermpp::vector < data_expression_list > :: const_iterator nextstate_value_combinations_begin() const
    { return nextstate_value_combinations.begin();
    }

    static bool power_of_2(const unsigned int i)
    { unsigned int k=2;
      for( ; k<i ; k=k<<1)
      { // std::cerr << "HAHAH " << k << "\n";
      }
      return k==i;
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
    void add_a_new_next_state_argument(
                         const context_type &context,
                         const rewriter &r,
                         const comp_struct &c)
    {
      assert(context.size()==new_values_for_xi_variables.size()+1);
      real_representing_variable new_xi_variable=context.back();
      data_expression xi_t=new_xi_variable.get_lowerbound();
      data_expression xi_u=new_xi_variable.get_upperbound();
      data_expression substituted_lowerbound=
                 realelm_data_expression_map_replace(xi_t,summand_real_nextstate_map);
      data_expression substituted_upperbound=
                 realelm_data_expression_map_replace(xi_u,summand_real_nextstate_map);
      // std::cerr << "BOUNDS " << pp(substituted_lowerbound) << " -- " << pp(substituted_upperbound) << "\n";

      // First check whether the new value for the new xi variable is equal to itself.
      // I do not know whether optimisation below is correct.
      if ((linear_inequality(substituted_lowerbound,xi_t,linear_inequality::equal,r).is_true(r)) &&
         (linear_inequality(substituted_upperbound,xi_u,linear_inequality::equal,r).is_true(r)))
      { new_values_for_xi_variables.push_back(context.back().get_variable());
      }
      else
      { // normalize_pair(substituted_lowerbound,substituted_upperbound,r,false);
        linear_inequality e(substituted_lowerbound,substituted_upperbound,linear_inequality::less,r);
        data_expression normalized_substituted_lowerbound;
        data_expression normalized_substituted_upperbound;
        e.typical_pair(normalized_substituted_lowerbound,normalized_substituted_upperbound,r);

        // First check whether this new next state argument follows from an existing argument
        if (r(data::less(normalized_substituted_lowerbound,normalized_substituted_upperbound))==sort_bool::true_())
        { new_values_for_xi_variables.push_back(c.smaller());
        }
        else if (r(data::equal_to(normalized_substituted_lowerbound,normalized_substituted_upperbound))==sort_bool::true_())
        { new_values_for_xi_variables.push_back(c.equal());
        }
        else if (r(data::less(normalized_substituted_upperbound,normalized_substituted_lowerbound))==sort_bool::true_())
        { new_values_for_xi_variables.push_back(c.larger());
        }
        else
        { bool success(false);
          // Second check whether this new next state argument is equal to an existing argument
          for(context_type::const_reverse_iterator c=context.rbegin();
                       ((c!=context.rend()) && !success) ; ++c)
          { if ((normalized_substituted_lowerbound==c->get_lowerbound()) &&
                (normalized_substituted_upperbound==c->get_upperbound()))
            { new_values_for_xi_variables.push_back(c->get_variable());
              success=true;
            }
          }
          if (!success)
          { // Leave an empty spot, indicated by data_expression(). No prefabricated value can be set.
            new_values_for_xi_variables.push_back(mcrl2::data::data_expression());
          }
        }
      }

      /* if (new_values_for_xi_variables.back()!=data_expression())
      { // std::cerr << "New standard value for " << pp(context.back().get_variable()) << " is "
                     << pp(new_values_for_xi_variables.back()) << ")\n";
      }
      else
      { // std::cerr << "New standard value for " << pp(context.back().get_variable()) << " is undefined.\n";
      } */

#ifndef NDEBUG
      // sanity check
      for(atermpp::vector<mcrl2::data::data_expression>::const_iterator i = new_values_for_xi_variables.begin();
          i != new_values_for_xi_variables.end(); ++i)
      {
        assert(mcrl2::data::is_data_expression(*i) || *i == mcrl2::data::data_expression());
      }
      assert(context.size()==new_values_for_xi_variables.size());
#endif

      // At this point a data_expression has been pushed to the back of the new_values_for_xi_variables
      // Now it must be checked whether the newly added xi variable can lead to a standard value of
      // existing xi variables.

      context_type::const_iterator cntxt=context.begin();
      for(atermpp::vector < mcrl2::data::data_expression >::iterator cxi=new_values_for_xi_variables.begin();
                       (cxi+1)!=new_values_for_xi_variables.end(); ++cxi, ++cntxt)
      { if (*cxi==data_expression())
        { // Check whether lowerbound,upperbound for xi imply the substituted, normalized lowerbounds for
          // cntxtc->get_variable().
          data_expression cxi_t=cntxt->get_lowerbound();
          data_expression cxi_u=cntxt->get_upperbound();
          data_expression substituted_cxi_t=realelm_data_expression_map_replace(cxi_t,summand_real_nextstate_map);
          data_expression substituted_cxi_u=realelm_data_expression_map_replace(cxi_u,summand_real_nextstate_map);
          linear_inequality e(substituted_cxi_t,substituted_cxi_u,linear_inequality::less,r);
          e.typical_pair(substituted_cxi_t,substituted_cxi_u,r);
          if ((substituted_cxi_t==xi_t) && (substituted_cxi_u==xi_u))
          { *cxi=new_xi_variable.get_variable();
          }
        }
      }

      // Update the nextstate_context_combinations if new_values_for_xi_variables for this summand
      // does not get a concrete variable for this context variable.

      if ((new_values_for_xi_variables.back()==data_expression())
          || (new_values_for_xi_variables.back()==new_xi_variable.get_variable()))
      {
      // std::cerr << "BOUND1 " << pp(substituted_lowerbound) << " -- " << pp(substituted_upperbound) << "\n";
        data_expression t=substituted_lowerbound;
                // realelm_data_expression_map_replace(
                  //           context.back().get_lowerbound(),
                   //          summand_real_nextstate_map);
        data_expression u=substituted_upperbound;
                  // realelm_data_expression_map_replace(
                    //         context.back().get_upperbound(),
                     //        summand_real_nextstate_map);

        std::vector < std::vector < linear_inequality > > new_nextstate_context_combinations;
        atermpp::vector < data_expression_list > new_nextstate_value_combinations;
        // atermpp::vector < vector < linear_inequality > > new_residual_inequalities;
        // atermpp::vector < vector < linear_inequality > > ::const_iterator k=residual_inequalities.begin();
        atermpp::vector < data_expression_list >::const_iterator j=nextstate_value_combinations.begin();
        for(std::vector < std::vector < linear_inequality > >::iterator i=nextstate_context_combinations.begin();
                    i!=nextstate_context_combinations.end(); ++i,++j/* ,++k*/)
        { if (new_values_for_xi_variables.back()!=data_expression()) // i.e. it is a xi variable.
          {
            new_nextstate_value_combinations.push_back(*j); // This can be done more efficiently,
            new_nextstate_context_combinations.push_back(*i); // by not copying the arrays step by step.
          }
          else
          {
            std::vector < linear_inequality > vec_lin_eq;
            if (power_of_2(i->size()))
            { remove_redundant_inequalities(
                      *i,
                      vec_lin_eq,
                      r);
            }
            else 
            { vec_lin_eq.swap(*i);
            }
            unsigned int old_size=vec_lin_eq.size();
            // vector < linear_inequality > new_condition_list;
            vec_lin_eq.push_back(linear_inequality(t,u,linear_inequality::equal,r));
            /* remove_redundant_inequalities(
                      vec_lin_eq,
                      new_condition_list,
                      r); */
            // std::cerr << "New conditions list 1 " << pp_vector(new_condition_list) << "\n";
            if (!is_inconsistent(vec_lin_eq,r))
            // if (new_condition_list.empty() || !new_condition_list.front().is_false())
            { // new_nextstate_context_combinations.push_back(new_condition_list);
              new_nextstate_context_combinations.push_back(vec_lin_eq);
              if (new_values_for_xi_variables.back()==data_expression())
              { new_nextstate_value_combinations.push_back(push_front(*j,data_expression(c.equal())));
              }
              else
              { new_nextstate_value_combinations.push_back(*j);
              }
            // std::cerr << "New values list 1 " << pp(reverse(new_nextstate_value_combinations.back())) << "\n";
            }

            vec_lin_eq[old_size]=linear_inequality(t,u,linear_inequality::less,r);
            /* new_condition_list.clear();
            remove_redundant_inequalities(
                      vec_lin_eq,
                      new_condition_list,
                      r); */
            // std::cerr << "New conditions list 2 " << pp_vector(new_condition_list) << "\n";
            if (!is_inconsistent(vec_lin_eq,r))
            // if (new_condition_list.empty() || !new_condition_list.front().is_false())
            { // new_nextstate_context_combinations.push_back(new_condition_list);
              new_nextstate_context_combinations.push_back(vec_lin_eq);
              if (new_values_for_xi_variables.back()==data_expression())
              { new_nextstate_value_combinations.push_back(push_front(*j,data_expression(c.smaller())));
              }
              else
              { new_nextstate_value_combinations.push_back(*j);
              }
            // std::cerr << "New values list 2 " << pp(reverse(new_nextstate_value_combinations.back())) << "\n";
            }

            // std::cerr << "HUH " << pp(u) << " < " << pp(t) << "\n";
            vec_lin_eq[old_size]=linear_inequality(u,t,linear_inequality::less,r);
            // new_condition_list.clear();
            /* std::cerr << "Before remove redunda " << pp_vector(vec_lin_eq) << "\n";
            remove_redundant_inequalities(
                      vec_lin_eq,
                      new_condition_list,
                      r); */
            // std::cerr << "New conditions list 3 " << pp_vector(new_condition_list) << "\n";
            // if (!is_inconsistent(new_condition_list,r))
            // if (new_condition_list.empty() || !new_condition_list.front().is_false())
            if (!is_inconsistent(vec_lin_eq,r))
            { // new_nextstate_context_combinations.push_back(new_condition_list);
              new_nextstate_context_combinations.push_back(vec_lin_eq);
              if (new_values_for_xi_variables.back()==data_expression())
              { new_nextstate_value_combinations.push_back(push_front(*j,data_expression(c.larger())));
              }
              else
              { new_nextstate_value_combinations.push_back(*j);
              }
            // std::cerr << "New values list 3 " << pp(reverse(new_nextstate_value_combinations.back())) << "\n";
            }
          }
        }
        nextstate_value_combinations.swap(new_nextstate_value_combinations);
        nextstate_context_combinations.swap(new_nextstate_context_combinations);
        // residual_inequalities.swap(new_residual_inequalities);
      }
      std::cerr << "SIZE new nextstate_context combinations " << nextstate_context_combinations.size() << "\n";
    }
};




#endif // MCRL2_LPSREALELM_REALELM_H
