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
 * \brief This file contains a class that contains labels for probabilistic transitions.
 *        These consist of a 64 bit enumerator and denominator. This library maintains
 *        precision as long as the enuemerator and denominator fit in 64 bits positive numbers.
 *        If they do not fit, rounding is applied.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_PROBABILISTIC_ARBITRARY_PRECISION_FRACTION_H
#define MCRL2_LTS_PROBABILISTIC_ARBITRARY_PRECISION_FRACTION_H

#include <cstddef>
#include <string>
#include <cassert>
#include <sstream>
#include <limits>
#include "mcrl2/utilities/hash_utility.h"
#include "mcrl2/utilities/big_numbers.h"


namespace mcrl2
{
namespace lts
{

/** \brief This class contains labels for probabilistic transistions, consisting of a numerator and a denominator
 *         as a string of digits.
 */
class probabilistic_arbitrary_precision_fraction
{
  protected:
    utilities::big_natural_number m_enumerator;
    utilities::big_natural_number m_denominator;

  public:

    /// \brief Constant zero.
    static probabilistic_arbitrary_precision_fraction& zero()
    {
      static probabilistic_arbitrary_precision_fraction zero(utilities::big_natural_number(0), utilities::big_natural_number(1));
      return zero;
    }

    /// \brief Constant one.
    static probabilistic_arbitrary_precision_fraction& one()
    {
      static probabilistic_arbitrary_precision_fraction one(utilities::big_natural_number(1), utilities::big_natural_number(1));
      return one;
    }                          

    /* \brief Default constructor. The label will contain the default string.
     */
    probabilistic_arbitrary_precision_fraction()
     : m_enumerator("0"),
       m_denominator("1")
    {}

    /* \brief A constructor, where the enumerator and denominator are constructed
     *        from two strings of digits.
     */
    probabilistic_arbitrary_precision_fraction(const utilities::big_natural_number& enumerator, const utilities::big_natural_number& denominator)
     : m_enumerator(enumerator),
       m_denominator(denominator)
    {
      assert(enumerator<= denominator);
    }

    /* \brief A constructor, where the enumerator and denominator are constructed
     *        from two strings of digits.
     */
    explicit probabilistic_arbitrary_precision_fraction(const std::string& enumerator, const std::string& denominator)
     : // probabilistic_arbitrary_precision_fraction(utilities::big_natural_number(enumerator),utilities::big_natural_number(denominator))
       m_enumerator(utilities::big_natural_number(enumerator)),
       m_denominator(utilities::big_natural_number(denominator))
    {
      assert(enumerator<= denominator);
    }

    /* \brief Return the enumerator of the label.
    */
    utilities::big_natural_number enumerator() const
    {
      return m_enumerator;
    }

    /* \brief Return the denominator of the label.
    */
    utilities::big_natural_number denominator() const
    {
      return m_denominator;
    }

    /* \brief Standard comparison operator.
    */
    bool operator==(const probabilistic_arbitrary_precision_fraction& other) const
    {
      return this->m_enumerator*other.m_denominator==other.m_enumerator*this->m_denominator;
    }

    /* \brief Standard comparison operator.
    */
    bool operator!=(const probabilistic_arbitrary_precision_fraction& other) const
    {
      return !this->operator==(other);
    }

    /* \brief Standard comparison operator.
    */
    bool operator<(const probabilistic_arbitrary_precision_fraction& other) const
    {
      // float is used to guard against overflow.
      return this->m_enumerator*other.m_denominator<other.m_enumerator*this->m_denominator;
    }

    /* \brief Standard comparison operator.
    */
    bool operator<=(const probabilistic_arbitrary_precision_fraction& other) const
    {
      return !this->operator>(other);
    }

    /* \brief Standard comparison operator.
    */
    bool operator>(const probabilistic_arbitrary_precision_fraction& other) const
    {
      return other.operator<(*this);
    }

    /* \brief Standard comparison operator.
    */
    bool operator>=(const probabilistic_arbitrary_precision_fraction& other) const
    {
      return other.operator<=(*this);
    }

