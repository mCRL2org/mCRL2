// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/ressolve.h
/// \brief This contains a gauss-elimination like algorithm to solve a res

#ifndef MCRL2_PRES_RESSOLVE_H
#define MCRL2_PRES_RESSOLVE_H

#include "mcrl2/data/real_utilities.h"
#include "mcrl2/pres/builder.h" 

namespace mcrl2 {

namespace pres_system {

/// \brief A builder that substitutes an expression for a propositional variable instantiation in a pres expression. 
///
class substitute_pres_equation_builder: public pres_expression_builder < substitute_pres_equation_builder >
{
protected:
  core::identifier_string m_pres_variable_name;
  pres_expression m_term;

public:
  typedef  pres_expression_builder < substitute_pres_equation_builder > super;
  using super::apply;

  substitute_pres_equation_builder(const propositional_variable& variable, const pres_expression& t)
   : m_pres_variable_name(variable.name()),
     m_term(t)
  {}

  template <class T>
  void apply(T& result, const pres_system::propositional_variable_instantiation& x)
  {
    if (x.name()==m_pres_variable_name)
    {
      result = m_term;
    }
    else
    {
      result = x;
    }
  }

};


namespace detail {

void push_and_inside(pres_expression& result, const pres_expression t1, const pres_expression& t2, const bool conjunctive_normal_form)
{
  pres_expression aux;
  if (is_condsm(t1))
  {
    push_and_inside(aux, atermpp::down_cast<condsm>(t1).arg2(), t2, conjunctive_normal_form);
    push_and_inside(result, atermpp::down_cast<condsm>(t1).arg3(), t2, conjunctive_normal_form);
    make_condsm(result, atermpp::down_cast<condsm>(t1).arg1(), aux, result);
  }
  else if (is_condsm(t2))
  {
    push_and_inside(aux, t1, atermpp::down_cast<condsm>(t2).arg2(), conjunctive_normal_form);
    push_and_inside(result, t1, atermpp::down_cast<condsm>(t2).arg3(), conjunctive_normal_form);
    make_or_(result, atermpp::down_cast<condsm>(t2).arg1(), aux, result);
  }
  else if (is_condeq(t1))
  {
    push_and_inside(aux, atermpp::down_cast<condeq>(t1).arg2(), t2, conjunctive_normal_form);
    push_and_inside(result, atermpp::down_cast<condeq>(t1).arg3(), t2, conjunctive_normal_form);
    make_condeq(result, atermpp::down_cast<condeq>(t1).arg1(), aux, result);
  }
  else if (is_condeq(t2))
  {
    push_and_inside(aux, t1, atermpp::down_cast<condeq>(t2).arg2(), conjunctive_normal_form);
    push_and_inside(result, t1, atermpp::down_cast<condeq>(t2).arg3(), conjunctive_normal_form);
    make_or_(result, atermpp::down_cast<condeq>(t2).arg1(), aux, result);
  }
  else if (!conjunctive_normal_form && is_or(t1))
  {
    push_and_inside(aux, atermpp::down_cast<or_>(t1).left(), t2, conjunctive_normal_form);
    push_and_inside(result, atermpp::down_cast<or_>(t1).right(), t2, conjunctive_normal_form);
    make_or_(result, aux, result);
  }
  else if (!conjunctive_normal_form && is_or(t2))
  {
    push_and_inside(aux, t1, atermpp::down_cast<or_>(t2).left(), conjunctive_normal_form);
    push_and_inside(result, t1, atermpp::down_cast<or_>(t2).right(), conjunctive_normal_form);
    make_or_(result, aux, result);
  }
  else make_and_(result, t1, t2);
}

void push_or_inside(pres_expression& result, const pres_expression t1, const pres_expression& t2, const bool conjunctive_normal_form)
{ 
  pres_expression aux;
  if (is_condsm(t1))
  { 
    push_or_inside(aux, atermpp::down_cast<condsm>(t1).arg2(), t2, conjunctive_normal_form);
    push_or_inside(result, atermpp::down_cast<condsm>(t1).arg3(), t2, conjunctive_normal_form);
    make_condsm(result, atermpp::down_cast<condsm>(t1).arg1(), aux, result);
  }
  else if (is_condsm(t2))
  { 
    push_or_inside(aux, t1, atermpp::down_cast<condsm>(t2).arg2(), conjunctive_normal_form);
    push_or_inside(result, t1, atermpp::down_cast<condsm>(t2).arg3(), conjunctive_normal_form);
    make_or_(result, atermpp::down_cast<condsm>(t2).arg1(), aux, result);
  }
  else if (is_condeq(t1))
  { 
    push_or_inside(aux, atermpp::down_cast<condeq>(t1).arg2(), t2, conjunctive_normal_form);
    push_or_inside(result, atermpp::down_cast<condeq>(t1).arg3(), t2, conjunctive_normal_form);
    make_condeq(result, atermpp::down_cast<condeq>(t1).arg1(), aux, result);
  }
  else if (is_condeq(t2))
  { 
    push_or_inside(aux, t1, atermpp::down_cast<condeq>(t2).arg2(), conjunctive_normal_form);
    push_or_inside(result, t1, atermpp::down_cast<condeq>(t2).arg3(), conjunctive_normal_form);
    make_or_(result, atermpp::down_cast<condeq>(t2).arg1(), aux, result);
  }
  else if (conjunctive_normal_form && is_and(t1))
  { 
    push_or_inside(aux, atermpp::down_cast<and_>(t1).left(), t2, conjunctive_normal_form);
    push_or_inside(result, atermpp::down_cast<and_>(t1).right(), t2, conjunctive_normal_form);
    make_or_(result, aux, result);
  }
  else if (conjunctive_normal_form && is_and(t2))
  { 
    push_or_inside(aux, t1, atermpp::down_cast<and_>(t2).left(), conjunctive_normal_form);
    push_or_inside(result, t1, atermpp::down_cast<and_>(t2).right(), conjunctive_normal_form);
    make_or_(result, aux, result);
  }
  else make_or_(result, t1, t2);
} 

void push_plus_inside(pres_expression& result, const pres_expression t1, const pres_expression& t2, const bool conjunctive_normal_form)
{
  pres_expression aux;
  if (is_condsm(t1))
  {
    push_plus_inside(aux, atermpp::down_cast<condsm>(t1).arg2(), t2, conjunctive_normal_form);
    push_plus_inside(result, atermpp::down_cast<condsm>(t1).arg3(), t2, conjunctive_normal_form);
    make_condsm(result, atermpp::down_cast<condsm>(t1).arg1(), aux, result);
  }
  else if (is_condsm(t2))
  {
    push_plus_inside(aux, t1, atermpp::down_cast<condsm>(t2).arg2(), conjunctive_normal_form);
    push_plus_inside(result, t1, atermpp::down_cast<condsm>(t2).arg3(), conjunctive_normal_form);
    make_or_(result, atermpp::down_cast<condsm>(t2).arg1(), aux, result);
  }
  else if (is_condeq(t1))
  {
    push_plus_inside(aux, atermpp::down_cast<condeq>(t1).arg2(), t2, conjunctive_normal_form);
    push_plus_inside(result, atermpp::down_cast<condeq>(t1).arg3(), t2, conjunctive_normal_form);
    make_condeq(result, atermpp::down_cast<condeq>(t1).arg1(), aux, result);
  }
  else if (is_condeq(t2))
  {
    push_plus_inside(aux, t1, atermpp::down_cast<condeq>(t2).arg2(), conjunctive_normal_form);
    push_plus_inside(result, t1, atermpp::down_cast<condeq>(t2).arg3(), conjunctive_normal_form);
    make_or_(result, atermpp::down_cast<condeq>(t2).arg1(), aux, result);
  }
  else if (conjunctive_normal_form && is_and(t1))  // CNF: first move || upwards.
  {
    push_plus_inside(aux, atermpp::down_cast<and_>(t1).left(), t2, conjunctive_normal_form);
    push_plus_inside(result, atermpp::down_cast<and_>(t1).right(), t2, conjunctive_normal_form);
    make_and_(result, aux, result);
  }
  else if (conjunctive_normal_form && is_and(t2))
  {
    push_plus_inside(aux, t1, atermpp::down_cast<and_>(t2).left(), conjunctive_normal_form);
    push_plus_inside(result, t1, atermpp::down_cast<and_>(t2).right(), conjunctive_normal_form);
    make_and_(result, aux, result);
  }
  else if (is_or(t1))
  {
    push_plus_inside(aux, atermpp::down_cast<or_>(t1).left(), t2, conjunctive_normal_form);
    push_plus_inside(result, atermpp::down_cast<or_>(t1).right(), t2, conjunctive_normal_form);
    make_or_(result, aux, result);
  }
  else if (is_or(t2))
  {
    push_plus_inside(aux, t1, atermpp::down_cast<or_>(t2).left(), conjunctive_normal_form);
    push_plus_inside(result, t1, atermpp::down_cast<or_>(t2).right(), conjunctive_normal_form);
    make_or_(result, aux, result);
  }
  else if (!conjunctive_normal_form && is_and(t1))  // DNF: first more || upwards. 
  {
    push_plus_inside(aux, atermpp::down_cast<and_>(t1).left(), t2, conjunctive_normal_form);
    push_plus_inside(result, atermpp::down_cast<and_>(t1).right(), t2, conjunctive_normal_form);
    make_and_(result, aux, result);
  }
  else if (!conjunctive_normal_form && is_and(t2))
  {
    push_plus_inside(aux, t1, atermpp::down_cast<and_>(t2).left(), conjunctive_normal_form);
    push_plus_inside(result, t1, atermpp::down_cast<and_>(t2).right(), conjunctive_normal_form);
    make_and_(result, aux, result);
  }
  else make_plus(result, t1, t2);
}

void push_constant_inside(pres_expression& result, const data::data_expression& constant, const pres_expression& t)
{
  pres_expression aux;
  if (is_condsm(t))
  { 
    push_constant_inside(aux, constant, atermpp::down_cast<condsm>(t).arg2());
    push_constant_inside(result, constant, atermpp::down_cast<condsm>(t).arg3());
    make_condsm(result, atermpp::down_cast<condsm>(t).arg1(), aux, result);
  }
  else if (is_condeq(t))
  {
    push_constant_inside(aux, constant, atermpp::down_cast<condsm>(t).arg2());
    push_constant_inside(result, constant, atermpp::down_cast<condsm>(t).arg3());
    make_condeq(result, atermpp::down_cast<condsm>(t).arg1(), aux, result);
  }
  else if (is_and(t))
  {
    push_constant_inside(aux, constant, atermpp::down_cast<and_>(t).left());
    push_constant_inside(result, constant, atermpp::down_cast<and_>(t).right());
    make_and_(result, aux, result);
  }
  else if (is_or(t))
  {
    pres_expression aux;
    push_constant_inside(aux, constant, atermpp::down_cast<or_>(t).left());
    push_constant_inside(result, constant, atermpp::down_cast<or_>(t).right());
    make_or_(result, aux, result);
  }
  else if (is_plus(t))
  {
    pres_expression aux;
    push_constant_inside(aux, constant, atermpp::down_cast<plus>(t).left());
    push_constant_inside(result, constant, atermpp::down_cast<plus>(t).right());
    make_plus(result, aux, result);
  }
  else if (is_const_multiply(t))
  {
    const const_multiply& tcm = atermpp::down_cast<const_multiply>(t);
    data::data_expression result_term;
    data::sort_real::make_times(result_term, tcm.left(), constant);
    make_const_multiply(result, result_term, tcm.right());
  }
  else if (is_const_multiply_alt(t))
  {
    const const_multiply_alt& tcm = atermpp::down_cast<const_multiply_alt>(t);
    data::data_expression result_term;
    data::sort_real::make_times(result_term, tcm.right(), constant);
    make_const_multiply(result, result_term, tcm.left());
  }
  else
  {
    make_const_multiply(result, constant, t);
  }
}

struct linear_fixed_point_equation
{
  data::data_expression c_j;
  pres_expression f_j;
  bool linear_term_present=false;    // is true if c_j > 0.
  bool f_j_term_present=false;
  bool eqninf_term_present=false;      // is true if c_j' = 1.

