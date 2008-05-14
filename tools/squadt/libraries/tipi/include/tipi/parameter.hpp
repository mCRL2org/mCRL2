//  Author(s): Jeroen van der Wulp
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file tipi/parameter.hpp

#ifndef PARAMETER_H__
#define PARAMETER_H__

#include "tipi/detail/utility/generic_visitor.hpp"

namespace tipi {

  /** \brief Base class for elements of a tipi::configuration */
  class parameter : public ::utility::visitable {

    public:

      /** \brief Destructor */
      virtual ~parameter() {
      }
  };
}

#endif
