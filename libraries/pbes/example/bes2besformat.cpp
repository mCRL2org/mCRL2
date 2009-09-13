// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes2besformat.cpp
/// Convert a PBES without parameterisation to BES

#include <algorithm>
#include <boost/bind.hpp>

#include "bes_common.h"

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/pbes/bes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/input_output_tool.h"

namespace mcrl2 {

  namespace bes {
    
    /// \brief Converts a boolean equation to standard form, adding the generated
    ///        equations to result.
    /// \param e a boolean equation
    /// \param generator an identifier generator.
    /// \param result an output iterator to which boolean equations can be written.
    /// \post All boolean equations that are required to get e in standard form are
    ///       added to result.
    template <typename IdentifierGenerator, typename OutputIterator>
    inline
    void to_standard_form(boolean_equation const& e, IdentifierGenerator& generator, OutputIterator result)
    {
      typedef core::term_traits<boolean_expression> tr;
      boolean_expression f = e.formula();
      if (tr::is_true(f) || tr::is_false(f))
      {
        *result++ = e;
      }
      else if (tr::is_not(f))
      {
        if(!tr::is_variable(tr::arg(f)))
        {
          boolean_variable v(generator(tr::name(e.variable())));
          *result++ = boolean_equation(e.symbol(), e.variable(), tr::not_(v));
          boolean_equation e(e.symbol(), v, tr::arg(f));
          to_standard_form(e, generator, result);
        }
        else
        {
          *result++ = e;
        }
      }
      else if (tr::is_and(f))
      {
        atermpp::set<boolean_expression> arguments(split_and(f));
        atermpp::set<boolean_expression> sf_arguments;
        for(atermpp::set<boolean_expression>::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
        {
          if(!tr::is_true(*i) && !tr::is_false(*i) && !tr::is_variable(*i))
          {
            boolean_variable v(generator(tr::name(e.variable())));
            sf_arguments.insert(v);
            boolean_equation e(e.symbol(), v, *i);
            to_standard_form(e, generator, result);
          }
          else
          {
            sf_arguments.insert(*i);
          }
        }
        *result++ = boolean_equation(e.symbol(), e.variable(), join_and(sf_arguments.begin(), sf_arguments.end()));
      }
      else if (tr::is_or(f))
      {
        atermpp::set<boolean_expression> arguments(split_or(f));
        atermpp::set<boolean_expression> sf_arguments;
        for(atermpp::set<boolean_expression>::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
        {
          if(!tr::is_true(*i) && !tr::is_false(*i) && !tr::is_variable(*i))
          {
            boolean_variable v(generator(tr::name(e.variable())));
            sf_arguments.insert(v);
            boolean_equation e(e.symbol(), v, *i);
            to_standard_form(e, generator, result);
          }
          else
          {
            sf_arguments.insert(*i);
          }
        }
        *result++ = boolean_equation(e.symbol(), e.variable(), join_or(sf_arguments.begin(), sf_arguments.end()));
      }
      else
      {
        *result++ = e;
      }
    }
    
    /// \brief Remove constant true from a BES by introduction an equation nu X = X,
    ///        where X fresh.
    /// \param bes a boolean equation system.
    /// \return A boolean equations system equivalent to bes, but without the
    ///         occurrence of the constant true.
    template <typename Container>
    inline
    boolean_equation_system<Container> remove_true(boolean_equation_system<Container> const& bes)
    {
      typedef core::term_traits<boolean_expression> tr;
    
      Container equations;
      mcrl2::data::set_identifier_generator generator(bes);
      boolean_variable v_true(generator("X_true"));
      boolean_equation true_(boolean_equation(fixpoint_symbol::nu(), v_true, v_true));
      equations.insert(equations.end(), true_);
    
      for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        equations.insert(equations.end(), atermpp::replace(*i, tr::true_(), v_true));
      }
      return boolean_equation_system<Container>(equations, bes.initial_state());
    }
    
    /// \brief Remove constant false from a BES by introduction an equation mu X = X,
    ///        where X fresh.
    /// \param bes a boolean equation system.
    /// \return A boolean equations system equivalent to bes, but without the
    ///         occurrence of the constant false.
    template <typename Container>
    inline
    boolean_equation_system<Container> remove_false(boolean_equation_system<Container> const& bes)
    {
      typedef core::term_traits<boolean_expression> tr;
    
      Container equations;
      mcrl2::data::set_identifier_generator generator(bes);
      boolean_variable v_false(generator("X_false"));
      boolean_equation false_(boolean_equation(fixpoint_symbol::mu(), v_false, v_false));
      equations.insert(equations.end(), false_);
    
      for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        equations.insert(equations.end(), atermpp::replace(*i, tr::false_(), v_false));
      }
      return boolean_equation_system<Container>(equations, bes.initial_state());
    }
    
