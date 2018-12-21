// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/machine_number.h
/// \brief The class machine_number, which is a subclass of data_expression.

#ifndef MCRL2_DATA_DETAIL_MACHINE_WORD_H
#define MCRL2_DATA_DETAIL_MACHINE_WORD_H

#include <limits>
#include "mcrl2/data/bool.h"
#include "mcrl2/data/machine_number.h"

namespace mcrl2
{

namespace data
{

namespace sort_machine_word
{

namespace detail
{

/* Below standard operations on machine numbers are implemented.  */

inline const machine_number& zero_word()
{
  static machine_number zero=machine_number(0);
  return zero;
}

inline const machine_number& one_word()
{
  static machine_number zero=machine_number(1);
  return zero;
}

inline const machine_number& two_word()
{
  static machine_number zero=machine_number(2);
  return zero;
}

inline const machine_number& three_word()
{
  static machine_number zero=machine_number(3);
  return zero;
}

inline const machine_number& four_word()
{
  static machine_number zero=machine_number(4);
  return zero;
}

inline const machine_number& max_word()
{
  static machine_number max=machine_number(std::numeric_limits<std::size_t>::max());
  return max;
}

inline machine_number succ_word(const machine_number& n)
{
  return machine_number(1+n.value());
}

inline bool equal_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return v1==v2;
}

inline bool less_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return v1<v2;
}

inline bool lessequal_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return v1<=v2;
}

inline machine_number add_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1+v2);
}

inline machine_number add_overflow_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return (v1+v2>=v1?detail::zero_word():detail::one_word());
}

inline machine_number times_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1*v2);
}

inline machine_number times_overflow_word(const machine_number& n1, const machine_number& n2)
{
  const __uint128_t m1=n1.value();
  const __uint128_t m2=n2.value();
  return machine_number(static_cast<std::size_t>((m1*m2) >> std::numeric_limits<std::size_t>::digits));
}

inline machine_number minus_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1-v2);
}

inline machine_number div_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1/v2);
}

inline machine_number mod_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1 % v2);
}

// Calculates (base*n1 + n2) div n3.
inline machine_number div_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  const __uint128_t m3=n3.value();;
  return machine_number(m1 / m3);
}

inline machine_number mod_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  const __uint128_t m3=n3.value();;
  return machine_number(m1 % m3);
}

inline machine_number div_double_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3, const machine_number& n4)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  __uint128_t m3=n3.value();;
  m3 = (m3 << std::numeric_limits<std::size_t>::digits) + n4.value();
  return machine_number(m1 / m3);
}

inline machine_number mod_double_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3, const machine_number& n4)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  __uint128_t m3=n3.value();;
  m3 = (m3 << std::numeric_limits<std::size_t>::digits) + n4.value();
  return machine_number(m1 % m3);
}

inline machine_number div_triple_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3, const machine_number& n4, const machine_number& n5)
{
  // TODO 
  static_cast<void>(n5);  // Suppress unused variable warnings.
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  __uint128_t m3=n3.value();;
  m3 = (m3 << std::numeric_limits<std::size_t>::digits) + n4.value();
  // Not correct.
  assert(0);
  return machine_number(m1 / m3);
}

inline std::size_t sqrt64(const std::size_t n)
{
  return static_cast<std::size_t>(sqrt(n.value()));
}

inline machine_number sqrt_word(const machine_number& n)
{
  return machine_number(sqrt64(n.value()));
}

inline machine_number sqrt_doubleword(const machine_number& n1, const machine_number& n2)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  return machine_number(static_cast<std::size_t>(sqrt(m1)));

  std::size_t w1=n1.value();
  std::size_t w2=n2.value();

  std::size_t p1 = sqrt64(w1);
  std::size_t q1 = w1 - p1*p1;

  if (p1 > (static_cast<std::size_t>(1)<<32))
  {
    y=mod_double_doubleword(q1,w2,2*p*b^1/2) //  TODO.
  }
  return p1<<32 + y;
}

