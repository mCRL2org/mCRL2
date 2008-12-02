// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_builder.h
/// \brief Visitor class for rebuilding a pbes expression.

#ifndef MCRL2_PBES_PBES_EXPRESSION_BUILDER_H
#define MCRL2_PBES_PBES_EXPRESSION_BUILDER_H

#include "mcrl2/exception.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/optimized_boolean_operators.h"

// this is only needed because of the data_true <-> pbes_true problems
#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

namespace pbes_system {

/// Visitor class for visiting the nodes of a pbes expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
/// An arbitrary additional argument may be passed during the recursion.
// TODO: rebuilding expressions with ATerms is very expensive. So it is probably
// more efficient to  check if the children of a node have actually changed,
// before rebuilding it.
template <typename Term, typename Arg = void>
struct pbes_expression_builder
{
  typedef Arg argument_type;
  typedef typename core::term_traits<Term>::term_type term_type;
  typedef typename core::term_traits<Term>::data_term_type data_term_type;
  typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
  typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;

  bool is_finished(const term_type& x)
  {
    return x != term_type();
  }
  
  /// Destructor.
  ///
  virtual ~pbes_expression_builder()
  { }

  /// Visit data expression node.
  ///
  virtual term_type visit_data_expression(const term_type& x, const data_term_type& /* d */, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit true node.
  ///
  virtual term_type visit_true(const term_type& x, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit false node.
  ///
  virtual term_type visit_false(const term_type& x, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit not node.
  ///
  virtual term_type visit_not(const term_type& x, const term_type& /* arg */, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit and node.
  ///
  virtual term_type visit_and(const term_type& x, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit or node.
  ///
  virtual term_type visit_or(const term_type& x, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }    

  /// Visit imp node.
  ///
  virtual term_type visit_imp(const term_type& x, const term_type& /* left */, const term_type& /* right */, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit forall node.
  ///
  virtual term_type visit_forall(const term_type& x, const variable_sequence_type& /* variables */, const term_type& /* expression */, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit exists node.
  ///
  virtual term_type visit_exists(const term_type& x, const variable_sequence_type& /* variables */, const term_type& /* expression */, Arg& /* arg */)
  {
    return term_type();
  }

  /// Visit propositional variable node.
  ///
  virtual term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& /* v */, Arg& /* arg */)
  {
    return term_type();
  }
  
  /// Visit unknown node. This function is called whenever a node of unknown type is encountered.
  /// By default a mcrl2::runtime_error exception will be generated.
  ///
  virtual term_type visit_unknown(const term_type& e, Arg& /* arg */)
  {
    throw mcrl2::runtime_error(std::string("error in pbes_expression_builder::visit() : unknown pbes expression ") + e.to_string());
    return term_type();
  }

  /// Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals term_type(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  term_type visit(term_type e, Arg& arg1)
  {
    typedef core::term_traits<term_type> tr;
    
#ifdef MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
std::cout << "<visit>" << tr::pp(e) << std::endl;
#endif

    term_type result;

    if (tr::is_data(e)) {
      result = visit_data_expression(e, tr::term2dataterm(e), arg1);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_true(e)) {
      result = visit_true(e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_false(e)) {
      result = visit_false(e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_not(e)) {
      term_type n = tr::arg(e);
      result = visit_not(e, n, arg1);
      if (!is_finished(result)) {
        result = core::optimized_not(visit(n, arg1));
      }
    } else if (tr::is_and(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_and(e, l, r, arg1);
      if (!is_finished(result)) {
        result = core::optimized_and(visit(l, arg1), visit(r, arg1));
      }
    } else if (tr::is_or(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_or(e, l, r, arg1);
      if (!is_finished(result)) {
        result = core::optimized_or(visit(l, arg1), visit(r, arg1));
      }
    } else if (tr::is_imp(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_imp(e, l, r, arg1);
      if (!is_finished(result)) {
        result = core::optimized_imp(visit(l, arg1), visit(r, arg1));
      }
    } else if (tr::is_forall(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      result = visit_forall(e, qvars, qexpr, arg1);
      if (!is_finished(result)) {
        result = core::optimized_forall(qvars, visit(qexpr, arg1));
      }
    } else if (tr::is_exists(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      result = visit_exists(e, qvars, qexpr, arg1);
      if (!is_finished(result)) {
        result = core::optimized_exists(qvars, visit(qexpr, arg1));
      }
    }
    else if(tr::is_prop_var(e)) {
      result = visit_propositional_variable(e, e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    }
    else {
      result = visit_unknown(e, arg1);
      if (!is_finished(result)) {
        result = e;
      }
    }

    // TODO: this is a hack, to deal with the data_true <-> pbes_true issue
    if (tr::is_true(result))
    {
      result = data::data_expr::true_();
    }
    if (tr::is_false(result))
    {
      result = data::data_expr::false_();
    }

#ifdef MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
std::cout << "<visit result>" << tr::pp(result) << std::endl;
#endif

    return result;
  }
};

/// Visitor class for visiting the nodes of a pbes expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns term_type(), the recursion is continued
/// in the children of this node, otherwise not.
template <typename Term>
struct pbes_expression_builder<Term, void>
{
  typedef void argument_type;
  typedef typename core::term_traits<Term>::term_type term_type;
  typedef typename core::term_traits<Term>::data_term_type data_term_type;
  typedef typename core::term_traits<Term>::variable_sequence_type variable_sequence_type;
  typedef typename core::term_traits<Term>::propositional_variable_type propositional_variable_type;

  bool is_finished(const term_type& x)
  {
    return x != term_type();
  }
  
  /// Destructor.
  ///
  virtual ~pbes_expression_builder()
  { }

  /// Visit data expression node.
  ///
  virtual term_type visit_data_expression(const term_type& x, const data_term_type& /* d */)
  {
    return term_type();
  }

  /// Visit true node.
  ///
  virtual term_type visit_true(const term_type& x)
  {
    return term_type();
  }

  /// Visit false node.
  ///
  virtual term_type visit_false(const term_type& x)
  {
    return term_type();
  }

  /// Visit not node.
  ///
  virtual term_type visit_not(const term_type& x, const term_type& /* arg */)
  {
    return term_type();
  }

  /// Visit and node.
  ///
  virtual term_type visit_and(const term_type& x, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// Visit or node.
  ///
  virtual term_type visit_or(const term_type& x, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }    

  /// Visit imp node.
  ///
  virtual term_type visit_imp(const term_type& x, const term_type& /* left */, const term_type& /* right */)
  {
    return term_type();
  }

  /// Visit forall node.
  ///
  virtual term_type visit_forall(const term_type& x, const variable_sequence_type& /* variables */, const term_type& /* expression */)
  {
    return term_type();
  }

  /// Visit exists node.
  ///
  virtual term_type visit_exists(const term_type& x, const variable_sequence_type& /* variables */, const term_type& /* expression */)
  {
    return term_type();
  }

  /// Visit propositional variable node.
  ///
  virtual term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& /* v */)
  {
    return term_type();
  }
  
  /// Visit unknown node. This function is called whenever a node of unknown type is encountered.
  /// By default a mcrl2::runtime_error exception will be generated.
  ///
  virtual term_type visit_unknown(const term_type& e)
  {
    throw mcrl2::runtime_error(std::string("error in pbes_expression_builder::visit() : unknown pbes expression ") + e.to_string());
    return term_type();
  }

  /// Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals term_type(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  term_type visit(term_type e)
  {
    typedef core::term_traits<term_type> tr;

#ifdef MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
std::cout << "<visit>" << tr::pp(e) << " " << e << std::endl;
#endif

    term_type result;

    if (tr::is_data(e)) {
      result = visit_data_expression(e, tr::term2dataterm(e));
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_true(e)) {
      result = visit_true(e);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_false(e)) {
      result = visit_false(e);
      if (!is_finished(result)) {
        result = e;
      }
    } else if (tr::is_not(e)) {
      term_type n = tr::arg(e);
      result = visit_not(e, n);
      if (!is_finished(result)) {
        result = core::optimized_not(visit(n));
      }
    } else if (tr::is_and(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_and(e, l, r);
      if (!is_finished(result)) {
        result = core::optimized_and(visit(l), visit(r));
      }
    } else if (tr::is_or(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_or(e, l, r);
      if (!is_finished(result)) {
        result = core::optimized_or(visit(l), visit(r));
      }
    } else if (tr::is_imp(e)) {
      term_type l = tr::left(e);
      term_type r = tr::right(e);
      result = visit_imp(e, l, r);
      if (!is_finished(result)) {
        result = core::optimized_imp(visit(l), visit(r));
      }
    } else if (tr::is_forall(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      result = visit_forall(e, qvars, qexpr);
      if (!is_finished(result)) {
        result = core::optimized_forall(qvars, visit(qexpr));
      }
    } else if (tr::is_exists(e)) {
      variable_sequence_type qvars = tr::var(e);
      term_type qexpr = tr::arg(e);
      result = visit_exists(e, qvars, qexpr);
      if (!is_finished(result)) {
        result = core::optimized_exists(qvars, visit(qexpr));
      }
    }
    else if(tr::is_prop_var(e)) {
      result = visit_propositional_variable(e, e);
      if (!is_finished(result)) {
        result = e;
      }
    }
    else {
      result = visit_unknown(e);
      if (!is_finished(result)) {
        result = e;
      }
    }

    // TODO: this is a temporary hack, to deal with the data_true <-> pbes_true problems in the rewriter
    if (tr::is_true(result))
    {
      result = data::data_expr::true_();
    }
    if (tr::is_false(result))
    {
      result = data::data_expr::false_();
    }

#ifdef MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
std::cout << "<visit result>" << tr::pp(result) << " " << result << std::endl;
#endif

    return result;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_BUILDER_H
