#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <transport/transporter.h>

namespace transport {
  namespace transceiver {

    class Transceiver {
      friend void transporter::disconnect(transporter&);

      protected:
        /** The local owner on this side of the connection */
        transporter& owner;

        /** Deliver the data from the input stream to the owner */
        inline void deliver(std::istream& input);

        /** Removes this transceiver object from a list of connections */
        void handle_disconnect(Transceiver*);

      public:

        Transceiver(transporter&);
     
        virtual ~Transceiver();

        /** Function that facilitates disconnection (on both sides of a connection) */
        virtual void disconnect(transporter::ConnectionPtr) = 0;
     
        /** Send a string input stream to the peer */
        virtual void send(const std::string&) = 0;
     
        /** Send the contents of an input stream to the peer */
        virtual void send(std::istream&) = 0;
    };

    inline Transceiver::Transceiver(transporter& o) : owner(o) {
    }

    inline Transceiver::~Transceiver() {
    }

    inline void Transceiver::deliver(std::istream& input) {
      owner.deliver(input);
    }

    class exception : public transport::exception {
      private:
        Transceiver* t;

      public:
        exception(Transceiver*);
 
        /** Returns the type of the exception */
        exception_type type();
 
        Transceiver* transceiver();
    };
  }
}

#endif

