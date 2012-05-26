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

#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/specification.h"

#include "comp.h"
#include "linear_inequalities.h"



mcrl2::lps::specification realelm(mcrl2::lps::specification s,
                                  int max_iterations = 5,
                                  const rewriter& r = rewriter());

template <typename Term, typename MapContainer>
Term realelm_data_expression_map_replace(Term t, const MapContainer& replacements);

void normalize_pair(data_expression&,data_expression&,const rewriter&, const bool);

class real_representing_variable
{
  private:
    mcrl2::data::variable variable;
    mcrl2::data::data_expression lowerbound;
    mcrl2::data::data_expression upperbound;

  public:
    real_representing_variable
    (mcrl2::data::variable v,
     mcrl2::data::data_expression lb,
     mcrl2::data::data_expression ub)
    {
      variable=v;
      lowerbound=lb;
      upperbound=ub;
    }


    /*  The code below gives rise to garbage collection problems. */
    ~real_representing_variable()
    {
    }

    real_representing_variable& operator = (const real_representing_variable& other)
    {
      variable=other.variable;
      lowerbound=other.lowerbound;
      upperbound=other.upperbound;
      return *this;
    }

    real_representing_variable(const real_representing_variable& other)
    {
      variable=other.variable;
      lowerbound=other.lowerbound;
      upperbound=other.upperbound;

    }

    mcrl2::data::variable get_variable() const
    {
      return variable;
    }

    mcrl2::data::data_expression get_lowerbound() const
    {
      return lowerbound;
    }

    mcrl2::data::data_expression get_upperbound() const
    {
      return upperbound;
    }
};

typedef std::vector< real_representing_variable > context_type;

class summand_information
{

  private:
    mcrl2::lps::deprecated::summand smd;
    variable_list real_summation_variables;
    variable_list non_real_summation_variables;
    std::vector < linear_inequality > summand_real_conditions;
    mutable_map_substitution< std::map<variable, data_expression> > summand_real_nextstate_map;
    // Variable below contains all combinations of nextstate_context_combinations that allow a
    // feasible solution, regarding the context variables that are relevant for this summand.
    std::vector < std::vector < linear_inequality > > nextstate_context_combinations;
    // vector < vector < linear_inequality > > residual_inequalities; // These are inequalities equal to
    // xi conditions which are used in the current
    // summand to be generated, but not added
    // to the context combinations. Adding them
    // is only necessary, if variables overlap.
    // context_type local_context;

  public:
    summand_information(
      const mcrl2::lps::deprecated::summand s,
      variable_list rsv,
      variable_list nrsv,
      std::vector < linear_inequality > src,
      mutable_map_substitution< std::map<mcrl2::data::variable, mcrl2::data::data_expression> > srnm
    ):
      smd(s),
      real_summation_variables(rsv),
      non_real_summation_variables(nrsv),
      summand_real_conditions(src),
      summand_real_nextstate_map(srnm),
      // Remove the variables in src that do not occur in nextstate via
      // fourier motzkin, because they are not needed for further calculations.
      // vector < linear_inequality >() reduced_src;
      // fourier_motzkin(src,vars.begin(),vars().end(),reduced_src);
      // nextstate_context_combinations(1,reduced_src),
      nextstate_context_combinations(1,src)
      // residual_inequalities(1,vector < linear_inequality >()),
    {
    }

    summand_information(const summand_information& s)
    {
      smd=s.smd;
      real_summation_variables=s.real_summation_variables;
      non_real_summation_variables=s.non_real_summation_variables;
      summand_real_conditions=s.summand_real_conditions;
      summand_real_nextstate_map=s.summand_real_nextstate_map;
      nextstate_context_combinations=s.nextstate_context_combinations;
      // residual_inequalities=s.residual_inequalities;
    }

    ~summand_information()
    {
    }

    mcrl2::lps::deprecated::summand get_summand() const
    {
      return smd;
    }

    variable_list get_real_summation_variables() const
    {
      return real_summation_variables;
    }

