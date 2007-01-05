#ifndef BASIC_MESSENGER_H
#define BASIC_MESSENGER_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <utility/print_logger.h>

#include <sip/detail/message.h>

namespace transport {
  class transporter;
}

namespace sip {

  namespace messaging {
    template < class M > class basic_messenger;

    template < class M >
    class basic_messenger_impl;

    /**
     * \brief Abstract communicator class that divides an incoming data stream in messages
     *
     * M is the type of a messenger::message or derived type
     */
    template < class M >
    class basic_messenger {

      protected:

        /** \brief Shared pointer to an implementation object */
        boost::shared_ptr < basic_messenger_impl < M > > impl;

      protected:
 
        /** \brief Alternate constructor */
        basic_messenger(basic_messenger_impl< M >*);

      public:

        /** \brief Convenience type for messages of type M */
        typedef M                                                         message;

        /** \brief Convenience type for shared pointers */
        typedef boost::shared_ptr < M >                                   message_ptr;

        /** \brief Convenience type for handlers */
        typedef boost::function < void (boost::shared_ptr < M > const&) > handler_type;
 
      public:

        /** \brief Default constructor */
        basic_messenger();

        /** \brief Default constructor */
        basic_messenger(boost::shared_ptr < utility::logger >);

        /** \brief Destroys all connections */
        void disconnect();
 
        /** \brief Wait until the next message of a certain type arrives */
        const boost::shared_ptr < M > await_message(typename M::type_identifier_t);
 
        /** \brief Wait until the next message of a certain type arrives */
        const boost::shared_ptr < M > await_message(typename M::type_identifier_t, long const&);
 
        /** \brief Send a message */
        void send_message(const message&);
 
        /** \brief Set the handler for a type */
        void add_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Clears the handlers for a message type */
        void clear_handlers(const typename M::type_identifier_t);

        /** \brief Remove a specific handlers for a message type */
        void remove_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Gets the associated logger object */
        utility::logger* get_logger();

        /** \brief Gets the associated logger object */
        static utility::logger* get_standard_logger();

        /** \brief Sets the standard logger object */
        static void set_standard_logger(boost::shared_ptr < utility::logger >);
    };
  }
}

#endif
