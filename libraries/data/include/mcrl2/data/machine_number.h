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

#ifndef MCRL2_DATA_MACHINE_NUMBER_H
#define MCRL2_DATA_MACHINE_NUMBER_H

#include <limits>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2
{

namespace data
{

typedef std::pair<atermpp::aterm, atermpp::aterm> machine_number_key_type;


//--- start generated class machine_number ---//
/// \brief A machine number
class machine_number: public data_expression
{
  public:
    /// \brief Default constructor.
    machine_number()
      : data_expression(atermpp::aterm_int(std::size_t(0)))
    {}

    /// \brief Constructor based on an aterm.
    /// \param term A term
    explicit machine_number(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(this->type_is_int());
    }

    /// \brief Constructor.
    machine_number(std::size_t value)
      : data_expression(atermpp::aterm_int(value))
    {}

    /// Move semantics
    machine_number(const machine_number&) noexcept = default;
    machine_number(machine_number&&) noexcept = default;
    machine_number& operator=(const machine_number&) noexcept = default;
    machine_number& operator=(machine_number&&) noexcept = default;

    std::size_t value() const
    {
      return atermpp::down_cast<atermpp::aterm_int>(static_cast<atermpp::aterm>(*this)).value();
    }
};

/// \brief list of machine_numbers
typedef atermpp::term_list<machine_number> machine_number_list;

/// \brief vector of machine_numbers
typedef std::vector<machine_number>    machine_number_vector;

// prototype declaration
std::string pp(const machine_number& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const machine_number& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(machine_number& t1, machine_number& t2)
{
  t1.swap(t2);
}
//--- end generated class machine_number ---//


/* Below standard operations on machine numbers are implemented.  */

/// \brief The machine number representing 0.
/// \return The machine number 0.
inline const machine_number zero_word()
{
  return machine_number();
}

/// \brief The machine number representing 1.
/// \return The machine number 1.
inline const machine_number one_word()
{
  return machine_number(1);
}

/// \brief The largest representable machine number.
/// \return The largest number a machine word can hold. 
inline const machine_number max_word()
{
  return machine_number(std::numeric_limits<std::size_t>::max());
}

/// \brief The successor function on a machine numbers, that wraps around.
/// \param n 
/// \return n+1, or zero if n is the maximum number.
inline machine_number succ_word(const machine_number& n)
{
  return machine_number(1+n.value());
}

/// \brief The result of adding two words modulo the maximal representable machine word plus 1.
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return n1+n2 modulo the machine word. 
inline machine_number add_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1+v2);
}

/// \brief An indication whether an overflow occurs when n1 and n2 are added. 
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return If n1+n2 is larger than a machine word, than 1, otherwise 0.
inline machine_number add_overflow_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return (v1+v2>v1?zero_word():one_word());
}

/// \brief The result of multiplying two words modulo the maximal representable machine word plus 1.
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return n1*n2 modulo the machine word. 
inline machine_number times_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1*v2);
}

/// \brief The result of multiplying two words divided by the maximal representable machine word plus 1.
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return n1*n2 div the maximal machine word+1. 
inline machine_number times_overflow_word(const machine_number& n1, const machine_number& n2)
{
  const __uint128_t m1=n1.value();
  const __uint128_t m2=n2.value();
  return machine_number(static_cast<std::size_t>((m1*m2) >> std::numeric_limits<std::size_t>::digits));
}

/// \brief The result of subtracting two words modulo the maximal representable machine word plus 1.
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return n1-n2 modulo the machine word. 
inline machine_number minus_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1-v2);
}

/// \brief The result of dividing the first word by the second. 
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return n1/n2.
inline machine_number div_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1/v2);
}

/// \brief The result n1 modulo n2.
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \return n1 modulo n2. 
inline machine_number mod_word(const machine_number& n1, const machine_number& n2)
{
  const std::size_t v1=n1.value();
  const std::size_t v2=n2.value();
  return machine_number(v1 % v2);
}

/// \brief The square root of n, rounded down to a machine word. 
/// \param n The argument.
/// \return The square root of n rounded down. 
inline machine_number sqrt_word(const machine_number& n)
{
  return machine_number(static_cast<std::size_t>(sqrt(n.value())));
}

/// \brief Calculates (base*n1 + n2) div n3.
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \param n3 The third argument. 
/// \return (base*n1 + n2) div n3
inline machine_number div_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  const __uint128_t m3=n3.value();;
  return machine_number(m1 / m3);
}

/// \brief Calculates (base*n1 + n2) div (base*n3 + n4).
/// \param n1 The first argument.
/// \param n2 The second argument. 
/// \param n3 The third argument. 
/// \param n4 The fourth argument. 
/// \return (base*n1 + n2) div (base*n3 + n4)
inline machine_number div_double_doubleword(const machine_number& n1, const machine_number& n2, const machine_number& n3, const machine_number& n4)
{
  __uint128_t m1=n1.value();
  m1 = (m1 << std::numeric_limits<std::size_t>::digits) + n2.value();
  __uint128_t m3=n3.value();;
  m3 = (m3 << std::numeric_limits<std::size_t>::digits) + n4.value();
  return machine_number(m1 / m3);
}

/*
% Core functions that are used by other datatypes.
map  one_word, max_word: Word;

     add_word, add_overflow_word, times_word, times_overflow_word, minus_word, div_word, mod_word: Word # Word -> Word;
     sqrt_word: Word->Word;                            % The square root on a word yielding the square root rounded down. 
     div_doubleword: Word # Word # Word -> Word;       % div_doubleword(w1,w2,w3) calculates (base*w1 + w2) div w3, where the result should fit in a word. 
     div_double_doubleword: Word # Word # Word # Word -> Word; % div_double_doubleword(w1,w2,w3,w4) calculates (base*w1 + w2) div (base*w3 + w4) where the result should fit in a word. 

XXXXXXXXXXX   HIER GEBLEVEN MET IMPLEMENTEREN. 
     div_triple_doubleword: Word # Word # Word # Word # Word -> Word; % div_triple_doubleword(w1,w2,w3,w4,w5) calculates (base*(base*w1 + w2)+w3) div (base*w4 + w5) where the result should fit in a word. 
     mod_doubleword: Word # Word # Word -> Word;       % mod_doubleword(w1,w2,w3) calculates (base*w1 + w2) mod w3. The result fits in one word. 
     sqrt_doubleword: Word # Word -> Word;             % The square root of base*w1+w2.
     sqrt_tripleword: Word # Word # Word -> Word;      % The least significant word of the square root of base*(base*w1+w2)+w3.
     sqrt_tripleword_overflow: Word # Word # Word -> Word;   % The most significant word of square root of base*(base*w1+w2)+w3.
     sqrt_quadrupleword: Word # Word # Word # Word -> Word;  % The least significant word of the square root of base*(base*(base*w1+w2)+w3)+w4.
     sqrt_quadrupleword_overflow: Word # Word # Word # Word -> Word;  % The most significant word of the square root of base*(base*(base*w1+w2)+w3)+w4.
     pred_word: Word->Word;   % Successor and predecessor that wraps around.
*/

/// \brief 

// template function overloads
std::string pp(const machine_number_list& x);
std::string pp(const machine_number_vector& x);
std::set<data::variable> find_all_variables(const data::machine_number& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MACHINE_NUMBER_H