    variable_list get_non_real_summation_variables() const
    {
      return non_real_summation_variables;
    }

    std::vector < linear_inequality >::const_iterator get_summand_real_conditions_begin() const
    {
      return summand_real_conditions.begin();
    }

    std::vector < linear_inequality >::const_iterator get_summand_real_conditions_end() const
    {
      return summand_real_conditions.end();
    }

    mutable_map_substitution< std::map<mcrl2::data::variable, mcrl2::data::data_expression> >
                          &get_summand_real_nextstate_map()
    {
      return summand_real_nextstate_map;
    }

    std::vector < std::vector < linear_inequality > > :: const_iterator cnextstate_context_combinations_begin() const
    {
      return nextstate_context_combinations.begin();
    }

    std::vector < std::vector < linear_inequality > > :: const_iterator cnextstate_context_combinations_end() const
    {
      return nextstate_context_combinations.end();
    }

    std::vector < std::vector < linear_inequality > > :: iterator nextstate_context_combinations_begin()
    {
      return nextstate_context_combinations.begin();
    }

    std::vector < std::vector < linear_inequality > > :: iterator nextstate_context_combinations_end()
    {
      return nextstate_context_combinations.end();
    }

    /* vector < vector < linear_inequality > > :: const_iterator residual_inequalities_begin() const
    { return residual_inequalities.begin();
    }

    vector < vector < linear_inequality > > :: const_iterator residual_inequalities_end() const
    { return residual_inequalities.end();
    } */

    static bool power_of_2(const size_t i)
    {
      size_t k=2;
      for (; k<i ; k=k<<1)
        {}
      return k==i;
    }

