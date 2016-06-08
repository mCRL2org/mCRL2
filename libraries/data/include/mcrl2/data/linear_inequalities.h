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

#include <algorithm>

#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/substitutions/map_substitution.h"

namespace mcrl2
{

namespace data
{

// Functions below should be made available in the data library.
data_expression& real_zero();
data_expression& real_one();
data_expression& real_minus_one();
data_expression min(const data_expression e1,const data_expression e2,const rewriter&);
data_expression max(const data_expression e1,const data_expression e2,const rewriter&);
bool is_closed_real_number(const data_expression e);
bool is_negative(const data_expression e,const rewriter& r);
bool is_positive(const data_expression e,const rewriter& r);
bool is_zero(const data_expression e);


// Efficient construction of times on reals.
// The functions times, divide, plus and minus in the standard library are not efficient as it determines the sort at runtime.
inline application real_times(const data_expression& arg0, const data_expression& arg1)
{
  static function_symbol times_f(sort_real::times_name(), make_function_sort(sort_real::real_(), sort_real::real_(), sort_real::real_()));
  assert(arg0.sort()==sort_real::real_() && arg1.sort()==sort_real::real_());
  return application(times_f,arg0,arg1);
}

inline application real_plus(const data_expression& arg0, const data_expression& arg1)
{
  static function_symbol plus_f(sort_real::plus_name(), make_function_sort(sort_real::real_(), sort_real::real_(), sort_real::real_()));
  assert(arg0.sort()==sort_real::real_() && arg1.sort()==sort_real::real_());
  return application(plus_f,arg0,arg1);
}

inline application real_divides(const data_expression& arg0, const data_expression& arg1)
{
  static function_symbol divides_f(sort_real::divides_name(), make_function_sort(sort_real::real_(), sort_real::real_(), sort_real::real_()));
  assert(arg0.sort()==sort_real::real_() && arg1.sort()==sort_real::real_());
  return application(divides_f,arg0,arg1);
}

inline application real_minus(const data_expression& arg0, const data_expression& arg1)
{
  static function_symbol minus_f(sort_real::minus_name(), make_function_sort(sort_real::real_(), sort_real::real_(), sort_real::real_()));
  assert(arg0.sort()==sort_real::real_() && arg1.sort()==sort_real::real_());
  return application(minus_f,arg0,arg1);
}

// End of functions that ought to be defined elsewhere.


inline data_expression rewrite_with_memory(const data_expression t,const rewriter& r);

// prototype
class linear_inequality;
namespace detail
{
  class lhs_t;
}

inline std::string pp(const linear_inequality& l);
template <class TYPE>
inline std::string pp_vector(const TYPE& inequalities);

namespace detail
{
  enum comparison_t { less, less_eq, equal };
  
  inline std::string pp(const detail::lhs_t& l);

  inline detail::comparison_t negate(const detail::comparison_t t)
  {
    switch (t)
    {
      case detail::less:  return detail::less_eq;
      case detail::less_eq: return detail::less;
      case detail::equal: return detail::equal;
      default: assert(0);
    };
  }
  
  inline std::string pp(const detail::comparison_t t)
  {
    switch (t)
    {
      case detail::less:  return "<";
      case detail::less_eq: return "<=";
      case detail::equal: return "==";
      default: assert(0);
    };
  }
  
  inline atermpp::function_symbol f_variable_with_a_rational_factor()
  {
    static atermpp::function_symbol f("variable_with_a_rational_factor",2);
    return f;
  }

  class variable_with_a_rational_factor: public atermpp::aterm_appl
  {
    public:
      // \brief default constructor
      variable_with_a_rational_factor(const variable& v, const data_expression f)
       : atermpp::aterm_appl(f_variable_with_a_rational_factor(),v,f)
      {
        assert(f!=real_zero());
      }
     
     // \brief Get the variable in a variable/rational factor pair.
     const variable& variable_name() const
     {
       assert(is_variable_with_a_rational_factor());
       return atermpp::down_cast<variable>((*this)[0]);
     }
     
     // \brief Get the rational factor in a variable/rational factor pair.
     const data_expression& factor() const
     {
       assert(is_variable_with_a_rational_factor());
       return atermpp::down_cast<data_expression>((*this)[1]);
     }

     // \brief Check that a term is indeed a variable with a rational factor pair.
     bool is_variable_with_a_rational_factor() const
     {
       return function()==f_variable_with_a_rational_factor();
     }
  };

  // typedef atermpp::term_list<variable_with_a_rational_factor> lhs_t;
  typedef std::map < variable, data_expression > map_based_lhs_t;

  class lhs_t: public atermpp::term_list<variable_with_a_rational_factor>
  {
    public:
      /// \brief Constructor
      lhs_t()
       : atermpp::term_list<variable_with_a_rational_factor>()
      {}  

      /// \brief Constructor
      template <class ITERATOR>
      lhs_t(const ITERATOR begin, const ITERATOR end)
       : atermpp::term_list<variable_with_a_rational_factor>(begin, end)
      {}  
      
      /// \brief Constructor
      template <class ITERATOR, class TRANSFORMER>
      lhs_t(const ITERATOR begin, const ITERATOR end, TRANSFORMER f)
       : atermpp::term_list<variable_with_a_rational_factor>(begin, end, f)
      {}  

      /// \brief Give an iterator of the factor/variable pair for v, or end() if v does not occur.
      lhs_t::const_iterator find(const variable& v) const
      { return std::find_if(begin(), 
                            end(), 
                            [&](const detail::variable_with_a_rational_factor& p)
                                                    {return p.variable_name()==v;});
      }

      /// \brief Erase a variable and its factor.
      lhs_t erase(const variable& v) const
      {
        std::vector <variable_with_a_rational_factor> result;
        for(const variable_with_a_rational_factor& p: *this)
        {
          if (p.variable_name()!=v)
          {
            result.push_back(p);
          }
        }
        return lhs_t(result.begin(),result.end());
      }

      /// \brief Give the factor of variable v.
      const data_expression& operator[](const variable& v) const
      {
        lhs_t::const_iterator i=find(v);
        if (i==end())  // Not found.
        {
          return real_zero();
        }
        return i->factor();
      }

      /// \brief Give the factor of variable v.
      size_t count(const variable& v) const
      {
        lhs_t::const_iterator i=find(v);
        if (i==end())  // Not found.
        {
          return 0;
        }
        return 1;
      }

      /// \brief Evaluate the variables in this lhs_t according to the subsitution function.
      template <typename SubstitutionFunction>
      data_expression evaluate(const SubstitutionFunction& beta, const rewriter& r) const
      {
        data_expression result=real_zero();
        bool result_defined=false; // save adding the initial zero.
        for (const variable_with_a_rational_factor& p: *this)
        {
          if (result_defined)
          {  
            result=r(real_plus(result,real_times(beta(p.variable_name()),p.factor())));
          }
          else
          {
            result=r(real_times(beta(p.variable_name()),p.factor()));
            result_defined=true;
          }
      
        }
        return result;
      }

  };

  inline void set_factor_for_a_variable(detail::map_based_lhs_t& new_lhs, const variable x, const data_expression e)
  {
    assert(is_closed_real_number(e));
    if (e==real_zero())
    {
      detail::map_based_lhs_t::iterator i=new_lhs.find(x);
      if (i!=new_lhs.end())
      {
        new_lhs.erase(i);
      }
    }
    else
    {
      new_lhs[x]=e;
    }
  }

  inline lhs_t set_factor_for_a_variable(const lhs_t& lhs, const variable& x, const data_expression& e)
  {
    assert(is_closed_real_number(e));
    bool inserted=false;
    std::vector<variable_with_a_rational_factor> result;
    for(const variable_with_a_rational_factor& p: lhs)
    {
      if (!inserted && x<=p.variable_name())
      {
        result.emplace_back(x,e);
        inserted=true;
        if (x!=p.variable_name())
        {
          result.emplace_back(p.variable_name(),p.factor());
        }
      }
      else result.emplace_back(p.variable_name(),p.factor());
    }
    if (!inserted)
    {
        result.emplace_back(x,e);
    }
    assert(std::find(result.begin(),result.end(),variable_with_a_rational_factor(x,e))!=result.end());
    return lhs_t(result.begin(),result.end());
  }

  inline const lhs_t map_to_lhs_type(const map_based_lhs_t& lhs)
  {
    lhs_t result;
    for(map_based_lhs_t::const_reverse_iterator i=lhs.rbegin(); i!=lhs.rend(); ++i)
    {
        result.push_front(variable_with_a_rational_factor(i->first,i->second));
    }
    return result;
  }

  inline const lhs_t map_to_lhs_type(const map_based_lhs_t& lhs, const data_expression& factor, const rewriter& r)
  {
    assert(factor!=real_one() && factor!=real_minus_one());
    lhs_t result;
    for(map_based_lhs_t::const_reverse_iterator i=lhs.rbegin(); i!=lhs.rend(); ++i)
    {
      result.push_front(variable_with_a_rational_factor(i->first,r(real_divides(i->second,factor))));
    }
    return result;
  }

  inline bool is_well_formed(const lhs_t& lhs)
  {
    if (lhs.empty())
    {
      return true;
    }
    if (lhs.front().factor()!=real_one() && lhs.front().factor()!=real_minus_one())
    {
      return false;
    }
    variable last_variable_seen=lhs.front().variable_name();
    bool first=true;
    for(const variable_with_a_rational_factor& p: lhs)
    {
      if (!first)
      {
        first=false;
        if (p.variable_name()<=last_variable_seen)
        {
          return false;
        }
        last_variable_seen=p.variable_name();
      }
    }
   return true;
  }

  inline const lhs_t remove_variable_and_divide(const lhs_t& lhs, const variable& v, const data_expression& f, const rewriter& r)
  {
    std::vector <variable_with_a_rational_factor> result;
    for(const variable_with_a_rational_factor& p: lhs)
    {
      if (p.variable_name()!=v)
      {
        result.emplace_back(p.variable_name(),r(real_divides(p.factor(),f)));
      }
    }
    return lhs_t(result.begin(),result.end());
  }
 
