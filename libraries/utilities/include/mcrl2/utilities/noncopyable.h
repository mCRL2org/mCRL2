// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_NONCOPYABLE_H_
#define MCRL2_UTILITIES_NONCOPYABLE_H_

namespace mcrl2
{
namespace utilities
{

/// \brief Inherit from this class to prevent it from being copyable.
/// \details Declares the copy (assignment) constructors as deleted.
class noncopyable
{
public:
  noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};

}
}

#endif // MCRL2_UTILITIES_NONCOPYABLE_H_
