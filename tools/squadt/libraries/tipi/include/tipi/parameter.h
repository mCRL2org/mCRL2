//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/parameter.h

#ifndef PARAMETER_H__
#define PARAMETER_H__

#include <tipi/utility/generic_visitor.h>

namespace tipi {

  /** \brief Base class for elements of a tipi::configuration */
  class parameter : public utility::visitable {

    public:

      /** \brief Destructor */
      virtual ~parameter() {
      }
  };
}

#endif