  inline void emplace_back_if_not_zero(std::vector<detail::variable_with_a_rational_factor>& r, const variable& v, const data_expression& f)
  {
    if (f!=real_zero())
    {
      r.emplace_back(v,f);
    }
  }


  template < application Operation(const data_expression&, const data_expression&) >
  inline const lhs_t meta_operation_constant(const lhs_t& argument, const data_expression& v, const rewriter& r)
  {
    std::vector<detail::variable_with_a_rational_factor> result;
    result.reserve(argument.size());
    for (const detail::variable_with_a_rational_factor& p: argument)
    {
      emplace_back_if_not_zero(result,p.variable_name(), rewrite_with_memory(Operation(v,p.factor()),r));
    } 
    return lhs_t(result.begin(),result.end());
  }

  // Template method to add or subtract lhs_t's
  // template < application Operation(const data_expression&, const data_expression&) >
  template <class OPERATION>
  inline lhs_t meta_operation_lhs(const lhs_t& argument1, 
                                  const lhs_t& argument2, 
                                  OPERATION operation,
                                  const rewriter& r)
  {
    std::vector<detail::variable_with_a_rational_factor> result;
    result.reserve(argument1.size()+argument2.size());

    lhs_t::const_iterator i1=argument1.begin();
    lhs_t::const_iterator i2=argument2.begin();
    
    while (i1!=argument1.end() && i2!=argument2.end())
    {
      if (i1->variable_name()<i2->variable_name())
      {
        emplace_back_if_not_zero(result,i1->variable_name(), rewrite_with_memory(operation(i1->factor(),real_zero()),r));
        ++i1;
      }
      else if (i1->variable_name()>i2->variable_name())
      {
        emplace_back_if_not_zero(result,i2->variable_name(), rewrite_with_memory(operation(real_zero(),i2->factor()),r));
        ++i2;
      }
      else 
      {
        assert(i1->variable_name()==i2->variable_name());
        emplace_back_if_not_zero(result,i1->variable_name(), rewrite_with_memory(operation(i1->factor(),i2->factor()),r));
        ++i1;
        ++i2;
      }
    }

    if (i1==argument1.end())
    {
      while (i2!=argument2.end())
      {
        emplace_back_if_not_zero(result,i2->variable_name(), rewrite_with_memory(operation(real_zero(),i2->factor()),r));
        ++i2;
      }
    }  
    else
    {
      while (i1!=argument1.end())
      {
        emplace_back_if_not_zero(result,i1->variable_name(), rewrite_with_memory(operation(i1->factor(),real_zero()),r));
        ++i1;
      }
    }  
    return lhs_t(result.begin(),result.end());
  }

  inline const lhs_t add(const data_expression& v, const lhs_t& argument, const rewriter& r)
  {
    return meta_operation_constant<real_plus>(argument,v,r);
  }

  inline const lhs_t subtract(const lhs_t& argument, const data_expression& v, const rewriter& r)
  {
    return meta_operation_constant<real_minus>(argument, v, r);
  }

  inline const lhs_t multiply(const lhs_t& argument, const data_expression& v, const rewriter& r)
  {
    return meta_operation_constant<real_times>(argument, v, r);
  }

  inline const lhs_t divide(const lhs_t& argument, const data_expression& v, const rewriter& r)
  {
    return meta_operation_constant<real_divides>(argument, v, r);
  }

  inline const lhs_t add(const lhs_t& argument, const lhs_t& e, const rewriter& r)
  {
    return meta_operation_lhs(argument, 
                              e, 
                              [](const data_expression& d1, const data_expression& d2)->data_expression
                                              { return real_plus(d1,d2); },  
                              r);
  }

  inline const lhs_t subtract(const lhs_t& argument, const lhs_t& e, const rewriter& r)
  {
    return meta_operation_lhs(argument, 
                              e, 
                              [](const data_expression& d1, const data_expression& d2)->data_expression
                                              { return real_minus(d1,d2); }, 
                              r);
  }

  inline std::string pp(const lhs_t& lhs)
  {
    std::string s;
    if (lhs.begin()==lhs.end())
    {
      s="0";
    }
    for (lhs_t::const_iterator i=lhs.begin(); i!=lhs.end(); ++i)
    {
      s=s + (i==lhs.begin()?"":" + ") ;
      
      if (i->factor()==real_one())
      {
        s=s + pp(i->variable_name());
      }
      else if (i->factor()==real_minus_one())
      {
        s=s + "-" + pp(i->variable_name());
      }
      else
      {
        s=s + data::pp(i->factor()) + "*" + data::pp(i->variable_name());
      }
    }
    return s;
  }

  inline atermpp::function_symbol linear_inequality_less()
  {
    static atermpp::function_symbol f("linear_inequality_less",2);
    return f;
  }


  inline atermpp::function_symbol linear_inequality_less_equal()
  {
    static atermpp::function_symbol f("linear_inequality_less_equal",2);
    return f;
  }


  inline atermpp::function_symbol linear_inequality_equal()
  {
    static atermpp::function_symbol f("linear_inequality_equal",2);
    return f;
  }

} // end namespace detail

class linear_inequality: public atermpp::aterm_appl
{
  // The structure of a linear equality is a function application 
  // to two arguments. The function application is either linear_inequality_less,
  // linear_inequality_less_equal, or linear_inequality_equal. There are two arguments,
  // namely an ordered list of pairs of a variable and a factor. This list is ordered
  // on the variables, and a closed expression which forms the right hand side.
  // The first variable in the list has a factor one or minus one.

  protected:

    static void parse_and_store_expression(
      const data_expression e,
      detail::map_based_lhs_t& new_lhs,
      data_expression& new_rhs,
      const rewriter& r,
      bool negate=false,
      const data_expression factor=real_one())
    {
      if (sort_real::is_minus_application(e) && application(e).size()==2)
      {
        parse_and_store_expression(data::binary_left(application(e)),new_lhs,new_rhs,r,negate,factor);
        parse_and_store_expression(data::binary_right(application(e)),new_lhs,new_rhs,r,!negate,factor);
      }
      else if (sort_real::is_negate_application(e) && application(e).size()==1)
      {
        parse_and_store_expression(*(application(e).begin()),new_lhs,new_rhs,r,!negate,factor);
      }
      else if (sort_real::is_plus_application(e))
      {
        parse_and_store_expression(data::binary_left(application(e)),new_lhs,new_rhs,r,negate,factor);
        parse_and_store_expression(data::binary_right(application(e)),new_lhs,new_rhs,r,negate,factor);
      }
      else if (sort_real::is_times_application(e))
      {
        data_expression lhs=rewrite_with_memory(data::binary_left(application(e)),r);
        data_expression rhs=rewrite_with_memory(data::binary_right(application(e)),r);
        if (is_closed_real_number(lhs))
        {
          parse_and_store_expression(rhs,new_lhs,new_rhs,r,negate,real_times(lhs,factor));
        }
        else if (is_closed_real_number(rhs))
        {
          parse_and_store_expression(lhs,new_lhs,new_rhs,r,negate,real_times(rhs,factor));
        }
        else
        {
          throw mcrl2::runtime_error("Expect constant multiplies expression: " + pp(e) + "\n");
        }
      }
      else if (is_variable(e))
      {
        if (e.sort() == sort_real::real_())
        {
          const variable& v=atermpp::down_cast<variable>(e);
          if (new_lhs.find(v) == new_lhs.end())
          {
            detail::set_factor_for_a_variable(new_lhs,v,(negate?rewrite_with_memory(sort_real::negate(factor),r)
                                         :rewrite_with_memory(factor,r)));
          }
          else
          {
            detail::set_factor_for_a_variable(new_lhs,v,(negate?rewrite_with_memory(real_minus(new_lhs[v],factor),r)
                                         :rewrite_with_memory(real_plus(new_lhs[v],factor),r)));
          }
        }
        else
        {
          throw mcrl2::runtime_error("Encountered a variable in a real expression which is not of sort real: " + pp(e) + "\n");
        }
      }
      else if (is_closed_real_number(rewrite_with_memory(e,r)))
      {
        if (factor==real_one())
        {
          new_rhs=(negate?rewrite_with_memory(real_plus(new_rhs,e),r)
                             :rewrite_with_memory(real_minus(new_rhs,real_times(factor,e)),r));
        }
        else
        {
          new_rhs=(negate?rewrite_with_memory(real_plus(new_rhs, real_times(factor,e)),r)
                          :rewrite_with_memory(real_minus(new_rhs,real_times(factor,e)),r));
        }
      }
      else
      {
        throw mcrl2::runtime_error("Expect linear expression over reals: " + pp(e) + "\n");
      }
    }


  public:

    /// \brief Constructor yielding an inconsistent inequality.
    linear_inequality()
      : linear_inequality(detail::lhs_t(),real_zero(),detail::less)
    {}

    /// Basic constructor. 
    linear_inequality(const detail::lhs_t lhs, const data_expression& r, detail::comparison_t t)
     : atermpp::aterm_appl((t==detail::less?
                      detail::linear_inequality_less():
                      (t==detail::less_eq?detail::linear_inequality_less_equal():detail::linear_inequality_equal())),
                  lhs,r)
    {
      assert(detail::is_well_formed(lhs));
      assert(t==detail::less || t==detail::less_eq || t==detail::equal);
    }
      
    /// \brief constructor. 
    linear_inequality(const detail::lhs_t& lhs, const data_expression& rhs, detail::comparison_t comparison, const rewriter& r)
    {
      if (lhs.empty())
      {
        *this=linear_inequality(detail::lhs_t(),rhs,comparison);
        return;
      }
      // Normalize the linear_inequality such that the first term has factor 1 or -1.
      data_expression factor=lhs.begin()->factor();
      if (factor==real_one() || factor==real_minus_one())
      {
        *this=linear_inequality(lhs,rhs,comparison);
        return;
      }
      if (is_negative(factor,r))
      {
        factor=r(real_divides(real_minus_one() ,factor));
      }
      
      *this=linear_inequality(divide(lhs,factor,r),r(real_divides(rhs,factor)),comparison);
    }

