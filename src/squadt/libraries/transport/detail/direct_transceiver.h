#ifndef DIRECT_TRANSCEIVER_H
#define DIRECT_TRANSCEIVER_H

#include <sstream>

#include <transport/detail/transceiver.h>

namespace transport {
  namespace transceiver {

    /* Class that is used internally for direct transmitting/receiving */
    class direct_transceiver : public basic_transceiver {
      friend class transport::transporter;
  
      private:
 
        /** The other side of the connection */
        direct_transceiver* peer;

        /** Deliver data to owner */
        inline void deliver(std::istream& input);

        /** Constructor for use by the transporter constructor */
        inline direct_transceiver(transporter& o);

        /** Constructor for use by the transporter constructor */
        inline direct_transceiver(transporter& o, direct_transceiver* p);

      public:

        /** Terminate the connection with the peer */
        inline void disconnect(transporter::connection_ptr);

        /** Send a string input stream to the peer */
        inline void send(const std::string& data);
  
        /** Send the contents of an input stream to the peer */
        inline void send(std::istream& data);

        /** Destructor */
        inline ~direct_transceiver();
    };

    inline direct_transceiver::direct_transceiver(transporter& o) : basic_transceiver(o) {
    }

    /* Constructor for use by the transporter constructor */
    inline direct_transceiver::direct_transceiver(transporter& o, direct_transceiver* p) : basic_transceiver(o), peer(p) {
      p->peer = this;
    }

    /* Send a string input stream to the peer */
    inline void direct_transceiver::disconnect(transporter::connection_ptr) {
      peer->handle_disconnect(this);

      handle_disconnect(peer);
    }

    /* Send a string input stream to the peer */
    inline void direct_transceiver::send(const std::string& data) {
      std::istringstream input(data);
  
      basic_transceiver::deliver(input);
    }
  
    /* Send the contents of an input stream to the peer */
    inline void direct_transceiver::send(std::istream& data) {
      basic_transceiver::deliver(data);
    }

    inline direct_transceiver::~direct_transceiver() {
    }
  }
}

#endif

