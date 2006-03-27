#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <boost/shared_ptr.hpp>

namespace transport {
  class transporter;

  namespace transceiver {

    /**
     * \brief Base class for transceivers
     **/
    class basic_transceiver {
      friend class transport::transporter;

      public:

        /** \brief Convenience type to hide the shared pointer */
        typedef boost::shared_ptr < basic_transceiver > ptr;

      private:

        /** \brief The local owner on this side of the connection */
        transporter* owner;

      private:

        /** \brief Get the current owner */
        inline transporter const* get_owner() const;

      protected:

        /** \brief Deliver the data from the input stream to the owner */
        inline void deliver(std::istream& input);

        /** \brief Removes this transceiver object from a list of connections */
        inline void handle_disconnect(basic_transceiver*);

      public:

        /** \brief Constructor */
        inline basic_transceiver(transporter*);
     
        /** \brief Function that facilitates disconnection (on both sides of a connection) */
        virtual void disconnect(basic_transceiver::ptr) = 0;
     
        /** \brief Send a string input stream to the peer */
        virtual void send(const std::string&) = 0;
     
        /** \brief Send the contents of an input stream to the peer */
        virtual void send(std::istream&) = 0;

        /** \brief Destructor */
        virtual inline ~basic_transceiver();
    };
  }
}

#endif

