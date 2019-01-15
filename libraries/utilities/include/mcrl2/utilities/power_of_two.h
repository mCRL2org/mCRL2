// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITY_POWER_OF_TWO_H_
#define MCRL2_UTILITY_POWER_OF_TWO_H_

#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace mcrl2
{
namespace utilities
{

/// \returns True when the given value is a power of two.
template<typename T,
         typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
static constexpr bool is_power_of_two(T value)
{
  // It is a power of two whenever exactly a single bit is one.
  return value != 0 && (value & (value - 1)) == 0;
}

/// \returns The smallest power of two that is larger than the given value.
template<typename T,
         typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
static T round_up_to_power_of_two(T value)
{
  if (is_power_of_two(value)) {
    return value;
  }

  if (value == 0) {
    return 1;
  }

  // This for loop essentially sets all bits to the right of a bit that is equal
  // to one to all being ones, i.e. 0x0...010...0 becomes 0x0...011...1.
  for(T i = 1; i < sizeof(T) * std::numeric_limits<unsigned char>::digits; i *= 2) {
      value |= value >> i;
  }

  // The result will be a value with 0x00..011..1 and adding one to it will result
  // in a power of two.
  assert(is_power_of_two(value + 1));
  return value + 1;
}

} // namespace utilities
} // namespace mcrl2`

#endif // MCRL2_UTILITY_POWER_OF_TWO_H_
