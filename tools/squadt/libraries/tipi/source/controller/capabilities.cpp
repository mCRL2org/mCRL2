//  Author(s): Jeroen van der Wulp
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file controller/capabilities.cpp

#include <ostream>
#include <sstream>
#include <utility>

#include <boost/bind.hpp>

#include "tipi/controller/capabilities.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/visitors.hpp"

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
