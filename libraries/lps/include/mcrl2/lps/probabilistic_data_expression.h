// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This file contains a class that contains labels for probabilistic transitions as a mCRL2 data expression of type real.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LPS_PROBABILISTIC_DATA_EXPRESSION_H
#define MCRL2_LPS_PROBABILISTIC_DATA_EXPRESSION_H

#include "mcrl2/utilities/big_numbers.h"
#include "mcrl2/utilities/probabilistic_arbitrary_precision_fraction.h"
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/data/rewriter.h"

namespace mcrl2::lps
{

namespace detail
{
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  inline utilities::big_natural_number make_bnn_nat(const data::data_expression& n)
  {
    utilities::big_natural_number result;
    data::data_expression const* n_walker=&n;
    while (data::sort_nat::is_concat_digit_application(*n_walker))
    {
      result.push_back(atermpp::down_cast<data::machine_number>(data::sort_nat::arg2(*n_walker)).value());
      n_walker=&data::sort_nat::arg1(*n_walker);
    }
    std::size_t v=atermpp::down_cast<data::machine_number>(data::sort_nat::arg(*n_walker)).value();
    assert(data::sort_nat::is_most_significant_digit_nat_application(*n_walker));
    if (v>0) 
    {
      result.push_back(v);
    }
    return result;
  }

  inline utilities::big_natural_number make_bnn_pos(const data::data_expression& n)
  {
    utilities::big_natural_number result;
    data::data_expression const* n_walker=&n;
    while (data::sort_pos::is_concat_digit_application(*n_walker))
    {
      result.push_back(atermpp::down_cast<data::machine_number>(data::sort_pos::arg2(*n_walker)).value());
      n_walker=&data::sort_pos::arg1(*n_walker);
    }
    assert(data::sort_pos::is_most_significant_digit_application(*n_walker));
    result.push_back(atermpp::down_cast<data::machine_number>(data::sort_pos::arg(*n_walker)).value());
    return result;
  }

  inline const data::data_expression make_nat_bnn(const utilities::big_natural_number& b)
  {
    std::size_t pos=b.size();
    if (pos==0)
    {
      return data::sort_nat::nat(0);
    }
    data::data_expression result;
    data::data_expression word;
    pos--;
    data::make_machine_number(word, b[pos]);
    data::sort_nat::make_most_significant_digit_nat(result, word);
    while (pos>0) 
    {
      pos--;
      data::make_machine_number(word, b[pos]);
      data::sort_nat::make_concat_digit(result, result, word);
    }
    return result;
  }

  inline const data::data_expression make_pos_bnn(const utilities::big_natural_number& b)
  {
    std::size_t pos=b.size();
    assert(pos>0);

    data::data_expression result;
    data::data_expression word;
    pos--;
    data::make_machine_number(word, b[pos]);
    data::sort_pos::make_most_significant_digit(result, word);
    while (pos>0) 
    {
      pos--;
      data::make_machine_number(word, b[pos]);
      data::sort_pos::make_concat_digit(result, result, word);
    }
    return result;
  }
#endif

  // An algorithm to calculate the greatest common divisor
  inline std::size_t greatest_common_divisor(std::size_t x, std::size_t y)
  {
    if (x == 0)
    {
      return y;
    }
    if (y == 0)
    {
      return x;
    }
    if (x>y)
    {
      const std::size_t temp=x; x=y; y=temp; // swap(x,y)
    }

    std::size_t remainder=y % x;
    while (remainder!=0)
    {
      y=x;
      x=remainder;
      remainder=y % x;
    }
    return x;
  }

  inline void remove_common_divisor(std::size_t& enumerator, std::size_t& denominator)
  {
    std::size_t gcd=greatest_common_divisor(enumerator,denominator);
    enumerator=enumerator/gcd;
    denominator=denominator/gcd;
  }

}

/** \brief This class contains labels for probabilistic transistions, consisting of a numerator and a denumerator.
 *  \details This class provides a number of operators to calculate with real constants (actually fractions). 
 *           Comparisons of more complex expressions may not always rewrite properly. 
 *           There are methods to add and subtract such labels. The numbers are used as 64 bit integers.
 *           If the integers do not fit into 64 bit, the numerator and denumerator are adapted to make
 *           them fit. This means that this library is not exact, but employs rounding at times.
 *
 */
class probabilistic_data_expression: public data::data_expression
{

