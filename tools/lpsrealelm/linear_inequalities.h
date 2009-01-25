// Author(s): Jeroen Keiren and Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linear_inequalities.h
/// \brief Contains a class linear_inequality to represent mcrl2 data
///        expressions that are linear equalities, or inequalities.
///        Furthermore, it contains some operations on these linear
///        inequalities, such as Fourier-Motzkin elimination.


#ifndef MCRL2_LPSREALELM_LINEAR_INEQUALITY_H
#define MCRL2_LPSREALELM_LINEAR_INEQUALITY_H


#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/atermpp/map.h"

namespace mcrl2 {

namespace data {

// Functions below should be made available in the data library.
data_expression real_zero();
data_expression real_one();
data_expression real_minus_one();
data_expression lhs_(const data_expression e);
data_expression rhs_(const data_expression e);
data_expression divide(const data_expression e1,const data_expression e2);
data_expression multiply(const data_expression e1,const data_expression e2);
bool is_number(const data_expression e);
bool is_negative(const data_expression e);
bool is_positive(const data_expression e);
bool is_zero(const data_expression e);
// End of functions that ought to be defined elsewhere.

// prototype
class linear_inequality;
inline std::string string(const linear_inequality& l);
inline std::string pp_vector(const std::vector < linear_inequality > &inequalities);

class linear_inequality
{
  public: 
    enum comparison_t { equal, less, less_eq };
    typedef atermpp::map < mcrl2::data::data_variable, mcrl2::data::data_expression > lhs_t;
    // void set_factor_for_a_variable(const data_variable x,const data_expression e);

  private:
    // The right hand sides are put in a vector, via an index to protect the aterms.
    // The right hand sides should only contain expressions representing constant reals.

    // Using static here means that there is one copy of m_empty_spots_in_rhss
    // and m_rhss in the entire program, instead of per object (see Stroustrup).
    // This seems wrong to me.

    atermpp::vector < data_expression > m_rhs;
    // The left hand side contains mappings from data variables of sort real, to
    // constants of sort real. If x is mapped to value r, it means that this inequality
    // contains r*x as a subexpression at the left hand side.
    lhs_t m_lhs;
    comparison_t m_comparison;

    void parse_and_store_expression(
                      const data_expression e, 
                      const rewriter& r,
                      bool negate=false,
                      const data_expression factor=real_one())
    {
      // Debugging only
      if (is_minus(e))
      { parse_and_store_expression(lhs_(e),r,negate,factor);
        parse_and_store_expression(rhs_(e),r,!negate,factor);
      }
      if (is_negate(e))
      { parse_and_store_expression(lhs_(e),r,!negate,factor);
      }
      else if (is_plus(e))
      { parse_and_store_expression(lhs_(e),r,negate,factor);
        parse_and_store_expression(rhs_(e),r,negate,factor);
      }
      else if (is_multiplies(e))
      { data_expression lhs=lhs_(e),rhs=rhs_(e);
        if (is_number(lhs))
        { parse_and_store_expression(rhs,r,negate,multiplies(lhs,factor));
        }
        else if (is_number(rhs))
        { parse_and_store_expression(lhs,r,negate,multiplies(rhs,factor));
        }
        else throw mcrl2::runtime_error("Expect constant multiplies expression: " + pp(e));
      }
      else if (is_data_variable(e))
      { if (e.sort()=sort_expr::real())
        { 
          if(m_lhs.find(e) == m_lhs.end())
          { 
            set_factor_for_a_variable(e,(negate?r(data_expr::negate(factor)):r(factor)));
          }
          else
          {
            set_factor_for_a_variable(e,(negate?r(minus(m_lhs[e],factor)):r(plus(m_lhs[e],factor))));
          }
        }
        else
           throw mcrl2::runtime_error("Encountered a variable in a real expression which is not of sort real: " + pp(e));
      }
      else if (is_number(r(e)))
      { set_rhs(negate?r(plus(rhs(),e)): r(minus(rhs(),e)));
      }
      else throw mcrl2::runtime_error("Expect linear expression over reals: " + pp(e));
    }
  public:

