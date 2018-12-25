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
#include "mcrl2/data/machine_word.h"

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
  static const machine_number zero=machine_number(0);
  return zero;
}

inline const machine_number& one_word()
{
  static const machine_number zero=machine_number(1);
  return zero;
}

inline const machine_number& two_word()
{
  static const machine_number zero=machine_number(2);
  return zero;
}

inline const machine_number& three_word()
{
  static const machine_number zero=machine_number(3);
  return zero;
}

inline const machine_number& four_word()
{
  static const machine_number zero=machine_number(4);
  return zero;
}

inline const machine_number& max_word()
{
  static const machine_number max=machine_number(std::numeric_limits<std::size_t>::max());
  return max;
}

inline std::size_t succ_word(const std::size_t n)
{
  return 1+n;
}

inline bool equal_word(const std::size_t n1, const std::size_t n2)
{
  return n1==n2;
}

inline bool less_word(const std::size_t n1, const std::size_t n2)
{
  return n1<n2;
}

inline bool less_equal_word(const std::size_t n1, const std::size_t n2)
{
  return n1<=n2;
}

inline std::size_t add_word(const std::size_t n1, const std::size_t n2)
{
  return n1+n2;
}

std::size_t add_overflow_word(const std::size_t n1, const std::size_t n2);

inline std::size_t times_word(const std::size_t n1, const std::size_t n2)
{
  return n1*n2;
}

std::size_t times_overflow_word(const std::size_t n1, const std::size_t n2);

inline std::size_t minus_word(const std::size_t n1, const std::size_t n2)
{
  return n1-n2;
}

inline std::size_t div_word(const std::size_t n1, const std::size_t n2)
{
  return n1/n2;
}

inline std::size_t mod_word(const std::size_t n1, const std::size_t n2)
{
  return n1 % n2;
}

std::size_t div_doubleword(const std::size_t n1, const std::size_t n2, const std::size_t n3);

std::size_t mod_doubleword(const std::size_t n1, const std::size_t n2, const std::size_t n3);

std::size_t div_double_doubleword(const std::size_t n1, const std::size_t n2, const std::size_t n3, const std::size_t n4);

std::size_t mod_double_doubleword(const std::size_t n1, const std::size_t n2, const std::size_t n3, const std::size_t n4);

std::size_t div_triple_doubleword(const std::size_t n1, const std::size_t n2, const std::size_t n3, const std::size_t n4, const std::size_t n5);

inline std::size_t sqrt_word(const std::size_t n)
{
  return sqrt(n);
}

std::size_t sqrt_doubleword(const std::size_t n1, const std::size_t n2);

std::size_t sqrt_tripleword(const std::size_t n1, const std::size_t n2, const std::size_t n3);

std::size_t sqrt_tripleword_overflow(const std::size_t n1, const std::size_t n2, const std::size_t n3);

std::size_t sqrt_quadrupleword(const std::size_t n1, const std::size_t n2, const std::size_t n3, const std::size_t n4);

std::size_t sqrt_quadrupleword_overflow(const std::size_t n1, const std::size_t n2, const std::size_t n3, const std::size_t n4);

inline std::size_t pred_word(const std::size_t n)
{
  return n-1;
}

inline const data_expression& rightmost_bit(const std::size_t n)
{
  if ((n & 1) == 1)
  {
    return sort_bool::true_();
  }
  return sort_bool::false_();
}

inline std::size_t shift_right(const data_expression& e1, const std::size_t n)
{
  std::size_t m = n>>1;
  if (e1==sort_bool::true_())
  {
    return m | static_cast<std::size_t>(1)<<((8*sizeof(std::size_t))-1);
  }
  return m;
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
  return machine_number(detail::succ_word(atermpp::down_cast<machine_number>(e).value()));
}

