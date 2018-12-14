// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_IGNOREUNUSED_H_
#define MCRL2_UTILITIES_IGNOREUNUSED_H_

namespace mcrl2
{
namespace utilities
{

/// \brief Can be used to suppress unused variable warnings, for example when
///        a variable is required in assertions.
template <typename... Ts>
inline void ignore_unused(Ts const& ...)
{}

}
}

#endif // MCRL2_UTILITIES_IGNOREUNUSED_H_
