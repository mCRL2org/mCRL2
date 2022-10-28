// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_SHARED_GUARD_H
#define MCRL2_ATERMPP_DETAIL_SHARED_GUARD_H

#include "mcrl2/utilities/noncopyable.h"

namespace atermpp::detail
{
  /// A lock guard for the shared mutex.
  class shared_guard : private mcrl2::utilities::noncopyable
  {
  public:
    shared_guard();

    ~shared_guard();
  };
} // namespace atermpp::detail

#endif // MCRL2_ATERMPP_DETAIL_SHARED_GUARD_H