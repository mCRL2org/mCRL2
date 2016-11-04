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
 * \brief This file contains a class big_natural_number that stores big positive numbers of arbitrary size.
 *        It has all common operations that one can expect on big numbers.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_UTILITIES_BIG_NUMBERS_H
#define MCRL2_UTILITIES_BIG_NUMBERS_H

#include <algorithm>
#include <limits>
#include <vector>
#include <string>
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/hash_utility.h"


namespace mcrl2
{
namespace utilities
{
namespace detail
{

  // Calculate <carry,result>:=n1+n2+carry. The carry can be either 0 or 1, both
  // at the input and the output.
  inline size_t add_single_number(const size_t n1, const size_t n2, size_t& carry)
  {
    assert(carry<=1);
    size_t result=n1+n2+carry;
    if (carry==0)
    {
      if (result<n1)
      {
        carry=1;
      }
    }
    else // carry==1
    {
      if (result>n1)
      {
        carry=0;
      }
    }
    return result;
  }

  // Calculate <carry,result>:=n1-n2-carry. The carry can be either 0 or 1, both
  // at the input and the output. If the carry is 1, this indicated that 1 must be subtracted.
  inline size_t subtract_single_number(const size_t n1, const size_t n2, size_t& carry)
  {
    assert(carry<=1);
    size_t result=n1-n2-carry;
    if (carry==0)
    {
      if (result>n1)
      {
        carry=1;
      }
    }
    else // carry==1
    {
      if (result<n1)
      {
        carry=0;
      }
    }
    return result;
  }
  
  // Calculate <carry,result>:=n1*n2+carry, where the lower bits of the calculation
  // are stored in the result, and the higher bits are stored in carry.
  inline size_t multiply_single_number(const size_t n1, const size_t n2, size_t& multiplication_carry)
  {
    // TODO: It is more concise and efficient to use 128bit machine calculation when available.
    const int no_of_bits_per_digit=std::numeric_limits<size_t>::digits;

    // split input numbers into no_of_bits_per_digit/2 digits
    size_t n1ls = n1 & ((1LL<<(no_of_bits_per_digit/2))-1);
    size_t n1ms = n1 >> (no_of_bits_per_digit/2);
    size_t n2ls = n2 & ((1LL<<(no_of_bits_per_digit/2))-1);
    size_t n2ms = n2 >> (no_of_bits_per_digit/2);
    
    // First calculate the result of the least significant no_of_bits_per_digit.
    size_t local_carry=0;
    size_t result = add_single_number(n1ls*n2ls,multiplication_carry,local_carry);
    size_t cumulative_carry=local_carry;
    local_carry=0;
    result=add_single_number(result,((n1ms*n2ls)<<(no_of_bits_per_digit/2)),local_carry);
    cumulative_carry=cumulative_carry+local_carry;
    local_carry=0;
    result=add_single_number(result,((n1ls*n2ms)<<(no_of_bits_per_digit/2)),local_carry);
    cumulative_carry=cumulative_carry+local_carry;

    // Now calculate the result of the most significant no_of_bits_per_digit.
    multiplication_carry=cumulative_carry;
    multiplication_carry=multiplication_carry+((n1ms*n2ls)>>(no_of_bits_per_digit/2));
    multiplication_carry=multiplication_carry+((n1ls*n2ms)>>(no_of_bits_per_digit/2));
    multiplication_carry=multiplication_carry+n1ms*n2ms;

    return result;
  }
  
  // Calculate <result,remainder>:=(remainder * 2^64 + p) / q assuming the result
  // fits in 64 bits. More concretely, q>remainder. 
  inline size_t divide_single_number(const size_t p, const size_t q, size_t& remainder)
  {
    assert(q>remainder);
    const int no_of_bits_per_digit=std::numeric_limits<size_t>::digits;

    // Split input numbers into no_of_bits_per_digit/2 digits.
    // First get the least significant part.
    size_t pms = (p >> (no_of_bits_per_digit/2)) + (remainder << (no_of_bits_per_digit/2));
    
    // First divide the most significant part by q.
    size_t resultms = pms/q;
    assert((resultms >> (no_of_bits_per_digit/2)) == 0);
    size_t remainderms = pms%q;

    // Now obtain the least significant part.
    size_t pls = (p & ((1LL<<(no_of_bits_per_digit/2))-1)) + (remainderms << (no_of_bits_per_digit/2));
    
    // Second divide the least significant part by q.
    size_t resultls = pls/q;
    remainder = pls%q;
    assert((resultls >> (no_of_bits_per_digit/2)) == 0);

    return resultls + (resultms << (no_of_bits_per_digit/2));
  }
}

class big_natural_number;
inline std::string pp(const big_natural_number& l);

class big_natural_number
{
    friend std::hash<big_natural_number>;
    friend inline void swap(big_natural_number& x, big_natural_number& y);

