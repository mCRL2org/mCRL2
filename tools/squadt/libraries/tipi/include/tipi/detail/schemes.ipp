//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/schemes.ipp

#ifndef TIPI_SCHEMES_TCC_
#define TIPI_SCHEMES_TCC_

#include "tipi/detail/schemes.hpp"

namespace tipi {

  namespace messaging {

    /** \brief Builds a connection between a tool communicator and a controller communicator */
    template < typename M >
    inline void traditional_scheme< M >::connect(basic_messenger< M >* t) const {
      /** Not yet implemented */
      throw std::runtime_error("Sorry direct connection is not yet implemented");
    }
 
    /** \brief Builds a connection between a tool communicator and a controller communicator */
    template < typename M >
    inline void socket_scheme< M >::connect(basic_messenger< M >* t) const {
      t->connect(host_name, static_cast < short int > (port));
    }
  }
}
#endif
