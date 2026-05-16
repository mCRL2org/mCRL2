// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriters/enumerate_quantifiers_rewriter.h
/// \brief A framework to instantiate quantifiers inside data expressions.

#ifndef MCRL2_DATA_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
#define MCRL2_DATA_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

#include <tuple>
#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/detail/split_finite_variables.h"
#include "mcrl2/data/enumerator.h"

namespace mcrl2::data
{

namespace detail 
{

template <typename DataRewriter, IsSubstitution MutableSubstitution>
struct enumerate_quantifiers_builder: public data_expression_builder<enumerate_quantifiers_builder<DataRewriter, MutableSubstitution>>
{
  using super = data_expression_builder<enumerate_quantifiers_builder<DataRewriter, MutableSubstitution>>;
  using self = enumerate_quantifiers_builder<DataRewriter, MutableSubstitution>;
  using enumerator_element = data::enumerator_list_element<data_expression>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  

  const DataRewriter& m_R;
  MutableSubstitution& m_sigma;
  const data::data_specification& m_dataspec;

  /// If false, only quantifier variables over finite sort are enumerated.
  bool m_enumerate_infinite_sorts;


  /// The enumerator
  data::enumerator_algorithm<self> E;

  /// \brief Constructor.
  /// \param R A data rewriter.
  /// \param sigma A mutable substitution.
  /// \param dataspec A data specification.
  /// \param id_generator A generator to generate fresh variable names.
  /// \param enumerate_infinite_sorts If true, quantifier variables of infinite sort are enumerated as well.
  enumerate_quantifiers_builder(const DataRewriter& R,
                                MutableSubstitution& sigma,
                                const data::data_specification& dataspec,
                                data::enumerator_identifier_generator& id_generator,
                                bool enumerate_infinite_sorts = true)
    : m_R(R),
      m_sigma(sigma),
      m_dataspec(dataspec),
      m_enumerate_infinite_sorts(enumerate_infinite_sorts),
      E(*this, m_dataspec, R, id_generator, (std::numeric_limits<std::size_t>::max)())
  {}
    

  atermpp::vector<data::data_expression> undo_substitution(const data::variable_list& variables)
  {
    atermpp::vector<data::data_expression> result;
    for (const data::variable& v : variables)
    {
      result.push_back(m_sigma(v));
      m_sigma[v] = v;
    }
    return result;
  }

  void redo_substitution(const data::variable_list& v, const atermpp::vector<data::data_expression>& undo)
  {
    assert(v.size() == undo.size());
    auto i = v.begin();
    auto j = undo.begin();
    while (i != v.end())
    {
      m_sigma[*i++] = *j++;
    }
  }

  // We assume that phi is already rewritten.
  void enumerate_forall(data_expression& result, const data::variable_list& v, const data_expression& phi)
  {
    assert(!v.empty());
    assert(&result!=&phi);
    atermpp::vector<data::data_expression> undo = undo_substitution(v);
    result = sort_bool::true_();
    E.enumerate(enumerator_element(v, phi),
                m_sigma,
                [&](const enumerator_element& p)
                {
                  data::optimized_and(result, result, p.expression());
                  return sort_bool::is_false_function_symbol(result);
                },
                sort_bool::is_true_function_symbol,
                sort_bool::is_false_function_symbol
               );

    redo_substitution(v, undo);
  }

  void enumerate_exists(data_expression& result, const data::variable_list& v, const data_expression& phi)
  {
    assert(!v.empty());
    assert(&result!=&phi);
    atermpp::vector<data::data_expression> undo = undo_substitution(v);
    result = sort_bool::false_();
    E.enumerate(enumerator_element(v, phi),
                m_sigma,
                [&](const enumerator_element& p)
                {
                  data::optimized_or(result, result, p.expression());
                  return sort_bool::is_false_function_symbol(result);
                },
                sort_bool::is_false_function_symbol,
                sort_bool::is_true_function_symbol
    );

    redo_substitution(v, undo);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    const bool remove_unused_variables=true;
    atermpp::vector<data::data_expression> undo = undo_substitution(x.variables());
    static_cast<super&>(*this).apply(result, x.body());
    std::set<data::variable> free_variables = find_free_variables(result);
    data::variable_list enumerable;
    data::variable_list non_enumerable;
    data::variable_list unused;

    if (m_enumerate_infinite_sorts)
    {
      data::detail::split_enumerable_variables(x.variables(), m_dataspec, m_R,
                                               enumerable, non_enumerable, unused,
                                               [&free_variables](const data::variable& v){ return free_variables.count(v)>0; });
    }
    else
    {
      data::detail::split_finite_variables(x.variables(), m_dataspec,
                                           enumerable, non_enumerable, unused,
                                           [&free_variables](const data::variable& v){ return free_variables.count(v)>0; });
    }
    if (enumerable.empty())
    {
      data::optimized_forall_no_empty_domain(result, non_enumerable, result, remove_unused_variables);
    }
    else
    {
      data_expression d;
      enumerate_forall(d, enumerable, result);
      result = d;
      data::optimized_forall_no_empty_domain(result, non_enumerable, result, remove_unused_variables);
    }
    redo_substitution(x.variables(), undo);
  }