    /// \brief Update the conditions for new summands, given that a new variable
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
      const context_type& context,
      const rewriter& r)
    {
      real_representing_variable new_xi_variable=context.back();
      data_expression xi_t=new_xi_variable.get_lowerbound();
      data_expression xi_u=new_xi_variable.get_upperbound();
      data_expression substituted_lowerbound = replace_free_variables(xi_t,summand_real_nextstate_map);
      data_expression substituted_upperbound = replace_free_variables(xi_u,summand_real_nextstate_map);
      // mCRL2log(debug) << "BOUNDS " << pp(substituted_lowerbound) << " -- " << pp(substituted_upperbound) << "\n";

      // First check whether the new value for the new xi variable is equal to itself.
      // I do not know whether optimisation below is correct.
      if ((linear_inequality(substituted_lowerbound,xi_t,linear_inequality::equal,r).is_true(r)) &&
          (linear_inequality(substituted_upperbound,xi_u,linear_inequality::equal,r).is_true(r)))
      {
        return;
      }

      linear_inequality e(substituted_lowerbound,substituted_upperbound,linear_inequality::less,r);
      data_expression normalized_substituted_lowerbound;
      data_expression normalized_substituted_upperbound;
      e.typical_pair(normalized_substituted_lowerbound,normalized_substituted_upperbound,r);

      // First check whether this new next state argument follows from an existing argument
      if (r(data::less(normalized_substituted_lowerbound,normalized_substituted_upperbound))==sort_bool::true_())
      {
        return;
      }
      else if (r(data::equal_to(normalized_substituted_lowerbound,normalized_substituted_upperbound))==sort_bool::true_())
      {
        return;
      }
      else if (r(data::less(normalized_substituted_upperbound,normalized_substituted_lowerbound))==sort_bool::true_())
      {
        return;
      }
      else
      {
        // Second check whether this new next state argument is equal to an existing argument
        for (context_type::const_reverse_iterator c=context.rbegin();
             c!=context.rend(); ++c)
        {
          if ((normalized_substituted_lowerbound==c->get_lowerbound()) &&
              (normalized_substituted_upperbound==c->get_upperbound()))
          {
            return;
          }
        }
      }

      data_expression t=substituted_lowerbound;
      data_expression u=substituted_upperbound;
      std::vector < std::vector < linear_inequality > > new_nextstate_context_combinations;
      for (std::vector < std::vector < linear_inequality > >::iterator i=nextstate_context_combinations.begin();
           i!=nextstate_context_combinations.end(); ++i)
      {
        std::vector < linear_inequality > vec_lin_eq;
        /* if (power_of_2(i->size()))
        { remove_redundant_inequalities(*i, vec_lin_eq, r);
        }
        else */

        vec_lin_eq.swap(*i);
        size_t old_size=vec_lin_eq.size();

        // Note that at this point, vec_lin_eq is consistent.
        assert(!is_inconsistent(vec_lin_eq,r));
        // If adding an equality t=u yields an inconsistency, then either adding t<u or t>u is
        // inconsistent. So, adding the reverse is redundant. Moreover, it cannot be that both
        // t<u and t>u are inconsistent (because then vec_lin_eq need to be inconsistent) and
        // it is also not possible that both t<u and t>u are consistent (because the solution
        // space for vec_lin_eq is a convex hull). So, if adding t=u is inconsistent, either
        // t<u or t>u can be added consistently, but in that case they are redundant. So,
        // we can simply take vec_lin_eq to remain untouched and we do not have to consider
        // t<u and t>u.

        vec_lin_eq.push_back(linear_inequality(t,u,linear_inequality::equal,r));
        if (is_inconsistent(vec_lin_eq,r))
        {
          // Add the original vec_lin_eq, as any extension is redundant.
          vec_lin_eq.pop_back();
          new_nextstate_context_combinations.push_back(std::vector < linear_inequality >());
          vec_lin_eq.swap(new_nextstate_context_combinations.back());
        }
        else
        {
          // Adding equality is consistent. Check whether adding a t<u and/or t>u is
          // also consistent, and if so, add new context combinations with t=u, and t<u and/or t>u.
          // If neither t<u and u>t are consistent, t=u is redundant, and vec_lin_eq can be
          // added to the new context combinations.

          vec_lin_eq[old_size].set_comparison(linear_inequality::less);
          if (!is_inconsistent(vec_lin_eq,r))
          {
            // Add a vector with t<u at the end.
            new_nextstate_context_combinations.push_back(vec_lin_eq);
            // Add a vector with t==u at the end.
            vec_lin_eq[old_size].set_comparison(linear_inequality::equal);
            new_nextstate_context_combinations.push_back(vec_lin_eq);

            vec_lin_eq[old_size]=linear_inequality(u,t,linear_inequality::less,r);
            if (!is_inconsistent(vec_lin_eq,r))
            {
              // add a vector with u<t at the end
              new_nextstate_context_combinations.push_back(std::vector < linear_inequality >());
              vec_lin_eq.swap(new_nextstate_context_combinations.back());
            }
          }
          else
          {
            vec_lin_eq[old_size]=linear_inequality(u,t,linear_inequality::less,r);
            if (!is_inconsistent(vec_lin_eq,r))
            {
              // add a vector with u<t at the end
              new_nextstate_context_combinations.push_back(vec_lin_eq);
              // add a vector with t==u at the end
              vec_lin_eq[old_size].set_comparison(linear_inequality::equal);
              new_nextstate_context_combinations.push_back(std::vector < linear_inequality >());
              vec_lin_eq.swap(new_nextstate_context_combinations.back());
            }
            else
            {
              // neither t<u, t>u hold. Only t==u, and it is redundant, so add the original vector.
              vec_lin_eq.pop_back();
              new_nextstate_context_combinations.push_back(std::vector < linear_inequality >());
              vec_lin_eq.swap(new_nextstate_context_combinations.back());
            }
          }
        }
      }
      nextstate_context_combinations.swap(new_nextstate_context_combinations);
      // mCRL2log(debug) << "SIZE new nextstate_context combinations " << nextstate_context_combinations.size() << "\n"
      //          << "IN summand " << pp(smd) << "\n";
    }
};




#endif // MCRL2_LPSREALELM_REALELM_H