/// \brief The equality function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression equal_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  if (is_machine_number(e1) && is_machine_number(e2))
  {
    const bool b=detail::equal_word(
                            atermpp::down_cast<machine_number>(e1).value(),
                            atermpp::down_cast<machine_number>(e2).value());
    return (b? sort_bool::true_(): sort_bool::false_());
  }
  return sort_machine_word::equal_word(e1,e2);
}

/// \brief The non equality function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression not_equal_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  if (is_machine_number(e1) && is_machine_number(e2))
  {
    const bool b=detail::equal_word(
                          atermpp::down_cast<machine_number>(e1).value(),
                          atermpp::down_cast<machine_number>(e2).value());
    return (b? sort_bool::false_(): sort_bool::true_());
  }
  return sort_machine_word::not_equal_word(e1,e2);
}

/// \brief The less than function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression less_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  if (is_machine_number(e1) && is_machine_number(e2))
  {
    const bool b=detail::less_word(atermpp::down_cast<machine_number>(e1).value(),atermpp::down_cast<machine_number>(e2).value());
    return (b? sort_bool::true_(): sort_bool::false_());
  }
  return sort_machine_word::less_word(e1,e2);
  
}

/// \brief The less than or equal function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression less_equal_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  if (is_machine_number(e1) && is_machine_number(e2))
  {
    const bool b=detail::less_equal_word(atermpp::down_cast<machine_number>(e1).value(),atermpp::down_cast<machine_number>(e2).value());
    return (b? sort_bool::true_(): sort_bool::false_());
  }
  return sort_machine_word::less_equal_word(e1,e2);
}

/// \brief The greater than function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression greater_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  if (is_machine_number(e1) && is_machine_number(e2))
  {
    const bool b=detail::less_word(atermpp::down_cast<machine_number>(e2).value(),atermpp::down_cast<machine_number>(e1).value());
    return (b? sort_bool::true_(): sort_bool::false_());
  }
  return sort_machine_word::greater_word(e1,e2);
}

/// \brief The greater than or equal function on two machine words. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1==e2.
inline data_expression greater_equal_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  if (is_machine_number(e1) && is_machine_number(e2))
  {
    const bool b=detail::less_equal_word(atermpp::down_cast<machine_number>(e2).value(),atermpp::down_cast<machine_number>(e1).value());
    return (b? sort_bool::true_(): sort_bool::false_());
  }
  return sort_machine_word::greater_equal_word(e1,e2);
}

/// \brief The result of adding two words modulo the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1+e2 modulo the machine word. 
inline data_expression add_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::add_word(
                              atermpp::down_cast<machine_number>(e1).value(),
                              atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief An indication whether an overflow occurs when e1 and e2 are added. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return If e1+e2 is larger than a machine word, than 1, otherwise 0.
inline data_expression add_overflow_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::add_overflow_word(
                              atermpp::down_cast<machine_number>(e1).value(),
                              atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief The result of multiplying two words modulo the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1*e2 modulo the machine word. 
inline data_expression times_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::times_word(
                              atermpp::down_cast<machine_number>(e1).value(),
                              atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief The result of multiplying two words divided by the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1*e2 div the maximal machine word+1. 
inline data_expression times_overflow_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  {
    return machine_number(detail::times_overflow_word(
                                atermpp::down_cast<machine_number>(e1).value(),
                                atermpp::down_cast<machine_number>(e2).value()));
  }
}

/// \brief The result of subtracting two words modulo the maximal representable machine word plus 1.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1-e2 modulo the machine word. 
inline data_expression minus_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::minus_word(
                               atermpp::down_cast<machine_number>(e1).value(),
                               atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief Calculates the division of the first word by the second. 
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1/e2.
inline data_expression div_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::div_word(
                                atermpp::down_cast<machine_number>(e1).value(),
                                atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief Calculates e1 modulo e2.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return e1 modulo e2. 
inline data_expression mod_word_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::mod_word(
                                atermpp::down_cast<machine_number>(e1).value(), 
                                atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief The square root of e, rounded down to a machine word. 
/// \param e The argument.
/// \return The square root of e rounded down. 
inline data_expression sqrt_word_manual_implementation(const data_expression& e)
{
  assert(is_machine_number(e));
  return machine_number(detail::sqrt_word(atermpp::down_cast<machine_number>(e).value()));
}

/// \brief Calculates (base*e1 + e2) div e3.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return (base*e1 + e2) div e3
inline data_expression div_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  return machine_number(detail::div_doubleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value()));
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
  return machine_number(detail::div_double_doubleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value(), 
                                 atermpp::down_cast<machine_number>(e4).value()));
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
  return machine_number(detail::div_triple_doubleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value(), 
                                 atermpp::down_cast<machine_number>(e4).value(),
                                 atermpp::down_cast<machine_number>(e5).value()));
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
  return machine_number(detail::mod_double_doubleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value(), 
                                 atermpp::down_cast<machine_number>(e4).value()));
}