    /// \brief constructor.
    linear_inequality(const data_expression& lhs,
                      const data_expression& rhs,
                      const detail::comparison_t comparison,
                      const rewriter& r,
                      const bool negate=false)
    {
      detail::map_based_lhs_t new_lhs;
      data_expression new_rhs(real_zero());
      parse_and_store_expression(lhs,new_lhs,new_rhs,r,negate);
      parse_and_store_expression(rhs,new_lhs,new_rhs,r,!negate);

      if (new_lhs.empty())
      {
        if ((comparison==detail::equal && new_rhs==real_zero()) ||
            (comparison!=detail::equal && is_positive(new_rhs,r)))
        {
          // The linear inequality represents true.
          *this=linear_inequality(detail::lhs_t(),real_one(),detail::less);
          return;
        }
        // The linear inequality represents false. 
        *this=linear_inequality(detail::lhs_t(),real_minus_one(),detail::less);
        return;
      }

      // Normalize the linear_inequality such that the first term has factor 1 or -1.
      data_expression factor=new_lhs.begin()->second;
      if (factor==real_one() || factor==real_minus_one())
      {
        *this=linear_inequality(detail::map_to_lhs_type(new_lhs),new_rhs,comparison);
        return;
      }
      if (is_negative(factor,r))
      {
        factor=r(real_times(real_minus_one() ,factor));
      }
      
      *this=linear_inequality(detail::map_to_lhs_type(new_lhs,factor,r),r(real_divides(new_rhs,factor)),comparison);
    }



    /// \brief Constructor that constructs a linear inequality out of a data expression.
    /// \details The data expression e is expected to have the form
    /// lhs op rhs where op is one of <=,<,==,>,>= and lhs and rhs
    /// satisfy the syntax t ::=  x | c*t | t*c | t+t | t-t | -t where x is
    /// a variable and c is a real constant.
    /// \param e Contains the expression to become a linear inequality.

    linear_inequality(const data_expression e,
                      const rewriter& r)
    {
      detail::comparison_t comparison;
      bool negate(false);
      if (is_equal_to_application(e))
      {
        comparison=detail::equal;
      }
      else if (is_less_application(e))
      {
        comparison=detail::less;
      }
      else if (is_less_equal_application(e))
      {
        comparison=detail::less_eq;
      }
      else if (is_greater_application(e))
      {
        comparison=detail::less;
        negate=true;
      }
      else if (is_greater_equal_application(e))
      {
        comparison=detail::less_eq;
        negate=true;
      }
      else
      {
        throw mcrl2::runtime_error("Unexpected equality or inequality: " + pp(e) + "\n") ;
      }

      data_expression lhs=data::binary_left(application(e));
      data_expression rhs=data::binary_right(application(e));
      *this=linear_inequality(lhs,rhs,comparison,r,negate);

    }


    detail::lhs_t::const_iterator lhs_begin() const
    {
      return lhs().begin();
    }

    detail::lhs_t::const_iterator lhs_end() const
    {
      return lhs().end();
    }

    const detail::lhs_t& lhs() const
    {
      return atermpp::down_cast<detail::lhs_t>((*this)[0]);
    }

    const data_expression& rhs() const
    {
      return atermpp::down_cast<data_expression>((*this)[1]);
    }

    /* void swap(linear_inequality& l)
    {
      m_rhs=l.m_rhs;
      m_lhs.swap(l.m_lhs);
      const detail::comparison_t c(m_comparison);
      m_comparison=l.m_comparison;
      l.m_comparison=c;
    } */

    /* void set_rhs(const data_expression e)
    {
      assert(is_closed_real_number(e));
      m_rhs=e;
    } */

    /* bool operator ==(const linear_inequality& other) const
    {
      return lhs()==other.lhs() && rhs()==other.rhs() && m_comparison==other.m_comparison;
    }

    bool operator <=(const linear_inequality& other) const
    {
      return m_comparison<other.m_comparison ||
                  (m_comparison==other.m_comparison && (rhs()<other.rhs() ||
                        (rhs()==other.rhs() && lhs()<=other.lhs())));
    }

    bool operator >=(const linear_inequality& other) const
    {
      return other <= *this;
    }

    bool operator <(const linear_inequality& other) const
    {
      return m_comparison<other.m_comparison ||
                  (m_comparison==other.m_comparison && (rhs()<other.rhs() ||
                        (rhs()==other.rhs() && lhs()<other.lhs())));
    }

    bool operator >(const linear_inequality& other) const
    {
      return other < *this;
    } */

    data_expression get_factor_for_a_variable(const variable x)
    {
      return lhs()[x];
      /* for(const detail::variable_with_a_rational_factor& p:lhs())
      {
        if (p.variable_name()==x) 
        {
          return p.factor();
        }
      }
      return real_zero(); */
    }

    detail::comparison_t comparison() const
    {
      if (this->function()==detail::linear_inequality_less())
      {
        return detail::less;
      }
      else if (this->function()==detail::linear_inequality_less_equal())
      {
        return detail::less_eq;
      }
      assert(this->function()==detail::linear_inequality_less_equal());
      return detail::equal;
    }

    /* void set_comparison(detail::comparison_t c)
    {
      m_comparison=c;
    } */

    bool is_false(const rewriter& r) const
    {
      return lhs().empty() &&
             ((comparison()==detail::less_eq)?is_negative(rhs(),r):
              ((comparison()==detail::equal)?!is_zero(rhs()):!is_positive(rhs(),r)));
    }

    bool is_true(const rewriter& r) const
    {
      return lhs().empty() &&
             ((comparison()==detail::less_eq)?!is_negative(rhs(),r):
              ((comparison()==detail::equal)?is_zero(rhs()):is_positive(rhs(),r)));
    }

    /// \brief Return this inequality as a typical pair of terms of the form <x1+c2 x2+...+cn xn, d> where c2,...,cn, d are real constants.
    /// \return The return value indicates whether the left and right hand side have been negated
    ///         when yielding the typical pair.
    bool typical_pair(
      data_expression& lhs_expression,
      data_expression& rhs_expression,
      detail::comparison_t& comparison_operator,
      const rewriter& r) const
    {
      if (lhs_begin()==lhs_end())
      {
        lhs_expression=real_zero();
        rhs_expression=rhs();
        comparison_operator=comparison();
        return false;
      }

      data_expression factor=lhs_begin()->factor();

      for (detail::lhs_t::const_iterator i=lhs_begin(); i!=lhs_end(); ++i)
      {
        variable v=i->variable_name();
        data_expression e=real_times(rewrite_with_memory(real_divides(i->factor(),factor),r),
                                           data_expression(v));
        if (i==lhs_begin())
        {
          lhs_expression=e;
        }
        else
        {
          lhs_expression=real_plus(lhs_expression,e);
        }
      }

      rhs_expression=rewrite_with_memory(real_divides(rhs(),factor),r);
      if (is_negative(factor,r))
      {
        comparison_operator=negate(comparison());
        return true;
      }
      else
      {
        comparison_operator=comparison();
        return false;
      }
    } 

    linear_inequality invert(const rewriter& r)
    {
      const detail::lhs_t new_lhs(lhs().begin(),
                              lhs().end(),
                              [&](const detail::variable_with_a_rational_factor& p)
                                      { return detail::variable_with_a_rational_factor(
                                                    p.variable_name(),
                                                    rewrite_with_memory(sort_real::negate(p.factor()),r));});

      const data_expression new_rhs=rewrite_with_memory(sort_real::negate(rhs()),r);
      if (comparison()==detail::less)
      {
        // set_comparison(detail::less_eq);
        return linear_inequality(new_lhs,new_rhs,detail::less_eq);
      }
      else if (comparison()==detail::less_eq)
      {
        // set_comparison(detail::less);
        return linear_inequality(new_lhs,new_rhs,detail::less);
      }
      return linear_inequality(new_lhs,new_rhs,detail::equal);
    } 

