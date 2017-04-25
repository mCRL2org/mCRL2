// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewriter.h"


namespace mcrl2
{
namespace lps
{

/** \brief This class contains labels for probabilistic transistions, consisting of a numerator and a denumerator.
 *  \details There are methods to add and subtract such labels. The numbers are used as 64 bit integers.
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

    inline static size_t to_number_t(const std::string& s)
    {
      return std::stoul(s);
    }

    // An algorithm to calculate the greatest common divisor
    static size_t greatest_common_divisor(size_t x, size_t y)
    {
      if (x==0) return y;
      if (y==0) return x;
      if (x>y)
      {
        const size_t temp=x; x=y; y=temp; // swap(x,y)
      }

      size_t remainder=y % x;
      while (remainder!=0)
      {
        y=x;
        x=remainder;
        remainder=y % x;
      }
      return x;
    }

    void remove_common_factors(size_t& enumerator, size_t& denominator)
    {
      size_t gcd=greatest_common_divisor(enumerator,denominator);
      while (gcd!=1)
      {
        enumerator=enumerator/gcd;
        denominator=denominator/gcd;
      }
    }


  public:
    /// \brief Constant zero.
    static probabilistic_data_expression zero()
    {
      using namespace data;
      static probabilistic_data_expression zero(sort_real::creal(data::sort_int::cint(sort_nat::c0()),sort_pos::c1()));
      return zero;
    }

    /// \brief Constant one.
    static probabilistic_data_expression one()
    {
      using namespace data;
      static probabilistic_data_expression one(sort_real::creal(data::sort_int::cint(sort_nat::cnat(sort_pos::c1())),sort_pos::c1()));
      return one;
    }

    /* \brief Default constructor. 
     */
    probabilistic_data_expression()
     : data::data_expression(zero())
    {}

    /** \brief Construct a probabilistic_data_expression from a data_expression, which must be of sort real.
     */
    probabilistic_data_expression(const data::data_expression& d)
     : data::data_expression(d)
    { 
      assert(d.sort()==data::sort_real::real_());
    }

    /* \brief Constructor on the basis of two numbers. The denominator must not be 0.
     */
    probabilistic_data_expression(size_t enumerator, size_t denominator)
    {
      assert(denominator!=0);
      remove_common_factors(enumerator,denominator);
      *this =data::sort_real::creal(data::sort_int::int_(enumerator),data::sort_pos::pos(denominator));
    }

    /* \brief A constructor, where the enumerator and denominator are constructed
     *        from two strings of digits.
     */
    probabilistic_data_expression(const std::string& enumerator, const std::string& denominator)
    {
      assert(enumerator.size() <= denominator.size());
      const data_expression d_enumerator=data::sort_nat::nat(enumerator);
      const data_expression d_denominator=data::sort_pos::pos(denominator);
      *this =data::sort_real::creal(data::sort_int::cint(d_enumerator),d_denominator);
    }

    /* \brief Standard comparison operator.
    */
    bool operator==(const probabilistic_data_expression& other) const
    {
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
      return probabilistic_data_expression(m_rewriter()(data::sort_real::plus(*this,other)));
    } 


    /** \brief Standard subtraction operator.
     */
    probabilistic_data_expression operator-(const probabilistic_data_expression& other) const
    {
      return probabilistic_data_expression(m_rewriter()(data::sort_real::minus(*this,other)));
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


} // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_PROBABILISTIC_DATA_EXPRESSION_H


