#ifndef SIP_CAPABILITIES_H
#define SIP_CAPABILITIES_H

namespace sip {
  /** Type for protocol version */
  struct version {
    unsigned char major; /// the major number
    unsigned char minor; /// the minor number
  };

  /** Protocol version {major,minor} */
  static const version protocol_version = {1,0};
}

#endif
