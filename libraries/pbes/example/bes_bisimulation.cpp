// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_bisimulation.cpp
/// Implements strong bisimulation and oblivious bisimulation reduction on
/// boolean equation systems.

#include <algorithm>
#include <boost/bind.hpp>

#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/pbes/bes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/utility.h"

namespace mcrl2 {
namespace bes {

/// FIXME: Workaround circumventing the problem of using find_all_if in
/// combination with tr::is_variable
inline
bool is_variable(const atermpp::aterm_appl& t)
{
  if(core::detail::check_rule_BooleanExpression(t))
  {
    return core::term_traits<boolean_expression>::is_variable(t);
  }
  else
  {
    return false;
  }
}

/// FIXME: Workaround circumventing the problem of using find_all_if in
/// combination with tr::is_true
inline
bool is_true(const atermpp::aterm_appl& t)
{
  if(core::detail::check_rule_BooleanExpression(t))
  {
    return core::term_traits<boolean_expression>::is_true(t);
  }
  else
  {
    return false;
  }
}

/// FIXME: Workaround circumventing the problem of using find_all_if in
/// combination with tr::is_false
inline
bool is_false(const atermpp::aterm_appl& t)
{
  if(core::detail::check_rule_BooleanExpression(t))
  {
    return core::term_traits<boolean_expression>::is_false(t);
  }
  else
  {
    return false;
  }
}

/// \brief Returns or applied to the sequence of boolean expressions [first, last)
/// \param first Start of a sequence of boolean expressions
/// \param last End of a sequence of boolean expressions
/// \return Or applied to the sequence of boolean expressions [first, last)
template <typename FwdIt>
boolean_expression join_or(FwdIt first, FwdIt last)
{
  typedef core::term_traits<boolean_expression> tr;
  return core::detail::join(first, last, tr::or_, tr::false_());
}

/// \brief Returns and applied to the sequence of boolean expressions [first, last)
/// \param first Start of a sequence of boolean expressions
/// \param last End of a sequence of boolean expressions
/// \return And applied to the sequence of boolean expressions [first, last)
template <typename FwdIt>
boolean_expression join_and(FwdIt first, FwdIt last)
{
  typedef core::term_traits<boolean_expression> tr;
  return core::detail::join(first, last, tr::and_, tr::true_());
}

/// \brief Splits a disjunction into a sequence of operands
/// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PBES expression
/// \return A sequence of operands
inline
atermpp::set<boolean_expression> split_or(const boolean_expression& expr)
{
  typedef core::term_traits<boolean_expression> tr;
  atermpp::set<boolean_expression> result;
  core::detail::split(expr, std::insert_iterator<atermpp::set<boolean_expression> >(result, result.begin()), tr::is_or, tr::left, tr::right);
  return result;
}

