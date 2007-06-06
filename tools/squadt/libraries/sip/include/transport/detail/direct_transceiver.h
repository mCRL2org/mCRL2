#ifndef DIRECT_TRANSCEIVER_H
#define DIRECT_TRANSCEIVER_H

#include <sstream>

#include <transport/detail/transceiver.tcc>

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
        inline direct_transceiver(transporter* o);

        /** Constructor for use by the transporter constructor */
        inline direct_transceiver(transporter* o, direct_transceiver* p);

      public:

        /** Terminate the connection with the peer */
        inline void disconnect(basic_transceiver::ptr);

        /** Send a string input stream to the peer */
        inline void send(const std::string& data);
  
        /** Send the contents of an input stream to the peer */
        inline void send(std::istream& data);

        /** Destructor */
        inline ~direct_transceiver();
    };

    /**
     * @param o the transporter to associate with
     **/
    inline direct_transceiver::direct_transceiver(transporter* o) : basic_transceiver(o) {
    }

    /**
     * @param o the transporter to associate with
     **/
    inline direct_transceiver::direct_transceiver(transporter* o, direct_transceiver* p) : basic_transceiver(o), peer(p) {
      p->peer = this;
    }

    /**
     * @param p a shared pointer to this object
     **/
    inline void direct_transceiver::disconnect(basic_transceiver::ptr p) {
      transporter* o = owner;

      peer->handle_disconnect(this);

      owner = o;

      handle_disconnect(peer);

      owner = 0;
    }

    /**
     * @param d the data to be sent
     **/
    inline void direct_transceiver::send(const std::string& d) {
      std::istringstream input(d);
  
      basic_transceiver::deliver(input);
    }
  
    /**
     * @param s stream that contains the data to be sent
     **/
    inline void direct_transceiver::send(std::istream& s) {
      basic_transceiver::deliver(s);
    }

    inline direct_transceiver::~direct_transceiver() {
    }
  }
}

#endif

