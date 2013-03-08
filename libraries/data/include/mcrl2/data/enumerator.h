// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator.h
/// \brief The class enumerator.

#ifndef MCRL2_DATA_ENUMERATOR_H
#define MCRL2_DATA_ENUMERATOR_H

#include <functional>
#include <utility>
#include <boost/shared_ptr.hpp>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/utilities/sequence.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/number_postfix_generator.h"

namespace mcrl2
{

namespace data
{

/// \cond INTERNAL_DOCS
namespace detail
{

struct data_enumerator_helper
{
  const data_expression_with_variables& e_;
  const std::vector<data_expression_with_variables>& values_;
  std::vector<data_expression_with_variables>& result_;

  data_enumerator_helper(const data_expression_with_variables& e,
                         const std::vector<data_expression_with_variables>& values,
                         std::vector<data_expression_with_variables>& result
                        )
    : e_(e), values_(values), result_(result)
  {}

  /// \brief Function call operator
  void operator()()
  {
    data_expression d = data::replace_variables(static_cast<const data_expression&>(e_), data::make_sequence_sequence_substitution(e_.variables(), values_));

    std::vector<variable> v;
    for (std::vector<data_expression_with_variables>::const_iterator i = values_.begin(); i != values_.end(); ++i)
    {
      v.insert(v.end(), i->variables().begin(), i->variables().end());
    }
    result_.push_back(data_expression_with_variables(d, variable_list(v.begin(), v.end())));
  }
};

} // namespace detail
/// \endcond

/// \brief Class for enumerating data expressions.
class data_enumerator
{
  protected:

    /// \brief A map that caches the constructors corresponding to sort expressions.
    typedef std::map<sort_expression, std::vector<function_symbol> > constructor_map;

    /// \brief A data specification.
    const data_specification* m_data;

    /// \brief A rewriter.
    const data::rewriter* m_rewriter;

    /// \brief An identifier generator.
    mutable utilities::number_postfix_generator m_generator;

    /// \brief A mapping with constructors.
    mutable constructor_map m_constructors;

    /// \brief Returns the constructors with target s.
    /// \param s A sort expression
    /// \return The constructors corresponding to the sort expression.
    const std::vector<function_symbol>& constructors(sort_expression s) const
    {
      constructor_map::const_iterator i = m_constructors.find(s);
      if (i != m_constructors.end())
      {
        return i->second;
      }
      m_constructors[s] = m_data->constructors(s);
      return m_constructors[s];
    }

  public:

    /// \brief The variable type of the enumerator.
    typedef variable variable_type;

    /// \brief The term type of the enumerator.
    typedef data_expression_with_variables term_type;

    /// \brief Constructor.
    /// \param data_spec A data specification.
    /// \param rewriter A rewriter.
    /// \param generator An identifier generator.
    /// \param identifier_prefix A unique prefix, used by the identifier generator.
    data_enumerator(const data_specification& data_spec,
                    const data::rewriter& rewriter,
                    const std::string& identifier_prefix = "UNIQUE_PREFIX"
                   )
      : m_data(&data_spec), m_rewriter(&rewriter), m_generator(identifier_prefix)
    {}

    /// \brief The data specification.
    /// \return The data specification.
    const data_specification& data() const
    {
      return *m_data;
    }

    /// \return The identifier generator used for generating new variables.
    const utilities::number_postfix_generator& generator() const
    {
      return m_generator;
    }

    /// \return The identifier generator used for generating new variables.
    utilities::number_postfix_generator& generator()
    {
      return m_generator;
    }

    /// \brief Enumerates a data variable.
    /// \param v A data variable
    /// \return A sequence of expressions that is the result of applying the enumerator to the variable once.
    std::vector<data_expression_with_variables> enumerate(const variable& v) const
    {
      std::vector<data_expression_with_variables> result;
      const std::vector<function_symbol>& c = constructors(v.sort());
      if (c.empty())
      {
        throw mcrl2::runtime_error("Could not enumerate variable " + data::pp(v) + " of sort " + data::pp(v.sort()) + " as there are no constructors.");
      }
      for (std::vector<function_symbol>::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        if (is_function_sort(i->sort()))
        {
          std::vector<variable> variables;

          sort_expression_list i_domain(function_sort(i->sort()).domain());
          for (sort_expression_list::const_iterator j = i_domain.begin(); j != i_domain.end(); ++j)
          {
            variables.push_back(variable(m_generator(), *j));
          }

          variable_list w(variables.begin(),variables.end());

          result.push_back(data_expression_with_variables(application(*i, atermpp::aterm_cast< data_expression_list >(w)), w));
        }
        else
        {
          result.push_back(data_expression_with_variables(data_expression(*i), variable_list()));
        }
      }
      return result;
    }

    /// \brief Enumerates a data expression. Only the variables of the enumerator
    /// expression are expanded. Fresh variables are created using the
    /// identifier generator that was passed in the constructor.
    /// \param e A data expression.
    /// \return A sequence of expressions that is the result of applying the enumerator to the expression once.
    std::vector<data_expression_with_variables> enumerate(const data_expression_with_variables& e) const
    {
      std::vector<data_expression_with_variables> result;

      // Compute the instantiations for each variable of e.
      std::vector<std::vector<data_expression_with_variables> > enumerated_values;
      variable_list variables(e.variables());
      for (variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
      {
        enumerated_values.push_back(enumerate(*i));
      }

      std::vector<data_expression_with_variables> values(enumerated_values.size());

      utilities::foreach_sequence(enumerated_values, values.begin(), detail::data_enumerator_helper(e, values, result));
      return result;
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
