#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <transport/transporter.h>

namespace transport {
  namespace transceiver {

    /**
     * \brief Base class for transceivers
     **/
    class basic_transceiver {
      friend void transporter::disconnect(transporter&);

      protected:
        /** The local owner on this side of the connection */
        transporter& owner;

        /** Deliver the data from the input stream to the owner */
        inline void deliver(std::istream& input);

        /** Removes this transceiver object from a list of connections */
        inline void handle_disconnect(basic_transceiver*);

      public:

        /** \brief Constructor */
        inline basic_transceiver(transporter&);
     
        /** \brief Destructor */
        virtual inline ~basic_transceiver();

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

    inline void basic_transceiver::handle_disconnect(basic_transceiver* t) {
      /* Remove instance from the list of connections */
      transporter::connection_list::iterator i = owner.connections.begin();
      transporter::connection_list::iterator b = owner.connections.end();

      while (i != b) {
        if ((*i).get() == t) {
          owner.connections.erase(i);

          break;
        }

        ++i;
      }
    }
  }
}

#endif