inline machine_number sqrt_tripleword(const machine_number& n1, const machine_number& n2, const machine_number& n3)
{
  // TODO 
  static_cast<void>(n2);  // Suppress unused variable warnings.
  static_cast<void>(n3);  // Suppress unused variable warnings.
  assert(0);  // Not implemented. 
  return n1;
}

inline machine_number sqrt_tripleword_overflow(const machine_number& n1, const machine_number& n2, const machine_number& n3)
{
  // TODO 
  static_cast<void>(n2);  // Suppress unused variable warnings.
  static_cast<void>(n3);  // Suppress unused variable warnings.
  assert(0);  // Not implemented. 
  return n1;
}

inline machine_number sqrt_quadrupleword(const machine_number& n1, const machine_number& n2, const machine_number& n3, const machine_number& n4)
{
  
  // TODO 
  static_cast<void>(n2);  // Suppress unused variable warnings.
  static_cast<void>(n3);  // Suppress unused variable warnings.
  static_cast<void>(n4);  // Suppress unused variable warnings.
  assert(0);  // Not implemented. 
  return n1;
}

inline machine_number sqrt_quadrupleword_overflow(const machine_number& n1, const machine_number& n2, const machine_number& n3, const machine_number& n4)
{
  // TODO 
  static_cast<void>(n2);  // Suppress unused variable warnings.
  static_cast<void>(n3);  // Suppress unused variable warnings.
  static_cast<void>(n4);  // Suppress unused variable warnings.
  assert(0);  // Not implemented. 
  return n1;
}

inline machine_number pred_word(const machine_number& n)
{
  return machine_number(n.value()-1);
}

inline const data_expression& rightmost_bit(const machine_number& n)
{
  if ((n.value() & 1) == 1)
  {
    return sort_bool::true_();
  }
  return sort_bool::false_();
}

inline const data_expression shift_right(const data_expression& e1, const machine_number& n)
{
  assert(e1==sort_bool::true_() || e1==sort_bool::false_());
  std::size_t m = n.value()>>1;
  if (e1==sort_bool::true_())
  {
    return machine_number(m | static_cast<std::size_t>(1)<<((8*sizeof(std::size_t))-1));
  }
  return machine_number(m);
}

} // end namespace detail 

/// \brief The machine number representing 0.
/// \return The machine number 0.
inline const data_expression& zero_word_manual_implementation()
{
  return detail::zero_word();
}

/// \brief The machine number representing 1.
/// \return The machine number 1.
inline const data_expression& one_word_manual_implementation()
{
  return detail::one_word();
}

/// \brief The machine number representing 2.
/// \return The machine number 2.
inline const data_expression& two_word_manual_implementation()
{
  return detail::two_word();
}

/// \brief The machine number representing 3.
/// \return The machine number 3.
inline const data_expression& three_word_manual_implementation()
{
  return detail::three_word();
}

/// \brief The machine number representing 4.
/// \return The machine number 4.
inline const data_expression& four_word_manual_implementation()
{
  return detail::four_word();
}

/// \brief The largest representable machine number.
/// \return The largest number a machine word can hold. 
inline const data_expression& max_word_manual_implementation()
{
  return detail::max_word();
}

/// \brief The successor function on a machine numbers, that wraps around.
/// \param e 
/// \return e+1, or zero if n is the maximum number.
inline data_expression succ_word_manual_implementation(const data_expression& e)
{
  assert(is_machine_number(e));
  {
    return detail::succ_word(atermpp::down_cast<machine_number>(e));
  }
}

/// \brief The equality function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression equal_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    const bool b=detail::equal_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
    return (b? sort_bool::true_(): sort_bool::false_());
  }
}

/// \brief The equality function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression less_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    const bool b=detail::less_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
    return (b? sort_bool::true_(): sort_bool::false_());
  }
}

