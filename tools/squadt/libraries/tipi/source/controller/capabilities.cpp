// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>
#include <sstream>
#include <utility>

#include "tipi/controller/capabilities.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/detail/visitors.hpp"

namespace tipi {
  namespace controller {
     /**
     * \brief Operator for writing to stream
     *
     * \param[in,out] s stream to write to
     * \param[in] c the capabilities object to write out
     **/
    std::ostream& operator << (std::ostream& s, controller::capabilities const& c) {
      visitors::store(c, s);

      return (s);
    }
  }
}