  void update_f_j(const pres_expression& t,  const bool minimal_fixed_point)
  {
    if (f_j_term_present)
    {
      if (minimal_fixed_point)             
      {
        f_j=or_(f_j, t);
      }
      else
      {
        f_j=and_(f_j, t);
      }
    }
    else
    {
      f_j_term_present=true;
      f_j = t;
    }
  }
};

// Collect the linear equations for variable v in the current conjunct/disjunct. 
void collect_line(linear_fixed_point_equation& line, const  propositional_variable& v, const pres_expression& t, const bool minimal_fixed_point)
{
  if (is_plus(t))
  {
    collect_line(line, v, atermpp::down_cast<plus>(t).left(), minimal_fixed_point);
    collect_line(line, v, atermpp::down_cast<plus>(t).right(), minimal_fixed_point);
  }
  else if (is_propositional_variable_instantiation(t))
  {
    if (v==t)
    {
      if (line.linear_term_present)
      {
        line.c_j=data::sort_real::plus(line.c_j, data::sort_real::real_one());  // Add this inclination for v to all inclinations. 
      }
      else
      {
        line.linear_term_present=true;
        line.c_j=data::sort_real::real_one();  // Set this inclination for v 
      }
    }
    else
    {
      line.update_f_j(t, minimal_fixed_point);
    }
  }
  else if (is_const_multiply(t))
  {
    const const_multiply& tcm = atermpp::down_cast<const_multiply>(t);
    const propositional_variable_instantiation& w = atermpp::down_cast<propositional_variable_instantiation>(tcm.right());
    if (v==w)
    {
      if (line.linear_term_present)
      {
        line.c_j=data::sort_real::plus(line.c_j, tcm.left());  // Add this inclination for v to all inclinations. 
      }
      else
      {
        line.linear_term_present=true;
        line.c_j=tcm.left();  // Set this inclination for v 
      }
    }
    else 
    {
      line.update_f_j(t, minimal_fixed_point);
    }
  }
  else if (is_eqninf(t))
  {
    const eqninf& te = atermpp::down_cast<eqninf>(t);
    const propositional_variable_instantiation& w = atermpp::down_cast<propositional_variable_instantiation>(te.operand());
    if (v==w)
    {
      line.eqninf_term_present=true;
    }
    else
    {
      line.update_f_j(t, minimal_fixed_point);
    }
  }
  else if (data::is_data_expression(t))
  {
    line.update_f_j(t, minimal_fixed_point);
  }
  else 
  {
    throw runtime_error("Unexpected term in collect line. Expect plus, constant multiplication or a data expression: " + pp(t) + ". ");
  }
}

void collect_lines(std::vector< linear_fixed_point_equation >& found_lines, 
                   const propositional_variable& v, 
                   const pres_expression& t,
                   const bool minimal_fixed_point)
{
  if (is_and(t))
  {
    and_ ta = atermpp::down_cast<and_>(t);
    collect_lines(found_lines, v, ta.left(), minimal_fixed_point);
    collect_lines(found_lines, v, ta.right(), minimal_fixed_point);
  }
  else if (is_or(t))
  {
    or_ to = atermpp::down_cast<or_>(t);
    collect_lines(found_lines, v, to.left(), minimal_fixed_point);
    collect_lines(found_lines, v, to.right(), minimal_fixed_point);
  }
  else
  {
    found_lines.emplace_back();
    collect_line(found_lines.back(), v, t, minimal_fixed_point);
  }
}

void collect_m_j_and_split_lines(const std::vector< linear_fixed_point_equation >& lines, 
                                 pres_expression& m_j, 
                                 std::vector< linear_fixed_point_equation >& shallow_lines, 
                                 std::vector< linear_fixed_point_equation >& steep_lines,
                                 std::vector< linear_fixed_point_equation >& flat_lines,
                                 const data::rewriter& rewriter,
                                 const bool minimal_fixed_point)
{
  bool m_j_defined=false;
  for(const linear_fixed_point_equation& eq: lines)
  {
    if (eq.linear_term_present)
    {
      data::data_expression is_shallow=rewriter(data::less(eq.c_j,data::sort_real::real_one()));
      if (data::is_true(is_shallow))
      {
        shallow_lines.push_back(eq);
      }
      else if (data::is_false(is_shallow))
      {
        steep_lines.push_back(eq);
      }
      else
      {
        throw runtime_error("It is not possible to determine steepness of the line in a pres: " + pp(is_shallow) +", gradient is " + pp(eq.c_j) + ". ");
      }
    }
    else if (eq.eqninf_term_present)
    {
      flat_lines.push_back(eq);
    }
    else 
    {
      if (m_j_defined)
      {
        if (minimal_fixed_point)
        {
           m_j = or_(m_j, eq.f_j);
        }
        else
        {
           m_j = and_(m_j, eq.f_j);
        }
      }
      else
      {
        m_j_defined=true;
        m_j = eq.f_j;
      }
    }
  }
}

void conjunction_disjunction_f_j(pres_expression& result, 
                                 bool& result_defined,
                                 std::vector< linear_fixed_point_equation >& l,
                                 const bool is_conjunction)
{
  for(const linear_fixed_point_equation& eq: l)
  {
    if (result_defined)
    {
      if (is_conjunction)
      {
        result = and_(result, eq.f_j);
      }
      else
      {
        result = or_(result, eq.f_j);
      }
    }
    else 
    {
      result = eq.f_j;
      result_defined=true;
    }
  }
}

pres_expression conjunction_disjunction_f_j(std::vector< linear_fixed_point_equation >& l1,
                                            std::vector< linear_fixed_point_equation >& l2,
                                            std::vector< linear_fixed_point_equation >& l3,
                                            const bool is_conjunction)
{
  pres_expression result=(is_conjunction?true_():false_());
  bool result_defined=false;
  conjunction_disjunction_f_j(result, result_defined, l1, is_conjunction);
  conjunction_disjunction_f_j(result, result_defined, l2, is_conjunction);
  conjunction_disjunction_f_j(result, result_defined, l3, is_conjunction);
  return result;
}

pres_expression disjunction_infinity_cj_prime(std::vector< linear_fixed_point_equation >& l1,
                                              std::vector< linear_fixed_point_equation >& l2, 
                                              std::vector< linear_fixed_point_equation >& l3)
{
  for(const linear_fixed_point_equation& eq: l1)
  {
    if (eq.eqninf_term_present)
    {
      return true_();
    }
  }
  for(const linear_fixed_point_equation& eq: l2)
  {
    if (eq.eqninf_term_present)
    {
      return true_();
    }
  }
  for(const linear_fixed_point_equation& eq: l3)
  {
    if (eq.eqninf_term_present)
    {
      return true_();
    }
  }

  return false_();
}

pres_expression disjunction_cj_fj(std::vector< linear_fixed_point_equation >& l)
{
  pres_expression result=false_();
  bool result_defined=false;
  for(const linear_fixed_point_equation& eq: l)
  {
    data::data_expression constant = data::sort_real::divides(data::sort_real::real_one(),
                                                              data::sort_real::minus(data::sort_real::real_one(), eq.c_j));
    if (result_defined)
    {
      result = or_(result, const_multiply(constant, eq.f_j));
    }
    else 
    {
      result = const_multiply(constant, eq.f_j);
      result_defined=true;
    }
  }
  return result;
}

pres_expression conjunction_cj_fj(std::vector< linear_fixed_point_equation >& l)
{
  pres_expression result=false_();
  bool result_defined=false;
  for(const linear_fixed_point_equation& eq: l)
  {
    if (!eq.eqninf_term_present)
    {
      data::data_expression constant = data::sort_real::divides(data::sort_real::real_one(),
                                                                data::sort_real::minus(data::sort_real::real_one(), eq.c_j));
      if (result_defined)
      {
        result = or_(result, const_multiply(constant, eq.f_j));
      }
      else
      {
        result = const_multiply(constant, eq.f_j);
        result_defined=true;
      }
    }
  }
  return result;
}


pres_expression disjunction_fj_cj(std::vector< linear_fixed_point_equation >& l, const pres_expression& U)
{
  pres_expression result=false_();
  bool result_defined=false;
  for(const linear_fixed_point_equation& eq: l)
  {
    pres_expression disjunct = plus(eq.f_j, const_multiply(data::sort_real::minus(eq.c_j,data::sort_real::real_one()),U));
    if (result_defined)
    {
      result = or_(result, disjunct);
    }
    else
    {
      result = disjunct;
      result_defined=true;
    }
  }
  return result;
}

pres_expression conjunction_fj_cj(std::vector< linear_fixed_point_equation >& l, const pres_expression& U)
{
  pres_expression result=true_();
  bool result_defined=false;
  for(const linear_fixed_point_equation& eq: l)
  {
    if (!eq.eqninf_term_present)
    {
      pres_expression disjunct = plus(eq.f_j, const_multiply(data::sort_real::minus(eq.c_j,data::sort_real::real_one()),U));
      if (result_defined)
      {
        result = and_(result, disjunct);
      }
      else
      {
        result = disjunct;
        result_defined=true;
      }
    }
  }
  return result;
}

pres_expression solve_fixed_point_inner(const propositional_variable& v, 
                                        const pres_expression& t, 
                                        const data::rewriter& rewriter, 
                                        const bool minimal_fixed_point)
{
  std::vector< linear_fixed_point_equation > lines; // equations c_j X + c'_j*eqninf(X) + f_j  with 0<c_j<1
std::cerr << "MAKE SOLUTION " << v << " ---> " << t << "\n";
  /* Here is is assumed that t is a disjunction of terms */
  collect_lines(lines, v, t, minimal_fixed_point);
 
  pres_expression m = false_();;
  std::vector< linear_fixed_point_equation > shallow_lines;     // lines with gradient between 0 and 1. 
  std::vector< linear_fixed_point_equation > steep_lines;       // lines with gradient >=1.
  std::vector< linear_fixed_point_equation > flat_lines;        // lines with gradient 0. eqninf(v) term is present. 
  collect_m_j_and_split_lines(lines, m, shallow_lines, steep_lines, flat_lines, rewriter, minimal_fixed_point);

  /* XXXXXX FINISH SOLVING ****/

  if (minimal_fixed_point)
  {
    pres_expression U= or_(m, disjunction_cj_fj(shallow_lines));
    pres_expression cond1 = disjunction_fj_cj(steep_lines, U);
    pres_expression cond2 = disjunction_infinity_cj_prime(shallow_lines, steep_lines, flat_lines);

    pres_expression solution = condeq(eqinf(conjunction_disjunction_f_j(shallow_lines, steep_lines, flat_lines, false)),
                                      condeq(eqninf(m),
                                             false_(),
                                             condeq(or_(cond1,cond2),
                                                    U,
                                                    true_())),
                                      true_());
std::cerr << "MINIMAL SOLUTION " << solution << "\n";
    return solution;
  }
  else // Maximal fixed point
  {
    pres_expression U = and_(m, conjunction_cj_fj(shallow_lines));

    pres_expression cond1 = conjunction_fj_cj(steep_lines, U);

    pres_expression solution = condeq(eqinf(conjunction_disjunction_f_j(shallow_lines,steep_lines,flat_lines, true)),
                                      condsm(cond1,
                                             false_(),
                                             U),
                                      true_());
std::cerr << "MAXIMAL SOLUTION " << solution << "\n";
    return solution;
  }
}

const pres_expression solve_single_equation(const fixpoint_symbol& f, const propositional_variable& v, const pres_expression& t, const data::rewriter& rewriter)
{
  std::cerr << "SOLVE " << f << "   " << v << " = " << t << "\n";
  pres_expression aux;
  if (is_condsm(t) && f==pbes_system::fixpoint_symbol::mu())
  { 
    const condsm tc = atermpp::down_cast<condsm>(t);
    pres_expression solution_arg2 = solve_single_equation(f, v, tc.arg2(), rewriter);
    
    const bool conjunctive_normal_form = true;
    push_or_inside(aux, tc.arg2(), tc.arg3(), conjunctive_normal_form);

    pres_expression solution_arg2_or_arg3 = solve_single_equation(f, v, aux, rewriter);
    
    pres_expression new_condition;
    substitute_pres_equation_builder variable_substituter(v, solution_arg2);
    variable_substituter.apply(new_condition, tc.arg1());
    
    return condsm(new_condition, solution_arg2, solution_arg2_or_arg3);
  }
  else if (is_condsm(t) && f==pbes_system::fixpoint_symbol::nu())
  { 
    const condsm tc = atermpp::down_cast<condsm>(t);
    pres_expression solution_arg2 = solve_single_equation(f, v, tc.arg2(), rewriter);
    pres_expression solution_arg3 = solve_single_equation(f, v, tc.arg3(), rewriter);
    
    pres_expression new_condition;
    substitute_pres_equation_builder variable_substituter(v, or_(solution_arg2, solution_arg3));
    variable_substituter.apply(new_condition, tc.arg1());
    
    return condsm(new_condition, solution_arg2, solution_arg3);
  }
  else if (is_condeq(t) && f==pbes_system::fixpoint_symbol::mu())
  { 
    const condeq tc = atermpp::down_cast<condeq>(t);
    pres_expression solution_arg2 = solve_single_equation(f, v, tc.arg2(), rewriter);
    pres_expression solution_arg3 = solve_single_equation(f, v, tc.arg3(), rewriter);
    
    pres_expression new_condition;
    substitute_pres_equation_builder variable_substituter(v, and_(solution_arg2, solution_arg3));
    variable_substituter.apply(new_condition, tc.arg1());
    
    return condeq(new_condition, solution_arg2, solution_arg3);
  }
  if (is_condeq(t) && f==pbes_system::fixpoint_symbol::nu())
  { 
    const condeq tc = atermpp::down_cast<condeq>(t);
    pres_expression solution_arg2 = solve_single_equation(f, v, tc.arg2(), rewriter);
    
    const bool conjunctive_normal_form = false;
    push_and_inside(aux, tc.arg2(), tc.arg3(), conjunctive_normal_form);

    pres_expression solution_arg2_or_arg3 = solve_single_equation(f, v, aux, rewriter);
    
    pres_expression new_condition;
    substitute_pres_equation_builder variable_substituter(v, solution_arg2);
    variable_substituter.apply(new_condition, tc.arg1());
    
    return condeq(new_condition, solution_arg2, solution_arg2_or_arg3);
  }
  else if (is_and(t) && f==pbes_system::fixpoint_symbol::mu())
  {
    const and_ tc = atermpp::down_cast<and_>(t);
    pres_expression solution_left = solve_single_equation(f, v, tc.left(), rewriter);
    pres_expression solution_right = solve_single_equation(f, v, tc.right(), rewriter);
    
    return and_(solution_left, solution_right);
  }
  else if (is_or(t) && f==pbes_system::fixpoint_symbol::nu())
  {
    const or_ tc = atermpp::down_cast<or_>(t);
    pres_expression solution_left = solve_single_equation(f, v, tc.left(), rewriter);
    pres_expression solution_right = solve_single_equation(f, v, tc.right(), rewriter);
    
    return or_(solution_left, solution_right);
  }
  else if (f==pbes_system::fixpoint_symbol::mu())
  {
    // Solve minimal fixed point. 
    return solve_fixed_point_inner(v, t, rewriter, true);
  }
  else 
  {
    assert(f==pbes_system::fixpoint_symbol::nu());
    // Solve maximal fixed point. 
    return solve_fixed_point_inner(v, t, rewriter, false);
  }
}

} // namespace detail



class res_conjunctive_disjunctive_normal_form_builder: public pres_expression_builder <res_conjunctive_disjunctive_normal_form_builder>
{
protected:
  bool m_conjunctive_normal_form;
  bool m_negate=false;

public:
  typedef pres_expression_builder<res_conjunctive_disjunctive_normal_form_builder> super;
  using super::apply;