  protected:

    static data::data_specification data_specification_with_real()
    {
      data::data_specification d;
      d.add_sort(data::sort_real::real_());
      return d;
    }

    static const data::rewriter& m_rewriter()
    {
      static data::rewriter m_r(data_specification_with_real());
      return m_r;
    }

    inline static std::size_t to_number_t(const std::string& s)
    {
      return std::stoul(s);
    }

#ifdef MCRL2_ENABLE_MACHINENUMBERS
    probabilistic_data_expression calculate_plus_minus(const data_expression& other, const bool is_minus) const
    {
      const data_expression& this_enumerator=data::sort_real::left(*this);
      const data_expression& this_denominator=data::sort_real::right(*this);
      const data_expression& other_enumerator=data::sort_real::left(other);
      const data_expression& other_denominator=data::sort_real::right(other);
      bool negate_first=false;
      bool negate_second=is_minus;
      bool second_is_negative=false;
      if (data::sort_int::is_cneg_application(this_enumerator))
      {
        negate_first=true;
      }
      if (data::sort_int::is_cneg_application(other_enumerator))
      {
        negate_second=!negate_second;
        second_is_negative=true;
      }
      const data_expression& this_enumerator_arg=data::sort_int::arg(this_enumerator);
      const data_expression& other_enumerator_arg=data::sort_int::arg(other_enumerator);
      if (data::sort_nat::is_most_significant_digit_nat_application(this_enumerator_arg) &&
          data::sort_nat::is_most_significant_digit_nat_application(other_enumerator_arg) &&
          data::sort_pos::is_most_significant_digit_application(this_denominator) &&
          data::sort_pos::is_most_significant_digit_application(other_denominator))
      {
        std::size_t n_this_enumerator=atermpp::down_cast<data::machine_number>(data::sort_nat::arg(this_enumerator_arg)).value();
        std::size_t n_other_enumerator=atermpp::down_cast<data::machine_number>(data::sort_nat::arg(other_enumerator_arg)).value();
        std::size_t n_this_denominator=atermpp::down_cast<data::machine_number>(data::sort_pos::arg(this_denominator)).value();
        std::size_t n_other_denominator=atermpp::down_cast<data::machine_number>(data::sort_pos::arg(other_denominator)).value();
        //
        // Removing the common denominators makes the multiplication slightly smaller, keeping the
        // numbers hopefully slightly more often in the 64 bit boundary of digits. 
        std::size_t gcd = detail::greatest_common_divisor(n_this_denominator, n_other_denominator);
        std::size_t n_this_denominator_with_gcd=n_this_denominator;
        n_this_denominator =  n_this_denominator / gcd;        
        n_other_denominator =  n_other_denominator / gcd;        

        std::size_t left_carry=0;
        std::size_t right_carry=0;
        std::size_t denominator_carry=0;

        std::size_t left_result=utilities::detail::multiply_single_number(n_this_enumerator,n_other_denominator, left_carry);
        std::size_t right_result=utilities::detail::multiply_single_number(n_other_enumerator,n_this_denominator, right_carry);
        std::size_t new_denominator=utilities::detail::multiply_single_number(n_other_denominator,n_this_denominator_with_gcd, denominator_carry);

        std::size_t new_enumerator_carry=0;        
        std::size_t new_enumerator=0;
        bool result_is_negative=negate_first;
        if (negate_first==negate_second)
        {
          new_enumerator=utilities::detail::add_single_number(left_result, right_result, new_enumerator_carry);
        }
        else if (left_result>=right_result)
        {
          new_enumerator=left_result-right_result;
        }
        else
        {
          new_enumerator=right_result-left_result;
          result_is_negative=!result_is_negative;
        }
        if (left_carry==0 && right_carry==0 && denominator_carry==0 && new_enumerator_carry==0)
        {
          // We did obtain a result within 64 digit numbers. 
          utilities::detail::remove_common_divisor(new_enumerator, new_denominator);
        
          data_expression result1 = data::sort_nat::nat(new_enumerator);
          const data_expression result2 = data::sort_pos::pos(new_denominator);
          if (result_is_negative)
          {
            data::sort_int::make_cneg(result1,result1);
          }
          else
          {
            data::sort_int::make_cint(result1,result1);
          }
          
          data::sort_real::make_creal(result1, result1, result2);
          assert(is_minus || m_rewriter()(data::sort_real::plus(*this,other))==result1);
          assert(!is_minus || m_rewriter()(data::sort_real::minus(*this,other))==result1);
          return probabilistic_data_expression(result1);
        }
      }
      // In this case one of the components of *this or other has more than one digit, or there are carries when doing the calculation.
      const utilities::big_natural_number bnn_this_enumerator=
                    negate_first?
                    detail::make_bnn_pos(this_enumerator_arg):
                    detail::make_bnn_nat(this_enumerator_arg);
      const utilities::big_natural_number bnn_other_enumerator=
                    second_is_negative?
                    detail::make_bnn_pos(other_enumerator_arg):
                    detail::make_bnn_nat(other_enumerator_arg);
      const utilities::big_natural_number bnn_this_denominator=detail::make_bnn_pos(this_denominator);
      const utilities::big_natural_number bnn_other_denominator=detail::make_bnn_pos(other_denominator);
      const utilities::probabilistic_arbitrary_precision_fraction left_arg(bnn_this_enumerator, bnn_this_denominator);
      const utilities::probabilistic_arbitrary_precision_fraction right_arg(bnn_other_enumerator, bnn_other_denominator);
      data::data_expression result1;
      data::data_expression result2;
      if (negate_first==negate_second)
      {
        utilities::probabilistic_arbitrary_precision_fraction result=left_arg+right_arg;
        result2 = detail::make_pos_bnn(result.denominator());
        if (negate_first)
        {
          result1 = detail::make_pos_bnn(result.enumerator()); 
          data::sort_int::make_cneg(result1, result1);
        }
        else
        {
          result1 = detail::make_nat_bnn(result.enumerator()); 
          data::sort_int::make_cint(result1, result1);
        }
      }
      else if (left_arg>right_arg)
      {
        utilities::probabilistic_arbitrary_precision_fraction result=left_arg-right_arg;
        result2 = detail::make_pos_bnn(result.denominator());
        if (negate_first)
        {
          result1 = detail::make_pos_bnn(result.enumerator()); 
          data::sort_int::make_cneg(result1, result1);
        }
        else
        {
          result1 = detail::make_nat_bnn(result.enumerator()); 
          data::sort_int::make_cint(result1, result1);
        }
      }
      else
      {
        utilities::probabilistic_arbitrary_precision_fraction result=right_arg-left_arg;
        result2 = detail::make_pos_bnn(result.denominator());
        if (negate_first)
        {
          result1 = detail::make_nat_bnn(result.enumerator()); 
          data::sort_int::make_cint(result1, result1);
        }
        else
        {
          result1 = detail::make_pos_bnn(result.enumerator()); 
          data::sort_int::make_cneg(result1, result1);
        }
      }
      data::sort_real::make_creal(result1, result1, result2);

      assert(is_minus || m_rewriter()(data::sort_real::plus(*this,other))==result1);
      assert(!is_minus || m_rewriter()(data::sort_real::minus(*this,other))==result1);
      return probabilistic_data_expression(result1);
    }
#endif