    /// \brief Constructor yielding an inconsistent inequality.
    linear_inequality():m_rhs(1,real_zero()),m_lhs(),m_comparison(less)
    {}

    /// \brief Constructor that constructs a linear inequality out of a data expression.
    /// \details The data expression e is expected to have the form
    /// lhs op rhs where op is one of <=,<,==,>,>= and lhs and rhs
    /// satisfy the syntax t ::=  x | c*t | t*c | t+t | t-t | -t where x is
    /// a variable and c is a real constant.
    /// \param e Contains the expression to become a linear inequality.

    linear_inequality(const mcrl2::data::data_expression e,
                      const rewriter &r):m_rhs(1,real_zero()),m_lhs(),m_comparison(less)
    { bool negate(false);
      if (is_equal_to(e))
      { m_comparison=equal;
      }
      else if (is_less(e))
      { m_comparison=less;
      }
      else if (is_less_equal(e))
      { m_comparison=less_eq;
      }
      else if (is_greater(e))
      { m_comparison=less;
        negate=true;
      }
      else if (is_greater_equal(e))
      { m_comparison=less_eq;
        negate=true;
      }
      else throw mcrl2::runtime_error("Unexpected equality or inequality: " + pp(e)) ; 
      
      data_expression lhs=lhs_(e);
      data_expression rhs=rhs_(e);
  
      parse_and_store_expression(lhs,r,negate);
      parse_and_store_expression(rhs,r,!negate);
    }
 
    linear_inequality(const data_expression lhs,
                      const data_expression rhs,
                      const comparison_t cmp,
                      const rewriter &r):m_rhs(1,real_zero()),m_lhs(),m_comparison(cmp)
    { // std::cerr << "lhs " << pp(lhs) << "  rhs " << pp(rhs) << "\n";
      parse_and_store_expression(lhs,r);
      parse_and_store_expression(rhs,r,true);
      // std::cerr << "Result-: " << string(*this) << "\n";
    }


    ~linear_inequality()
    { 
    }

    lhs_t::const_iterator lhs_begin() const
    { return m_lhs.begin();
    }

    lhs_t::const_iterator lhs_end() const
    { return m_lhs.end();
    }

    lhs_t &lhs() 
    { return m_lhs;
    }

    data_expression rhs() const
    {
      return m_rhs.front();
    }

    void swap(linear_inequality &l)
    { 
      m_rhs.swap(l.m_rhs);
      m_lhs.swap(l.m_lhs);
      const comparison_t c(m_comparison);
      m_comparison=l.m_comparison;
      l.m_comparison=c;
    }

    void set_rhs(const data_expression e)
    { assert(is_number(e));
      m_rhs[0]=e;
    }
 
    void set_factor_for_a_variable(const data_variable x,const data_expression e)
    { assert(is_number(e));
      if (e==real_zero())
      { lhs_t::iterator i=m_lhs.find(x);
        if (i!=m_lhs.end())
        { m_lhs.erase(i);
        }
      }
      else m_lhs[x]=e;
    }

    data_expression get_factor_for_a_variable(const data_variable x) 
    {
      if (m_lhs.find(x) == m_lhs.end())
      { return real_zero();
      }
      return m_lhs[x];
    }

    comparison_t comparison() const
    { return m_comparison;
    }
    
    void set_comparison(comparison_t c)
    { m_comparison=c;
    }

    bool is_false() const
    { 
      return m_lhs.empty() && 
        ((m_comparison==less_eq)?is_negative(rhs()):
        ((m_comparison==equal)?!is_zero(rhs()):!is_positive(rhs())));
    } 

    bool is_true() const
    { return m_lhs.empty() && 
        ((m_comparison==less_eq)?!is_negative(rhs()):
        ((m_comparison==equal)?is_zero(rhs()):is_positive(rhs())));
    } 