  res_conjunctive_disjunctive_normal_form_builder(const bool conjunctive)
   : m_conjunctive_normal_form(conjunctive)
  {}

  template <class T>
  void apply(T& result, const pres_system::propositional_variable_instantiation& x)
  {
    pres_system::make_propositional_variable_instantiation(result, x.name(), [&](data::data_expression_list& result){ apply(result, x.parameters()); });
    if (m_negate)
    {
      pres_system::make_minus(result, result);
    }
  }

  template <class T>
  void apply(T& result, const pres_system::and_& x)
  {
    pres_expression aux1;
    pres_expression aux2;
    apply(aux1, x.left());
    apply(aux2, x.right());
    detail::push_and_inside(result, aux1, aux2, m_conjunctive_normal_form);
std::cerr << "PUSH AND INSIDE " << result << "\n";
    
  }

  template <class T>
  void apply(T& result, const pres_system::or_& x)
  {
    pres_expression aux1;
    pres_expression aux2;
    apply(aux1, x.left());
    apply(aux2, x.right());
    detail::push_or_inside(result, aux1, aux2, m_conjunctive_normal_form);
  }

  template <class T>
  void apply(T& result, const pres_system::imp& x)
  {
    pres_expression aux;
    make_minus(aux, x.left());
    make_or_(aux, aux, x.right());
    apply(result,aux);
  }