  public:
    /// \brief Constant zero.
    static probabilistic_data_expression zero()
    {
      using namespace data;
      static probabilistic_data_expression zero(data::sort_real::real_zero());
      return zero;
    }

    /// \brief Constant one.
    static probabilistic_data_expression one()
    {
      using namespace data;
      static probabilistic_data_expression one(data::sort_real::real_one());
      return one;
    }

    /* \brief Default constructor. 
     */
    probabilistic_data_expression()
     : data::data_expression(zero())
    {}

    /** \brief Construct a probabilistic_data_expression from a data_expression, which must be of sort real.
     */
    explicit probabilistic_data_expression(const data::data_expression& d)
     : data::data_expression(d)
    { 
      assert(d.sort()==data::sort_real::real_());
    }

    /* \brief Constructor on the basis of two numbers. The denominator must not be 0.
     */
    probabilistic_data_expression(std::size_t enumerator, std::size_t denominator)
    {
      assert(denominator!=0);
      detail::remove_common_divisor(enumerator,denominator);
      *this = lps::probabilistic_data_expression(data::sort_real::creal(data::sort_int::int_(enumerator),data::sort_pos::pos(denominator)));
    }

    /* \brief A constructor, where the enumerator and denominator are constructed
     *        from two strings of digits.
     */
    probabilistic_data_expression(const std::string& enumerator, const std::string& denominator)
    {
      assert(enumerator.size() <= denominator.size());
      const data_expression d_enumerator=data::sort_nat::nat(enumerator);
      const data_expression d_denominator=data::sort_pos::pos(denominator);
      *this = lps::probabilistic_data_expression(data::sort_real::creal(data::sort_int::cint(d_enumerator),d_denominator));
    }