  template <atermpp::IsATerm T>
  void apply(T& result, const data::exists& x)
  {
    const bool remove_unused_variables=true;
    atermpp::vector<data::data_expression> undo = undo_substitution(x.variables());
    static_cast<super&>(*this).apply(result, x.body());
    std::set<data::variable> free_variables = find_free_variables(result);
    data::variable_list enumerable;
    data::variable_list non_enumerable;
    data::variable_list unused;
    
    if (m_enumerate_infinite_sorts)
    {
      data::detail::split_enumerable_variables(x.variables(), m_dataspec, m_R,
                                               enumerable, non_enumerable, unused,
                                               [&free_variables](const data::variable& v){ return free_variables.count(v)>0; });
    }
    else
    {
      data::detail::split_finite_variables(x.variables(), m_dataspec,
                                           enumerable, non_enumerable, unused,
                                           [&free_variables](const data::variable& v){ return free_variables.count(v)>0; });
    }
    if (enumerable.empty())
    {
      data::optimized_exists_no_empty_domain(result, non_enumerable, result, remove_unused_variables);
    }
    else
    {
      data_expression d;
      enumerate_exists(d, enumerable, result);
      result = d;

      data::optimized_exists_no_empty_domain(result, non_enumerable, d, remove_unused_variables);
    }
    redo_substitution(x.variables(), undo);
  }

  template <atermpp::IsATerm T>
  void apply(T& result, const data::lambda& x)
  {
     atermpp::vector<data::data_expression> undo = undo_substitution(x.variables());
     static_cast<super&>(*this).apply(result, x.body());
     make_lambda(result, x.variables(), result);
     redo_substitution(x.variables(), undo);
  }

  template <atermpp::IsATerm T>
  void apply(T& result, const data::variable& x)
  {
    result=m_sigma(x);
  }

  // The enumerator requires the operator() below.
  void operator()(data_expression& result, const data_expression& x, MutableSubstitution& sigma)
  {
    m_R(result, x, sigma);
  } 
};

} // namespace detail

struct enumerate_quantifiers_rewriter
{       
  protected:
    /// \brief A data rewriter
    const data::rewriter& m_rewriter;
    
    /// \brief A data specification
    const data::data_specification& m_dataspec;
    
    /// \brief If true, quantifier variables of infinite sort are enumerated.
    bool m_enumerate_infinite_sorts;
    
    data::enumerator_identifier_generator& m_id_generator;
    
  public:
    using variable_type = data::variable;
    
    enumerate_quantifiers_rewriter(const data::rewriter& R, 
                                   const data::data_specification& dataspec, 
                                   data::enumerator_identifier_generator& id_generator, 
                                   bool enumerate_infinite_sorts = true)
      : m_rewriter(R), 
        m_dataspec(dataspec), 
        m_enumerate_infinite_sorts(enumerate_infinite_sorts),
        m_id_generator(id_generator)
    {}
    
    template<IsSubstitution Substitution>
    void operator()(data_expression& result, const data_expression& x, Substitution& sigma) const
    {
      detail::enumerate_quantifiers_builder<data::rewriter, Substitution>(m_rewriter, sigma, m_dataspec, m_id_generator, m_enumerate_infinite_sorts).apply(result, x);
    }

    template<IsSubstitution Substitution>
    data_expression operator()(const data_expression& x, Substitution& sigma) const
    {
      data_expression result;
      this->operator()(result, x, sigma);
      return result;
    }
};



} // namespace mcrl2::data

#endif // MCRL2_DATA_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
