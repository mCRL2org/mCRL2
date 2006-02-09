#ifndef ADDRESS_H
#define ADDRESS_H

#include <boost/asio.hpp>

namespace transport {
  /** IP version 4 address verifier (refer to the asio documentation) */
  typedef asio::ipv4::address address;

  /** Default port for TCP/UDP connections */
  static long default_port = 10946;
}

#endif

