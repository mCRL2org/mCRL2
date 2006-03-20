#ifndef TRANSPORT_EXCEPTION_H
#define TRANSPORT_EXCEPTION_H

#include <exception/exception.h>

namespace transport {
  namespace exception_identifier {
    /** \brief Type for exception identification */
    enum values {
      listener_failure,    ///< \brief a listener failed
      transceiver_failure  ///< \brief a transceiver failed
    };
  }

  /** \brief The common exception type */
  typedef exception::exception < exception_identifier::values > exception;
}

#endif