    /// \brief Subtract the given equality, multiplied by f1/f2.
    /// 
    void subtract(const linear_inequality &e,
                  const rewriter &r)
    { 
      for(lhs_t::const_iterator i=e.lhs_begin();
              i!=e.lhs_end(); ++i)
      { set_factor_for_a_variable(i->first,r(minus(get_factor_for_a_variable(i->first),i->second)));
      }
      set_rhs(r(minus(rhs(),e.rhs())));
    }

    /// \brief Return this inequality as a typical pair of terms of the form <x1+c2 x2+...+cn xn, d> where c2,...,cn, d are real constants.
    /// \brief Subtract the given equality, multiplied by f1/f2.
    /// 
    void subtract(const linear_inequality &e,
                  const data_expression f1,
                  const data_expression f2, 
                  const rewriter &r)
    { data_expression f=r(mcrl2::data::divide(f1,f2));
      for(lhs_t::const_iterator i=e.lhs_begin();
              i!=e.lhs_end(); ++i)
      { set_factor_for_a_variable(i->first,r(minus(get_factor_for_a_variable(i->first),multiply(f,i->second))));
      }
      set_rhs(r(minus(rhs(),multiply(f,e.rhs()))));
    }

    /// \brief Return this inequality as a typical pair of terms of the form <x1+c2 x2+...+cn xn, d> where c2,...,cn, d are real constants.
    void typical_pair(
            data_expression &lhs_expression, 
            data_expression &rhs_expression, 
            const rewriter &r) const
    { 
      if (lhs_begin()==lhs_end())
      { lhs_expression=real_zero();
        rhs_expression=rhs();
        return;
      }
      
      data_expression factor=lhs_begin()->second;
      
      for(lhs_t::const_iterator i=lhs_begin(); i!=lhs_end(); ++i)
      { data_variable v=i->first;
        data_expression e=multiply(r(mcrl2::data::divide(i->second,factor)),
                                           data_expression(v));
        if (i==lhs_begin())
        { lhs_expression=e;
        }
        else
        { lhs_expression=plus(lhs_expression,e);
        }
      }

      rhs_expression=r(mcrl2::data::divide(rhs(),factor));
    }

    void divide(const data_expression e, const rewriter &r)
    { assert(is_number(e));
      assert(!is_zero(e));
      for(lhs_t::const_iterator i=m_lhs.begin();
              i!=m_lhs.end(); ++i)
      { // m_lhs.find(i->first) == i
        m_lhs[i->first]=r(mcrl2::data::divide(m_lhs[i->first],e));
      }
      set_rhs(r(mcrl2::data::divide(rhs(),e)));
    }   

    void invert(const rewriter &r)
    { 
      for(lhs_t::const_iterator i=m_lhs.begin();
              i!=m_lhs.end(); ++i)
      { 
        m_lhs[i->first]=r(negate(m_lhs[i->first]));
      }
      set_rhs(r(negate(rhs())));
      if (comparison()==less)
      { set_comparison(less_eq);
      }
      else if (comparison()==less_eq)
      { set_comparison(less);
      }

    }
 
    void add_variables(set < data_variable > & variable_set) const
    { 
      for(lhs_t::const_iterator i=m_lhs.begin(); i!=m_lhs.end(); ++i)
      { variable_set.insert(i->first);
      }
    }

};    

//static set < unsigned int > linear_inequality::m_empty_spots_in_rhss;
//static atermpp::vector < mcrl2::data::data_expression > linear_inequality::m_rhss;

std::string string(const linear_inequality &l)
{ std::string s;
  if (l.lhs_begin()==l.lhs_end())
  { s="0";
  }
  for( linear_inequality::lhs_t::const_iterator i=l.lhs_begin(); i!=l.lhs_end(); ++i)
  { s=s + (i==l.lhs_begin()?"":" + ") ;
    if (i->second==real_one())
    { s=s + pp(i->first);
    }
    else if (i->second==real_minus_one())
    { s=s + "-" + pp(i->first);
    }
    else 
    { s=s + pp(i->second) + "*" + pp(i->first);
    }
  }
  if (l.comparison()==linear_inequality::less)
  { s=s + " < ";
  }
  else if (l.comparison()==linear_inequality::less_eq)
  { s=s + " <= ";
  }
  else if (l.comparison()==linear_inequality::equal)
  { s=s + " == ";
  }
  else assert(0);
  s=s + pp(l.rhs());
  return s;
}


// Real zero and real one are an ad hoc solution. They should be provided by
// the data type library.

static data_expression init_real_zero(data_expression &real_zero)
{ real_zero=gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprC0()), gsMakeDataExprC1());
  ATprotect(reinterpret_cast<ATerm*>(&real_zero));
  return real_zero;
}