  protected:
    // Numbers are stored as size_t words, with the most significant number last. 
    // Note that the number representation is not unique. Numbers have no trailing
    // zero's, i.e., this->back()!=0 (if this->size()>0). Therefore their representation is unique.
    std::vector<size_t> m_number;

    /* Multiply this number with the indicated digit, and multiply the result with 
     * size_t^offset. The result is (*this)* digit * |size_t|^offset.
     */
    big_natural_number multiply_with_single_number(const size_t digit, const size_t offset) const
    {
      big_natural_number result;
      result.m_number.resize(offset,0);
      size_t carry=0;
      for(size_t d:m_number)
      {
        result.m_number.push_back(detail::multiply_single_number(digit,d,carry));
      }
      if (carry!=0)
      {
        result.m_number.push_back(carry);
      }
      result.remove_significant_digits_that_are_zero();
      return result;
    }

    /* Divide the current number by 2 */
    /* void divide_by_two()
    {
      size_t carry=0;
      for(std::vector<size_t>::reverse_iterator i=m_number.rbegin(); i!=m_number.rend(); ++i)
      {
        size_t new_carry=(*i) & 1;
        *i = *i/2;
        if (carry==1)
        {
          *i=(*i)+(static_cast<size_t>(1)<<(std::numeric_limits<size_t>::digits-1));
        }
        carry=new_carry;
      }
      remove_significant_digits_that_are_zero();
      is_well_defined();
    } */

    // Remove zero's at the end of m_number vector that are
    // leading and irrelevant zero's in the number representation.
    void remove_significant_digits_that_are_zero()
    {
      for( ; m_number.size()>0 && m_number.back()==0 ; )
      {
        m_number.pop_back();
      }
      m_number.shrink_to_fit();
    }

    // Check that the number is well defined, in the sense
    // that there are no most significant digits that are zero.
    void is_well_defined() const
    {
      assert(m_number.size()==0 || m_number.back()!=0);
    }

    // \brief This functions prints a number in internal represenation. This function is useful and only meant for debugging.
    void print_number(const std::string& s) const
    {
      std::cerr << s << "  " << m_number.size() << "\n";
      for(size_t i: m_number)
      {
        std::cerr << i << " ";
      }
      std::cerr << "\n---------------------\n";
    }

  public:
    // Default constructor. The value is 0 by default.
    big_natural_number()
    {
    }

    // Constructor based on a size_t. The value of the number will be n.
    big_natural_number(const size_t n)
    {
      if (n>0)
      {
        m_number.push_back(n);
      }
      is_well_defined();
    }

    // Constructor based on a string. The string is a digital number.
    big_natural_number(const std::string& s)
    {
      for(char c: s)
      {
        if (!isdigit(c))
        {
          throw mcrl2::runtime_error("Number " + s + " contains symbol '" + c + "' which is not a digit.");
        }
        if (c>='0')
        {
          multiply_by(10,size_t(c-'0'));
        }
      }
      is_well_defined();
    }

    /** \brief Returns whether this number equals zero.
        \details This is more efficient than checking x==big_natural_number(0).
    */
    bool is_zero() const
    {
      return m_number.size()==0;
    }

    /** \brief Transforms this number to a size_t, provided it is sufficiently small.
               If not an mcrl2::runtime_error is thrown. 
    */
    explicit operator size_t() const
    {
      is_well_defined();
      if (m_number.size()>1)
      {
        throw mcrl2::runtime_error("It is not possible to transform a big natural number into a machine size number if it does not fit.");
      }
      if (m_number.size()==0)
      {
        return 0;
      }
      return m_number.front();
    }

    /* \brief Standard comparison operator.
    */
    bool operator==(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined(); 
      return m_number==other.m_number;
    }

    /* \brief Standard comparison operator.
    */
    bool operator!=(const big_natural_number& other) const
    {
      return !this->operator==(other);
    } 

    /* \brief Standard comparison operator.
    */
    bool operator<(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined(); 
      if (m_number.size()<other.m_number.size())
      {
        return true;
      }
      if (m_number.size()>other.m_number.size())
      {
        return false;
      }
      assert(m_number.size()==other.m_number.size());
      std::vector<size_t>::const_reverse_iterator j=other.m_number.rbegin();
      for(std::vector<size_t>::const_reverse_iterator i=m_number.rbegin(); i!=m_number.rend(); ++i, ++j)
      {
        if (*i < *j)
        {
          return true;
        }
        if (*i > *j)
        {
          return false;
        }
      }
      // The numbers are equal.
      assert(m_number==other.m_number);
      return false;
    }

    /* \brief Standard comparison operator.
    */
    bool operator<=(const big_natural_number& other) const
    {
      return !this->operator>(other);
    }