/// \brief The equality function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression lessequal_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    const bool b=detail::lessequal_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
    return (b? sort_bool::true_(): sort_bool::false_());
  }
}

/// \brief The result of adding two words modulo the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1+e2 modulo the machine word. 
inline data_expression add_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::add_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief An indication whether an overflow occurs when e1 and e2 are added. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return If e1+e2 is larger than a machine word, than 1, otherwise 0.
inline data_expression add_overflow_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::add_overflow_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief The result of multiplying two words modulo the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1*e2 modulo the machine word. 
inline data_expression times_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::times_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief The result of multiplying two words divided by the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1*e2 div the maximal machine word+1. 
inline data_expression times_overflow_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::times_overflow_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief The result of subtracting two words modulo the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1-e2 modulo the machine word. 
inline data_expression minus_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::minus_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief Calculates the division of the first word by the second. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1/e2.
inline data_expression div_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::div_word(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief Calculates e1 modulo e2.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1 modulo e2. 
inline data_expression mod_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::mod_word(atermpp::down_cast<machine_number>(e1), atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief The square root of e, rounded down to a machine word. 
/// \param e The argument.
/// \return The square root of e rounded down. 
inline data_expression sqrt_word_manual_implementation(const data_expression& e)
{
  assert(is_machine_number(e));
  {
    return detail::sqrt_word(atermpp::down_cast<machine_number>(e));
  }
}

/// \brief Calculates (base*e1 + e2) div e3.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return (base*e1 + e2) div e3
inline data_expression div_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  {
    return detail::div_doubleword(atermpp::down_cast<machine_number>(e1), atermpp::down_cast<machine_number>(e2), atermpp::down_cast<machine_number>(e3));
  }
}

/// \brief Calculates (base*e1 + e2) div (base*e3 + e4).
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \param e4 The fourth argument. 
/// \return (base*e1 + e2) div (base*e3 + e4)
inline data_expression div_double_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3, const data_expression& e4)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3) && is_machine_number(e4));
  {
    return detail::div_double_doubleword(atermpp::down_cast<machine_number>(e1), 
                                         atermpp::down_cast<machine_number>(e2), 
                                         atermpp::down_cast<machine_number>(e3), 
                                         atermpp::down_cast<machine_number>(e4));
  }
}

/// \brief Calculates (base*(base*e1 + e2)+e3) div (base*e4 + e5).
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \param e4 The fourth argument. 
/// \param e5 The fifth argument. 
/// \return (base*(base*e1 + e2)+e3) div (base*e4 + e5)
inline data_expression div_triple_doubleword_manual_implementation(
                        const data_expression& e1, 
                        const data_expression& e2, 
                        const data_expression& e3, 
                        const data_expression& e4, 
                        const data_expression& e5)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3) && is_machine_number(e4) && is_machine_number(e5));
  {
    return detail::div_triple_doubleword(atermpp::down_cast<machine_number>(e1), 
                                         atermpp::down_cast<machine_number>(e2), 
                                         atermpp::down_cast<machine_number>(e3), 
                                         atermpp::down_cast<machine_number>(e4),
                                         atermpp::down_cast<machine_number>(e5));
  }
}

/// \brief Calculates (base*e1 + e2) mod (base*e3 + e4).
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \param e4 The fourth argument. 
/// \return (base*e1 + e2) mod (base*e3 + e4)
inline data_expression mod_double_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3, const data_expression& e4)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3) && is_machine_number(e4));
  {
    return detail::mod_double_doubleword(atermpp::down_cast<machine_number>(e1), 
                                         atermpp::down_cast<machine_number>(e2), 
                                         atermpp::down_cast<machine_number>(e3), 
                                         atermpp::down_cast<machine_number>(e4));
  }
}

