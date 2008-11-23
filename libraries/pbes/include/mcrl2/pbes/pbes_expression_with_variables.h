// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_with_variables.h
/// \brief A pbes expression with an associated sequence of data variables.

#ifndef MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
#define MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H

#include <set>
#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/data/term_traits.h"
#include "mcrl2/data/data_expression_with_variables.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"

namespace mcrl2 {

namespace pbes_system {

  /// A pbes expression that stores a list of variables. The
  /// intended use case is to store the free variables of the
  /// expression.
  class pbes_expression_with_variables: public pbes_expression
  {
    protected:
      data::data_variable_list m_variables;
      
    public:
      /// Constructor.
      ///             
      pbes_expression_with_variables()
      {}
  
      /// Constructor. Creates a data expression with an empty sequence of variables.
      ///             
      pbes_expression_with_variables(atermpp::aterm_appl term)
        : pbes_expression(term)
      {}
  
      /// Constructor. Creates a data expression with an empty sequence of variables.
      ///             
      pbes_expression_with_variables(ATermAppl term)
        : pbes_expression(term)
      {}

      /// Constructor.
      ///             
      pbes_expression_with_variables(pbes_expression expression, data::data_variable_list variables)
        : pbes_expression(expression), m_variables(variables)
      {}
      
      /// Return the variables.
      ///
      data::data_variable_list variables() const
      {
        return m_variables;
      }
      
      /// Return the variables.
      ///
      data::data_variable_list& variables()
      {
        return m_variables;
      }
      
      /// Returns true if the expression has no free variables.
      bool is_closed() const
      {
        return m_variables.empty();
      }
  }; 

} // namespace pbes_system

namespace core {

  template <>
  struct term_traits<pbes_system::pbes_expression_with_variables>
  {
    typedef pbes_system::pbes_expression_with_variables term_type;
    typedef data::data_variable variable_type;
    typedef data::data_expression_with_variables data_term_type;
    typedef data::data_expression_list data_term_sequence_type;
    typedef data::data_variable data_variable_type;
    typedef data::data_variable_list variable_sequence_type;
    typedef pbes_system::propositional_variable propositional_variable_decl_type;   
    typedef pbes_system::propositional_variable_instantiation propositional_variable_type;
    typedef core::identifier_string string_type;

    typedef core::term_traits<pbes_system::pbes_expression> tr;

    static inline
    bool is_constant(const term_type& t)
    {
      return t.variables().empty();
    }

    static inline
    bool is_true(term_type t) { return tr::is_true(t); }
    
    static inline 
    bool is_false(term_type t) { return tr::is_false(t); }
    
    static inline 
    bool is_not(term_type t) { return tr::is_not(t); }
    
    static inline 
    bool is_and(term_type t) { return tr::is_and(t); }
    
    static inline 
    bool is_or(term_type t) { return tr::is_or(t); }
    
    static inline 
    bool is_imp(term_type t) { return tr::is_imp(t); }
    
    static inline 
    bool is_forall(term_type t) { return tr::is_forall(t); }
    
    static inline 
    bool is_exists(term_type t) { return tr::is_exists(t); }
    
    static inline 
    bool is_data(term_type t) { return tr::is_data(t); }
    
    static inline 
    bool is_prop_var(term_type t) { return tr::is_prop_var(t); }

    static inline
    term_type true_() { return tr::true_(); }
    
    static inline
    term_type false_() { return tr::false_(); }
    
    static inline
    term_type not_(term_type p) { return term_type(tr::not_(p), p.variables()); }
    
    static inline
    term_type and_(term_type p, term_type q)
    {
      return term_type(tr::and_(p, q), atermpp::term_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type or_(term_type p, term_type q)
    {
      return term_type(tr::or_(p, q), atermpp::term_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type imp(term_type p, term_type q)
    {
      return term_type(tr::imp(p, q), atermpp::term_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type forall(variable_sequence_type l, term_type p)
    {
      return term_type(tr::forall(l, p), atermpp::term_list_difference(p.variables(), l));
    }
    
    static inline
    term_type exists(variable_sequence_type l, term_type p)
    {
      return term_type(tr::exists(l, p), atermpp::term_list_difference(p.variables(), l));
    }

    template <typename Iter>
    static
    term_type prop_var(const string_type& name, Iter first, Iter last)
    {
      std::set<data_term_type> v;
      for (Iter i = first; i != last; ++i)
      {
        v.insert(i->variables().begin(), i->variables().end());
      }
      return term_type(tr::prop_var(name, first, last), variable_sequence_type(v.begin(), v.end()));
    }

    static inline
    data_term_type val(term_type t) { return tr::val(t); }  

    static inline
    term_type arg(term_type t) { return tr::arg(t); }
    
    static inline
    term_type left(term_type t) { return tr::left(t); }
    
    static inline
    term_type right(term_type t) { return tr::right(t); }
    
    static inline
    variable_sequence_type var(term_type t) { return tr::var(t); }
    
    static inline
    string_type name(term_type t) { return tr::name(t); }
    
    static inline
    data_term_sequence_type param(term_type t) { return tr::param(t); }

    static inline
    term_type variable2term(variable_type v)
    {
      return tr::variable2term(v);
    }

    static inline
    term_type dataterm2term(data_term_type t)
    {
      return term_type(t, t.variables());
    }    
   
    static inline
    std::string pp(term_type t)
    {
      return core::pp(t) + " " + core::pp(t.variables());
    }
  };

} // namespace core

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {
template<>
struct aterm_traits<mcrl2::pbes_system::pbes_expression_with_variables >
{
  typedef ATermAppl aterm_type;
  static void protect(mcrl2::pbes_system::pbes_expression_with_variables t)   { t.protect(); t.variables().protect(); }
  static void unprotect(mcrl2::pbes_system::pbes_expression_with_variables t) { t.unprotect(); t.variables().unprotect(); }
  static void mark(mcrl2::pbes_system::pbes_expression_with_variables t)      { t.mark(); t.variables().mark(); }
  static ATerm term(mcrl2::pbes_system::pbes_expression_with_variables t)     { return t.term(); }
  static ATerm* ptr(mcrl2::pbes_system::pbes_expression_with_variables& t)    { return &t.term(); }
};
}
/// \endcond

#endif // MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