    /* \brief Standard comparison operator.
    */
    bool operator==(const probabilistic_data_expression& other) const
    {
#ifdef MCRL2_ENABLE_MACHINENUMBERS
      if (static_cast<data_expression>(*this)==static_cast<data_expression>(other))
      {
        return true;
      }
      return false;
#else      
      const data_expression result= m_rewriter()(data::equal_to(*this,other));
      if (result==data::sort_bool::true_())
      {
        return true;
      }
      if (result==data::sort_bool::false_())
      {
        return false;
      }
      throw mcrl2::runtime_error("Equality between fractions does not rewrite to true or false: " + pp(result) + ".");
#endif
    }

    /* \brief Standard comparison operator.
    */
    bool operator!=(const probabilistic_data_expression& other) const
    {
      return !this->operator==(other);
    }

    /* \brief Standard comparison operator.
    */
    bool operator<(const probabilistic_data_expression& other) const
    {
#ifdef MCRL2_ENABLE_MACHINENUMBERS
      const data_expression& this_enumerator=data::sort_real::left(*this);
      const data_expression& this_denominator=data::sort_real::right(*this);
      const data_expression& other_enumerator=data::sort_real::left(other);
      const data_expression& other_denominator=data::sort_real::right(other);
      bool negate_outcome=false;
      if (data::sort_int::is_cneg_application(this_enumerator))
      {
        if (data::sort_int::is_cneg_application(other_enumerator))
        {
          negate_outcome=true;
        }
        else
        {
          assert(data::sort_int::is_cint_application(other_enumerator));
assert(data::sort_bool::true_()==m_rewriter()(data::less(*this,other)));
          return true; // First number is negative, second is positive or zero. 
        }
      }
      else 
      {
        if (data::sort_int::is_cneg_application(other_enumerator))
        {
assert(data::sort_bool::false_()==m_rewriter()(data::less(*this,other)));
          return false; //First number is positive or zero, second is negative.
        }
        assert(data::sort_int::is_cint_application(other_enumerator));
      }
      const data_expression& this_enumerator_arg=data::sort_int::arg(this_enumerator);
      const data_expression& other_enumerator_arg=data::sort_int::arg(other_enumerator);
      if (data::sort_nat::is_most_significant_digit_nat_application(this_enumerator_arg) &&
          data::sort_nat::is_most_significant_digit_nat_application(other_enumerator_arg) &&
          data::sort_pos::is_most_significant_digit_application(this_denominator) &&
          data::sort_pos::is_most_significant_digit_application(other_denominator))
      {
        std::size_t n_this_enumerator=atermpp::down_cast<data::machine_number>(data::sort_nat::arg(this_enumerator_arg)).value();
        std::size_t n_other_enumerator=atermpp::down_cast<data::machine_number>(data::sort_nat::arg(other_enumerator_arg)).value();
        std::size_t n_this_denominator=atermpp::down_cast<data::machine_number>(data::sort_pos::arg(this_denominator)).value();
        std::size_t n_other_denominator=atermpp::down_cast<data::machine_number>(data::sort_pos::arg(other_denominator)).value();
        std::size_t left_carry=0;
        std::size_t right_carry=0;
        std::size_t left_result=utilities::detail::multiply_single_number(n_this_enumerator,n_other_denominator, left_carry);
        std::size_t right_result=utilities::detail::multiply_single_number(n_other_enumerator,n_this_denominator, right_carry);
        if (left_carry<right_carry || 
            (left_carry==right_carry && left_result<right_result))
        {
          assert(data::sort_bool::true_()==m_rewriter()(data::less(*this,other)));
          return !negate_outcome;
        }
        else
        {
          assert(data::sort_bool::false_()==m_rewriter()(data::less(*this,other)));
          return negate_outcome;
        }
      }
      // In this case one of the components of *this or other has more than one digit.
      const utilities::big_natural_number bnn_this_enumerator=detail::make_bnn_nat(this_enumerator_arg);
      const utilities::big_natural_number bnn_other_enumerator=detail::make_bnn_nat(other_enumerator_arg);
      const utilities::big_natural_number bnn_this_denominator=detail::make_bnn_pos(this_denominator);
      const utilities::big_natural_number bnn_other_denominator=detail::make_bnn_pos(other_denominator);
      utilities::big_natural_number bnn_left_result;
      utilities::big_natural_number bnn_right_result;
      utilities::big_natural_number bnn_buffer;
      bnn_this_enumerator.multiply(bnn_other_denominator, bnn_left_result, bnn_buffer);
      bnn_other_enumerator.multiply(bnn_this_denominator, bnn_right_result, bnn_buffer);
      if (bnn_left_result<bnn_right_result)
      {
        return !negate_outcome;
      }
      else
      {
        return negate_outcome;
      }
#else
      const data_expression result= m_rewriter()(data::less(*this,other));
      if (result==data::sort_bool::true_())
      {
        return true;
      }
      if (result==data::sort_bool::false_())
      {
        return false;
      }
      throw mcrl2::runtime_error("Comparison of fraction does not rewrite to true or false: " + pp(result) + ".");
#endif
    } 