static data_expression init_real_one(data_expression &real_one)
{ real_one=gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprC1())), 
                                       gsMakeDataExprC1());
  ATprotect(reinterpret_cast<ATerm*>(&real_one));
  return real_one;
}

static data_expression init_real_minus_one(data_expression &real_minus_one)
{ real_minus_one=gsMakeDataExprNeg(gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprC1())), 
                              gsMakeDataExprC1()));
  ATprotect(reinterpret_cast<ATerm*>(&real_minus_one));
  return real_minus_one;
}

inline data_expression real_zero()
{ static data_expression real_zero=init_real_zero(real_zero);
  return real_zero;
}

inline data_expression real_one()
{ static data_expression real_one=init_real_one(real_one);
  return real_one;
}

inline data_expression real_minus_one()
{ static data_expression real_minus_one=init_real_minus_one(real_minus_one);
  return real_minus_one;
}

inline data_expression divide(const data_expression e1,const data_expression e2)
{ return gsMakeDataExprDivide(e1,e2);
}

inline data_expression multiply(const data_expression e1,const data_expression e2)
{ return gsMakeDataExprMult(e1,e2);
}


inline bool is_number(const data_expression e)
{ // TODO: Check that the number is closed.
  return core::detail::gsIsDataExprC0(e) ||
         core::detail::gsIsDataExprCDub(e) ||
         core::detail::gsIsDataExprC1(e) ||
         core::detail::gsIsDataExprCNat(e) ||
         core::detail::gsIsDataExprCNeg(e) ||
         core::detail::gsIsDataExprCInt(e) ||
         core::detail::gsIsDataExprCReal(e);
}

inline bool is_negative(const data_expression e)
{ // Assume data_expression is in normal form.
  assert(is_number(e));
  return is_negate(e) || gsIsDataExprCNeg(e) || (gsIsDataExprCReal(e) && is_negative(lhs_(e)));
}

inline bool is_positive(const data_expression e)
{ // Assume data_expression is in normal form.
  assert(is_number(e));
  // std::cerr << "Is positive internal " << lhs_(e) << "\n";
  
  return (e!=real_zero()) && (gsIsDataExprCInt(e) || (gsIsDataExprCReal(e) && is_positive(lhs_(e))));
}

inline bool is_zero(const data_expression e)
{ // Assume data_expression is in normal form.
  assert(is_number(e));
  assert(core::detail::gsIsDataExprCReal(e));
  return (e==real_zero());
}


/// \brief Retrieve the left hand side of a data expression
/// \param e A data expression
/// \pre e is a data application d(x,y) with two arguments
/// \ret x

