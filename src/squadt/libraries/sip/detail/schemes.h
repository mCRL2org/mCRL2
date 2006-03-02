#ifndef SIP_SCHEMES_H
#define SIP_SCHEMES_H

#include <sip/detail/exception.h>

namespace sip {

  namespace cli {
    class command_line_argument_extractor;
  }

  /** \brief Abstract base class for container types for parsed information about schemes */
  class scheme : public boost::noncopyable {
    public:
      virtual ~scheme() = 0;

      /** Builds a connection between a tool communicator and a controller communicator */
      virtual void connect(tool_communicator&) const = 0;
  };

  /** \brief Derived class for the traditional scheme */
  class traditional_scheme : public scheme {
    friend class cli::command_line_argument_extractor;

    public:
      inline traditional_scheme();

      /** Builds a connection between a tool communicator and a controller communicator, using the traditional scheme */
      inline void connect(tool_communicator&) const;
  };

  /** \brief Derived class for the socket scheme */
  class socket_scheme : public scheme {
    friend class cli::command_line_argument_extractor;

    private:
      /** \brief IPv4 address of endpoint */
      std::string host_name;

      /** \brief Port number */
      long int    port;

    public:
      inline socket_scheme();

      /** Builds a connection between a tool communicator and a controller communicator, using the socket scheme */
      inline void connect(tool_communicator&) const;
  };

  /** Destructor */
  inline scheme::~scheme() {
  }

  /** Constructor */
  inline traditional_scheme::traditional_scheme() {
  }

  /* Builds a connection between a tool communicator and a controller communicator */
  inline void traditional_scheme::connect(tool_communicator& t) const {
    /** Not yet implemented */
    throw (exception(exception_identifier::not_yet_implemented));
  }

  /** Constructor */
  inline socket_scheme::socket_scheme() {
  }

  /* Builds a connection between a tool communicator and a controller communicator */
  inline void socket_scheme::connect(tool_communicator& t) const {
    /** \attention Only ipv4 addresses are accepted for the host specification at this moment. */
  }
}
#endif