    /* \brief Standard comparison operator.
    */
    bool operator>(const big_natural_number& other) const
    {
      return other.operator<(*this);
    }

    /* \brief Standard comparison operator.
    */
    bool operator>=(const big_natural_number& other) const
    {
      return other.operator<=(*this);
    }

    /* Divide the current number by n. If there is a remainder return it. */
    size_t divide_by(size_t n)
    {
      size_t remainder=0;
      for(std::vector<size_t>::reverse_iterator i=m_number.rbegin(); i!=m_number.rend(); ++i)
      {
        *i=detail::divide_single_number(*i,n,remainder);
      }
      remove_significant_digits_that_are_zero();
      is_well_defined();
      return remainder;
    }

    /* Multiply the current number by n and add the carry */
    void multiply_by(size_t n, size_t carry)
    {
      for(size_t& i: m_number)
      {
        i=detail::multiply_single_number(i,n,carry);
      }
      if (carry)
      {
        /* Add an extra digit with the carry */
        m_number.push_back(carry);
      }
      is_well_defined();
    }

    // Add the argument to this big natural number 
    void add(const big_natural_number& other)
    {
      is_well_defined();
      other.is_well_defined();

      // big_natural_number result;
      // result.m_number.reserve((std::max)(m_number.size(),other.m_number.size()));
      size_t carry=0;
      for(size_t i=0; i < (std::max)(m_number.size(),other.m_number.size()); ++i)
      {
        if (i>=m_number.size())
        {
          m_number.push_back(detail::add_single_number(0,other.m_number[i],carry));
        }
        else if (i>=other.m_number.size())
        {
          m_number[i]=detail::add_single_number(m_number[i],0,carry);
        }
        else
        {
          m_number[i]=detail::add_single_number(m_number[i],other.m_number[i],carry);
        }
      }
      if (carry>0)
      {
        m_number.push_back(carry);
      }
      is_well_defined();
    }


    /* \brief Standard addition operator.
     */
    big_natural_number operator+(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined();

      big_natural_number result=*this;
      result.add(other);
      return result;
      
      /* result.m_number.reserve((std::max)(m_number.size(),other.m_number.size()));
      size_t carry=0;
      for(size_t i=0; i < (std::max)(m_number.size(),other.m_number.size()); ++i)
      {
        if (i>=m_number.size())
        {
          result.m_number.push_back(detail::add_single_number(0,other.m_number[i],carry));
        }
        else if (i>=other.m_number.size())
        {
          result.m_number.push_back(detail::add_single_number(m_number[i],0,carry));
        }
        else
        {
          result.m_number.push_back(detail::add_single_number(m_number[i],other.m_number[i],carry));
        }
      }
      if (carry>0)
      {
        result.m_number.push_back(carry);
      }
      result.is_well_defined();
      return result; */
    }

    /* \brief Standard subtraction. 
       \detail Subtract other from this number. Throws an exception if 
               the result is negative and cannot be represented. 
    */
    void subtract(const big_natural_number& other)
    {
      is_well_defined();
      other.is_well_defined();

      assert(m_number.size()==0 || m_number.back()!=0);
      assert(other.m_number.size()==0 || other.m_number.back()!=0);
      if (m_number.size()<other.m_number.size())
      {
        throw mcrl2::runtime_error("Subtracting numbers " + pp(*this) + " and " + pp(other) + " yields a non representable negative result (1).");
      }
      // big_natural_number result;
      // result.m_number.reserve(m_number.size());
      size_t carry=0;
      for(size_t i=0; i<m_number.size(); ++i)
      {
        if (i>=other.m_number.size())
        {
          m_number[i]=detail::subtract_single_number(m_number[i],0,carry);
        }
        else
        {
          m_number[i]=detail::subtract_single_number(m_number[i],other.m_number[i],carry);
        }
      }
      if (carry>0)
      {
        throw mcrl2::runtime_error("Subtracting numbers " + pp(*this) + " and " + pp(other) + " yields a non representable negative result (2).");
      }
      remove_significant_digits_that_are_zero();
      is_well_defined();
    } 

    /* \brief Standard subtraction operator. Throws an exception if the result
     *        is negative and cannot be represented.
     */
    big_natural_number operator-(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined();
      big_natural_number result=*this;
      result.subtract(other);
      return result;

      /* assert(m_number.size()==0 || m_number.back()!=0);
      assert(other.m_number.size()==0 || other.m_number.back()!=0);
      if (m_number.size()<other.m_number.size())
      {
        throw mcrl2::runtime_error("Subtracting numbers " + pp(*this) + " and " + pp(other) + " yields a non representable negative result (1).");
      }
      result.m_number.reserve(m_number.size());
      size_t carry=0;
      for(size_t i=0; i<m_number.size(); ++i)
      {
        if (i>=other.m_number.size())
        {
          result.m_number.push_back(detail::subtract_single_number(m_number[i],0,carry));
        }
        else
        {
          result.m_number.push_back(detail::subtract_single_number(m_number[i],other.m_number[i],carry));
        }
      }
      if (carry>0)
      {
        throw mcrl2::runtime_error("Subtracting numbers " + pp(*this) + " and " + pp(other) + " yields a non representable negative result (2).");
      }
      result.remove_significant_digits_that_are_zero();
      result.is_well_defined();
      return result; */
    }  

