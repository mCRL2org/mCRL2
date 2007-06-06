#ifndef SIP_SCHEMES_TCC_
#define SIP_SCHEMES_TCC_

#include <sip/detail/schemes.h>

namespace sip {

  namespace messaging {

    /** \brief Builds a connection between a tool communicator and a controller communicator */
    template < typename M >
    inline void traditional_scheme< M >::connect(basic_messenger_impl< M >* t) const {
      /** Not yet implemented */
      throw std::runtime_error("Sorry direct connection is not yet implemented");
    }
 
    /** \brief Builds a connection between a tool communicator and a controller communicator */
    template < typename M >
    inline void socket_scheme< M >::connect(basic_messenger_impl< M >* t) const {
      t->connect(host_name, static_cast < transport::port_t > (port));
    }
  }
}
#endif
