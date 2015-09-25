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


#ifndef MCRL2_LTS_PROBABILISTIC_LABEL_H
#define MCRL2_LTS_PROBABILISTIC_LABEL_H

#include <cstddef>
#include <string>
#include <cassert>
#include <sstream>
#include <limits>


namespace mcrl2
{
namespace lts
{

/** \brief This class contains labels for probabilistic transistions, consisting of a numerator and a denumerator.
 *  \details There are methods to add and subtract such labels. The numbers are used as 64 bit integers.
 *           If the integers do not fit into 64 bit, the numerator and denumerator are adapted to make
 *           them fit. This means that this library is not exact, but employs rounding at times.
 *
 */
class probabilistic_label
{
  public:
    typedef unsigned long number_t;
  protected:
    number_t m_enumerator;
    number_t m_denominator;

    inline
    static number_t to_number_t(const std::string& s)
    {
      return std::stoul(s);
    }

    // An algorithm to calculate the greatest common divisor
    static number_t greatest_common_divisor(number_t x, number_t y)
    {
      if (x==0) return y;
      if (y==0) return x;
      if (x>y)
      {
        const number_t temp=x; x=y; y=temp; // swap(x,y)
      }

      number_t remainder=y % x;
      while (remainder!=0)
      {
        y=x;
        x=remainder;
        remainder=y % x;
      }
      return x;
    }

    void remove_common_factors(number_t& enumerator, number_t& denominator)
    {
      number_t gcd=greatest_common_divisor(enumerator,denominator);
      while (gcd!=1)
      {
        enumerator=enumerator/gcd;
        denominator=denominator/gcd;
      }
    }

  public:

    /* \brief Default constructor. The label will contain the default string.
     */
    probabilistic_label()
     : m_enumerator(0),
       m_denominator(1)
    {}

    /* \brief Constructor on the basis of two numbers. The denominator must not be 0.
     */
    probabilistic_label(const number_t enumerator, const number_t denominator)
     : m_enumerator(enumerator),
       m_denominator(denominator)
    {
      assert(m_denominator!=0);
      remove_common_factors(m_enumerator,m_denominator);
    }

    /* \brief A constructor, where the enumerator and denominator are constructed
     *        from two strings of digits.
     */
    probabilistic_label(const std::string& enumerator, const std::string& denominator)
    {
      assert(enumerator.size() <= denominator.size());

      // Check whether the denominator fits in number_t.
      if (denominator.size() >= (size_t)std::numeric_limits<number_t>::digits10)
      {
        std::string shortened_denominator = denominator.substr(0, std::numeric_limits<number_t>::digits10);
        m_denominator = to_number_t(shortened_denominator);
        size_t shortened_enumerator_size = enumerator.size() + std::numeric_limits<number_t>::digits10 - denominator.size();
        if (shortened_enumerator_size <= 0)
        {
          m_enumerator = 0;
        }
        else
        {
          std::string shortened_enumerator = enumerator.substr(0, shortened_enumerator_size);
          m_enumerator = to_number_t(shortened_enumerator);
        }
      }
      else
      {
        m_enumerator = to_number_t(enumerator);
        m_denominator = to_number_t(denominator);
        assert(m_denominator != 0);
        remove_common_factors(m_enumerator, m_denominator);
        assert(m_enumerator <= m_denominator);
      }
    }

    /* \brief Return the enumerator of the label.
    */
    number_t enumerator() const
    {
      return m_enumerator;
    }

    /* \brief Return the denominator of the label.
    */
    number_t denominator() const
    {
      return m_denominator;
    }

    /* \brief Standard comparison operator.
    */
    bool operator==(const probabilistic_label& other) const
    {
      assert(greatest_common_divisor(other.m_enumerator,other.m_denominator)==1);
      assert(greatest_common_divisor(m_enumerator,m_denominator)==1);
      return this->m_enumerator==other.m_enumerator && this->m_denominator==other.m_denominator;
    }

    /* \brief Standard comparison operator.
    */
    bool operator!=(const probabilistic_label& other) const
    {
      return !this->operator==(other);
    }

    /* \brief Standard comparison operator.
    */
    bool operator<(const probabilistic_label& other) const
    {
      // float is used to guard against overflow.
      return float(this->m_enumerator)*float(other.m_denominator)<float(other.m_enumerator)*float(this->m_denominator);
    }

    /* \brief Standard comparison operator.
    */
    bool operator<=(const probabilistic_label& other) const
    {
      return !this->operator>(other);
    }

    /* \brief Standard comparison operator.
    */
    bool operator>(const probabilistic_label& other) const
    {
      return other.operator<(*this);
    }

    /* \brief Standard comparison operator.
    */
    bool operator>=(const probabilistic_label& other) const
    {
      return other.operator<=(*this);
    }

    /* \brief Standard addition operator.
     */
    probabilistic_label operator+(const probabilistic_label& other) const
    {
      // TODO: Guard against overflow.
      number_t enumerator=this->enumerator()*other.denominator()+other.enumerator()*this->denominator();
      number_t denominator=this->denominator()*other.denominator();
      return probabilistic_label(enumerator,denominator);
    }


    /* \brief Standard subtraction operator.
     */
    probabilistic_label operator-(const probabilistic_label& other) const
    {
      // TODO: Guard against overflow.
      assert(this->enumerator()*other.denominator()>=other.enumerator()*this->denominator());
      number_t enumerator=this->enumerator()*other.denominator()-other.enumerator()*this->denominator();
      number_t denominator=this->denominator()*other.denominator();
      return probabilistic_label(enumerator,denominator);
    }
};

/* \brief A pretty print operator on action labels, returning it as a string.
*/
inline std::string pp(const probabilistic_label& l)
{
  std::stringstream s;
  s << l.enumerator() << "/" << l.denominator();
  return s.str();
}

} // namespace lts
} // namespace mcrl2

#endif // MCRL2_LTS_PROBABILISTIC_LABEL_H