  template <class T>
  void apply(T& result, const pres_system::plus& x)
  {
    pres_expression aux1, aux2;
    apply(aux1, x.left());
    apply(aux2, x.right());
    detail::push_plus_inside(result, aux1, aux2, m_conjunctive_normal_form);
  }

  template <class T>
  void apply(T& result, const pres_system::const_multiply& x)
  {
    pres_expression aux;
    apply(aux, x.right());
    const data::data_expression& constant=x.left();
    detail::push_constant_inside(result, constant, aux);
  }

  template <class T>
  void apply(T& result, const pres_system::const_multiply_alt& x)
  {
    pres_expression aux;
    apply(aux, x.left());
    const data::data_expression& constant=x.right();
    detail::push_constant_inside(result, constant, aux);
  }

  template <class T>
  void apply(T&, const pres_system::minall& x)
  {
    throw runtime_error("Cannot normalise and solve a res with a minall operator: " + pp(static_cast<pres_expression>(x)) + ".");
  }

  template <class T>
  void apply(T&, const pres_system::maxall& x)
  {
    throw runtime_error("Cannot normalise and solve a res with a maxall operator: " + pp(static_cast<pres_expression>(x)) + ".");
  }

  template <class T>
  void apply(T&, const pres_system::sum& x)
  {
    throw runtime_error("Cannot normalise and solve a res with a sum operator: " + pp(static_cast<pres_expression>(x)) + ".");
  }