    /* \brief Standard comparison operator.
    */
    bool operator<=(const probabilistic_data_expression& other) const
    {
      return !this->operator>(other);
    } 

    /* \brief Standard comparison operator.
    */
    bool operator>(const probabilistic_data_expression& other) const
    {
      return other.operator<(*this);
    } 

    /* \brief Standard comparison operator.
    */
    bool operator>=(const probabilistic_data_expression& other) const
    {
      return other.operator<=(*this);
    } 

    /** \brief Standard addition operator. Note that the expression is not evaluated.
     *        For this the rewriter has to be applied on it explicitly.
     */
    probabilistic_data_expression operator+(const probabilistic_data_expression& other) const
    {
#ifdef MCRL2_ENABLE_MACHINENUMBERS
      return calculate_plus_minus(other, false);
#else
      return probabilistic_data_expression(m_rewriter()(data::sort_real::plus(*this,other)));
#endif
    } 


    /** \brief Standard subtraction operator.
     */
    probabilistic_data_expression operator-(const probabilistic_data_expression& other) const
    {
#ifdef MCRL2_ENABLE_MACHINENUMBERS
      return calculate_plus_minus(other, true);
#else
      return probabilistic_data_expression(m_rewriter()(data::sort_real::minus(*this,other)));
#endif
    } 
};

/* \brief A pretty print operator on action labels, returning it as a string.
*/
inline std::string pp(const probabilistic_data_expression& l)
{
  return pp(static_cast<data::data_expression>(l));
}

/** \brief Pretty print to an outstream 
*/
inline
std::ostream& operator<<(std::ostream& out, const probabilistic_data_expression& x)
{
  return out << static_cast<data::data_expression>(x);
}

} // namespace mcrl2::lps

namespace std
{

template <>
struct hash<mcrl2::lps::probabilistic_data_expression >
{
  std::size_t operator()(const mcrl2::lps::probabilistic_data_expression& p) const
  {
    hash<atermpp::aterm> aterm_hasher;
    return aterm_hasher(p);
  }
};

} // namespace std

#endif // MCRL2_LPS_PROBABILISTIC_DATA_EXPRESSION_H


