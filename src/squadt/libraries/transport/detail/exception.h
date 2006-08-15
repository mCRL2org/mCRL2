#ifndef TRANSPORT_EXCEPTION_H
#define TRANSPORT_EXCEPTION_H

#include <utility/exception.h>

namespace transport {

  /** \brief Type for exception identification */
  enum values {
    listener_failure,    ///< \brief a listener failed
    transceiver_failure  ///< \brief a transceiver failed
  };

  /** \brief The common exception type */
  typedef ::utility::exception < values > exception;

#ifdef IMPORT_STATIC_DEFINITIONS
  template < >
  const char* const ::utility::exception< transport::values >::descriptions[] = {
    "Listener failed connection attempt!",
    "Transceiver failed send or receive!"
  };
#endif
}

#endif

