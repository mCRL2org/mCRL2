#ifndef DIRECT_TRANSCEIVER_H
#define DIRECT_TRANSCEIVER_H

#include <sstream>

#include <detail/transceiver.h>

namespace transport {
  namespace transceiver {

    /* Class that is used internally for direct transmitting/receiving */
    class DirectTransceiver : public Transceiver {
      friend class transport::transporter;
  
      private:
 
        /** The other side of the connection */
        DirectTransceiver* peer;

        /** Deliver data to owner */
        inline void deliver(std::istream& input);

        /** Constructor for use by the transporter constructor */
        inline DirectTransceiver(transporter& o);

        /** Constructor for use by the transporter constructor */
        inline DirectTransceiver(transporter& o, DirectTransceiver* p);

      public:

        /** Terminate the connection with the peer */
        inline void disconnect(transporter::ConnectionPtr);

        /** Send a string input stream to the peer */
        inline void send(const std::string& data);
  
        /** Send the contents of an input stream to the peer */
        inline void send(std::istream& data);

        /** Destructor */
        inline ~DirectTransceiver();
    };

    inline DirectTransceiver::DirectTransceiver(transporter& o) : Transceiver(o) {
    }

    /* Constructor for use by the transporter constructor */
    inline DirectTransceiver::DirectTransceiver(transporter& o, DirectTransceiver* p) : Transceiver(o), peer(p) {
      p->peer = this;
    }

    /* Send a string input stream to the peer */
    inline void DirectTransceiver::disconnect(transporter::ConnectionPtr) {
      peer->handle_disconnect(this);

      handle_disconnect(peer);
    }

    /* Send a string input stream to the peer */
    inline void DirectTransceiver::send(const std::string& data) {
      std::istringstream input(data);
  
      Transceiver::deliver(input);
    }
  
    /* Send the contents of an input stream to the peer */
    inline void DirectTransceiver::send(std::istream& data) {
      Transceiver::deliver(data);
    }

    inline DirectTransceiver::~DirectTransceiver() {
    }
  }
}

#endif