/// \brief The square root of base*e1+e2 rounded down.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \return The square root of base*e1+e2 rounded down.
inline data_expression sqrt_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(is_machine_number(e1) && is_machine_number(e2));
  return machine_number(detail::sqrt_doubleword(
                                 atermpp::down_cast<machine_number>(e1).value(),
                                 atermpp::down_cast<machine_number>(e2).value()));
}

/// \brief Calculates (base*e1 + e2) mod e3. The result fits in one word.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return (base*e1 + e2) mod e3
inline data_expression mod_doubleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  return machine_number(detail::mod_doubleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value()));
}

/// \brief Calculates the least significant word of the square root of base*(base*e1+e2)+e3.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return the least significant word of the square root of base*(base*e1+e2)+e3.
inline data_expression sqrt_tripleword_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  return machine_number(detail::sqrt_tripleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value()));
}

/// \brief Calculates the most significant word of the square root of base*(base*e1+e2)+e3.
/// \param e1 The first argument.
/// \param e2 The second argument. 
/// \param e3 The third argument. 
/// \return the most significant word of the square root of base*(base*e1+e2)+e3.
inline data_expression sqrt_tripleword_overflow_manual_implementation(const data_expression& e1, const data_expression& e2, const data_expression& e3)
{
  assert(is_machine_number(e1) && is_machine_number(e2) && is_machine_number(e3));
  return machine_number(detail::sqrt_tripleword_overflow(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value()));
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
  return machine_number(detail::sqrt_quadrupleword(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value(), 
                                 atermpp::down_cast<machine_number>(e4).value()));
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
  return machine_number(detail::sqrt_quadrupleword_overflow(
                                 atermpp::down_cast<machine_number>(e1).value(), 
                                 atermpp::down_cast<machine_number>(e2).value(), 
                                 atermpp::down_cast<machine_number>(e3).value(), 
                                 atermpp::down_cast<machine_number>(e4).value()));
}

/// \brief The predecessor function on a machine numbers, that wraps around.
/// \param e 
/// \return e-1, or maxword if e is zero.
inline data_expression pred_word_manual_implementation(const data_expression& e)
{
  return machine_number(detail::pred_word(atermpp::down_cast<machine_number>(e).value())); 
}

/// \brief The right most bit of a machine number.
/// \param e 
/// \return true if the rightmost bit is 1.
inline data_expression rightmost_bit_manual_implementation(const data_expression& e)
{
  return detail::rightmost_bit(atermpp::down_cast<machine_number>(e).value()); 
}

/// \brief The machine word shifted one position to the right.
/// \param e1 A boolean indicating what the left most bit must be.  
/// \param e2 The value shifted to the right. 
/// \return The machine number e2 divided by 2 prepended with a bit 1 if e1 is true. 
inline data_expression shift_right_manual_implementation(const data_expression& e1, const data_expression& e2)
{
  assert(e1==sort_bool::true_() || e1==sort_bool::false_());
  return machine_number(detail::shift_right(e1, atermpp::down_cast<machine_number>(e2).value())); 
}


} // namespace sort_machine_word

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MACHINE_NUMBER_H

