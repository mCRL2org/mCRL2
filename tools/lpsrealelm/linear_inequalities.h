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
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/fresh_variable_generator.h"
#include <algorithm>

namespace mcrl2 {

namespace data {

// Functions below should be made available in the data library.
data_expression real_zero();
data_expression real_one();
data_expression real_minus_one();
// data_expression lhs_(const data_expression e);
// data_expression rhs_(const data_expression e);
// data_expression divide(const data_expression e1,const data_expression e2);
// data_expression multiply(const data_expression e1,const data_expression e2);
data_expression min(const data_expression e1,const data_expression e2,const rewriter &);
data_expression max(const data_expression e1,const data_expression e2,const rewriter &);
bool is_closed_real_number(const data_expression e);
bool is_negative(const data_expression e,const rewriter &r);
bool is_positive(const data_expression e,const rewriter &r);
bool is_zero(const data_expression e);
// End of functions that ought to be defined elsewhere.


inline data_expression rewrite_with_memory(
                      const data_expression t,const rewriter &r);

// prototype
class linear_inequality;
inline std::string string(const linear_inequality& l);
inline std::string pp_vector(const std::vector < linear_inequality > &inequalities);

class linear_inequality
{
  public:
    enum comparison_t { equal, less, less_eq };

    class lhs_t:public atermpp::map < variable, data_expression >
    { private:
        // TODO: the meta operations below insert variables with constant 0, which should
        // be avoided to keep linear equations clean. Note that with the binary operation
        // care must be taken to also consider the variables in this that do not occur in e.
        template < application Operation(const data_expression &, const data_expression &) >
        lhs_t &meta_operation_constant(const data_expression v, const rewriter &r)
        { for(lhs_t::iterator i=begin();i!=end();++i)
          { i->second=rewrite_with_memory(Operation(v,i->second),r);
          }
          return *this;
        }

        // Template method to add or subtract lhs_t's
        template < application Operation(const data_expression &, const data_expression &) >
        lhs_t &meta_operation_lhs(const lhs_t &e, const rewriter &r)
        { for(lhs_t::const_iterator i=e.begin();i!=e.end();++i)
          { if (count(i->first)==0)
            { (*this)[i->first]=rewrite_with_memory(Operation(real_zero(),i->second),r);
            }
            else
            { (*this)[i->first]=rewrite_with_memory(Operation((*this)[i->first],i->second),r);
            }
          }
          return *this;
        }

      public:
        lhs_t &add(const data_expression v, const rewriter&r)
        { return meta_operation_constant<sort_real::plus>(v,r);
        }

        lhs_t &subtract(const data_expression v, const rewriter&r)
        { return meta_operation_constant<sort_real::minus>(v,r);
        }

        lhs_t &multiply(const data_expression v, const rewriter&r)
        { return meta_operation_constant<sort_real::times>(v,r);
        }

        lhs_t &divide(const data_expression v, const rewriter&r)
        { return meta_operation_constant<sort_real::divides>(v,r);
        }

        lhs_t &add(const lhs_t &e, const rewriter&r)
        { return meta_operation_lhs<sort_real::plus>(e,r);
        }

        lhs_t &subtract(const lhs_t &e, const rewriter&r)
        { return meta_operation_lhs<sort_real::minus>(e,r);
        }

        template <typename SubstitutionFunction>
        data_expression evaluate(SubstitutionFunction &beta,const rewriter &r) const
        { data_expression result=real_zero();
          for(const_iterator i=begin(); i!=end(); ++i)
          { assert(beta.count(i->first)>0);
            const data_expression d=i->first;
            result=sort_real::plus(result,sort_real::times(d,i->second));
          }
          return r(result,make_map_substitution_adapter(beta));
        }

        std::string string() const
        { std::string s;
          if (begin()==end())
          { s="0";
          }
          for( linear_inequality::lhs_t::const_iterator i=begin(); i!=end(); ++i)
          { s=s + (i==begin()?"":" + ") ;
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
          return s;
        }
      };


    // void set_factor_for_a_variable(const variable x,const data_expression e);

  private:
    // The right hand sides should only contain expressions representing constant reals.

    data_expression m_rhs;
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
      // std::cerr << "EXPRESSION " << pp(e) <<  "    --    " << e << "\n";
      if (sort_real::is_minus_application(e) && application(e).arguments().size()==2)
      { // std::cerr << "is_minus_application\n";
        parse_and_store_expression(application(e).left(),r,negate,factor);
        parse_and_store_expression(application(e).right(),r,!negate,factor);
      }
      else if (sort_real::is_negate_application(e) && application(e).arguments().size()==1)
      { // std::cerr << "is_negate_application\n";
        parse_and_store_expression(*(application(e).arguments().begin()),r,!negate,factor);
      }
      else if (sort_real::is_plus_application(e))
      { parse_and_store_expression(application(e).left(),r,negate,factor);
        parse_and_store_expression(application(e).right(),r,negate,factor);
      }
      else if (sort_real::is_times_application(e))
      { data_expression lhs=rewrite_with_memory(application(e).left(),r), rhs=rewrite_with_memory(application(e).right(),r);
        if (is_closed_real_number(lhs))
        { parse_and_store_expression(rhs,r,negate,sort_real::times(lhs,factor));
        }
        else if (is_closed_real_number(rhs))
        { parse_and_store_expression(lhs,r,negate,sort_real::times(rhs,factor));
        }
        else throw mcrl2::runtime_error("Expect constant multiplies expression: " + pp(e) + "\n");
      }
      else if (e.is_variable())
      { if (e.sort() == sort_real::real_())
        {
          if(m_lhs.find(e) == m_lhs.end())
          {
            set_factor_for_a_variable(e,(negate?rewrite_with_memory(sort_real::negate(factor),r)
                                               :rewrite_with_memory(factor,r)));
          }
          else
          {
            set_factor_for_a_variable(e,(negate?rewrite_with_memory(sort_real::minus(m_lhs[e],factor),r)
                                               :rewrite_with_memory(sort_real::plus(m_lhs[e],factor),r)));
          }
        }
        else
           throw mcrl2::runtime_error("Encountered a variable in a real expression which is not of sort real: " + pp(e) + "\n");
      }
      else if (is_closed_real_number(rewrite_with_memory(e,r)))
      { set_rhs(negate?rewrite_with_memory(sort_real::plus(rhs(),e),r)
                      :rewrite_with_memory(sort_real::minus(rhs(),e),r));
      }
      else throw mcrl2::runtime_error("Expect linear expression over reals: " + pp(e) + "\n");
    }
  public:

