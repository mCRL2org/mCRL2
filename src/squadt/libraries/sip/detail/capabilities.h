#ifndef SIP_CAPABILITIES_H
#define SIP_CAPABILITIES_H

namespace sip {
  /** \brief Type for protocol version */
  struct version {
    unsigned char major; ///< the major number
    unsigned char minor; ///< the minor number
  };

  /** \brief Protocol version {major,minor} */
  static const version default_protocol_version = {1,0};
}

#endif