    void add_variables(std::set < variable >&  variable_set) const
    {
      for (detail::lhs_t::const_iterator i=lhs().begin(); i!=lhs().end(); ++i)
      {
        variable_set.insert(i->variable_name());
      }
    } 
};

inline std::string pp(const linear_inequality& l)
{
  return pp(l.lhs()) + " " + detail::pp(l.comparison()) + " " + pp(l.rhs());
}

/// \brief Subtract the given equality, multiplied by f1/f2. The result is e1-(f1/f2)e2,
//         where the comparison operator of e1 is kept.
inline linear_inequality subtract(const linear_inequality& e1,
                                  const linear_inequality& e2,
                                  const data_expression f1,
                                  const data_expression f2,
                                  const rewriter& r)
{
  const data_expression f=rewrite_with_memory(real_divides(f1,f2),r);
  return linear_inequality(
                detail::meta_operation_lhs(e1.lhs(),e2.lhs(),
                              [&](const data_expression& d1, const data_expression& d2)->data_expression
                                         { return real_minus(d1,real_times(f,d2)); },r),
                rewrite_with_memory(real_minus(e1.rhs(),real_times(f,e2.rhs())),r),
                e1.comparison(),
                r);
}

// Real zero and real one are an ad hoc solution. They should be provided by
// the data type library.

inline data_expression& real_zero()
{
  static data_expression real_zero=sort_real::real_("0");
  return real_zero;
}

inline data_expression& real_one()
{
  static data_expression real_one=sort_real::real_("1");
  return real_one;
}

inline data_expression& real_minus_one()
{
  static data_expression real_minus_one=sort_real::real_("-1");
  return real_minus_one;
}

inline data_expression min(const data_expression e1,const data_expression e2,const rewriter& r)
{
  if (rewrite_with_memory(less_equal(e1,e2),r)==sort_bool::true_())
  {
    return e1;
  }
  if (rewrite_with_memory(less_equal(e2,e1),r)==sort_bool::true_())
  {
    return e2;
  }
  throw mcrl2::runtime_error("Fail to determine the minimum of: " + pp(e1) + " and " + pp(e2) + "\n");
}

inline data_expression max(const data_expression e1,const data_expression e2,const rewriter& r)
{
  if (rewrite_with_memory(less_equal(e2,e1),r)==sort_bool::true_())
  {
    return e1;
  }
  if (rewrite_with_memory(less_equal(e1,e2),r)==sort_bool::true_())
  {
    return e2;
  }
  throw mcrl2::runtime_error("Fail to determine the maximum of: " + pp(e1) + " and " + pp(e2) + "\n");
}


inline bool is_closed_real_number(const data_expression e)
{
  // TODO: Check that the number is closed.
  if (e.sort()!=sort_real::real_())
  {
    return false;
  }

  std::set < variable > s=find_all_variables(e);
  if (!s.empty())  // The expression e contains variables.
  {
    return false;
  }
  return true;
}

inline bool is_negative(const data_expression e,const rewriter& r)
{
  data_expression result=rewrite_with_memory(less(e,real_zero()),r);
  if (result==sort_bool::true_())
  {
    return true;
  }
  if (result==sort_bool::false_())
  {
    return false;
  }
  throw mcrl2::runtime_error("Cannot determine that " + pp(e) + " is smaller than 0");
}

inline bool is_positive(const data_expression e,const rewriter& r)
{
  data_expression result=rewrite_with_memory(greater(e,real_zero()),r);
  if (result==sort_bool::true_())
  {
    return true;
  }
  if (result==sort_bool::false_())
  {
    return false;
  }
  throw mcrl2::runtime_error("Cannot determine that " + pp(e) + " is larger than or equal to 0");
}

inline bool is_zero(const data_expression e)
{
  // Assume data_expression is in normal form.
  assert(is_closed_real_number(e));
  return (e==real_zero());
}

/// \brief Print the vector of inequalities to stderr in readable form.
template <class TYPE>
inline std::string pp_vector(const TYPE& inequalities)
{
  std::string s="[";
  bool first=true;
  for (const linear_inequality& l: inequalities) 
  {
    s=s+ (first?"":", ") + pp(l);
    first=false;
  }
  s=s+ "]";
  return s;
}

bool is_inconsistent(
  const std::vector < linear_inequality >& inequalities,
  const rewriter& r,
  const bool use_cache=true);


// Count the occurrences of variables that occur in inequalities.
inline
void count_occurrences(
  const std::vector < linear_inequality >& inequalities,
  std::map < variable, size_t>& nr_positive_occurrences,
  std::map < variable, size_t>& nr_negative_occurrences,
  const rewriter& r)
{
  for (std::vector < linear_inequality >::const_iterator i=inequalities.begin();
       i!=inequalities.end(); ++i)
  {
    for (detail::lhs_t::const_iterator j=i->lhs_begin(); j!=i->lhs_end(); ++j)
    {
      if (is_positive(j->factor(),r))
      {
        nr_positive_occurrences[j->variable_name()]=nr_positive_occurrences[j->variable_name()]+1;
      }
      else
      {
        nr_negative_occurrences[j->variable_name()]=nr_negative_occurrences[j->variable_name()]+1;
      }
    }
  }
}

template < class Variable_iterator >
std::set < variable >  gauss_elimination(
  const std::vector < linear_inequality >& inequalities,
  std::vector < linear_inequality >& resulting_equalities,
  std::vector < linear_inequality >& resulting_inequalities,
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
void fourier_motzkin(const std::vector < linear_inequality >& inequalities_in,
                     Data_variable_iterator variables_begin,
                     Data_variable_iterator variables_end,
                     std::vector < linear_inequality >& resulting_inequalities,
                     const rewriter& r)
{
  assert(resulting_inequalities.empty());
  if (mCRL2logEnabled(log::debug))
  {
    mCRL2log(log::debug) << "Starting Fourier-Motzkin elimination on " + pp_vector(inequalities_in) + " on variables ";
    for (Data_variable_iterator i=variables_begin;
         i!=variables_end; ++i)
    {
      mCRL2log(log::debug) << " " << pp(*i) ;
    }
    mCRL2log(log::debug) << std::endl;
  }

  std::vector < linear_inequality > inequalities;
  std::vector < linear_inequality > equalities;
  std::set < variable > vars=
  gauss_elimination(inequalities_in,
                    equalities,      // Store all resulting equalities here.
                    inequalities,    // Store all resulting non equalities here.
                    variables_begin,
                    variables_end,
                    r);

  // mCRL2log(log::debug) << "Fourier-Motzkin after Gauss elimination elimination on " + pp_vector(equalities) + "\n Inequalities " +
  //          pp_vector(inequalities) + "\n";
  // At this stage, the variables that should be eliminated only occur in
  // inequalities. Group the inequalities into positive, 0, and negative
  // occurrences of each variable, and create a new system.
  for (std::set < variable >::const_iterator i = vars.begin(); i != vars.end(); ++i)
  {
    std::map < variable, size_t> nr_positive_occurrences;
    std::map < variable, size_t> nr_negative_occurrences;
    count_occurrences(inequalities,nr_positive_occurrences,nr_negative_occurrences,r);

    bool found=false;
    size_t best_choice=0;
    variable best_variable;
    for (std::set < variable >::const_iterator k = vars.begin(); k != vars.end(); ++k)
    {
      const size_t p=nr_positive_occurrences[*k];
      const size_t n=nr_negative_occurrences[*k];
      if ((p!=0) || (n!=0))
      {
        if (found)
        {
          if (n*p<best_choice)
          {
            best_choice=n*p;
            best_variable=*k;
          }
        }
        else
        {
          // found is false
          best_choice=n*p;
          best_variable=*k;
          found=true;
        }
      }
      if (found && (best_choice==0))
      {
        // Stop searching, we cannot find a better candidate.
        break;
      }
    }

    // mCRL2log(log::debug) << "Best variable " << pp(best_variable) << "\n";

    if (!found)
    {
      // There are no variables anymore that can be removed from inequalities
      break;
    }
    std::vector < linear_inequality > new_inequalities;
    std::vector < linear_inequality> inequalities_with_positive_variable;
    std::vector < linear_inequality> inequalities_with_negative_variable;   // Idem.

    for (const linear_inequality& e: inequalities)
    {
      const detail::lhs_t::const_iterator factor_it=e.lhs().find(best_variable); 
      if (factor_it==e.lhs().end()) // variable best_variable does not occur in inequality e.
      {
        new_inequalities.push_back(e);
      }
      else
      {
        data_expression f=factor_it->factor();
        if (is_positive(f,r))
        {
          inequalities_with_positive_variable.push_back(e);
        }
        else if (is_negative(f,r))
        {
          inequalities_with_negative_variable.push_back(e);
        }
        else
        {
          assert(0);
        }
      }
    }

    // mCRL2log(log::debug) << "Positive :" << pp_vector(inequalities_with_positive_variable) << "\n";
    // mCRL2log(log::debug) << "Negative :" << pp_vector(inequalities_with_negative_variable) << "\n";
    // mCRL2log(log::debug) << "Equalities :" << pp_vector(equalities) << "\n";
    // mCRL2log(log::debug) << "Rest :" << pp_vector(new_inequalities) << "\n";

    // Variables are grouped, now construct new inequalities as follows:
    // Keep the zero occurrences
    // Combine each positive and negative equation as follows with x the best variable:
    // Given inequalities N + bi * x <= ci
    //                    M - bj * x <= cj
    // This is equivalent to N/bi + M/bj <= ci/bi + cj/bj
    for (const linear_inequality& e1: inequalities_with_positive_variable)
    {
      for (const linear_inequality e2: inequalities_with_negative_variable)
      {
        const detail::lhs_t::const_iterator e1_best_variable_it=e1.lhs().find(best_variable);
        const data_expression& e1_factor=e1_best_variable_it->factor();
        const data_expression& e1_reduced_rhs=real_divides(e1.rhs(),e1_factor);  
        const detail::lhs_t e1_reduced_lhs=detail::remove_variable_and_divide(e1.lhs(),best_variable,e1_factor,r);

        const detail::lhs_t::const_iterator e2_best_variable_it=e2.lhs().find(best_variable);
        const data_expression& e2_factor=e2_best_variable_it->factor();
        const data_expression& e2_reduced_rhs=real_divides(e2.rhs(),e2_factor);  
        const detail::lhs_t e2_reduced_lhs=detail::remove_variable_and_divide(e2.lhs(),best_variable,e2_factor,r);
        const linear_inequality new_inequality(subtract(e1_reduced_lhs,e2_reduced_lhs,r),
                                               r(real_minus(e1_reduced_rhs,e2_reduced_rhs)),
                                               (e1.comparison()==detail::less_eq) && (e2.comparison()==detail::less_eq)?
                                                   detail::less_eq:
                                                   detail::less,r);
        if (new_inequality.is_false(r))
        {
          resulting_inequalities.push_back(linear_inequality()); // This is a single contraditory inequality;
          return;
        }
        if (!new_inequality.is_true(r))
        {
          new_inequalities.push_back(new_inequality);
        }
      }
    }
    inequalities.swap(new_inequalities);
  }

  resulting_inequalities.swap(inequalities);
  // Add the equalities to the inequalities and return the result
  for (std::vector < linear_inequality > :: const_iterator i=equalities.begin();
       i!=equalities.end(); ++i)
  {
    assert(!i->is_false(r));
    if (!i->is_true(r))
    {
      resulting_inequalities.push_back(*i);
    }
  }
  mCRL2log(log::debug) << "Fourier-Motzkin elimination yields " << pp_vector(resulting_inequalities) << std::endl;
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
/// \return An indication whether the inequality referred to by i is inconsistent
///      in the context of inequalities.
inline bool is_a_redundant_inequality(
  const std::vector < linear_inequality >& inequalities,
  const std::vector < linear_inequality > :: iterator i,
  const rewriter& r)
{
#ifndef NDEBUG
  // Check that i points to some position in inequalities.
  bool found=false;
  for (std::vector < linear_inequality >:: const_iterator j=inequalities.begin() ;
       j!=inequalities.end() ; ++j)
  {
    if (j==i)
    {
      found=true;
      break;
    }
  }
  assert(found);
#endif
  // Check whether the inequalities, with the i-th equality with a reversed comparison operator is inconsistent.
  // If yes, the i-th inequality is redundant.
  if (i->comparison()==detail::equal)
  {
    // An inequality t==u is only redundant for equalities if
    // t<u and t>u are both inconsistent
    const linear_inequality old_inequality=*i;
    *i=linear_inequality(i->lhs(),i->rhs(),detail::less);
    if (is_inconsistent(inequalities,r))
    {
      *i=i->invert(r);
      if (is_inconsistent(inequalities,r))
      {
        *i=old_inequality; 
        return true;
      }
    }
    *i=old_inequality; 
    return false;
  }
  else
  {
    // an inequality t<u, t<=u, t>u and t>=u is redundant in equalities
    // if its inversion is inconsistent.
    const linear_inequality old_inequality=*i;
    *i=i->invert(r);
    if (is_inconsistent(inequalities,r))
    {
      *i=old_inequality;
      return true;
    }
    else
    {
      *i=old_inequality;
      return false;
    }
  }
}

/// \brief Remove every redundant inequality from a vector of inequalities.
/// \details If inequalities is inconsistent, [false] is returned. Otherwise
///          a list of inequalities is returned, from which no inequality can
///          be removed without changing the vector of solutions of the inequalities.
///          Redundancy of equalities is not checked, because this is quite expensive.
/// \param inequalities A list of inequalities
/// \param resulting_inequalities A list of inequalities to which the result is stored.
//                                Initially this list must be empty.
/// \param r A rewriter

inline void remove_redundant_inequalities(
  const std::vector < linear_inequality >& inequalities,
  std::vector < linear_inequality >& resulting_inequalities,
  const rewriter& r)
{
  assert(resulting_inequalities.empty());
  if (inequalities.empty())
  {
    return;
  }

  // If false is among the inequalities, [false] is the minimal result.
  if (is_inconsistent(inequalities,r))
  {
    resulting_inequalities.push_back(linear_inequality());
    return;
  }

  resulting_inequalities=inequalities;
  for (size_t i=0; i<resulting_inequalities.size();)
  {
    // Check whether the inequalities, with the i-th equality with a reversed comparison operator is inconsistent.
    // If yes, the i-th inequality is redundant.
    if (resulting_inequalities[i].comparison()==detail::equal)
    {
      // Do nothing, as removing redundant inequalities is expensive.
      i++;
    }
    else
    {
      if (is_a_redundant_inequality(resulting_inequalities,
                                    resulting_inequalities.begin()+i,
                                    r))
      {
        if (i+1<resulting_inequalities.size())
        {
          // Copy the last element to the current position.
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

//---------------------------------------------------------------------------------------------------

static void pivot_and_update(
  const variable xi,  // a basic variable
  const variable xj,  // a non basic variable
  const data_expression v,
  const data_expression v_delta_correction,
  std::map < variable,data_expression >& beta,
  std::map < variable,data_expression >& beta_delta_correction,
  std::set < variable >& basic_variables,
  std::map < variable, detail::lhs_t >& working_equalities,
  const rewriter& r)
{
  mCRL2log(log::debug) << "Pivoting " << pp(xi) << "   " << pp(xj) << "\n";
  const data_expression aij=working_equalities[xi][xj];
  const data_expression theta=rewrite_with_memory(real_divides(real_minus(v,beta[xi]),aij),r);
  const data_expression theta_delta_correction=rewrite_with_memory(real_divides(real_minus(v,beta_delta_correction[xi]),aij),r);
  beta[xi]=v;
  beta_delta_correction[xi]=v_delta_correction;
  beta[xj]=rewrite_with_memory(real_plus(beta[xj],theta),r);
  beta_delta_correction[xj]=rewrite_with_memory(real_plus(beta_delta_correction[xj],theta_delta_correction),r);

  mCRL2log(log::debug) << "Pivoting phase 0\n";
  for (std::set < variable >::const_iterator k=basic_variables.begin();
       k!=basic_variables.end(); ++k)
  {
    mCRL2log(log::debug) << "Working equalities " << *k << ":  " << pp(working_equalities[*k]) << "\n";
    if ((*k!=xi) && (working_equalities[*k].count(xj)>0))
    {
      const data_expression akj=working_equalities[*k][xj];
      beta[*k]=rewrite_with_memory(real_plus(beta[*k],real_times(akj ,theta)),r);
      beta_delta_correction[*k]=rewrite_with_memory(real_plus(beta_delta_correction[*k],real_times(akj ,theta_delta_correction)),r);
    }
  }
  // Apply pivoting on variables xi and xj;
  mCRL2log(log::debug) << "Pivoting phase 1\n";
  basic_variables.erase(xi);
  basic_variables.insert(xj);

  detail::lhs_t expression_for_xj=working_equalities[xi];
  expression_for_xj=expression_for_xj.erase(xj);
  expression_for_xj=set_factor_for_a_variable(expression_for_xj,xi,real_minus_one());
  expression_for_xj=multiply(expression_for_xj,real_divides(real_minus_one(),aij),r);
  mCRL2log(log::debug) << "Expression for xj:" << pp(expression_for_xj) << "\n";
  mCRL2log(log::debug) << "Pivoting phase 2\n";
  working_equalities.erase(xi);

  for (std::map < variable, detail::lhs_t >::iterator j=working_equalities.begin();
       j!=working_equalities.end(); ++j)
  {
    if (j->second.count(xj)>0)
    {
      const data_expression factor=j->second[xj];
      mCRL2log(log::debug) << "VAR: " << pp(j->first) << " Factor " << pp(factor) << "\n";
      j->second=j->second.erase(xj);
      // We need a temporary copy of expression_for_xj as otherwise the multiply
      // below will change this expression.
      //detail::lhs_t temporary_expression_for_xj(expression_for_xj);
      j->second=add(j->second,multiply(expression_for_xj,factor,r),r);
    }
  }

  working_equalities[xj]=expression_for_xj;

  mCRL2log(log::debug) << "Pivoting phase 3\n";
  // Calculate the values for beta and beta_delta_correction for the basic variables
  for (std::map < variable, detail::lhs_t >::const_iterator i=working_equalities.begin();
       i!=working_equalities.end() ; ++i)
  {
    beta[i->first]=i->second.evaluate(make_map_substitution(beta),r);
    beta_delta_correction[i->first]=i->second.evaluate(make_map_substitution(beta_delta_correction),r);
  }

  mCRL2log(log::debug) << "End pivoting " << pp(xj) << "\n";
  if (mCRL2logEnabled(log::debug))
  {
    for (std::map < variable,data_expression >::const_iterator i=beta.begin();
         i!=beta.end(); ++i)
    {
      mCRL2log(log::debug) << "(1) beta[" << pp(i->first) << "]= " << pp(beta[i->first]) << "+ delta* " << pp(beta_delta_correction[i->first]) << "\n";
    }
    for (std::map < variable, detail::lhs_t >::const_iterator i=working_equalities.begin();
         i!=working_equalities.end() ; ++i)
    {
      mCRL2log(log::debug) << "EQ: " << pp(i->first) << " := " << pp(i->second) << "\n";
    }
  }
}

namespace detail
{
  /* False end nodes could be associated with NULL */
  typedef enum { true_end_node, false_end_node, intermediate_node } node_type;
  class inequality_inconsistency_cache;
  class inequality_consistency_cache;

  // Sort the contents of a vector, if necessary.
  /* inline void conditional_sort(std::vector < linear_inequality >& v)
  {
    bool sorted=true;
    for(std::vector < linear_inequality >::const_iterator i=v.begin(); i!=v.end() && (i+1)!=v.end();  ++i)
    {
      if (*i>=*(i+1))
      {
        sorted=false;
      }
    }
    if (!sorted)
    {
      sort(v.begin(),v.end());
    }
  } */



  template <class CHILD>
  class inequality_inconsistency_cache_base
  {
    friend inequality_inconsistency_cache;
    friend inequality_consistency_cache;

    protected:
      node_type m_node;
      linear_inequality m_inequality;
      CHILD m_present_branch; 
      CHILD m_non_present_branch;

    public:

      inequality_inconsistency_cache_base(const node_type node)
        : m_node(node), m_present_branch(), m_non_present_branch() 
      {} 
    
      inequality_inconsistency_cache_base(
                  const node_type node,
                  const linear_inequality& inequality,
                  const CHILD& present_branch, 
                  const CHILD& non_present_branch)
        : m_node(node), 
          m_inequality(inequality), 
          m_present_branch(present_branch), 
          m_non_present_branch(non_present_branch) 
      {} 

      ~inequality_inconsistency_cache_base()
      {
        // Destroy the subtrees. At the moment this is possible, because
        // we do not use sharing of subtrees.
        // delete &m_present_branch;
        // delete &m_non_present_branch;
      }
    

  };

  class inequality_inconsistency_cache
  {
    protected:
      typedef inequality_inconsistency_cache_base<inequality_inconsistency_cache> cache_type;

      cache_type* m_cache;


    public:

      inequality_inconsistency_cache(
                  const node_type node,
                  const linear_inequality& inequality,
                  const inequality_inconsistency_cache& present_branch,
                  const inequality_inconsistency_cache& non_present_branch)
        : m_cache(new cache_type(node,inequality,present_branch,non_present_branch))
      {}  

      inequality_inconsistency_cache(const node_type node)
        : m_cache(new cache_type(node))
      {
      }  

      inequality_inconsistency_cache()
        : m_cache(NULL)
      {}  

      ~inequality_inconsistency_cache()
      {
        if (m_cache!=NULL)
        {
          // delete m_cache;
        }
      }

      bool is_inconsistent(const std::vector < linear_inequality >& inequalities_in_) const
      {
        std::set < linear_inequality > inequalities_in(inequalities_in_.begin(),inequalities_in_.end());
// std::cerr << "IS INCONSISTENT " << pp_vector(inequalities_in) << "\n";
        cache_type* current_root=m_cache;
        for(linear_inequality l: inequalities_in)
        {
          /* First walk down the three until an endnode is found
             that with l<=current_root.m_inequality. */
          while (current_root->m_node==intermediate_node && l>current_root->m_inequality)
          {
            current_root=current_root->m_non_present_branch.m_cache;
          }
          if (current_root->m_node==intermediate_node)
          {
            if (l==current_root->m_inequality)
            {
              current_root=current_root->m_present_branch.m_cache;
            }
            assert(current_root->m_node!=intermediate_node || l<current_root->m_inequality);
          }
          else
          {
            if (current_root->m_node==true_end_node)
            {
// std::cerr << "IS INCONSISTENT: TRUE 1\n";
              return true;
            }
            else
            {
// std::cerr << "IS INCONSISTENT: FALSE 1\n";
              return false;
            }
          }
        }
        if (current_root->m_node==true_end_node)
        {
// std::cerr << "IS INCONSISTENT: TRUE 2\n";
          return true;
        }
        else
        {
// std::cerr << "IS INCONSISTENT: FALSE 2\n";
          return false;
        }
      }

      void add_inconsistent_inequality_set(const std::vector < linear_inequality >& inequalities_in_)
      {
        std::set < linear_inequality > inequalities_in(inequalities_in_.begin(),inequalities_in_.end());
//std::cerr << "ADD INCONSISTENT " << pp_vector(inequalities_in) << "\n";
        cache_type** current_root=&m_cache;
        for(linear_inequality l: inequalities_in)
        {
//std::cerr << "CHECK INPUT " << pp(l) << "\n";
          /* First walk down the three until an endnode is found
                that with l<=current_root->m_inequality. */
          while ((*current_root)->m_node==intermediate_node && l>(*current_root)->m_inequality)
          {
//std::cerr << "INSERT WHILE TREE " << pp((*current_root)->m_inequality) << "\n";
            current_root=&((*current_root)->m_non_present_branch.m_cache);
          }
          if ((*current_root)->m_node==intermediate_node)
          {
            if (l==(*current_root)->m_inequality)
            {
//std::cerr << "INSERT EQUAL " << pp((*current_root)->m_inequality) << "\n";
              current_root=&((*current_root)->m_present_branch.m_cache);
              assert((*current_root)->m_node!=intermediate_node || l<(*current_root)->m_inequality);
            }
            else 
            {
//std::cerr << "INSERT NODE BEFORE NEXT NODE " << pp((*current_root)->m_inequality) << "\n";
              // Add the node.
              inequality_inconsistency_cache new_false_node(false_end_node); 
              cache_type* new_node = inequality_inconsistency_cache(intermediate_node,l,new_false_node,*reinterpret_cast<inequality_inconsistency_cache*>(current_root)).m_cache;
              *current_root=new_node;
              current_root = &(new_node->m_present_branch.m_cache);
            }
          }
          else
          {
//std::cerr << "AT AN ENDNODE " << pp((*current_root)->m_inequality) << "\n";
            if ((*current_root)->m_node==true_end_node)
            {
              // A shorter sequence that the linear inequality set is already inconsistent.
              // This should not occur, assuming that the linear inequality sequence is checked
              // in this cache, before being proven inconsistent.
              assert(0);
            }
            else
            {
              // Add the remaining sequence. 
//std::cerr << "AT A FALSE  ENDNODE " << pp((*current_root)->m_inequality) << "\n";
              inequality_inconsistency_cache new_false_node(false_end_node); 
              cache_type* new_node = inequality_inconsistency_cache(
                                   intermediate_node,l,new_false_node,*reinterpret_cast<inequality_inconsistency_cache*>(current_root)).m_cache;
              *current_root=new_node;
              current_root = &(new_node->m_present_branch.m_cache);
            }
          }
        }
        // At this point the sequence of inequalities has been explored, but the tree has not ended.
        // We expect the current node to be a true_end_node. If not, we replace it by one.
        if ((*current_root)->m_node!=true_end_node)
        {
//std::cerr << "SET TRUE  ENDNODE " << pp((*current_root)->m_inequality) << "\n";
          *current_root=inequality_inconsistency_cache(true_end_node).m_cache;
        }
      }
  };

  class inequality_consistency_cache
  {
    protected:
      typedef inequality_inconsistency_cache_base<inequality_consistency_cache> cache_type;

      cache_type* m_cache;

      // inequality_consistency_cache(const inequality_consistency_cache& ); // Non construction copyable.
      // inequality_consistency_cache& operator=(const inequality_consistency_cache& ); // Non copyable.

    public:

      inequality_consistency_cache(
                  const node_type node,
                  const linear_inequality& inequality,
                  const inequality_consistency_cache& present_branch,
                  const inequality_consistency_cache& non_present_branch)
        : m_cache(new cache_type(node,inequality,present_branch,non_present_branch))
      {
// std::cerr << "CREATED CONSISTENCY NODE: " << m_cache << " LEFT: " << m_cache->m_present_branch.m_cache << " RIGHT: " << m_cache->m_non_present_branch.m_cache << "\n";
      }  

      inequality_consistency_cache()
        : m_cache(NULL)
      {}  

      inequality_consistency_cache(const node_type node)
        : m_cache(new cache_type(node))
      {
// std::cerr << "CREATED CONSISTENCY NODE: " << m_cache << " VALUE: " << node << "\n";
      }  

      ~inequality_consistency_cache()
      {
        if (m_cache!=NULL)
        {
          // delete m_cache;
        }
      }

      // Sort the vector inequalities_in if not sorted.
      bool is_consistent(const std::vector < linear_inequality >& inequalities_in_) const
      {
        std::set < linear_inequality > inequalities_in(inequalities_in_.begin(),inequalities_in_.end());
// std::cerr << "IS CONSISTENT " << pp_vector(inequalities_in) << "\n";
        cache_type* current_root=m_cache;
        for(std::set < linear_inequality >::const_iterator i=inequalities_in.begin(); i!=inequalities_in.end(); ++i)
        {
// std::cerr << "CHECK INPUT " << pp(*i) << "\n";
          while (current_root->m_node==intermediate_node && *i>current_root->m_inequality)
          {
// std::cerr << "WHILE TREE " << pp(current_root->m_inequality) << "\n";
            current_root=current_root->m_non_present_branch.m_cache;
          }
          if (current_root->m_node==intermediate_node)
          {
            if (*i==current_root->m_inequality)
            {
// std::cerr << "EQUAL TREE " << pp(current_root->m_inequality) << "\n";
              current_root=current_root->m_present_branch.m_cache;
            }
            else
            {
//std::cerr << "IS CONSISTENT: FALSE 1\n";
              return false; // there are more inequalities than available in the tree. We know nothing about being consistent.
            }
// std::cerr << "WASDADAN " << pp(current_root->m_inequality) << "\n";
            assert(current_root->m_node!=intermediate_node || *i<current_root->m_inequality);
          }
          else
          {
            if (current_root->m_node==false_end_node)
            {
//std::cerr << "IS CONSISTENT: FALSE 2\n";
              return false;
            }
            if (i==inequalities_in.end())
            {
//std::cerr << "IS CONSISTENT: TRUE 1\n";
              return true;
            }
            else 
            {
//std::cerr << "IS CONSISTENT: FALSE 3\n";
              return false;
            }
          }
        }
//std::cerr << "IS CONSISTENT: TRUE 2\n";
        return true;
      }

      void add_consistent_inequality_set(const std::vector < linear_inequality >& inequalities_in_)
      {
        std::set < linear_inequality > inequalities_in(inequalities_in_.begin(),inequalities_in_.end());
// std::cerr << "ADD CONSISTENT " << pp_vector(inequalities_in) << "\n";
        cache_type** current_root=&m_cache;
        for(linear_inequality l: inequalities_in)
        {
// std::cerr << "INSERT " << pp(l) << std::endl;
          /* First walk down the three until an endnode is found
             with l<=current_root->m_inequality. */
          while ((*current_root)->m_node==intermediate_node && l>(*current_root)->m_inequality)
          {
// std::cerr << "INSERT WHILE TREE " << pp((*current_root)->m_inequality) << "\n";
            current_root=&((*current_root)->m_non_present_branch.m_cache);
          }
          if ((*current_root)->m_node==intermediate_node)
          {
            if (l==(*current_root)->m_inequality)
            {
// std::cerr << "INSERT EQUAL TREE " << pp((*current_root)->m_inequality) << "\n";
              current_root=&((*current_root)->m_present_branch.m_cache);
              assert((*current_root)->m_node!=intermediate_node || l<(*current_root)->m_inequality);
            }
            else 
            {
// std::cerr << "INSERT NODE BEFORE NEXT NODE " << pp((*current_root)->m_inequality) << "\n";
              // Add the node.
              inequality_consistency_cache new_true_node(true_end_node); 
              cache_type* new_node = inequality_consistency_cache(intermediate_node,l,new_true_node,*reinterpret_cast<inequality_consistency_cache*>(current_root)).m_cache;
              *current_root=new_node;
              current_root = &(new_node->m_present_branch.m_cache);
            }
          }
          else
          {
// std::cerr << "INSERT ADD NODE " << pp((*current_root)->m_inequality) << "\n";
            // Add the remaining sequence. 
            inequality_consistency_cache new_true_node(true_end_node); 
            cache_type* new_node = inequality_consistency_cache(intermediate_node,l,new_true_node,*reinterpret_cast<inequality_consistency_cache*>(current_root)).m_cache;
            *current_root=new_node;
            current_root = &(new_node->m_present_branch.m_cache);
          }
        }
      }
  };
  
}


/// \brief Determine whether a list of data expressions is inconsistent
/// \details First it is checked whether false is among the input. If
///          not, Fourier-Motzkin is applied to all variables in the
///          inequalities. If the empty vector of equalities is the result,
///          the input was consistent. Otherwise the resulting vector contains
///          an inconsistent inequality.
///          The implementation uses a feasible point detection algorithm as described by
///          Bruno Dutertre and Leonardo de Moura. Integrating Simplex with DPLL(T).
///          CSL Technical Report SRI-CSL-06-01, 2006.
/// \param inequalities A list of inequalities
/// \param r A rewriter
/// \return true if the system of inequalities can be determined to be
///      inconsistent, false otherwise.


inline bool is_inconsistent(
  const std::vector < linear_inequality >& inequalities_in,
  const rewriter& r,
  const bool use_cache/*=true*/)
{
  // Transform the linear inequalities into a vector of equalities and a
  // sequence of constraints on variables. All variables, including
  // those that will be generated as slack variables will have values indicated
  // by beta, which must lie between the lowerbounds and the upperbounds.

  // First remove all equalities by Gauss elimination and make a fresh variable
  // generator.

  mCRL2log(log::debug) << "Starting an inconsistency check on " + pp_vector(inequalities_in) << "\n";

  static detail::inequality_inconsistency_cache inconsistency_cache(detail::false_end_node);
  static detail::inequality_consistency_cache consistency_cache(detail::false_end_node);

  if (use_cache && consistency_cache.is_consistent(inequalities_in))
  {
    assert(!is_inconsistent(inequalities_in,r,false));
    return false;
  }
  if (use_cache && inconsistency_cache.is_inconsistent(inequalities_in))
  {
    assert(is_inconsistent(inequalities_in,r,false));
    return true;
  } 
  // The required data structures
  std::map < variable,data_expression > lowerbounds;
  std::map < variable,data_expression > upperbounds;
  std::map < variable,data_expression > beta;
  std::map < variable,data_expression > lowerbounds_delta_correction;
  std::map < variable,data_expression > upperbounds_delta_correction;
  std::map < variable,data_expression > beta_delta_correction;
  std::set < variable > non_basic_variables;
  std::set < variable > basic_variables;
  std::map < variable, detail::lhs_t > working_equalities;

  set_identifier_generator fresh_variable_name;

  for (std::vector < linear_inequality >::const_iterator i=inequalities_in.begin();
       i!=inequalities_in.end(); ++i)
  {
    if (i->is_false(r))
    {
      mCRL2log(log::debug) << "Inconsistent, because linear inequalities contains an inconsistent inequality\n";
      if (use_cache) 
      { 
        inconsistency_cache.add_inconsistent_inequality_set(inequalities_in); // Necessary? Only false should be added.
        assert(inconsistency_cache.is_inconsistent(inequalities_in));
      }
      return true;
    }
    i->add_variables(non_basic_variables);
    for (detail::lhs_t::const_iterator j=i->lhs_begin();
         j!=i->lhs_end(); ++j)
    {
      fresh_variable_name.add_identifier(j->variable_name().name());
    }
  }

  std::vector < linear_inequality > inequalities;
  std::vector < linear_inequality > equalities;
  non_basic_variables=
    gauss_elimination(inequalities_in,
                      equalities,      // Store all resulting equalities here.
                      inequalities,    // Store all resulting non equalities here.
                      non_basic_variables.begin(),
                      non_basic_variables.end(),
                      r);

  assert(equalities.size()==0); // There are no resulting equalities left.
  non_basic_variables.clear(); // gauss_elimination has removed certain variables. So, we reconstruct the non
  // basic variables again below.

  mCRL2log(log::debug) << "Resulting equalities " << pp_vector(equalities) << "\n";
  mCRL2log(log::debug) << "Resulting inequalities " << pp_vector(inequalities) << "\n";

  // Now bring the linear equalities in the basic form described
  // in the article by Bruno Dutertre and Leonardo de Moura.

  // First set lower and upperbounds, and introduce slack variables
  // if required.
  for (std::vector < linear_inequality >::iterator i=inequalities.begin();
       i!=inequalities.end(); ++i)
  {
    mCRL2log(log::debug) << "Investigate inequality: " << ":=" << pp(*i) << " ||  " << pp(i->lhs()) << "\n";
    if (i->is_false(r))
    {
      mCRL2log(log::debug) << "Inconsistent, because linear inequalities contains an inconsistent inequality after Gauss elimination\n";
      if (use_cache) 
      {
        inconsistency_cache.add_inconsistent_inequality_set(inequalities_in); // Necessary? Only false should be added.
        assert(inconsistency_cache.is_inconsistent(inequalities_in));
      }
      return true;
    }
    if (!i->is_true(r))  // This inequality is redundant and can be skipped.
    {
      assert(i->comparison()!=detail::equal);
      assert(i->lhs().size()>0); // this signals a redundant or an inconsistent inequality.
      i->add_variables(non_basic_variables);

      if (i->lhs().size()==1)  // the left hand side consists of a single variable.
      {
        variable v=i->lhs_begin()->variable_name();
        data_expression factor=i->lhs_begin()->factor();
        assert(factor!=real_zero());
        data_expression bound=rewrite_with_memory(real_divides(i->rhs(),factor),r);
        if (is_positive(factor,r))
        {
          // The inequality has the shape factor*v<=c or factor*v<c with factor positive
          if ((upperbounds.count(v)==0) ||
              (rewrite_with_memory(less(bound,upperbounds[v]),r)==sort_bool::true_()))
          {
            upperbounds[v]=bound;
            upperbounds_delta_correction[v]=
              ((i->comparison()==detail::less)?real_minus_one():real_zero());

          }
          else
          {
            if (bound==upperbounds[v])
            {
              upperbounds_delta_correction[v]=
                min(upperbounds_delta_correction[v],
                    ((i->comparison()==detail::less)?real_minus_one():real_zero()),r);
            }
          }
        }
        else
        {
          // The inequality has the shape factor*v<=c or factor*v<c with factor negative
          if ((lowerbounds.count(v)==0) ||
              (rewrite_with_memory(less(lowerbounds[v],bound),r)==sort_bool::true_()))
          {
            lowerbounds[v]=bound;
            lowerbounds_delta_correction[v]=
              ((i->comparison()==detail::less)?real_one():real_zero());
          }
          else
          {
            if (bound==lowerbounds[v])
            {
              lowerbounds_delta_correction[v]=
                max(lowerbounds_delta_correction[v],
                    ((i->comparison()==detail::less)?real_one():real_zero()),r);
            }
          }
        }
      }
      else
      {
        // The inequality has more than one variable at the left hand side.
        // We transform it into an equation with a new slack variable.
        variable new_basic_variable(fresh_variable_name("slack_var"), sort_real::real_());
        basic_variables.insert(new_basic_variable);
        upperbounds[new_basic_variable]=i->rhs();
        upperbounds_delta_correction[new_basic_variable]=
          ((i->comparison()==detail::less)?real_minus_one():real_zero());
        working_equalities[new_basic_variable]=i->lhs();
        mCRL2log(log::debug) << "New slack variable: " << pp(new_basic_variable) << ":=" << pp(*i) << "   " << pp(i->lhs()) << "\n";
      }
    }
  }
  // Now set the values for beta:
  // The beta values for the non basic variables must satisfy the lower and
  // upperbounds.
  for (std::set < variable >::const_iterator i=non_basic_variables.begin();
       i!=non_basic_variables.end(); ++i)
  {
    if (lowerbounds.count(*i)>0)
    {
      if ((upperbounds.count(*i)>0) &&
          ((rewrite_with_memory(less(upperbounds[*i],lowerbounds[*i]),r)==sort_bool::true_()) ||
           ((upperbounds[*i]==lowerbounds[*i]) &&
            (rewrite_with_memory(less(upperbounds_delta_correction[*i],lowerbounds_delta_correction[*i]),r)==sort_bool::true_()))))
      {
        mCRL2log(log::debug) << "Inconsistent, preprocessing " << pp(*i) << "\n";
        if (use_cache) 
        {
          inconsistency_cache.add_inconsistent_inequality_set(inequalities_in); 
          assert(inconsistency_cache.is_inconsistent(inequalities_in));
        }
        return true; // Inconsistent.
      }
      beta[*i]=lowerbounds[*i];
      beta_delta_correction[*i]=lowerbounds_delta_correction[*i];
    }
    else if (upperbounds.count(*i)>0)
    {
      beta[*i]=upperbounds[*i];
      beta_delta_correction[*i]=upperbounds_delta_correction[*i];
    }
    else // *i has neither a lower or an upperbound
    {
      beta[*i]=real_zero();
      beta_delta_correction[*i]=real_zero();
    }
    mCRL2log(log::debug) << "(2) beta[" << pp(*i) << "]=" << pp(beta[*i])<< "+delta*" << pp(beta_delta_correction[*i]) <<"\n";
  }

  // Subsequently set the values for the basic variables
  for (std::set < variable >::const_iterator i=basic_variables.begin();
       i!=basic_variables.end(); ++i)
  {
    beta[*i]=working_equalities[*i].evaluate(make_map_substitution(beta),r);
    beta_delta_correction[*i]=working_equalities[*i].
                              evaluate(make_map_substitution(beta_delta_correction),r);
    mCRL2log(log::debug) << "(3) beta[" << pp(*i) << "]=" << pp(beta[*i])<< "+delta*" << pp(beta_delta_correction[*i]) <<"\n";
  }

  // Now the basic data structure has been set up.
  // We must find the first basic variable that does not satisfy its
  // upper and bounds. This is essentially the check algorithm in the
  // article by Bruno Dutertre and Leonardo de Moura.

  for (; true ;)
  {
    // select the smallest basic variable that does not satisfy the bounds.
    bool found=false;
    bool lowerbound_violation = false;
    variable xi;
    for (std::set < variable > :: const_iterator i=basic_variables.begin() ;
         i!=basic_variables.end() ; ++i)
    {
      mCRL2log(log::debug) << "Evaluate start\n";
      assert(!found);
      data_expression value=beta[*i]; // working_equalities[*i].evaluate(beta,r);
      data_expression value_delta_correction=beta_delta_correction[*i]; // working_equalities[*i].evaluate(beta_delta_correction,r);
      mCRL2log(log::debug) << "Evaluate end\n";
      if ((upperbounds.count(*i)>0) &&
          ((rewrite_with_memory(less(upperbounds[*i],value),r)==sort_bool::true_()) ||
           ((upperbounds[*i]==value) &&
            (rewrite_with_memory(less(upperbounds_delta_correction[*i],value_delta_correction),r)==sort_bool::true_()))))
      {
        // The value of variable *i does not satisfy its upperbound. This must
        // be corrected using pivoting.
        mCRL2log(log::debug) << "Upperbound violation " << pp(*i) << "  bound: " << pp(upperbounds[*i]) << "\n";
        found=true;
        xi=*i;
        lowerbound_violation=false;
        break;
      }
      else if ((lowerbounds.count(*i)>0) &&
               ((rewrite_with_memory(less(value,lowerbounds[*i]),r)==sort_bool::true_()) ||
                ((lowerbounds[*i]==value) &&
                 (rewrite_with_memory(less(value_delta_correction,lowerbounds_delta_correction[*i]),r)==sort_bool::true_()))))
      {
        // The value of variable *i does not satisfy its upperbound. This must
        // be corrected using pivoting.
        mCRL2log(log::debug) << "Lowerbound violation " << pp(*i) << "  bound: " << pp(lowerbounds[*i]) << "\n";
        found=true;
        xi=*i;
        lowerbound_violation=true;
        break;
      }
    }
    if (!found)
    {
      // The inequalities are consistent. Return false.
      mCRL2log(log::debug) << "Consistent while pivoting\n";
      if (use_cache) 
      {
        consistency_cache.add_consistent_inequality_set(inequalities_in); 
        assert(consistency_cache.is_consistent(inequalities_in));
      }
      return false;
    }

    mCRL2log(log::debug) << "The smallest basic variable that does not satisfy the bounds is " << pp(xi) << "\n";
    if (lowerbound_violation)
    {
      mCRL2log(log::debug) << "Lowerbound violation \n";
      // select the smallest non-basic variable with which pivoting can take place.
      bool found=false;
      const detail::lhs_t& lhs=working_equalities[xi];
      for (detail::lhs_t::const_iterator xj_it=lhs.begin(); xj_it!=lhs.end(); ++xj_it)
      {
        const variable xj=xj_it->variable_name();
        mCRL2log(log::debug) << pp(xj) << "  --  " << pp(xj_it->factor()) << "\n";
        if ((is_positive(xj_it->factor(),r) &&
             ((upperbounds.count(xj)==0) ||
              ((rewrite_with_memory(less(beta[xj],upperbounds[xj]),r)==sort_bool::true_())||
               ((beta[xj]==upperbounds[xj])&& (rewrite_with_memory(less(beta_delta_correction[xj],upperbounds_delta_correction[xj]),r)==sort_bool::true_()))))) ||
            (is_negative(xj_it->factor(),r) &&
             ((lowerbounds.count(xj)==0) ||
              ((rewrite_with_memory(greater(beta[xj],lowerbounds[xj]),r)==sort_bool::true_())||
               ((beta[xj]==lowerbounds[xj]) && (rewrite_with_memory(greater(beta_delta_correction[xj],lowerbounds_delta_correction[xj]),r)==sort_bool::true_()))))))
        {
          found=true;
          pivot_and_update(xi,xj,lowerbounds[xi],lowerbounds_delta_correction[xi],
                           beta, beta_delta_correction,
                           basic_variables,working_equalities,r);
          break;
        }
      }
      if (!found)
      {
        // The inequalities are inconsistent.
        mCRL2log(log::debug) << "Inconsistent while pivoting\n";
        if (use_cache) 
        {
          inconsistency_cache.add_inconsistent_inequality_set(inequalities_in); 
          assert(inconsistency_cache.is_inconsistent(inequalities_in));
        }
        return true;
      }

    }
    else  // Upperbound violation.
    {
      mCRL2log(log::debug) << "Upperbound violation \n";
      // select the smallest non-basic variable with which pivoting can take place.
      bool found=false;
      for (detail::lhs_t::const_iterator xj_it=working_equalities[xi].begin();
           xj_it!=working_equalities[xi].end(); ++xj_it)
      {
        const variable xj=xj_it->variable_name();
        mCRL2log(log::debug) << pp(xj) << "  --  " << pp(xj_it->factor()) <<  " POS " << is_positive(xj_it->factor(),r) << "\n";
        if ((is_negative(xj_it->factor(),r) &&
             ((upperbounds.count(xj)==0) ||
              ((rewrite_with_memory(less(beta[xj],upperbounds[xj]),r)==sort_bool::true_()) ||
               ((beta[xj]==upperbounds[xj])&& (rewrite_with_memory(less(beta_delta_correction[xj],upperbounds_delta_correction[xj]),r)==sort_bool::true_()))))) ||
            (is_positive(xj_it->factor(),r) &&
             ((lowerbounds.count(xj)==0) ||
              ((rewrite_with_memory(greater(beta[xj],lowerbounds[xj]),r)==sort_bool::true_()) ||
               ((beta[xj]==lowerbounds[xj]) && (rewrite_with_memory(greater(beta_delta_correction[xj],lowerbounds_delta_correction[xj]),r)==sort_bool::true_()))))))
        {
          found=true;
          pivot_and_update(xi,xj,upperbounds[xi],upperbounds_delta_correction[xi],
                           beta,beta_delta_correction,
                           basic_variables,working_equalities,r);
          break;
        }
      }
      if (!found)
      {
        // The inequalities are inconsistent.
        mCRL2log(log::debug) << "Inconsistent while pivoting (1)\n";
        if (use_cache) 
        {
          inconsistency_cache.add_inconsistent_inequality_set(inequalities_in); 
          assert(inconsistency_cache.is_inconsistent(inequalities_in));
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
/// \return The variables that could not be removed by gauss elimination.

template < class Variable_iterator >
std::set < variable >  gauss_elimination(
  const std::vector < linear_inequality >& inequalities,
  std::vector < linear_inequality >& resulting_equalities,
  std::vector < linear_inequality >& resulting_inequalities,
  Variable_iterator variables_begin,
  Variable_iterator variables_end,
  const rewriter& r)
{
  std::set < variable >  remaining_variables;

  // First copy equalities to the resulting_equalities and the inequalites to resulting_inequalities.
  for (std::vector < linear_inequality > ::const_iterator j = inequalities.begin(); j != inequalities.end(); ++j)
  {
    if (j->is_false(r))
    {
      // The input contains false. Return false and stop.
      resulting_equalities.clear();
      resulting_inequalities.clear();
      resulting_inequalities.push_back(linear_inequality());
      return remaining_variables;
    }
    else if (!j->is_true(r)) // Do not consider redundant equations.
    {
      if (j->comparison()==detail::equal)
      {
        resulting_equalities.push_back(*j);
      }
      else
      {
        resulting_inequalities.push_back(*j);
      }
    }
  }

  // Now find out whether there are variables that occur in an equality, so
  // that we can perform gauss elimination.
  for (Variable_iterator i = variables_begin; i != variables_end; ++i)
  {
    size_t j;
    for (j=0; j<resulting_equalities.size(); ++j)
    {
      bool check_equalities_for_redundant_inequalities(false);
      std::set < variable > vars;
      resulting_equalities[j].add_variables(vars);
      if (vars.count(*i)>0)
      {
        // Equality *j contains data variable *i.
        // Perform gauss elimination, and break the loop.

        for (size_t k = 0; k < resulting_inequalities.size();)
        {
          resulting_inequalities[k]=subtract(resulting_inequalities[k],
                                             resulting_equalities[j],
                                             resulting_inequalities[k].get_factor_for_a_variable(*i),
                                             resulting_equalities[j].get_factor_for_a_variable(*i),
                                             r);
          if (resulting_inequalities[k].is_false(r))
          {
            // The input is inconsistent. Return false.
            resulting_equalities.clear();
            resulting_inequalities.clear();
            resulting_inequalities.push_back(linear_inequality());
            remaining_variables.clear();
            return remaining_variables;
          }
          else if (resulting_inequalities[k].is_true(r))
          {
            // Inequality k has become redundant, and can be removed.
            if ((k+1)<resulting_inequalities.size())
            {
              resulting_inequalities[k].swap(resulting_inequalities.back());
            }
            resulting_inequalities.pop_back();
          }
          else
          {
            ++k;
          }
        }

        for (size_t k = 0; k<resulting_equalities.size();)
        {
          if (k==j)
          {
            ++k;
          }
          else
          {
            resulting_equalities[k]=subtract(
              resulting_equalities[k],
              resulting_equalities[j],
              resulting_equalities[k].get_factor_for_a_variable(*i),
              resulting_equalities[j].get_factor_for_a_variable(*i),
              r);
            if (resulting_equalities[k].is_false(r))
            {
              // The input is inconsistent. Return false.
              resulting_equalities.clear();
              resulting_inequalities.clear();
              resulting_inequalities.push_back(linear_inequality());
              remaining_variables.clear();
              return remaining_variables;
            }
            else if (resulting_equalities[k].is_true(r))
            {
              // Equality k has become redundant, and can be removed.
              if (j+1==resulting_equalities.size())
              {
                // It is not possible to move move the last element of resulting
                // inequalities to position k, because j is at this last position.
                // Hence, we must recall to check the resulting_equalities for inequalities
                // that are true.
                check_equalities_for_redundant_inequalities=true;
              }
              else
              {
                if ((k+1)<resulting_equalities.size())
                {
                  resulting_equalities[k].swap(resulting_equalities.back());
                }
                resulting_equalities.pop_back();
              }
            }
            else
            {
              ++k;
            }
          }
        }

        // Remove equation j.

        if (j+1<resulting_equalities.size())
        {
          resulting_equalities[j].swap(resulting_equalities.back());
        }
        resulting_equalities.pop_back();

        // If there are unremoved resulting equalities, remove them now.
        if (check_equalities_for_redundant_inequalities)
        {
          for (size_t k = 0; k<resulting_equalities.size();)
          {
            if (resulting_equalities[k].is_true(r))
            {
              // Equality k is redundant, and can be removed.
              if ((k+1)<resulting_equalities.size())
              {
                resulting_equalities[k].swap(resulting_equalities.back());
              }
              resulting_equalities.pop_back();
            }
            else
            {
              ++k;
            }
          }
        }
      }
    }
    remaining_variables.insert(*i);
  }

  return remaining_variables;
}

// The introduction of the function rewrite_with_memory using a
// hash table here is a temporary trick, to boost
// performance, which is slow due to translations necessary from and to
// rewrite format.

inline data_expression rewrite_with_memory(
  const data_expression t,const rewriter& r)
{
  static std::map < data_expression, data_expression > rewrite_hash_table;
  std::map < data_expression, data_expression > :: iterator i=rewrite_hash_table.find(t);
  if (i==rewrite_hash_table.end())
  {
    data_expression t1=r(t);
    return t1;
  }
  return i->second;
}


} // namespace data

} // namespace mcrl2

#endif // MCRL2_LPSREALELM_LINEAR_INEQUALITY_H
