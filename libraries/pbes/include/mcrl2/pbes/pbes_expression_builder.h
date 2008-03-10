// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_builder.h
/// \brief Visitor class for rebuilding a pbes expression.

#ifndef MCRL2_PBES_PBES_EXPRESSION_BUILDER_H
#define MCRL2_PBES_PBES_EXPRESSION_BUILDER_H

#include <stdexcept>
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

inline
pbes_expression expr(const pbes_expression& x)
{
  return x;
}

inline
bool is_finished(const pbes_expression& x)
{
  return x != pbes_expression();
}

inline
pbes_expression make_result(const pbes_expression& n, const pbes_expression& x)
{
  return x;
}

/// Visitor class for visiting the nodes of a pbes expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
/// If a visit_<node> function returns pbes_expression(), the recursion is continued
/// in the children of this node, otherwise not.
// TODO: rebuilding expressions with ATerms is very expensive. So it is probably
// more efficient to  check if the children of a node have actually changed,
// before rebuilding it.
template <typename Node, typename Result>
struct pbes_builder
{
  /// Destructor.
  ///
  virtual ~pbes_builder()
  { }

  /// Visit data expression node.
  ///
  virtual Result visit_data_expression(const Node& n, const data::data_expression& /* d */)
  {
    return Result();
  }

  /// Visit true node.
  ///
  virtual Result visit_true(const Node& n)
  {
    return Result();
  }

  /// Visit false node.
  ///
  virtual Result visit_false(const Node& n)
  {
    return Result();
  }

  /// Visit not node.
  ///
  virtual Result visit_not(const Node& n, const pbes_expression& /* arg */)
  {
    return Result();
  }

  /// Visit and node.
  ///
  virtual Result visit_and(const Node& n, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return Result();
  }

  /// Visit or node.
  ///
  virtual Result visit_or(const Node& n, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return Result();
  }    

  /// Visit imp node.
  ///
  virtual Result visit_imp(const Node& n, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return Result();
  }

  /// Visit forall node.
  ///
  virtual Result visit_forall(const Node& n, const data::data_variable_list& /* variables */, const pbes_expression& /* expression */)
  {
    return Result();
  }

  /// Visit exists node.
  ///
  virtual Result visit_exists(const Node& n, const data::data_variable_list& /* variables */, const pbes_expression& /* expression */)
  {
    return Result();
  }

  /// Visit propositional variable node.
  ///
  virtual Result visit_propositional_variable(const Node& n, const propositional_variable_instantiation& /* v */)
  {
    return Result();
  }
  
  /// Visit unknown node. This function is called whenever a node of unknown type is encountered.
  /// By default a std::runtime_error exception will be generated.
  ///
  virtual Result visit_unknown(const pbes_expression& e)
  {
    throw std::runtime_error(std::string("error in pbes_builder::visit() : unknown pbes expression ") + e.to_string());
    return Result();
  }

  /// Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals pbes_expression(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  pbes_expression visit(const Node& n)
  {
    using namespace pbes_expr_optimized;
    using namespace accessors;

    Result result;
    pbes_expression e = expr(n);

    if (is_data(e)) {
      result = visit_data_expression(n, val(e));
      if (!is_finished(result)) {
        result = make_result(n, e);
      }
    } else if (is_pbes_true(e)) {
      result = visit_true(n);
      if (!is_finished(result)) {
        result = make_result(n, e);
      }
    } else if (is_pbes_false(e)) {
      result = visit_false(n);
      if (!is_finished(result)) {
        result = make_result(n, e);
      }
    } else if (is_pbes_not(e)) {
      const pbes_expression& arg = not_arg(e);
      result = visit_not(n, arg);
      if (!is_finished(result)) {
        e = not_(visit(arg));
        result = make_result(n, e);
      }
    } else if (is_pbes_and(e)) {
      const pbes_expression& left  = lhs(e);
      const pbes_expression& right = rhs(e);
      result = visit_and(n, left, right);
      if (!is_finished(result)) {
        e = and_(visit(left), visit(right));
        result = make_result(n, e);
      }
    } else if (is_pbes_or(e)) {
      const pbes_expression& left  = lhs(e);
      const pbes_expression& right = rhs(e);
      result = visit_or(n, left, right);
      if (!is_finished(result)) {
        e = or_(visit(left), visit(right));
        result = make_result(n, e);
      }
    } else if (is_pbes_imp(e)) {
      const pbes_expression& left  = lhs(e);
      const pbes_expression& right = rhs(e);
      result = visit_imp(n, left, right);
      if (!is_finished(result)) {
        e = imp(visit(left), visit(right));
        result = make_result(n, e);
      }
    } else if (is_pbes_forall(e)) {
      const data::data_variable_list& qvars = quant_vars(e);
      const pbes_expression& qexpr = quant_expr(e);
      result = visit_forall(n, qvars, qexpr);
      if (!is_finished(result)) {
        e = forall(qvars, visit(qexpr));
        result = make_result(n, e);
      }
    } else if (is_pbes_exists(e)) {
      const data::data_variable_list& qvars = quant_vars(e);
      const pbes_expression& qexpr = quant_expr(e);
      result = visit_exists(n, qvars, qexpr);
      if (!is_finished(result)) {
        e = exists(qvars, visit(qexpr));
        result = make_result(n, e);
      }
    }
    else if(is_propositional_variable_instantiation(e)) {
      result = visit_propositional_variable(n, propositional_variable_instantiation(e));
      if (!is_finished(result)) {
        result = make_result(n, e);
      }
    }
    else {
      result = visit_unknown(n);
      if (!is_finished(result)) {
        result = make_result(n, e);
      }
    }
    return result;
  }
};

typedef pbes_builder<pbes_expression, pbes_expression> pbes_expression_builder;

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_BUILDER_H