    /// \brief Constructor yielding an inconsistent inequality.
    linear_inequality():m_lhs(),m_comparison(less)
    { m_rhs.protect();
      m_rhs=real_zero();
    }

    linear_inequality(const linear_inequality &l)
    { m_rhs.protect();
      m_rhs=l.m_rhs;
      m_lhs=l.m_lhs;
      m_comparison=l.m_comparison;
    }

    ~linear_inequality()
    { m_rhs.unprotect();
    }

    linear_inequality &operator=(const linear_inequality &l)
    { m_rhs=l.m_rhs;
      m_lhs=l.m_lhs;
      m_comparison=l.m_comparison;
      return *this;
    }

    /// \brief Constructor that constructs a linear inequality out of a data expression.
    /// \details The data expression e is expected to have the form
    /// lhs op rhs where op is one of <=,<,==,>,>= and lhs and rhs
    /// satisfy the syntax t ::=  x | c*t | t*c | t+t | t-t | -t where x is
    /// a variable and c is a real constant.
    /// \param e Contains the expression to become a linear inequality.

    linear_inequality(const data_expression e,
                      const rewriter &r)
                     :m_lhs(),m_comparison(less)
    {
      m_rhs.protect();
      m_rhs=real_zero();


      bool negate(false);
      if (is_equal_to_application(e))
      { m_comparison=equal;
      }
      else if (is_less_application(e))
      { m_comparison=less;
      }
      else if (is_less_equal_application(e))
      { m_comparison=less_eq;
      }
      else if (is_greater_application(e))
      { m_comparison=less;
        negate=true;
      }
      else if (is_greater_equal_application(e))
      { m_comparison=less_eq;
        negate=true;
      }
      else throw mcrl2::runtime_error("Unexpected equality or inequality: " + pp(e) + "\n") ;

      data_expression lhs=application(e).left();
      data_expression rhs=application(e).right();

      parse_and_store_expression(lhs,r,negate);
      parse_and_store_expression(rhs,r,!negate);
    }

    linear_inequality(const data_expression lhs,
                      const data_expression rhs,
                      const comparison_t cmp,
                      const rewriter &r):m_lhs(),m_comparison(cmp)
    { // std::cerr << "lhs " << pp(lhs) << "  rhs " << pp(rhs) << "\n";
      m_rhs.protect();
      m_rhs=real_zero();

      parse_and_store_expression(lhs,r);
      parse_and_store_expression(rhs,r,true);
      // std::cerr << "Result-: " << string(*this) << "\n";
    }

    lhs_t::const_iterator lhs_begin() const
    { return m_lhs.begin();
    }

    lhs_t::const_iterator lhs_end() const
    { return m_lhs.end();
    }

    std::string lhs_string() const
    { return m_lhs.string();
    }

    lhs_t &lhs()
    { return m_lhs;
    }

    size_t lhs_size() const
    { return m_lhs.size();
    }

    data_expression rhs() const
    {
      return m_rhs;
    }

    void swap(linear_inequality &l)
    {
      m_rhs=l.m_rhs;
      m_lhs.swap(l.m_lhs);
      const comparison_t c(m_comparison);
      m_comparison=l.m_comparison;
      l.m_comparison=c;
    }

    void set_rhs(const data_expression e)
    { assert(is_closed_real_number(e));
      m_rhs=e;
    }

    void set_factor_for_a_variable(const variable x,const data_expression e)
    { assert(is_closed_real_number(e));
      if (e==real_zero())
      { lhs_t::iterator i=m_lhs.find(x);
        if (i!=m_lhs.end())
        { m_lhs.erase(i);
        }
      }
      else m_lhs[x]=e;
    }

    data_expression get_factor_for_a_variable(const variable x)
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

    bool is_false(const rewriter &r) const
    {
      return m_lhs.empty() &&
        ((m_comparison==less_eq)?is_negative(rhs(),r):
        ((m_comparison==equal)?!is_zero(rhs()):!is_positive(rhs(),r)));
    }

    bool is_true(const rewriter &r) const
    { return m_lhs.empty() &&
        ((m_comparison==less_eq)?!is_negative(rhs(),r):
        ((m_comparison==equal)?is_zero(rhs()):is_positive(rhs(),r)));
    }



    /// \brief Subtract the given equality, multiplied by f1/f2.
    ///
    void subtract(const linear_inequality &e,
                  const rewriter &r)
    {
      for(lhs_t::const_iterator i=e.lhs_begin();
              i!=e.lhs_end(); ++i)
      {
        set_factor_for_a_variable(i->first,
             rewrite_with_memory(sort_real::minus(get_factor_for_a_variable(i->first),i->second),r));
      }
      set_rhs(rewrite_with_memory(sort_real::minus(rhs(),e.rhs()),r));
    }

    /// \brief Return this inequality as a typical pair of terms of the form <x1+c2 x2+...+cn xn, d> where c2,...,cn, d are real constants.
    /// \brief Subtract the given equality, multiplied by f1/f2.
    ///
    void subtract(const linear_inequality &e,
                  const data_expression f1,
                  const data_expression f2,
                  const rewriter &r)
    { data_expression f=rewrite_with_memory(sort_real::divides(f1,f2),r);
      for(lhs_t::const_iterator i=e.lhs_begin();
              i!=e.lhs_end(); ++i)
      { set_factor_for_a_variable(
                  i->first,
                  rewrite_with_memory(
                     sort_real::minus(get_factor_for_a_variable(i->first),sort_real::times(f,i->second)),r));
      }
      set_rhs(rewrite_with_memory(sort_real::minus(rhs(),sort_real::times(f,e.rhs())),r));
    }