    /* \brief Standard multiplication operator.
     */
    big_natural_number operator*(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined();

      big_natural_number result;
      size_t offset=0;  
      for(size_t digit: other.m_number)
      {
        result.add(multiply_with_single_number(digit,offset));
        offset++;
      }
      result.is_well_defined();
      return result;
    }

    /* \brief Standard division operator. This is currently implemented by a bit wise subtraction. Can be optimized by a 64 bit calculation.
     */
    big_natural_number operator/(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined();

      if (m_number.size()<other.m_number.size())
      {
         return big_natural_number(0);
      }
      const int no_of_bits_per_digit=std::numeric_limits<size_t>::digits;
      big_natural_number remainder=(*this);
      big_natural_number result; // Most significant bit first.
      big_natural_number divisor;
      divisor.m_number=std::vector<size_t>((m_number.size()-other.m_number.size())+1,0);
      // Place 0 digits at least significant position of the divisor to make it of comparable length as the remainder.
      for(size_t i: other.m_number)
      {
        divisor.m_number.push_back(i);
      }
      divisor.remove_significant_digits_that_are_zero();
      
      for(size_t i=0; i<=no_of_bits_per_digit*(m_number.size()-other.m_number.size()+1); ++i)
      {
        if (remainder<divisor)
        {
          // We cannot subtract the divisor from the remainder.
          result.multiply_by(2,0); // result=result*2
        }
        else
        {
          // We subtract the divisor from the remainder.
          result.multiply_by(2,1); // result=result*2 + 1
          remainder=remainder-divisor;
        }
        divisor.divide_by(2); // Shift the divisor one bit to the left.
      }
      
      result.remove_significant_digits_that_are_zero();
      result.is_well_defined();
      return result;
    }

    /* \brief Standard modulo operator. This is currently implemented by a bit wise subtraction. Can be optimized by a 64 bit calculation.
     */
    big_natural_number operator%(const big_natural_number& other) const
    {
      is_well_defined();
      other.is_well_defined();

      if (m_number.size()<other.m_number.size())
      {
         return (*this); 
      }
      const int no_of_bits_per_digit=std::numeric_limits<size_t>::digits;
      big_natural_number remainder=(*this);
      big_natural_number divisor;
      divisor.m_number=std::vector<size_t>((m_number.size()-other.m_number.size()+1),0);
      // Place 0 digits at the least significant position of the divisor to make it of comparable length as the remainder.
      for(size_t i: other.m_number)
      {
        divisor.m_number.push_back(i);
      }
      divisor.remove_significant_digits_that_are_zero();
      
      for(size_t i=0; i<=no_of_bits_per_digit*(m_number.size()-other.m_number.size()+1); ++i)
      {
        if (remainder>=divisor)
        {
          // We subtract the divisor from the remainder.
          remainder=remainder-divisor;
        }
        divisor.divide_by(2); // Shift the divisor one bit to the left.
      }
      
      remainder.is_well_defined();
      return remainder;
    }
};

inline std::ostream& operator<<(std::ostream& ss, const big_natural_number& l)
{
  big_natural_number n=l;
  std::string s; // This string contains the number in reverse ordering.
  for( ; !n.is_zero() ; )
  {
    size_t remainder = n.divide_by(10); /* This divides n by 10. */
    s.push_back(static_cast<char>('0'+remainder));
  }
  if (s.empty())
  {
    ss << "0";
  }
  else
  {
    for(std::string::const_reverse_iterator i=s.rbegin(); i!=s.rend(); ++i)
    {
      ss << *i;
    }
  }
  return ss;
}


/* \brief A pretty print operator on action labels, returning it as a string.
*/
inline std::string pp(const big_natural_number& l)
{
  std::stringstream s;
  s << l;
  return s.str();
}

/** \brief Standard overload of swap.
 **/
inline void swap(big_natural_number& x, big_natural_number& y)
{
  x.m_number.swap(y.m_number);
}

} // namespace utilities
} // namespace mcrl2

namespace std
{

template <>
struct hash< mcrl2::utilities::big_natural_number >
{
  std::size_t operator()(const mcrl2::utilities::big_natural_number& n) const
  {
    hash<std::vector<size_t> > hasher;
    return hasher(n.m_number);
  }
};

  
} // namespace std


#endif // MCRL2_UTILITIES_BIG_NUMBERS_H