  /// \brief Splits a conjunction into a sequence of operands
  /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
  /// function symbol.
  /// \param expr A PBES expression
  /// \return A sequence of operands
inline
atermpp::set<boolean_expression> split_and(const boolean_expression& expr)
{
  typedef core::term_traits<boolean_expression> tr;
  atermpp::set<boolean_expression> result;
  core::detail::split(expr, std::insert_iterator<atermpp::set<boolean_expression> >(result, result.begin()), tr::is_and, tr::left, tr::right);
  return result;
}

/// \brief Determines whether a boolean expression is in standard form.
/// \param e a boolean expression
/// \return true iff e is in standard form.
inline
bool is_standard_form(boolean_expression const& e)
{
  typedef core::term_traits<boolean_expression> tr;
  if(tr::is_true(e) || tr::is_false(e))
  {
    return true;
  }
  else if(tr::is_and(e))
  {
    atermpp::set<boolean_expression> arguments(split_and(e));
    for(atermpp::set<boolean_expression>::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
    {
      if(tr::is_or(*i) || tr::is_imp(*i))
      {
        return false;
      }
    }
    return true;
  }
  else if(tr::is_or(e))
  {
    atermpp::set<boolean_expression> arguments(split_or(e));
    for(atermpp::set<boolean_expression>::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
    {
      if(tr::is_and(*i) || tr::is_imp(*i))
      {
        return false;
      }
    }
    return true;
  }   
  else
  {
    return true;
  }
}

/// \brief Determines whether a BES is in standard form
/// \param bes a boolean equation system.
/// \return true iff bes is in standard form.
template <typename Container>
inline
bool is_standard_form(boolean_equation_system<Container> const& bes)
{
  for(typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
  {
    if(!is_standard_form(i->formula()))
    {
      return false;
    }
  }
  return true;
}

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

/// \brief Determines whether the constant true occurs in bes
/// \param bes a boolean equation system.
/// \return true iff the constant true occurs in bes.
template <typename Container>
inline
bool has_true(boolean_equation_system<Container> const& bes)
{
  for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
  {
    if(atermpp::find_if(i->formula(), is_true))
    {
      return true;
    }
  }
  return false;
}

/// \brief Determines whether the constant false occurs in bes
/// \param bes a boolean equation system.
/// \return true iff the constant false occurs in bes.
template <typename Container>
inline
bool has_false(boolean_equation_system<Container> const& bes)
{
  for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
  {
    if(atermpp::find_if(i->formula(), is_false))
    {
      return true;
    }
  }
  return false;
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

/// \brief Class to perform strong bisimulation or oblivious bisimulation
///        reduction on a boolean equation system.
class bisimulation_reduction
{
  protected:
    /// \brief A partitioning is a set of sets of equations (the so called blocks), they
    ///        are used to denote equivalence classes
    typedef std::set< std::set< boolean_equation > > partitioning;

    /// \brief Traits to identify boolean expressions
    typedef core::term_traits<boolean_expression> tr;

    /// \brief Maps each boolean equation to its alternation depth.
    std::map< boolean_equation, int > m_alternation_depths;

    /// \brief Cache that stores occurring variables.
    std::map< boolean_equation, std::set< boolean_equation > > m_postsets;

    /// \brief The BES that is being reduced.
    boolean_equation_system<> m_bes;

    /// \brief The current set of blocks.
    partitioning m_blocks;

    /// \brief initialises the alternation depths
    /// \param bes the bes of which the alternation depths are determined.
    /// \post for each equation e in bes, m_alternation_depths[i] equals the
    ///       alternation depth of e.
    template <typename Container>
    inline
    void initialise_alternation_depths(boolean_equation_system<Container> const& bes)
    {
      fixpoint_symbol symbol(fixpoint_symbol::nu());
      int ad = 0;
      assert(symbol.is_nu());
      
      for(typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        if(i->symbol() == symbol)
        {
          m_alternation_depths[*i] = ad;
        }
        else
        {
          m_alternation_depths[*i] = ++ad;
          symbol = i->symbol();
        }
      }
    }

    /// \brief Initialises the mapping of boolean equations to the defining
    ///        equations of the variables occuring in the right hand side.
    /// \param bes a boolean equation system
    /// \post m_postsets[e] is the set of equations e' such that e'.variable()
    ///       occurs in e.formula().
    template <typename Container>
    inline
    void initialise_postsets(boolean_equation_system<Container> const& bes)
    {
      std::map< boolean_variable, boolean_equation > variable_mapping;
      for(typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        variable_mapping[i->variable()] = *i;
      }

      for(typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        std::set< boolean_variable > occurring_variables;
        atermpp::find_all_if(i->formula(), is_variable, std::inserter(occurring_variables, occurring_variables.end()));

        std::set< boolean_equation > postset;
        for(std::set<boolean_variable>::const_iterator j = occurring_variables.begin(); j != occurring_variables.end(); ++j)
        {
          postset.insert(variable_mapping[*j]);
        }

        m_postsets[*i] = postset;
      }
    }

    /// \brief Returns the blocks that can be reached in one step because they
    ///        contain a variable in the formula of equation.
    /// \param equation a boolean equation.
    /// \return the set of blocks that are reachable in one step from equation.
    // TODO: This is not efficient, we could easily cache the sets that have been
    //       computed here.
    inline
    partitioning postset_blocks(boolean_equation const& equation)
    {
      if(core::gsDebug)
      {
        std::cerr << "      postset_blocks ENTER" << std::endl;
        std::cerr << "      computing blocks reachable in one step from " << pp(equation) << std::endl;
      }

      partitioning result;

      for(partitioning::const_iterator i = m_blocks.begin(); i != m_blocks.end(); ++i)
      {
        std::set< boolean_equation > postset(m_postsets[equation]);
        if(std::find_first_of(postset.begin(), postset.end(),
                              i->begin(), i->end()) != postset.end())
        {
          result.insert(*i);
        }
      }
     
      if(core::gsDebug)
      {
        std::cerr << "      can reach " << result.size() << " blocks" << std::endl;
        std::cerr << "      postset_blocks EXIT" << std::endl;
      }

      return result; 
    }

    /// \brief Determines whether e1 and e2 are equivelent according to the current
    ///        partitioning in m_blocks.
    /// \param e1 a boolean equation.
    /// \param e2 a boolean equation.
    /// \param oblivious_bisimulation perform oblivious bisimulation reduction instead of 
    ///        strong bisimulation reduction.
    /// \return false iff e1 and e2 are equivalent, i.e. true iff e1 and e2 are not
    ///         equivalent, and hence should be split.
    // 
    inline
    bool do_split(boolean_equation const& e1, boolean_equation const& e2, bool const& oblivious_bisimulation)
    {
      if(core::gsDebug)
      {
        std::cerr << "    do_split ENTER" << std::endl;
        std::cerr << "    do_split on " << pp(e1) << " and " << pp(e2) << std::endl;
      }

      bool result = m_alternation_depths[e1] == m_alternation_depths[e2];
      if(!result)
      {
        return result;
      }

      bool equal_boolean_connective = (tr::is_variable(e1.formula()) && tr::is_variable(e2.formula())) ||
           (tr::is_and(e1.formula()) && tr::is_and(e2.formula())) ||
           (tr::is_or(e1.formula()) && tr::is_or(e2.formula()));
      
      if(oblivious_bisimulation)
      {
        partitioning p1(postset_blocks(e1));
        result = result && (p1.size() == 1 || equal_boolean_connective) && p1 == postset_blocks(e2);
      }
      else
      {
        result = result && equal_boolean_connective && (postset_blocks(e1) == postset_blocks(e2));
      }

      if(core::gsDebug)
      {
        std::cerr << "    do_split EXIT" << std::endl;
      }
      
      return result;
    }

    /// \brief Splits the block into two blocks if there are any equations in the block
    ///        that are not equivalent to the first equation. Otherwise a single block is
    ///        returned.
    /// \param block the block to be split.
    /// \return The partitioning containing the single block block if all equations in 
    ///         block are equivalent, otherwise the partitioning containing two blocks is
    ///         returned.
    template <typename SplitPredicate>
    partitioning split(std::set<boolean_equation> const& block, SplitPredicate split_predicate)
    {
      if(core::gsDebug)
      {
        std::cerr << "  split ENTER" << std::endl;
        std::cerr << "  Splitting block" << std::endl;
      }

      assert(!block.empty());
      boolean_equation first(*block.begin());
      std::set<boolean_equation> b1;
      std::set<boolean_equation> b2;

      for(std::set<boolean_equation>::const_iterator i = block.begin(); i != block.end(); ++i)
      {
        if(split_predicate(first, *i))
        {
          b1.insert(*i);
        }
        else
        {
          b2.insert(*i);
        }
      }

      partitioning result;
      result.insert(b1);
      if(!b2.empty())
      {
        result.insert(b2);
      }
      
      if(core::gsDebug)
      {
        std::cerr << "  split EXIT" << std::endl;
      }

      return result;
    }

    /// \brief Constructs BES from partitioning in m_blocks
    boolean_equation_system<> construct_bes()
    {
      std::map<int, std::set<boolean_equation> > blocks;

      // Group blocks by alternation depth
      for(partitioning::const_iterator i = m_blocks.begin(); i != m_blocks.end(); ++i)
      {
        blocks[m_alternation_depths[*(i->begin())]].insert(*(i->begin()));
      }

      atermpp::vector<boolean_equation> equations;
      int last_nd = 0;
      // Per block we generate a single equation.
      for(std::map<int, std::set<boolean_equation> >::const_reverse_iterator i = blocks.rbegin(); i != blocks.rend(); ++i)
      {
        assert(i->first >= last_nd);
        last_nd = i->first;

        for(std::set<boolean_equation>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
          boolean_expression formula;

          partitioning reachable(postset_blocks(*j));
          if(reachable.size() == 1)
          {
            formula = reachable.begin()->begin()->variable();
          }
          else
          {
            std::vector<boolean_expression> bnd;
            for(partitioning::const_iterator k = reachable.begin(); k != reachable.end(); ++k)
            {
              bnd.push_back(boolean_expression(k->begin()->variable()));
            }

            if(tr::is_and(j->formula()))
            {
              formula = join_and(bnd.begin(), bnd.end());
            }
            else if(tr::is_or(j->formula()))
            {
              formula = join_or(bnd.begin(), bnd.end());
            }
            else
            {
              throw mcrl2::runtime_error("Don't know how to treat formula " + pp(j->formula()) + ", operand not supported");
            }
          }

          boolean_equation(j->symbol(), j->variable(), formula);
        }
      }

      // TODO: Check that the initial state is indeed defined!
      return boolean_equation_system<>(equations, m_bes.initial_state());
    }

    /// \brief Resets members.
    void reset()
    {
      m_blocks.clear();      
    }

  public:
    /// \brief Constructor.
    /// Initialises the bisimulation reduction with a bes, and computes the alternation
    /// depths.
    /// \param bes A boolean equation system.
    bisimulation_reduction(boolean_equation_system<> const& bes):
      m_bes(bes)
    {
//      assert(bes.is_closed());
      initialise_alternation_depths(bes);
      initialise_postsets(bes);
    }

    /// \brief Return a BES that is reduced maximally modulo strong bisimulation,
    ///        or oblivious bisimulation.
    ///        This implements the O(mn) algorithm due to Kanellakis and
    ///        Smolka.
    /// \param oblivious_bisimulation denotes whether to oblivious bisimulation
    ///        or strong bisimulation (oblivious bisimulation is used iff
    ///        oblivious_bisimulation == true).
    /// \post m_blocks contains the partitioning.
    boolean_equation_system<> naive_partitioning(bool oblivious_bisimulation = true)
    {
      reset(); // Make sure we start with a clean environment!

      // initial partitioning
      m_blocks.insert(std::set<boolean_equation>(m_bes.equations().begin(), m_bes.equations().end()));

      bool changed = true;
      int iteration = 0; // Counter for verbose information
      while(changed)
      {
        ++iteration;
        if(core::gsDebug)
        {
          std::cerr << "Iteration " << iteration << std::endl;
        }

        changed = false;
        partitioning fresh_partitioning;
        for(partitioning::const_iterator i = m_blocks.begin(); i != m_blocks.end(); ++i)
        {
          partitioning splitter = split(*i, boost::bind(&mcrl2::bes::bisimulation_reduction::do_split, this, _1, _2, oblivious_bisimulation));
          fresh_partitioning.insert(splitter.begin(), splitter.end());
          changed = changed || (splitter.size() != 1);
        }
        m_blocks = fresh_partitioning;
      }

      if(core::gsVerbose)
      {
        std::cerr << "Computed " << m_blocks.size() << " equivalence classes in " << iteration << " iterations" << std::endl;
      }

      return construct_bes();
    }
};

} //namespace bes
} //namespace mcrl2


/// \brief Simple input/output tool to perform strong as well as oblivious bisimulation
///        reduction on a boolean equation system.
class bes_bisimulation_tool: public mcrl2::utilities::tools::input_output_tool
{
  protected:

    typedef mcrl2::utilities::tools::input_output_tool super;

    bool m_oblivious_bisimulation;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("strong_bisimulation", "perform strong bisimulation reduction instead of oblivious bisimulation reduction", 's');
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      m_oblivious_bisimulation = 0 >= parser.options.count("strong_bisimulation");
    }      

  public:

    bes_bisimulation_tool()
      : super(
          "bes_bisimulation",
          "Jeroen Keiren",
          "reduce a bes modulo strong bisimulation or oblivious bisimulation",
          "Convert the PBES in INFILE to BES, reduce the resulting BES"
          "modulo strong bisimulation or oblivious "
          "bisimulation and write the result to OUTFILE. If INFILE is not "
          "present, stdin is used. If OUTFILE is not present, stdout is used.")
    {}

    bool run()
    {
      using namespace mcrl2::bes;
      using namespace mcrl2;

      // For now we load a pbes as input, and convert that to BES internally.
      pbes_system::pbes<> pbes;
      pbes.load(m_input_filename);
      
      boolean_equation_system<> bes;
      pbes_to_bes(pbes, bes);
      
      // Convert BES to standard recursive form if needed.
      if(!is_standard_form(bes))
      {
        if(core::gsVerbose)
        {
          std::cerr << "Converting BES to standard form" << std::endl;
        }
        bes = to_standard_form(bes);
      }
      if(has_true(bes))
      {
        if(core::gsVerbose)
        {
          std::cerr << "Removing constant true from BES" << std::endl;
        }
        bes = remove_true(bes);
      }
      if(has_false(bes))
      {
        if(core::gsVerbose)
        {
          std::cerr << "Removing constant false from BES" << std::endl;
        }
        bes = remove_false(bes);
      }


/*      if(!bes.is_closed())
      {
        throw mcrl2::runtime_error("The boolean equation system in the input file is not closed");
      }
*/     
      bisimulation_reduction bisimulation(bes);
      bes = bisimulation.naive_partitioning(m_oblivious_bisimulation);

      bes.save(m_output_filename);
      return true;
    }
};


int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return bes_bisimulation_tool().execute(argc, argv);
}

