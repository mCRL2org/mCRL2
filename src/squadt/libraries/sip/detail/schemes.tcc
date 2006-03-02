#ifndef SIP_SCHEMES_TCC
#define SIP_SCHEMES_TCC

#include <sip/detail/schemes.h>

namespace sip {

  /** \brief Destructor */
  inline scheme::~scheme() {
  }

  /** \brief Constructor */
  inline traditional_scheme::traditional_scheme() {
  }

  /* Builds a connection between a tool communicator and a controller communicator */
  inline void traditional_scheme::connect(tool_communicator& t) const {
    /** Not yet implemented */
    throw (exception(exception_identifier::not_yet_implemented));
  }

  /** \brief Constructor */
  inline socket_scheme::socket_scheme() {
  }

  /* Builds a connection between a tool communicator and a controller communicator */
  inline void socket_scheme::connect(tool_communicator& t) const {
    t.connect(host_name, port);
  }
}

#endif
