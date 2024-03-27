// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_DATA_CONFIGURATION_H
#define MCRL2_DATA_CONFIGURATION_H

namespace mcrl2
{
namespace data
{
namespace detail
{
/// \brief If Enable64bitNumbers is defined, positive and natural numbers in 64 bits representation are used.
/// \detail If undefined constructors @c0, @c1 and @cDub are used in accordance to the book Modelling and Analysis of Communicating Systems, 2014. 
#define Enable64bitNumbers = true;

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_CONFIGURATION_H

