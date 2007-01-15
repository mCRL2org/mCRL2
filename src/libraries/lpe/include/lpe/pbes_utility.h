///////////////////////////////////////////////////////////////////////////////
/// \file pbes_utility.h

#ifndef LPE_PBES_UTILITY_H
#define LPE_PBES_UTILITY_H

#include "lpe/pbes.h"
#include "lpe/data_init.h"
#include "lpe/data_utility.h"
#include "lpe/pbes_init.h"
#include "lpe/sort_init.h"
#include "atermpp/algorithm.h"
#include "dataimpl.h"

namespace lpe {

/// Returns all propositional variables that occur in the term t.
template <typename Term>
std::set<propositional_variable> find_propositional_variables(Term t)
{
  std::set<propositional_variable> variables;
  atermpp::find_all_if(t, is_propositional_variable, std::inserter(variables, variables.end()));
  return variables;
}

/// Returns all propositional variable instantiations that occur in the term t.
template <typename Term>
std::set<propositional_variable_instantiation> find_propositional_variable_instantiations(Term t)
{
  std::set<propositional_variable_instantiation> variables;
  atermpp::find_all_if(t, is_propositional_variable_instantiation, std::inserter(variables, variables.end()));
  return variables;
}

inline
specification implement_data_specification(const specification& spec)
{
  specification result(implement_data_spec(spec));
  return result;
}

/// Fresh variable generator that generates propositional variables with
/// names that do not appear in the given context.
class fresh_propositional_variable_generator
{
  protected:
    atermpp::set<aterm_string> m_identifiers;
    std::string m_hint;                // used as a hint for operator()()
    data_variable_list m_parameters;   // used for operator()()

  public:
    fresh_propositional_variable_generator()
     : m_hint("t")
    { }

    template <typename Term>
    fresh_propositional_variable_generator(Term context, std::string hint = "t", data_variable_list parameters = propositional_variable_list())
    {
      m_identifiers = identifiers(context);
      m_hint = hint;
      m_parameters = parameters;
    }

    /// Set a new hint.
    void set_hint(std::string hint)
    {
      m_hint = hint;
    }

    /// Returns the current hint.
    std::string hint() const
    {
      return m_hint;
    }

    /// Set a new context.
    template <typename Term>
    void set_context(Term context)
    {
      m_identifiers = identifiers(context);
    }

    /// Sets new parameters.
    void set_parameters(data_variable_list parameters)
    {
      m_parameters = parameters;
    }

    /// Returns the current parameters.
    data_variable_list parameters() const
    {
      return m_parameters;
    }

    /// Add term t to the context.
    template <typename Term>
    void add_to_context(Term t)
    {
      std::set<aterm_string> ids = identifiers(t);
      std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
    }

    /// Add all terms of the sequence [first .. last) to the context.
    template <typename Iter>
    void add_to_context(Iter first, Iter last)
    {
      for (Iter i = first; i != last; ++i)
      {
        std::set<aterm_string> ids = identifiers(*i);
        std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
      }
    }

    /// Returns a unique variable of the given sort, with the given hint as prefix.
    /// The returned variable is added to the context.
    propositional_variable operator()()
    {
      aterm_string id(m_hint);
      int index = 0;
      while (m_identifiers.find(id) != m_identifiers.end())
      {   
        std::string name = str(boost::format(m_hint + "%02d") % index++);
        id = aterm_string(name);
      }
      m_identifiers.insert(id);
      return propositional_variable(id, m_parameters);
    }

    /// Returns a unique variable with the same sort as the variable v, and with
    /// the same prefix. The returned variable is added to the context.
    propositional_variable operator()(propositional_variable v)
    {
      std::string hint = atermpp::unquote(v.name());
      aterm_string id(hint);
      int index = 0;
      while (m_identifiers.find(id) != m_identifiers.end())
      {   
        std::string name = str(boost::format(hint + "%02d") % index++);
        id = aterm_string(name);
      }
      m_identifiers.insert(id);
      return propositional_variable(id, v.parameters());
    }
};

/// Converts a pbes expression to a data expression.
inline
data_expression pbes2data(const pbes_expression& p, const specification& spec)
{
  using namespace pbes_init;
  namespace d = lpe::data_init;
  namespace s = lpe::sort_init;
  
  if (is_data(p)) {
    return val(p);
  } else if (is_true(p)) {
    return d::true_();
  } else if(is_false(p)) {
    return d::false_();
  } else if (is_and(p)) {
    return d::and_(pbes2data(arg1(p), spec), pbes2data(arg2(p), spec));
  } else if (is_or(p)) {
    return d::or_(pbes2data(arg1(p), spec), pbes2data(arg2(p), spec));
  } else if (is_forall(p)) {
      data_expression d1 = gsMakeForall(list_arg1(p), pbes2data(arg2(p), implement_data_specification(spec)));
      return d1;
  } else if (is_exists(p)) {
      data_expression d1 = pbes2data(arg2(p), implement_data_specification(spec));
aterm x = gsMakeExists(list_arg1(p), d1);
std::cout << "x = " << x << std::endl;
      data_expression d2 = gsMakeExists(list_arg1(p), d1);
      return d2;
  } else if (is_propositional_variable_instantiation(p)) {
    aterm_string vname = arg1(p);
    data_expression_list parameters = list_arg2(p);
    sort_list sorts = apply(parameters, gsGetSort);
    lpe::sort vsort = gsMakeSortArrowList(sorts, s::bool_());
    data_variable v(gsMakeDataVarId(vname, vsort));
    return gsMakeDataApplList(v, parameters);
  }
  throw std::runtime_error(std::string("pbes2data error: unknown pbes_variable_instantiation ") + p.to_string());
  return data_expression();
}

/// Converts a data expression to a pbes expression.
inline
pbes_expression data2pbes(data_expression q)
{
  namespace d = lpe::data_init;
  namespace p = lpe::pbes_init;
  namespace s = lpe::sort_init;

  data_expression head = q.head();
  data_expression_list arguments = q.arguments();
  if (is_data_variable(head))
  {
    return propositional_variable_instantiation(data_variable(head).name(), arguments);
  }
  else // head must be an operation id
  {
    assert(is_data_operation(head));
    if (d::is_true(head)) {
      return p::true_();
    } else if (d::is_false(head)) {
      return p::false_();
    } else if (d::is_and(head)) {
      return p::and_(data2pbes(arg1(q)), data2pbes(arg2(q)));
    } else if (d::is_or(head)) {
      return p::and_(data2pbes(arg1(q)), data2pbes(arg2(q)));
    }
    throw std::runtime_error(std::string("data2pbes error: unknown data_expression ") + q.to_string());
    return pbes_expression();
  }
  throw std::runtime_error(std::string("data2pbes error: unknown data_expression ") + q.to_string());
  return pbes_expression();
}

} // namespace lpe

#endif // LPE_PBES_UTILITY_H
