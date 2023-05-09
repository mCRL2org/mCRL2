// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/pres2res.h
/// \brief A straightforward algorithm that gets pres, and instantiates it to a res. 

#ifndef MCRL2_PRES_PRES2RES_H
#define MCRL2_PRES_PRES2RES_H


namespace mcrl2 {

namespace pres_system {

/// \brief A simple algorithm that takes a pres, instantiates it into a res, without parameterized 
///        variables, and sum, minall and maxall operators.
/// \details It does not attempt to optimize the solution, by partially solving it. It only applies some
///          simple rewriting, and small ad hoc optimisations. 
/// The result will be put in the structure graph that is passed in the constructor.
class pres2res_algorithm
{
  protected:
    atermpp::indexed_set<propositional_variable_instantiation> stored_variables;
    atermpp::vector<bes_expression> stored_rhs;
    std::size_t next_res_variable_index=0;

  public:
    pres2res(
      const pressolve_options& options,
      const pres& input_pres,
      const res& output_res
    )
    {}

    void on_report_equation(const propositional_variable_instantiation& X,
                            const pres_expression& psi,
                            std::size_t k
                           ) override
    {
      // the body of this if statement will only be executed for the first equation
      if (m_graph_builder.m_initial_state == data::undefined_data_expression())
      {
        m_graph_builder.set_initial_state(X);
      }
    }

    void run(const pres_expression& initial_pres_expression)
    {
      if (is_propositional_variable_instantiation(initial_pres_expression)
      {
        XXXXXXX
      }
      stored_variables.insert(
      while (next_res_variable<stored_index<stored_variables.size())
      {
        
      }
    }
};

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_PRES2RES_H