inline data_expression lhs_(const data_expression e)
{
  assert(is_data_application(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() <= 2); // Allow application to be applied on unary functions!
  return *(arguments.begin());
}

/// \brief Retrieve the right hand side of a data expression
/// \param e A data expression
/// \pre e is a data application d(x,y) with two arguments
/// \ret y
  
inline data_expression rhs_(const data_expression e)
{
  assert(is_data_application(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 2);
  return *(++arguments.begin());
}

/// \brief Print the vector of inequalities to stderr in readable form.
inline std::string pp_vector(const std::vector < linear_inequality > &inequalities)
{ std::string s="[";
  for(std::vector < linear_inequality > ::const_iterator i=inequalities.begin();
                i!=inequalities.end(); ++i)
  {  s=s+ string(*i) + (i+1==inequalities.end()?"":", ");
  }
  s=s+ "]";
  return s;
}

bool is_inconsistent(
              const std::vector < linear_inequality > &inequalities,
              const rewriter& r);

/// \brief Eliminate variables from inequalities using Gauss elimination and
///        Fourier-Motzkin elimination.
/// \details Deliver a set of inequalities equivalent to exists variables.inequalities.
//           If the resulting list of inequalities is inconsistent, then [false] is 
//           returned. Furthermore, the list of resulting inequalities is minimal in
//           the sense that no individual inequality can be removed, without altering the
//           set of solutions of the inequalities.
/// \param inequalities A list of linear inequalities; the input can also contain linear equations.
/// \param variables The list of variables to be eliminated
/// \param r A rewriter
/// \pre inequalities has been normalized
/// \post All variables in variables have been eliminated, inequalities contains
///       the resulting system of normalized inequalities.

template < class Data_variable_iterator >
void fourier_motzkin(const std::vector < linear_inequality > &inequalities_in,
                     Data_variable_iterator variables_begin, 
                     Data_variable_iterator variables_end, 
                     std::vector < linear_inequality > &resulting_inequalities,
                     const rewriter& r)
{
  assert(resulting_inequalities.empty());
  if (core::gsDebug)
  { std::cerr << "Starting Fourier-Motzkin elimination on " + pp_vector(inequalities_in) + " on variables ";
    for(Data_variable_iterator i=variables_begin;
                i!=variables_end; ++i)
    { std::cerr << " " << pp(*i) ;
    }
    std::cerr << "\n";
  }

  std::vector < linear_inequality > inequalities;
  std::vector < linear_inequality > equalities;
  atermpp::vector < data_variable > vars=
             gauss_elimination (inequalities_in, 
                                equalities,      // Store all resulting equalities here.
                                inequalities,    // Store all resulting non equalities here.
                                variables_begin, 
                                variables_end, 
                                r);

  // At this stage, the variables that should be eliminated only occur in
  // inequalities. Group the inequalities into positive, 0, and negative
  // occurrences of each variable, and create a new system.
  for(atermpp::vector < data_variable >::const_iterator i = vars.begin(); i != vars.end(); ++i)
  {
    vector < linear_inequality > new_inequalities;
    // The vectors below contain references for efficiency.
    // It is important that "inequalities" is not touched while using the arrays below.
    vector < linear_inequality *> inequalities_with_positive_variable;  
    vector < linear_inequality *> inequalities_with_negative_variable;  // Idem.

    for(std::vector < linear_inequality >::iterator j = inequalities.begin();
                    j != inequalities.end(); ++j)
    { linear_inequality::lhs_t::const_iterator factor_it=(j->lhs()).find(*i);
      if (factor_it==j->lhs_end()) // variable *i does not occur in inequality *j.
      { new_inequalities.push_back(*j);
      }
      else 
      { data_expression f=factor_it->second;
        j->lhs().erase(*i);
        j->divide(f,r);
        if (is_positive(f))
        { inequalities_with_positive_variable.push_back(&(*j));
        }
        else if (is_negative(f))
        { inequalities_with_negative_variable.push_back(&(*j));
        }
        else assert(0);
      }
    }

    // std::cerr << "Positive :" << pp_vector(inequalities_with_positive_variable) << "\n";
    // std::cerr << "Negative :" << pp_vector(inequalities_with_negative_variable) << "\n";
    // std::cerr << "Equalities :" << pp_vector(equalities) << "\n";
    // std::cerr << "Rest :" << pp_vector(new_inequalities) << "\n";

    // Variables are grouped, now construct new inequalities as follows:
    // Keep the zero occurrences
    // Combine each positive and negative equation as follows:
    // Given inequalities x1 + bi * x <= ci
    //                   -x1 + bj * x <= cj
    // This is equivalent to bj * x + bi * x <= ci + cj
    for(std::vector < linear_inequality *>::iterator j = inequalities_with_positive_variable.begin(); 
                    j != inequalities_with_positive_variable.end(); ++j)
    { for(std::vector < linear_inequality *>::iterator k = inequalities_with_negative_variable.begin(); 
                    k != inequalities_with_negative_variable.end(); ++k)
      { linear_inequality e= *(*j);
        e.subtract(*(*k),r);
        e.set_comparison(((*j)->comparison()==linear_inequality::less_eq) && 
                         ((*k)->comparison()==linear_inequality::less_eq)?
                                linear_inequality::less_eq:
                                linear_inequality::less);
        if (e.is_false())
        { resulting_inequalities.push_back(linear_inequality()); // This is a single contraditory inequality;
          if (core::gsDebug)
          { std::cerr << "Fourier-Motzkin elimination yields " + pp_vector(resulting_inequalities) + "\n";
          }
          return;
        }
        if (!e.is_true())
        { new_inequalities.push_back(e);
        }
      }
    }
    inequalities.swap(new_inequalities);
  }
  
  resulting_inequalities.swap(inequalities);
  // Add the equalities to the inequalities and return the result
  for(std::vector < linear_inequality > :: const_iterator i=equalities.begin();
              i!=equalities.end(); ++i)
  { assert(!i->is_false());
    if (!i->is_true())
    { resulting_inequalities.push_back(*i);
    }
  }
  if (core::gsDebug)
  { std::cerr << "Fourier-Motzkin elimination yields " + pp_vector(resulting_inequalities) + "\n";
  }
}
 


/// \brief Remove every redundant inequality from a set of inequalities.
/// \details If inequalities is inconsistent, [false] is returned. Otherwise
///          a list of inequalities is returned, from which no inequality can
///          be removed without changing the set of solutions of the inequalities.
///          Redundancy of equalities is not checked, because this is quite expensive.
/// \param inequalities A list of inequalities
/// \param resulting_inequalities A list of inequalities to which the result is stored. 
//                                Initially this list must be empty.
/// \param r A rewriter

inline void remove_redundant_inequalities(
              const std::vector < linear_inequality > &inequalities, 
              std::vector < linear_inequality > &resulting_inequalities,
              const rewriter &r)
{
  assert(resulting_inequalities.empty());
  if (inequalities.empty())
  { return;
  }

  // If false is among the inequalities, [false] is the minimal result.
  if (is_inconsistent(inequalities,r))
  { resulting_inequalities.push_back(linear_inequality());
    return;
  }

  // std::cerr << "redundant in " << pp_vector(inequalities) << "\n";
  resulting_inequalities=inequalities;
  for(unsigned int i=0; i<resulting_inequalities.size(); )
  { // Check whether the inequalities, with the i-th equality with a reversed comparison operator is inconsistent.
    // If yes, the i-th inequality is redundant.
    if (resulting_inequalities[i].comparison()==linear_inequality::equal)
    { // Do nothing, as removing redundant inequalities is expensive.
      i++;
      /* resulting_inequalities[i].set_comparison(linear_inequality::less);
      if (is_inconsistent(resulting_inequalities,r))
      { 
        resulting_inequalities[i].invert(r);
        resulting_inequalities[i].set_comparison(linear_inequality::less);
        if (is_inconsistent(resulting_inequalities,r))
        { // So, t<c and t>c is inconsistent. So, t==c is redundant.
          if (i+1<resulting_inequalities.size())
          { // Copy the last element to the current position.
            resulting_inequalities[i].swap(resulting_inequalities.back());
          }
          resulting_inequalities.pop_back();
        }
        else 
        { 
          resulting_inequalities[i].invert(r);
          resulting_inequalities[i].set_comparison(linear_inequality::equal);
          ++i;
        } 
      }
      else
      { resulting_inequalities[i].set_comparison(linear_inequality::equal);
        ++i;
      } */
    }
    else 
    { 
      resulting_inequalities[i].invert(r);
      if (is_inconsistent(resulting_inequalities,r))
      { 
        if (i+1<resulting_inequalities.size())
        { // Copy the last element to the current position.
          resulting_inequalities[i].swap(resulting_inequalities.back());
        }
        resulting_inequalities.pop_back();
      }
      else 
      { 
        resulting_inequalities[i].invert(r);
        ++i;
      }
    }
  }
  // std::cerr << "redundant out " << pp_vector(resulting_inequalities) << "\n";
}

/// \brief Determine whether a list of data expressions is inconsistent
/// \details First it is checked whether false is among the input. If
///          not, Fourier-Motzkin is applied to all variables in the
///          inequalities. If the empty set of equalities is the result, 
///          the input was consistent. Otherwise the resulting set contains
///          an inconsistent inequality.
/// \param inequalities A list of inequalities
/// \param r A rewriter
/// \ret true if the system of inequalities can be determined to be
///      inconsistent, false otherwise.

inline bool is_inconsistent(
              const std::vector < linear_inequality > &inequalities,
              const rewriter& r)
{
  // If false is among the inequalities, [false] is the minimal result.
  for(std::vector < linear_inequality >::const_iterator i=inequalities.begin(); 
                i!=inequalities.end(); ++i)
  { if(i->is_false())
    { return true;
    }
  }
  
  atermpp::set<data_variable> dvs;
  for(std::vector < linear_inequality >::const_iterator i=inequalities.begin(); 
                i!=inequalities.end(); ++i)
  { i->add_variables(dvs);
  }

  std::vector < linear_inequality > resulting_inequalities;
  fourier_motzkin (inequalities,dvs.begin(),dvs.end(),resulting_inequalities,r);
#if NDEBUG
  return (!resulting_inequalities.empty());
#else
  if (resulting_inequalities.empty())
  { return false;
  }
  // Check if result contains false

  for(std::vector < linear_inequality >::const_iterator i=resulting_inequalities.begin(); 
                 i!=resulting_inequalities.end(); ++i)
  { if(i->is_false())
    { return true;
    }
    else 
    { assert(0); // All inequalities in resulting_inequalities must be false;
    }
  }
  return false;
#endif
}


/// \brief Try to eliminate variables from a system of inequalities using Gauss elimination.
/// \details For all variables yi in y1,...,yn indicated by variables_begin to variables_end, it
///          attempted to find and equation among inequalities of the form yi==expression. All
///          occurrences of yi in equalities are subsequently replaced by yi. If no equation of
///          the form yi can be found, yi is added to the list of variables that is returned by
///          this function. If the input contains an inconsistent inequality, resulting_equalities
///          becomes empty, resulting_inequalities contains false and the returned list of variables
///          is also empty. The resulting equalities and inequalities do not contain linear inequalites
///          equivalent to true.
/// \param inequalities A list of inequalities over real numbers
/// \param resulting_inequalities A list with the resulting equalities.
/// \param resulting_inequalities A list of the resulting inequalities
/// \param variables_begin An iterator indicating the beginning of the eliminatable variables.
/// \param variables_end An iterator indicating the end of the eliminatable variables.
/// \param r A rewriter.
/// \post variables contains the list of variables that have not been eliminated
/// \ret The variables that could not be removed by gauss elimination.

template < class Variable_iterator >
atermpp::vector < data_variable > gauss_elimination(
                         const std::vector < linear_inequality > &inequalities, 
                         std::vector < linear_inequality > &resulting_equalities,
                         std::vector < linear_inequality > &resulting_inequalities,
                         Variable_iterator variables_begin, 
                         Variable_iterator variables_end, 
                         const rewriter& r)
{
  // gsDebugMsg("Trying to eliminate variables %P from system %P using gauss elimination\n", (ATermList)variables, (ATermList)inequalities);

  atermpp::vector < data_variable > remaining_variables;

  // First copy equalities to the resulting_equalities and the inequalites to resulting_inequalities.
  for(vector < linear_inequality > ::const_iterator j = inequalities.begin(); j != inequalities.end(); ++j)
  { if (j->is_false())
    { // The input contains false. Return false and stop.
      resulting_equalities.clear();
      resulting_inequalities.clear();
      resulting_inequalities.push_back(linear_inequality());
      return remaining_variables;
    }
    else if (!j->is_true()) // Do not consider redundant equations.
    { if (j->comparison()==linear_inequality::equal)
      { resulting_equalities.push_back(*j);
      }
      else
      { resulting_inequalities.push_back(*j);
      }
    }
  }

  // Now find out whether there are variables that occur in an equality, so
  // that we can perform gauss elimination.
  data_variable_list eliminated_variables;
  for(Variable_iterator i = variables_begin; i != variables_end; ++i)
  { unsigned int j;
    for(j=0; j<resulting_equalities.size(); ++j)
    {
      bool check_equalities_for_redundant_inequalities(false);
      set < data_variable > vars;
      resulting_equalities[j].add_variables(vars);
      if (vars.count(*i)>0)
      {
        // Equality *j contains data variable *i.
        // Perform gauss elimination, and break the loop.

        for(unsigned int k = 0; k < resulting_inequalities.size(); )
        { resulting_inequalities[k].subtract(resulting_equalities[j],
                      resulting_inequalities[k].get_factor_for_a_variable(*i),
                      resulting_equalities[j].get_factor_for_a_variable(*i),
                      r);
          if (resulting_inequalities[k].is_false())
          { // The input is inconsistent. Return false.
            resulting_equalities.clear();
            resulting_inequalities.clear();
            resulting_inequalities.push_back(linear_inequality());
            remaining_variables.clear();
            return remaining_variables;
          }
          else if (resulting_inequalities[k].is_true())
          { // Inequality k has become redundant, and can be removed.
            if ((k+1)<resulting_inequalities.size())
            { resulting_inequalities[k].swap(resulting_inequalities.back());
            }
            resulting_inequalities.pop_back();
          }
          else ++k;
        }

        for(unsigned int k = 0; k<resulting_equalities.size(); ) 
        { if (k==j)
          { ++k;
          }
          else
          { resulting_equalities[k].subtract(
                                       resulting_equalities[j],
                                       resulting_equalities[k].get_factor_for_a_variable(*i),
                                       resulting_equalities[j].get_factor_for_a_variable(*i),
                                       r);
            if (resulting_equalities[k].is_false())
            { // The input is inconsistent. Return false.
              resulting_equalities.clear();
              resulting_inequalities.clear();
              resulting_inequalities.push_back(linear_inequality());
              remaining_variables.clear();
              return remaining_variables;
            }
            else if (resulting_equalities[k].is_true())
            { // Equality k has become redundant, and can be removed.
              if (j+1==resulting_equalities.size())
              { // It is not possible to move move the last element of resulting
                // inequalities to position k, because j is at this last position.
                // Hence, we must recall to check the resulting_equalities for inequalities
                // that are true.
                check_equalities_for_redundant_inequalities=true;
              }
              else 
              { if ((k+1)<resulting_equalities.size())
                { resulting_equalities[k].swap(resulting_equalities.back());
                }
                resulting_equalities.pop_back();
              }
            }
            else ++k;
          }
        }

        // Remove equation j.

        if (j+1<resulting_equalities.size())
        { resulting_equalities[j].swap(resulting_equalities.back());
        }
        resulting_equalities.pop_back();

        // If there are unremoved resulting equalities, remove them now.
        if (check_equalities_for_redundant_inequalities)
        { for(unsigned int k = 0; k<resulting_equalities.size(); )
          { if (resulting_equalities[k].is_true())
            { // Equality k is redundant, and can be removed.
              if ((k+1)<resulting_equalities.size())
              { resulting_equalities[k].swap(resulting_equalities.back());
              }
              resulting_equalities.pop_back();
            }
            else ++k;
          }
        }
      }
    }
    remaining_variables.push_back(*i);
  }

  // gsDebugMsg("Gauss elimination eliminated variables %P, resulting in the system %P\n", 
  //                       (ATermList)eliminated_variables, (ATermList)inequalities);

  return remaining_variables;
}
  

} // namespace data

} // namespace mcrl2

#endif // MCRL2_LPSREALELM_LINEAR_INEQUALITY_H