  template <class T>
  void apply(T& result, const pres_system::eqinf& x)
  {
std::cerr << "TODO: " << pp(static_cast<pres_expression>(x)) << "\n";
    pres_system::make_eqinf(result, [&](pres_expression& result){ apply(result, x.operand()); });
  }

  template <class T>
  void apply(T& result, const pres_system::eqninf& x)
  {
std::cerr << "TODO: " << pp(static_cast<pres_expression>(x)) << "\n";
    pres_system::make_eqninf(result, [&](pres_expression& result){ apply(result, x.operand()); });
  }

/*  template <class T>
  void apply(T& result, const pres_system::condsm& x)
  {
std::cerr << "TODO: " << x << "\n";
    pres_system::make_condsm(result, [&](pres_expression& result){ apply(result, x.arg1()); }, [&](pres_expression& result){ apply(result, x.arg2()); }, [&](pres_expression& result){ apply(result, x.arg3()); });
  }

  template <class T>
  void apply(T& result, const pres_system::condeq& x)
  {
std::cerr << "TODO: " << x << "\n";
    pres_system::make_condeq(result, [&](pres_expression& result){ apply(result, x.arg1()); }, [&](pres_expression& result){ apply(result, x.arg2()); }, [&](pres_expression& result){ apply(result, x.arg3()); });
  } */
}; 

/// \brief An algorithm that takes a res, i.e. a pres with propositional variables without parameters
///        and solves it by Gauss elimination.

class ressolve_by_gauss_elimination_algorithm
{
  protected:
    pressolve_options m_options;
    data::rewriter m_datar;    // data_rewriter
    pres m_input_pres;
    enumerate_quantifiers_rewriter m_R;   // The rewriter.