/// \brief The square root of base*e1+e2 rounded down.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return The square root of base*e1+e2 rounded down.
inline data_expression sqrt_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return detail::sqrt_doubleword(atermpp::down_cast<machine_number>(e1),atermpp::down_cast<machine_number>(e2));
  }
}

/// \brief Calculates (base*e1 + e2) mod e3. The result fits in one word.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return (base*e1 + e2) mod e3
inline data_expression mod_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  {
    return detail::mod_doubleword(atermpp::down_cast<machine_number>(e1), atermpp::down_cast<machine_number>(e2), atermpp::down_cast<machine_number>(e3));
  }
}

/// \brief Calculates the least significant word of the square root of base*(base*e1+e2)+e3.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return the least significant word of the square root of base*(base*e1+e2)+e3.
inline data_expression sqrt_tripleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  {
    return detail::sqrt_tripleword(atermpp::down_cast<machine_number>(e1), atermpp::down_cast<machine_number>(e2), atermpp::down_cast<machine_number>(e3));
  }
}

/// \brief Calculates the most significant word of the square root of base*(base*e1+e2)+e3.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return the most significant word of the square root of base*(base*e1+e2)+e3.
inline data_expression sqrt_tripleword_overflow_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  {
    return detail::sqrt_tripleword_overflow(atermpp::down_cast<machine_number>(e1), atermpp::down_cast<machine_number>(e2), atermpp::down_cast<machine_number>(e3));
  }
}

/// \brief Calculates the least significant word of the square root of base*(base*(base*e1+e2)+e3)+e4.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \param e4 The fourth argument. 
/// \return The least significant word of the square root of base*(base*(base*e1+e2)+e3)+e4.
inline data_expression sqrt_quadrupleword_manual_implementation(
                                      const data_expression& e1, 
                                      const data_expression& e2, 
                                      const data_expression& e3, 
                                      const data_expression& e4)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3) && is_machine_number(e4));
  {
    return detail::sqrt_quadrupleword(atermpp::down_cast<machine_number>(e1), 
                                      atermpp::down_cast<machine_number>(e2), 
                                      atermpp::down_cast<machine_number>(e3), 
                                      atermpp::down_cast<machine_number>(e4));
  }
}

/// \brief Calculates the most significant word of the square root of base*(base*(base*e1+e2)+e3)+e4.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \param e4 The fourth argument. 
/// \return The most significant word of the square root of base*(base*(base*e1+e2)+e3)+e4.
inline data_expression sqrt_quadrupleword_overflow_manual_implementation(
                                               const data_expression& e1, 
                                               const data_expression& e2, 
                                               const data_expression& e3, 
                                               const data_expression& e4)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3) && is_machine_number(e4));
  {
    return detail::sqrt_quadrupleword_overflow(atermpp::down_cast<machine_number>(e1), 
                                               atermpp::down_cast<machine_number>(e2), 
                                               atermpp::down_cast<machine_number>(e3), 
                                               atermpp::down_cast<machine_number>(e4));
  }
}

/// \brief The predecessor function on a machine numbers, that wraps around.
/// \param e 
/// \return e-1, or maxword if e is zero.
inline data_expression pred_word_manual_implementation(const data_expression& e)
{
  return detail::pred_word(atermpp::down_cast<machine_number>(e)); 
}

/// \brief The right most bit of a machine number.
/// \param e 
/// \return true if the rightmost bit is 1.
inline data_expression rightmost_bit_manual_implementation(const data_expression& e)
{
  return detail::rightmost_bit(atermpp::down_cast<machine_number>(e)); 
}

/// \brief The machine word shifted one position to the right.
/// \param e1 A boolean indicating what the left most bit must be.  
/// \param e2 The value shifted to the right. 
/// \return The machine number e2 divided by 2 prepended with a bit 1 if e1 is true. 
inline data_expression shift_right_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  return detail::shift_right(e1, atermpp::down_cast<machine_number>(e2)); 
}


} // namespace sort_machine_word

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MACHINE_NUMBER_H