    /* \brief Standard addition operator.
     */
    probabilistic_arbitrary_precision_fraction operator+(const probabilistic_arbitrary_precision_fraction& other) const
    {
      utilities::big_natural_number enumerator=this->enumerator()*other.denominator() +
                                               other.enumerator()*this->denominator();
      utilities::big_natural_number denominator=this->denominator()*other.denominator();
      remove_common_factors(enumerator,denominator);
      return probabilistic_arbitrary_precision_fraction(enumerator,denominator);
    }

    /* \brief Standard subtraction operator.
     */
    probabilistic_arbitrary_precision_fraction operator-(const probabilistic_arbitrary_precision_fraction& other) const
    {
      utilities::big_natural_number enumerator= this->enumerator()*other.denominator() -
                                    other.enumerator()*this->denominator();
      utilities::big_natural_number denominator=this->denominator()*other.denominator();
      remove_common_factors(enumerator,denominator);
      return probabilistic_arbitrary_precision_fraction(enumerator,denominator);
    }

    /* \brief Standard multiplication operator.
     */
    probabilistic_arbitrary_precision_fraction operator*(const probabilistic_arbitrary_precision_fraction& other) const
    {
      utilities::big_natural_number enumerator= this->enumerator()*other.enumerator();
      utilities::big_natural_number denominator=this->denominator()*other.denominator();
      remove_common_factors(enumerator,denominator);
      return probabilistic_arbitrary_precision_fraction(enumerator,denominator);
    }

    /* \brief Standard division operator.
     */
    probabilistic_arbitrary_precision_fraction operator/(const probabilistic_arbitrary_precision_fraction& other) const
    {
      utilities::big_natural_number enumerator= this->enumerator()*other.denominator();
      utilities::big_natural_number denominator=this->denominator()*other.enumerator();
      remove_common_factors(enumerator,denominator);
      return probabilistic_arbitrary_precision_fraction(enumerator,denominator);
    }

    // An algorithm to calculate the greatest common divisor
    // The arguments are intentionally passed by value.
    static utilities::big_natural_number greatest_common_divisor(utilities::big_natural_number x, utilities::big_natural_number y)
    {
      if (x.is_zero()) return y;
      if (y.is_zero()) return x;
      if (x>y)
      {
        utilities::swap(x,y);
      }

      utilities::big_natural_number remainder=y % x;
      while (remainder!=0)
      {
        y=x;
        x=remainder;
        remainder=y % x;
      }
      return x;
    }

    static void remove_common_factors(utilities::big_natural_number& enumerator, utilities::big_natural_number& denominator)
    {
      for(utilities::big_natural_number gcd=greatest_common_divisor(enumerator,denominator);
          gcd!=1;
          gcd=greatest_common_divisor(enumerator,denominator))
      {
        enumerator=enumerator/gcd;
        denominator=denominator/gcd;
      }
    }
    
};

/* \brief A pretty print operator on action labels, returning it as a string.
*/
inline std::string pp(const probabilistic_arbitrary_precision_fraction& l)
{
  std::stringstream s;
  s << l.enumerator() << "/" << l.denominator();
  return s.str();
}

inline
std::ostream& operator<<(std::ostream& out, const probabilistic_arbitrary_precision_fraction& x)
{
  return out << pp(x);
}


} // namespace lts
} // namespace mcrl2

namespace std
{

/// \brief specialization of the standard std::hash function.
template <>
struct hash< mcrl2::lts::probabilistic_arbitrary_precision_fraction >
{
  std::size_t operator()(const mcrl2::lts::probabilistic_arbitrary_precision_fraction& p) const
  {
    hash<mcrl2::utilities::big_natural_number> hasher;
    return mcrl2::utilities::detail::hash_combine(hasher(p.enumerator()), hasher(p.denominator()));
  }
};

} // namespace std

#endif // MCRL2_LTS_PROBABILISTIC_ARBITRARY_PRECISION_FRACTION_H