    /// \brief Return this inequality as a typical pair of terms of the form <x1+c2 x2+...+cn xn, d> where c2,...,cn, d are real constants.
    /// \return The return value indicates whether the left and right hand side have been negated
    ///         when yielding the critical pair. 
    bool typical_pair(
            data_expression &lhs_expression,
            data_expression &rhs_expression,
            const rewriter &r) const
    { 
      if (lhs_begin()==lhs_end())
      { lhs_expression=real_zero();
        rhs_expression=rhs();
        return false;
      }

      data_expression factor=lhs_begin()->second;

      for(lhs_t::const_iterator i=lhs_begin(); i!=lhs_end(); ++i)
      { variable v=i->first;
        data_expression e=sort_real::times(rewrite_with_memory(sort_real::divides(i->second,factor),r),
                                           data_expression(v));
        if (i==lhs_begin())
        { lhs_expression=e;
        }
        else
        { lhs_expression=sort_real::plus(lhs_expression,e);
        }
      }

      rhs_expression=rewrite_with_memory(sort_real::divides(rhs(),factor),r);
      return is_negative(factor,r);
    }

    void divide(const data_expression e, const rewriter &r)
    {
      assert(is_closed_real_number(e));
      assert(!is_zero(e));
      for(lhs_t::const_iterator i=m_lhs.begin();
              i!=m_lhs.end(); ++i)
      { // m_lhs.find(i->first) == i
        m_lhs[i->first]=rewrite_with_memory(sort_real::divides(m_lhs[i->first],e),r);
      }
      set_rhs(rewrite_with_memory(sort_real::divides(rhs(),e),r));
    }

    void invert(const rewriter &r)
    {
      for(lhs_t::const_iterator i=m_lhs.begin();
              i!=m_lhs.end(); ++i)
      {
        m_lhs[i->first]=rewrite_with_memory(sort_real::negate(m_lhs[i->first]),r);
      }
      set_rhs(rewrite_with_memory(sort_real::negate(rhs()),r));
      if (comparison()==less)
      { set_comparison(less_eq);
      }
      else if (comparison()==less_eq)
      { set_comparison(less);
      }

    }