    data::rewriter construct_rewriter(const pres& presspec)
    {
      if (m_options.remove_unused_rewrite_rules)
      {
        return data::rewriter(presspec.data(),
                              data::used_data_equation_selector(presspec.data(), pres_system::find_function_symbols(presspec), presspec.global_variables()),
                              m_options.rewrite_strategy);
      }
      else
      {
        return data::rewriter(presspec.data(), m_options.rewrite_strategy);
      }
    }

  public:
    ressolve_by_gauss_elimination_algorithm(
      const pressolve_options& options,
      const pres& input_pres
    ) 
     : m_options(options),
       m_datar(construct_rewriter(input_pres)),
       m_input_pres(input_pres),
       m_R(m_datar,input_pres.data())
    {}

    const pres_expression run()
    {
      std::vector<pres_equation> res_equations(m_input_pres.equations().begin(), m_input_pres.equations().end());
      assert(res_equations.size()>0);
      // Take care that the first equation has the initial variable at the lhs.
      if (res_equations.front().variable().name()!=m_input_pres.initial_state().name())
      {
        core::identifier_string new_name("initial_variable$$"); // Name should not be seen externally.
        res_equations.insert(res_equations.begin(),
                             pres_equation(res_equations.front().symbol(),
                                           propositional_variable(new_name, data::variable_list()),
                                           m_input_pres.initial_state()));
         m_input_pres.initial_state() = propositional_variable_instantiation(new_name, data::data_expression_list());
      }

      atermpp::indexed_set<core::identifier_string> variable_names;
      pres_expression result;
      res_conjunctive_disjunctive_normal_form_builder conjunctive_normal_form_builder(true);
      res_conjunctive_disjunctive_normal_form_builder disjunctive_normal_form_builder(false);
      for(pres_equation& e: res_equations)
      {
        variable_names.insert(e.variable().name());
      }

      for(std::vector<pres_equation>::reverse_iterator equation_it=res_equations.rbegin(); equation_it!=res_equations.rend(); equation_it++)
      {
std::cerr << "-------------------------------------------------------------------------------------------------\n";
std::cerr << "NORMALFORM IN " << *equation_it << "\n";
        if (equation_it->symbol().is_mu())
        {
          conjunctive_normal_form_builder.apply(result, equation_it->formula());
        }
        else
        {
          disjunctive_normal_form_builder.apply(result, equation_it->formula());
        }
std::cerr << "NORMALFORM " << result << "\n";

        pres_expression solution = detail::solve_single_equation(equation_it->symbol(),
                                                                 equation_it->variable(),
                                                                 result,
                                                                 m_datar);
        equation_it->formula() = solution;

        substitute_pres_equation_builder substitute_pres_equation(equation_it->variable(), solution);
       
        for(std::vector<pres_equation>::iterator substitution_equation_it=res_equations.begin();
                                                 substitution_equation_it!=equation_it.base(); 
                                                 substitution_equation_it++)
        {
          substitute_pres_equation.apply(result, substitution_equation_it->formula());
          substitution_equation_it->formula() = result;
        }
      }
      return res_equations.front().formula();
    } 
};

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_RESSOLVE_H