    /// \brief Convert a PBES expression that is a BES expression to a BES
    ///        expression.
    /// \param e a pbes expression
    /// \return the bes expression that is the same as e.
    inline
    boolean_expression pbes_expression_to_boolean_expression(pbes_system::pbes_expression const& e)
    {
      typedef core::term_traits<boolean_expression> tr;
      typedef core::term_traits<pbes_system::pbes_expression> pb;
    
      if(!pbes_system::is_bes(e))
      {
          throw mcrl2::runtime_error("PBES is not a BES because of expression " + pp(e) + ".");
      }
    
      if(pb::is_true(e))
      {
        return tr::true_();
      }
      else if (pb::is_false(e))
      {
        return tr::false_();
      }
      else if (pb::is_not(e))
      {
        return tr::not_(pbes_expression_to_boolean_expression(pb::arg(e)));
      }
      else if (pb::is_and(e))
      {
        return tr::and_(pbes_expression_to_boolean_expression(pb::left(e)), pbes_expression_to_boolean_expression(pb::right(e)));
      }
      else if (pb::is_or(e))
      {
        return tr::or_(pbes_expression_to_boolean_expression(pb::left(e)), pbes_expression_to_boolean_expression(pb::right(e)));
      }
      else if (pb::is_imp(e))
      {
        return tr::imp(pbes_expression_to_boolean_expression(pb::left(e)), pbes_expression_to_boolean_expression(pb::right(e)));
      }
      else if (pb::is_prop_var(e))
      {
        if(!pb::param(e).empty())
        {
          throw mcrl2::runtime_error("Propositional variable with non-empty parameter list");
        }
        else
        {
          return boolean_variable(pb::name(e));
        }
      }
      else
      {
        throw mcrl2::runtime_error("Unhandled expression " + pp(e));
      }
    }
    
    /// \brief Convert a container of pbes equations to a container of
    ///        boolean equations.
    /// \param equations a container of pbes equations that are bes equations
    /// \return the container of boolean equations corresponding to equations.
    template <typename BESContainer, typename PBESContainer>
    void pbes_equations_to_boolean_equations(PBESContainer const& equations, BESContainer& result)
    {
      using namespace pbes_system;
    
      for(typename PBESContainer::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        result.push_back(boolean_equation(i->symbol(),  propositional_variable_to_boolean_variable(i->variable()), pbes_expression_to_boolean_expression(i->formula())));
      }
    }
    
    /// \brief Convert a PBES to BES format, assuming the pbes is already a BES.
    /// \param p a PBES.
    /// \return the BES corresponding to PBES.
    template <typename BESContainer, typename PBESContainer>
    void pbes_to_bes(pbes_system::pbes<PBESContainer> const& p, boolean_equation_system<BESContainer>& result)
    {
      using namespace mcrl2::pbes_system;
    
      if(!p.global_variables().empty())
      {
        throw mcrl2::runtime_error("PBES contains free variables, cannot convert to BES");
      }
    
      BESContainer equations;
      pbes_equations_to_boolean_equations<BESContainer>(p.equations(), equations);
      boolean_variable initial_state = pbes_expression_to_boolean_expression(p.initial_state());
      result = boolean_equation_system<BESContainer>(equations, initial_state);
    }
    
    /// \brief Converts a BES to standard form
    /// \param bes a boolean equation system
    /// \return A BES equivalent to bes, that is in standard form.
    template <typename Container>
    inline
    boolean_equation_system<Container> to_standard_form(boolean_equation_system<Container> const& bes)
    {
      Container equations;
      mcrl2::data::set_identifier_generator generator(bes);
      for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        to_standard_form(*i, generator, std::back_inserter(equations));
      }
      return boolean_equation_system<Container>(equations, bes.initial_state());
    }
    
    /// \brief convert a propositional variable to a boolean variable
    /// \param v a propositional variable
    /// \return the boolean variable with the same name as v.
    inline
    boolean_variable propositional_variable_to_boolean_variable(pbes_system::propositional_variable const& v)
    {
      if(!v.parameters().empty())
      {
        throw mcrl2::runtime_error("Propositional variable with non-empty parameter list");
      }
      return boolean_variable(v.name());
    }
    
  }

}

/// \brief Simple input/output tool to perform strong as well as oblivious bisimulation
///        reduction on a boolean equation system.
class bes2besformat_tool: public mcrl2::utilities::tools::input_output_tool
{
  protected:

    typedef mcrl2::utilities::tools::input_output_tool super;

    bool m_srf;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("srf", "ensure the resulting BES is in standard recursive form", 's');
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      m_srf = 0 >= parser.options.count("srf");
    }      

  public:

    bes2besformat_tool()
      : super(
          "bes2besformat",
          "Jeroen Keiren",
          "Convert the PBES in INFILE to BES, assuming that the PBES already does not have parameterization",
          "Convert the PBES in INFILE to BES and write the result to OUTFILE. If INFILE is not "
          "present, stdin is used. If OUTFILE is not present, stdout is used.")
    {}

    bool run()
    {
      using namespace mcrl2::pbes_system;
      using namespace mcrl2::bes;
      using namespace mcrl2;

      pbes<> pbes;
      pbes.load(m_input_filename);

      boolean_equation_system<> bes;
      pbes_to_bes(pbes, bes);

      if(m_srf)
      {
        if(has_true(bes))
        {
          bes = remove_true(bes);
        }
        if(has_false(bes))
        {
          bes = remove_false(bes);
        }
        if(!is_standard_form(bes))
        {
          bes = to_standard_form(bes);
        }
      }

      bes.save(m_output_filename);
      return true;
    }
};


int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return bes2besformat_tool().execute(argc, argv);
}