    void add_variables(std::set < variable > & variable_set) const
    {
      for(lhs_t::const_iterator i=m_lhs.begin(); i!=m_lhs.end(); ++i)
      { variable_set.insert(i->first);
      }
    }
};


//static set < unsigned int > linear_inequality::m_empty_spots_in_rhss;
//static atermpp::vector < mcrl2::data::data_expression > linear_inequality::m_rhss;

std::string string(const linear_inequality &l)
{ std::string s=l.lhs_string();
  /* if (l.lhs_begin()==l.lhs_end())
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
  } */

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
{ real_zero=sort_real::real_("0");
  real_zero.protect();
  // ATprotect(reinterpret_cast<ATerm*>(&real_zero));
  return real_zero;
}

static data_expression init_real_one(data_expression &real_one)
{ real_one=sort_real::real_("1");
  real_one.protect();
  // ATprotect(reinterpret_cast<ATerm*>(&real_one));
  return real_one;
}

static data_expression init_real_minus_one(data_expression &real_minus_one)
{ real_minus_one=sort_real::real_("-1");
  real_minus_one.protect();
  // ATprotect(reinterpret_cast<ATerm*>(&real_minus_one));
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

/* inline data_expression divide(const data_expression e1,const data_expression e2)
{ return mcrl2::data::sort_real::divides(e1,e2);
}

inline data_expression multiply(const data_expression e1,const data_expression e2)
{ return mcrl2::data::sort_real::times(e1,e2);
} */

inline data_expression min(const data_expression e1,const data_expression e2,const rewriter &r)
{ if (rewrite_with_memory(less_equal(e1,e2),r)==sort_bool::true_())
  { return e1;
  }
  if (rewrite_with_memory(less_equal(e2,e1),r)==sort_bool::true_())
  { return e2;
  }
  throw mcrl2::runtime_error("Fail to determine the minimum of: " + pp(e1) + " and " + pp(e2) + "\n" );
}

inline data_expression max(const data_expression e1,const data_expression e2,const rewriter &r)
{ if (rewrite_with_memory(less_equal(e2,e1),r)==sort_bool::true_())
  { return e1;
  }
  if (rewrite_with_memory(less_equal(e1,e2),r)==sort_bool::true_())
  { return e2;
  }
  throw mcrl2::runtime_error("Fail to determine the maximum of: " + pp(e1) + " and " + pp(e2) + "\n" );
}


inline bool is_closed_real_number(const data_expression e)
{ // TODO: Check that the number is closed.
  if (e.sort()!=sort_real::real_())
  { return false;
  }

  std::set < variable > s=find_variables(e);
  if (!s.empty())  // The expression e contains variables.
  { return false;
  }
  return true;
}

inline bool is_negative(const data_expression e,const rewriter &r)
{
  data_expression result=rewrite_with_memory(less(e,real_zero()),r);
  if (result==sort_bool::true_())
  { return true;
  }
  if (result==sort_bool::false_())
  { return false;
  }
  throw mcrl2::runtime_error("Cannot determine that " + pp(e) + " is smaller than 0");
}

inline bool is_positive(const data_expression e,const rewriter &r)
{
  data_expression result=rewrite_with_memory(greater(e,real_zero()),r);
  if (result==sort_bool::true_())
  { return true;
  }
  if (result==sort_bool::false_())
  { return false;
  }
  throw mcrl2::runtime_error("Cannot determine that " + pp(e) + " is larger than or equal to 0");
}

inline bool is_zero(const data_expression e)
{ // Assume data_expression is in normal form.
  assert(is_closed_real_number(e));
  return (e==real_zero());
}


/// \brief Retrieve the left hand side of a data expression
/// \param e A data expression
/// \pre e is a data application d(x,y) with two arguments
/// \ret x

/* inline data_expression e.left()nst data_expression e)
{
  assert(is_data_application(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() <= 2); // Allow application to be applied on unary functions!
  return *(arguments.begin());
} */

/// \brief Retrieve the right hand side of a data expression
/// \param e A data expression
/// \pre e is a data application d(x,y) with two arguments
/// \ret y

/* inline data_expression rhs_(const data_expression e)
{
  assert(is_data_application(e));
  data_expression_list arguments = static_cast<const data_application&>(e).arguments();
  assert(arguments.size() == 2);
  return *(++arguments.begin());
} */

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


// Count the occurrences of variables that occur in inequalities.
static void count_occurrences(
                 const std::vector < linear_inequality > &inequalities,
                 std::map < variable, unsigned int> &nr_positive_occurrences,
                 std::map < variable, unsigned int> &nr_negative_occurrences,
                 const rewriter &r)
{
  for(std::vector < linear_inequality >::const_iterator i=inequalities.begin();
           i!=inequalities.end(); ++i)
  { for(linear_inequality::lhs_t::const_iterator j=i->lhs_begin(); j!=i->lhs_end(); ++j)
    { if (is_positive(j->second,r))
      { nr_positive_occurrences[j->first]=nr_positive_occurrences[j->first]+1;
      }
      else
      { nr_negative_occurrences[j->first]=nr_negative_occurrences[j->first]+1;
      }
    }
  }
}

template < class Variable_iterator >
atermpp::set < variable >  gauss_elimination(
                         const std::vector < linear_inequality > &inequalities,
                         std::vector < linear_inequality > &resulting_equalities,
                         std::vector < linear_inequality > &resulting_inequalities,
                         Variable_iterator variables_begin,
                         Variable_iterator variables_end,
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
  atermpp::set < variable > vars=
             gauss_elimination (inequalities_in,
                                equalities,      // Store all resulting equalities here.
                                inequalities,    // Store all resulting non equalities here.
                                variables_begin,
                                variables_end,
                                r);

  // std::cerr << "Fourier-Motzkin after Gauss elimination elimination on " + pp_vector(equalities) + "\n Inequalities " +
    //          pp_vector(inequalities) + "\n";
  // At this stage, the variables that should be eliminated only occur in
  // inequalities. Group the inequalities into positive, 0, and negative
  // occurrences of each variable, and create a new system.
  for(atermpp::set < variable >::const_iterator i = vars.begin(); i != vars.end(); ++i)
  {
    std::map < variable, unsigned int> nr_positive_occurrences;
    std::map < variable, unsigned int> nr_negative_occurrences;
    count_occurrences(inequalities,nr_positive_occurrences,nr_negative_occurrences,r);

    bool found=false;
    unsigned int best_choice=0;
    variable best_variable;
    for(atermpp::set < variable >::const_iterator k = vars.begin(); k != vars.end(); ++k)
    { const unsigned int p=nr_positive_occurrences[*k];
      const unsigned int n=nr_negative_occurrences[*k];
      if ((p!=0) || (n!=0))
      { if (found)
        { if (n*p<best_choice)
          { best_choice=n*p;
            best_variable=*k;
          }
        }
        else
        { // found is false
          best_choice=n*p;
          best_variable=*k;
          found=true;
        }
      }
      if (found && (best_choice==0))
      { // Stop searching, we cannot find a better candidate.
        break;
      }
    }

    // std::cerr << "Best variable " << pp(best_variable) << "\n";

    if (!found)
    { // There are no variables anymore that can be removed from inequalities
      break;
    }
    std::vector < linear_inequality > new_inequalities;
    // The vectors below contain references for efficiency.
    // It is important that "inequalities" is not touched while using the arrays below.
    std::vector < linear_inequality *> inequalities_with_positive_variable;
    std::vector < linear_inequality *> inequalities_with_negative_variable;  // Idem.

    for(std::vector < linear_inequality >::iterator j = inequalities.begin();
                    j != inequalities.end(); ++j)
    { linear_inequality::lhs_t::const_iterator factor_it=(j->lhs()).find(best_variable);
      if (factor_it==j->lhs_end()) // variable best_variable does not occur in inequality *j.
      { new_inequalities.push_back(*j);
      }
      else
      { data_expression f=factor_it->second;
        j->lhs().erase(best_variable);
        j->divide(f,r);
        if (is_positive(f,r))
        { inequalities_with_positive_variable.push_back(&(*j));
        }
        else if (is_negative(f,r))
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
        if (e.is_false(r))
        { resulting_inequalities.push_back(linear_inequality()); // This is a single contraditory inequality;
          if (core::gsDebug)
          { std::cerr << "Fourier-Motzkin elimination yields " + pp_vector(resulting_inequalities) + "\n";
          }
          return;
        }
        if (!e.is_true(r))
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
  { assert(!i->is_false(r));
    if (!i->is_true(r))
    { resulting_inequalities.push_back(*i);
    }
  }
  if (core::gsDebug)
  { std::cerr << "Fourier-Motzkin elimination yields " + pp_vector(resulting_inequalities) + "\n";
  }
}



/// \brief Indicate whether an inequality from a set of inequalities is redundant.
/// \details Return whether the inequality referred to by i is inconsistent.
///          It is expected that i refers to an equality in the vector inequalities.
///          The vector inequalities might be changed within the procedure, but
///          will be restored to its original value when this function terminates.
/// \param inequalities A list of inequalities
/// \param resulting_inequalities A list of inequalities to which the result is stored.
///                               Initially this list must be empty.
/// \param r A rewriter
/// \ret An indication whether the inequality referred to by i is inconsistent
///      in the context of inequalities.
inline bool is_a_redundant_inequality(
              const std::vector < linear_inequality > &inequalities,
              const std::vector < linear_inequality > :: iterator i,
              const rewriter &r)
{
#ifndef NDEBUG
  // Check that i points to some position in inequalities.
  bool found=false;
  for(std::vector < linear_inequality >:: const_iterator j=inequalities.begin() ;
           j!=inequalities.end() ; ++j)
  { if (j==i)
    { found=true;
      break;
    }
  }
  assert(found);
#endif
  // Check whether the inequalities, with the i-th equality with a reversed comparison operator is inconsistent.
  // If yes, the i-th inequality is redundant.
  if (i->comparison()==linear_inequality::equal)
  { // An inequality t==u is only redundant for equalities if
    // t<u and t>u are both inconsistent
    i->set_comparison(linear_inequality::less);
    if (is_inconsistent(inequalities,r))
    { i->invert(r);
      if (is_inconsistent(inequalities,r))
      { i->set_comparison(linear_inequality::equal);
        return true;
      }
    }
    i->set_comparison(linear_inequality::equal);
    return false;
  }
  else
  { // an inequality t<u, t<=u, t>u and t>=u is redundant in equalities
    // if its inversion is inconsistent.
    i->invert(r);
    if (is_inconsistent(inequalities,r))
    { i->invert(r);
      return true;
    }
    else
    { i->invert(r);
      return false;
    }
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

  resulting_inequalities=inequalities;
  for(unsigned int i=0; i<resulting_inequalities.size(); )
  { // Check whether the inequalities, with the i-th equality with a reversed comparison operator is inconsistent.
    // If yes, the i-th inequality is redundant.
    if (resulting_inequalities[i].comparison()==linear_inequality::equal)
    { // Do nothing, as removing redundant inequalities is expensive.
      i++;
    }
    else
    {
      // resulting_inequalities[i].invert(r);
      // if (is_inconsistent(resulting_inequalities,r))
      if (is_a_redundant_inequality(resulting_inequalities,
                                    resulting_inequalities.begin()+i,
                                    r))
      {
        if (i+1<resulting_inequalities.size())
        { // Copy the last element to the current position.
          resulting_inequalities[i].swap(resulting_inequalities.back());
        }
        resulting_inequalities.pop_back();
      }
      else
      {
        ++i;
      }
    }
  }
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

/* inline bool is_inconsistent(
              const std::vector < linear_inequality > &inequalities,
              const rewriter& r)
{
  // If false is among the inequalities, [false] is the minimal result.
  for(std::vector < linear_inequality >::const_iterator i=inequalities.begin();
                i!=inequalities.end(); ++i)
  { if (i->is_false(r))
    { return true;
    }
  }

  atermpp::set<variable> dvs;
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
  { if(i->is_false(r))
    { return true;
    }
    else
    { assert(0); // All inequalities in resulting_inequalities must be false;
    }
  }
  return false;
#endif
} */

//---------------------------------------------------------------------------------------------------

static void pivot_and_update(
               const variable xi,  // a basic variable
               const variable xj,  // a non basic variable
               const data_expression v,
               const data_expression v_delta_correction,
               atermpp::map < variable,data_expression > &beta,
               atermpp::map < variable,data_expression > &beta_delta_correction,
               atermpp::set < variable > &basic_variables,
               std::map < variable, linear_inequality::lhs_t > &working_equalities,
               const rewriter &r)
{ // std::cerr << "Pivoting " << pp(xi) << "   " << pp(xj) << "\n";
  const data_expression aij=working_equalities[xi][xj];
  const data_expression theta=rewrite_with_memory(sort_real::divides(sort_real::minus(v,beta[xi]),aij),r);
  const data_expression theta_delta_correction=rewrite_with_memory(sort_real::divides(sort_real::minus(v,beta_delta_correction[xi]),aij),r);
  beta[xi]=v;
  beta_delta_correction[xi]=v_delta_correction;
  beta[xj]=rewrite_with_memory(sort_real::plus(beta[xj],theta),r);
  beta_delta_correction[xj]=rewrite_with_memory(sort_real::plus(beta_delta_correction[xj],theta_delta_correction),r);

  // std::cerr << "Pivoting phase 0\n";
  for(atermpp::set < variable >::const_iterator k=basic_variables.begin();
          k!=basic_variables.end(); ++k)
  { if ((*k!=xi) && (working_equalities[*k].count(xj)>0))
    { const data_expression akj=working_equalities[*k][xj];
      beta[*k]=rewrite_with_memory(sort_real::plus(beta[*k],sort_real::times(akj ,theta)),r);
      beta_delta_correction[*k]=rewrite_with_memory(sort_real::plus(beta_delta_correction[*k],sort_real::times(akj ,theta_delta_correction)),r);
    }
  }
  // Apply pivoting on variables xi and xj;
  // std::cerr << "Pivoting phase 1\n";
  basic_variables.erase(xi);
  basic_variables.insert(xj);

  linear_inequality::lhs_t expression_for_xj=working_equalities[xi];
  expression_for_xj.erase(xj);
  expression_for_xj[xi]=real_minus_one();
  expression_for_xj.multiply(sort_real::divides(real_minus_one(),aij),r);
  // std::cerr << "Expression for xj:" << expression_for_xj.string() << "\n";   
  // std::cerr << "Pivoting phase 2\n";
  working_equalities.erase(xi);
  for(std::map < variable, linear_inequality::lhs_t >::iterator j=working_equalities.begin();
           j!=working_equalities.end(); ++j)
  { if (j->second.count(xj)>0)
    { const data_expression factor=j->second[xj];
      // std::cerr << "VAR: " << pp(j->first) << " Factor " << pp(factor) << "\n";
      j->second.erase(xj);
      // We need a temporary copy of expression_for_xj as otherwise the multiply
      // below will change this expression.
      linear_inequality::lhs_t temporary_expression_for_xj(expression_for_xj);
      j->second.add(temporary_expression_for_xj.multiply(factor,r),r);
    }
  }
  working_equalities[xj]=expression_for_xj;

  // std::cerr << "Pivoting phase 3\n";
  // Calculate the values for beta and beta_delta_correction for the basic variables
  for(std::map < variable, linear_inequality::lhs_t >::const_iterator i=working_equalities.begin();
            i!=working_equalities.end() ; ++i)
  { beta[i->first]=i->second.evaluate(beta,r);
    beta_delta_correction[i->first]=i->second.evaluate(beta_delta_correction,r);
  }

  // std::cerr << "End pivoting " << pp(xj) << "\n";
  if (core::gsDebug)
  { for(atermpp::map < variable,data_expression >::const_iterator i=beta.begin();
               i!=beta.end(); ++i)
    { 
      // std::cerr << "beta[" << pp(i->first) << "]= " << pp(beta[i->first]) << "+ delta* " <<
      //                  pp(beta_delta_correction[i->first]) << "\n";
    }
    for(std::map < variable, linear_inequality::lhs_t >::const_iterator i=working_equalities.begin();
            i!=working_equalities.end() ; ++i)
    { // std::cerr << "EQ: " << pp(i->first) << " := " << i->second.string() << "\n";
    }
  }
}

/// \brief Determine whether a list of data expressions is inconsistent
/// \details First it is checked whether false is among the input. If
///          not, Fourier-Motzkin is applied to all variables in the
///          inequalities. If the empty set of equalities is the result,
///          the input was consistent. Otherwise the resulting set contains
///          an inconsistent inequality.
///          The implementation uses a feasible point detection algorithm as described by
///          Bruno Dutertre and Leonardo de Moura. Integrating Simplex with DPLL(T).
///          CSL Technical Report SRI-CSL-06-01, 2006.
/// \param inequalities A list of inequalities
/// \param r A rewriter
/// \ret true if the system of inequalities can be determined to be
///      inconsistent, false otherwise.


inline bool is_inconsistent(
              const std::vector < linear_inequality > &inequalities_in,
              const rewriter& r)
{ // Transform the linear inequalities into a vector of equalities and a
  // sequence of constraints on variables. All variables, including
  // those that will be generated as slack variables will have values indicated
  // by beta, which must lie between the lowerbounds and the upperbounds.

  // First remove all equalities by Gauss elimination and make a fresh variable
  // generator.

  if (core::gsDebug)
  { std::cerr << "Starting an inconsistency check on " + pp_vector(inequalities_in) << "\n";
  }

  // The required data structures
  atermpp::map < variable,data_expression > lowerbounds;
  atermpp::map < variable,data_expression > upperbounds;
  atermpp::map < variable,data_expression > beta;
  atermpp::map < variable,data_expression > lowerbounds_delta_correction;
  atermpp::map < variable,data_expression > upperbounds_delta_correction;
  atermpp::map < variable,data_expression > beta_delta_correction;
  atermpp::set < variable > non_basic_variables;
  atermpp::set < variable > basic_variables;
  std::map < variable, linear_inequality::lhs_t > working_equalities;

  fresh_variable_generator<> fresh_variable("slack_var");

  for(std::vector < linear_inequality >::const_iterator i=inequalities_in.begin();
                i!=inequalities_in.end(); ++i)
  { if (i->is_false(r))
    { if (core::gsDebug)
      { std::cerr << "Inconsistent, because linear inequalities contains an inconsistent inequality\n";
      }
      return true;
    }
    i->add_variables(non_basic_variables);
    for(linear_inequality::lhs_t::const_iterator j=i->lhs_begin();
             j!=i->lhs_end(); ++j)
    { fresh_variable.add_to_context(j->first);
    }
  }

  std::vector < linear_inequality > inequalities;
  std::vector < linear_inequality > equalities;
  non_basic_variables=
             gauss_elimination (inequalities_in,
                                equalities,      // Store all resulting equalities here.
                                inequalities,    // Store all resulting non equalities here.
                                non_basic_variables.begin(),
                                non_basic_variables.end(),
                                r);

  assert(equalities.size()==0); // There are no resulting equalities left.
  non_basic_variables.clear(); // gauss_elimination has removed certain variables. So, we reconstruct the non
                               // basic variables again below.

  // std::cerr << "Resulting equalities " << pp_vector(equalities) << "\n";
  // std::cerr << "Resulting inequalities " << pp_vector(inequalities) << "\n";

  // Now bring the linear equalities in the basic form described
  // in the article by Bruno Dutertre and Leonardo de Moura.

  // First set lower and upperbounds, and introduce slack variables
  // if required.
  for(std::vector < linear_inequality >::iterator i=inequalities.begin();
         i!=inequalities.end(); ++i)
  { if (i->is_false(r))
    { if (core::gsDebug)
      { std::cerr << "Inconsistent, because linear inequalities contains an inconsistent inequality after gaus elimination\n";
      }
      return true;
    }
    if (!i->is_true(r))  // This inequality is redundant and can be skipped.
    { assert(i->comparison()!=linear_inequality::equal);
      assert(i->lhs_size()>0); // this signals a redundant or an inconsistent inequality.
      i->add_variables(non_basic_variables);

      if (i->lhs_size()==1)  // the left hand side consists of a single variable.
      { variable v=i->lhs_begin()->first;
        data_expression factor=i->lhs_begin()->second;   assert(factor!=real_zero());
        data_expression bound=rewrite_with_memory(sort_real::divides(i->rhs(),factor),r);
        if (is_positive(factor,r))
        { // The inequality has the shape factor*v<=c or factor*v<c with factor positive
          if ((upperbounds.count(v)==0) ||
               (rewrite_with_memory(less(bound,upperbounds[v]),r)==sort_bool::true_()))
          { upperbounds[v]=bound;
            upperbounds_delta_correction[v]=
                ((i->comparison()==linear_inequality::less)?real_minus_one():real_zero());

          }
          else
          { if (bound==upperbounds[v])
            { upperbounds_delta_correction[v]=
                  min(upperbounds_delta_correction[v],
                      ((i->comparison()==linear_inequality::less)?real_minus_one():real_zero()),r);
            }
          }
        }
        else
        { // The inequality has the shape factor*v<=c or factor*v<c with factor negative
          if ((lowerbounds.count(v)==0) ||
               (rewrite_with_memory(less(lowerbounds[v],bound),r)==sort_bool::true_()))
          { lowerbounds[v]=bound;
            lowerbounds_delta_correction[v]=
                ((i->comparison()==linear_inequality::less)?real_one():real_zero());
          }
          else
          { if (bound==lowerbounds[v])
            { lowerbounds_delta_correction[v]=
                  max(lowerbounds_delta_correction[v],
                      ((i->comparison()==linear_inequality::less)?real_one():real_zero()),r);
            }
          }
        }
      }
      else
      { // The inequality has more than one variable at the left hand side.
        // We transform it into an equation with a new slack variable.
        variable new_basic_variable=fresh_variable(sort_real::real_());
        basic_variables.insert(new_basic_variable);
        upperbounds[new_basic_variable]=i->rhs();
        upperbounds_delta_correction[new_basic_variable]=
                ((i->comparison()==linear_inequality::less)?real_minus_one():real_zero());
        working_equalities[new_basic_variable]=i->lhs();
        // std::cerr << "New slack variable: " << pp(new_basic_variable) << ":=" << string(*i) << "\n";
      }
    }
  }
  // Now set the values for beta:
  // The beta values for the non basic variables must satisfy the lower and
  // upperbounds.
  for(atermpp::set < variable >::const_iterator i=non_basic_variables.begin();
        i!=non_basic_variables.end(); ++i)
  { if (lowerbounds.count(*i)>0)
    { if ((upperbounds.count(*i)>0) &&
          ((rewrite_with_memory(less(upperbounds[*i],lowerbounds[*i]),r)==sort_bool::true_()) ||
           ((upperbounds[*i]==lowerbounds[*i]) &&
            (rewrite_with_memory(less(upperbounds_delta_correction[*i],lowerbounds_delta_correction[*i]),r)==sort_bool::true_()))))
      { if (core::gsDebug)
        { std::cerr << "Inconsistent, preprocessing " << pp(*i) << "\n";
        }
        return true; // Inconsistent.
      }
      beta[*i]=lowerbounds[*i];
      beta_delta_correction[*i]=lowerbounds_delta_correction[*i];
    }
    else if (upperbounds.count(*i)>0)
    { beta[*i]=upperbounds[*i];
      beta_delta_correction[*i]=upperbounds_delta_correction[*i];
    }
    else // *i has neither a lower or an upperbound
    { beta[*i]=real_zero();
      beta_delta_correction[*i]=real_zero();
    }
    // std::cerr << "beta[" << pp(*i) << "]=" << pp(beta[*i])<< "+delta*" <<
                           // pp(beta_delta_correction[*i]) <<"\n";
  }

  // Subsequently set the values for the basic variables
  for(atermpp::set < variable >::const_iterator i=basic_variables.begin();
        i!=basic_variables.end(); ++i)
  { beta[*i]=working_equalities[*i].evaluate(beta,r);
    beta_delta_correction[*i]=working_equalities[*i].
                   evaluate(beta_delta_correction,r);
    // std::cerr << "beta[" << pp(*i) << "]=" << pp(beta[*i])<< "+delta*" <<
    //                       pp(beta_delta_correction[*i]) <<"\n";
  }

  // Now the basic data structure has been set up.
  // We must find the first basic variable that does not satisfy its
  // upper and bounds. This is essentially the check algorithm in the
  // article by Bruno Dutertre and Leonardo de Moura.

  for ( ; true ; )
  { // select the smallest basic variable that does not satisfy the bounds.
    bool found=false;
    bool lowerbound_violation = false;
    variable xi;
    for(atermpp::set < variable > :: const_iterator i=basic_variables.begin() ;
           i!=basic_variables.end() ; ++i)
    { // std::cerr << "Evaluate start\n";
      assert(!found);
      data_expression value=beta[*i]; // working_equalities[*i].evaluate(beta,r);
      data_expression value_delta_correction=beta_delta_correction[*i]; // working_equalities[*i].evaluate(beta_delta_correction,r);
      // std::cerr << "Evaluate end\n";
      if ((upperbounds.count(*i)>0) &&
          ((rewrite_with_memory(less(upperbounds[*i],value),r)==sort_bool::true_()) ||
           ((upperbounds[*i]==value) &&
            (rewrite_with_memory(less(upperbounds_delta_correction[*i],value_delta_correction),r)==sort_bool::true_()))))
      { // The value of variable *i does not satisfy its upperbound. This must
        // be corrected using pivoting.
        // std::cerr << "Upperbound violation " << pp(*i) << "  bound: " << pp(upperbounds[*i]) << "\n";
        found=true;
        xi=*i;
        lowerbound_violation=false;
        break;
      }
      else if ((lowerbounds.count(*i)>0) &&
               ((rewrite_with_memory(less(value,lowerbounds[*i]),r)==sort_bool::true_()) ||
                ((lowerbounds[*i]==value) &&
                 (rewrite_with_memory(less(value_delta_correction,lowerbounds_delta_correction[*i]),r)==sort_bool::true_()))))
      { // The value of variable *i does not satisfy its upperbound. This must
        // be corrected using pivoting.
        // std::cerr << "Lowerbound violation " << pp(*i) << "  bound: " << pp(lowerbounds[*i]) << "\n";
        found=true;
        xi=*i;
        lowerbound_violation=true;
        break;
      }
    }
    if (!found)
    { // The in_equalities are consistent. Return false.
      if (core::gsDebug)
      { std::cerr << "Consistent while pivoting\n";
        /* for(atermpp::map < variable,data_expression >::const_iterator i=lowerbounds.begin();
                      i!=lowerbounds.end(); ++i)
        { variable v=i->first;
          if (lowerbounds.count(v)>0) 
          { if (rewrite_with_memory(less(beta[v],lowerbounds[v]),r)==sort_bool::true_())
            { std::cerr << "FOUT1\n"; exit(0);
            }
            if (beta[v]==lowerbounds[v])
            { if (rewrite_with_memory(less(beta_delta_correction[v],lowerbounds_delta_correction[v]),r)==sort_bool::true_())
              { std::cerr << "FOUT2\n"; exit(0);
              }
            }
            else 
            { if (rewrite_with_memory(less(lowerbounds[v],beta[v]),r)!=sort_bool::true_()) 
              { std::cerr << "FOUT2a\n"; exit(0);
              }
            }
          }

          if (upperbounds.count(v)>0)
          { if (rewrite_with_memory(less(upperbounds[v],beta[v]),r)==sort_bool::true_())
            { std::cerr << "FOUT3\n"; exit(0);
            }
            if ((beta[v]==upperbounds[v]) &&
                (rewrite_with_memory(less(lowerbounds_delta_correction[v],beta_delta_correction[v]),r)==sort_bool::true_()))
            { std::cerr << "FOUT4\n"; exit(0);
            }
          }
        } */
      }
      return false;
    }

    // std::cerr << "The smallest basic variable that does not satisfy the bounds is " << pp(xi) << "\n";
    if (lowerbound_violation)
    { // std::cerr << "Lowerbound violationdt \n";
      // select the smallest non-basic variable with which pivoting can take place.
      bool found=false;
      const linear_inequality::lhs_t &lhs=working_equalities[xi];
      for(linear_inequality::lhs_t::const_iterator xj_it=lhs.begin(); xj_it!=lhs.end(); ++xj_it)
      { const variable xj=xj_it->first;
        // std::cerr << pp(xj) << "  --  " << pp(xj_it->second) << "\n";
        if ((is_positive(xj_it->second,r) &&
                ((upperbounds.count(xj)==0) ||
                    ((rewrite_with_memory(less(beta[xj],upperbounds[xj]),r)==sort_bool::true_())||
                     ((beta[xj]==upperbounds[xj])&& (rewrite_with_memory(less(beta_delta_correction[xj],upperbounds_delta_correction[xj]),r)==sort_bool::true_()))))) ||
            (is_negative(xj_it->second,r) &&
                 ((lowerbounds.count(xj)==0) ||
                    ((rewrite_with_memory(greater(beta[xj],lowerbounds[xj]),r)==sort_bool::true_())||
                     ((beta[xj]==lowerbounds[xj]) && (rewrite_with_memory(greater(beta_delta_correction[xj],lowerbounds_delta_correction[xj]),r)==sort_bool::true_()))))))
        { found=true;
          pivot_and_update(xi,xj,lowerbounds[xi],lowerbounds_delta_correction[xi],
                           beta, beta_delta_correction,
                           basic_variables,working_equalities,r);
          break;
        }
      }
      if (!found)
      { // The inequalities are inconsistent.
        if (core::gsDebug)
        { std::cerr << "Inconsistent while pivoting\n";
        }
        return true;
      }

    }
    else  // Upperbound violation.
    { // std::cerr << "Upperbound violationdt \n";
      // select the smallest non-basic variable with which pivoting can take place.
      bool found=false;
      for(linear_inequality::lhs_t::const_iterator xj_it=working_equalities[xi].begin();
                xj_it!=working_equalities[xi].end(); ++xj_it)
      { const variable xj=xj_it->first;
        // std::cerr << pp(xj) << "  --  " << pp(xj_it->second) <<  " POS " <<
        //                 is_positive(xj_it->second,r) << "\n";
        if ((is_negative(xj_it->second,r) &&
                ((upperbounds.count(xj)==0) ||
                   ((rewrite_with_memory(less(beta[xj],upperbounds[xj]),r)==sort_bool::true_()) ||
                     ((beta[xj]==upperbounds[xj])&& (rewrite_with_memory(less(beta_delta_correction[xj],upperbounds_delta_correction[xj]),r)==sort_bool::true_()))))) ||
            (is_positive(xj_it->second,r) &&
                 ((lowerbounds.count(xj)==0) ||
                    ((rewrite_with_memory(greater(beta[xj],lowerbounds[xj]),r)==sort_bool::true_()) ||
                     ((beta[xj]==lowerbounds[xj]) && (rewrite_with_memory(greater(beta_delta_correction[xj],lowerbounds_delta_correction[xj]),r)==sort_bool::true_()))))))
        { found=true;
          pivot_and_update(xi,xj,upperbounds[xi],upperbounds_delta_correction[xi],
                           beta,beta_delta_correction,
                           basic_variables,working_equalities,r);
          break;
        }
      }
      if (!found)
      { // The inequalities are inconsistent.
        if (core::gsDebug)
        { std::cerr << "Inconsistent while pivoting (1)\n";
        }
        return true;
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------


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
atermpp::set < variable >  gauss_elimination(
                         const std::vector < linear_inequality > &inequalities,
                         std::vector < linear_inequality > &resulting_equalities,
                         std::vector < linear_inequality > &resulting_inequalities,
                         Variable_iterator variables_begin,
                         Variable_iterator variables_end,
                         const rewriter& r)
{
  // gsDebugMsg("Trying to eliminate variables %P from system %P using gauss elimination\n", (ATermList)variables, (ATermList)inequalities);

  atermpp::set < variable >  remaining_variables;

  // First copy equalities to the resulting_equalities and the inequalites to resulting_inequalities.
  for(std::vector < linear_inequality > ::const_iterator j = inequalities.begin(); j != inequalities.end(); ++j)
  { if (j->is_false(r))
    { // The input contains false. Return false and stop.
      resulting_equalities.clear();
      resulting_inequalities.clear();
      resulting_inequalities.push_back(linear_inequality());
      return remaining_variables;
    }
    else if (!j->is_true(r)) // Do not consider redundant equations.
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
  for(Variable_iterator i = variables_begin; i != variables_end; ++i)
  { unsigned int j;
    for(j=0; j<resulting_equalities.size(); ++j)
    {
      bool check_equalities_for_redundant_inequalities(false);
      std::set < variable > vars;
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
          if (resulting_inequalities[k].is_false(r))
          { // The input is inconsistent. Return false.
            resulting_equalities.clear();
            resulting_inequalities.clear();
            resulting_inequalities.push_back(linear_inequality());
            remaining_variables.clear();
            return remaining_variables;
          }
          else if (resulting_inequalities[k].is_true(r))
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
            if (resulting_equalities[k].is_false(r))
            { // The input is inconsistent. Return false.
              resulting_equalities.clear();
              resulting_inequalities.clear();
              resulting_inequalities.push_back(linear_inequality());
              remaining_variables.clear();
              return remaining_variables;
            }
            else if (resulting_equalities[k].is_true(r))
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
          { if (resulting_equalities[k].is_true(r))
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
    remaining_variables.insert(*i);
  }

  // gsDebugMsg("Gauss elimination eliminated variables %P, resulting in the system %P\n",
  //                       (ATermList)eliminated_variables, (ATermList)inequalities);

  return remaining_variables;
}

// The introduction of the function rewrite_with_memory using a
// hash table here is a temporary trick, to boost
// performance, which is slow due to translations necessary from and to
// rewrite format.

inline data_expression rewrite_with_memory(
                      const data_expression t,const rewriter &r)
{
  static atermpp::map < data_expression, data_expression > rewrite_hash_table;
  atermpp::map < data_expression, data_expression > :: iterator i=rewrite_hash_table.find(t);
  if (i==rewrite_hash_table.end())
  {
    // std::cerr << "Size of hash table: " << rewrite_hash_table.size() << "\n";
    data_expression t1=r(t);
    // rewrite_hash_table[t]=t1;
    // std::cerr << "Term " << pp(t) << "Result " << pp(t1) << "\n";
    return t1;
  }
  // std::cerr << "Term " << pp(t) << "FROM HASH " << pp(i->second) << "\n";
  return i->second;
}


} // namespace data

} // namespace mcrl2

#endif // MCRL2_LPSREALELM_LINEAR_INEQUALITY_H
