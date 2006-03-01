#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <transport/transporter.h>

namespace transport {
  namespace transceiver {

    class basic_transceiver {
      friend void transporter::disconnect(transporter&);

      protected:
        /** The local owner on this side of the connection */
        transporter& owner;

        /** Deliver the data from the input stream to the owner */
        inline void deliver(std::istream& input);

        /** Removes this transceiver object from a list of connections */
        void handle_disconnect(basic_transceiver*);

      public:

        inline basic_transceiver(transporter&);
     
        inline virtual ~basic_transceiver();

        /** Function that facilitates disconnection (on both sides of a connection) */
        virtual void disconnect(transporter::connection_ptr) = 0;
     
        /** Send a string input stream to the peer */
        virtual void send(const std::string&) = 0;
     
        /** Send the contents of an input stream to the peer */
        virtual void send(std::istream&) = 0;
    };

    inline basic_transceiver::basic_transceiver(transporter& o) : owner(o) {
    }

    inline basic_transceiver::~basic_transceiver() {
    }

    inline void basic_transceiver::deliver(std::istream& input) {
      owner.deliver(input);
    }

    class exception : public transport::exception {
      private:
        basic_transceiver* t;

      public:
        exception(basic_transceiver*);
 
        /** Returns the type of the exception */
        exception_type type();
 
        basic_transceiver* originator();
    };
  }
}

#endif

