#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <deque>
#include <iosfwd>
#include <map>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/function_equal.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <transport/transporter.h>

#include <sip/detail/message.h>
#include <sip/detail/common.h>

namespace sip {

  namespace messaging {
    template < class M > class basic_messenger;

    using transport::transceiver::basic_transceiver;

    /**
     * \brief Abstract communicator class that divides an incoming data stream in messages
     *
     * M is the type of a messenger::message or derived type
     */
    template < class M >
    class basic_messenger : public transport::transporter {
      public:

        /** \brief Convenience type for messages of type M */
        typedef M                                                                       message;

        /** \brief Convenience type for pointers to messages using a boost shared pointer */
        typedef boost::shared_ptr < message >                                           message_ptr;

        /** \brief Convenience type for shared pointers */
        typedef boost::weak_ptr < basic_messenger < M > >                               wptr;

        /** \brief Convenience type for handlers */
        typedef boost::function2 < void, const message_ptr&, const basic_transceiver* > handler_type;
 
      private:

        /** \brief Monitor synchronisation construct */
        struct monitor {
          boost::condition condition; ///< a boost::condition synchronisation primitive
          boost::mutex     mutex;     ///< a boost::mutex synchronisation primitive

          /** \brief Convenience type for shared pointers */
          typedef boost::shared_ptr < monitor > ptr;
        };

        /** \brief Type for the map used to associate a handler to a message type */
        typedef std::map < typename M::type_identifier_t, handler_type >            handler_map;

        /** \brief Type for the map used to associate a handler to a lock primitive */
        typedef std::map < typename M::type_identifier_t, typename monitor::ptr >   waiter_map;

        /** \brief Type for the message queue */
        typedef std::deque < message_ptr >                                          message_queue_t;

        /** \brief The XML-like tag used for wrapping the content */
        static const std::string                                                    tag_open;

        /** \brief The XML-like tag used for wrapping the content */
        static const std::string                                                    tag_close;

      private:

        /** \brief Handlers based on message types */
        handler_map                handlers;
 
        /** \brief For blocking until delivery (used with function await_message) */
        waiter_map                 waiters;
 
        /** \brief Used to ensure any element t (in M::type_identifier_t) in waiters is assigned to at most once */
        boost::mutex               waiter_lock;

        /** \brief The current message queue (unhandled messages end up here) */
        message_queue_t            message_queue;

        /** \brief Buffer that holds content until a message is complete */
        std::string                buffer;
 
        /** \brief Whether a message start tag has been matched after the most recent message end tag */
        bool                       message_open;

        /** \brief The number of tag elements (of message::tag) that have been matched at the last delivery */
        unsigned char              partially_matched;

      private:

        /** \brief Helper function that services the handlers */
        inline void   service_handlers(const message_ptr, const basic_transceiver*);

        /** \brief Helper function that delivers an incoming message directly to a waiter */
        inline static void   deliver_to_waiter(const message_ptr&, const basic_transceiver*, message_ptr&);

        /** \brief Helper function that delivers an incoming message directly to a waiter */
        inline static void   deliver_to_waiter(const message_ptr&, const basic_transceiver*, message_ptr&, handler_type);

      public:

        /** \brief Default constructor */
        basic_messenger();
 
        /** \brief Queues incoming messages */
        virtual void deliver(std::istream&, basic_transceiver*);
 
        /** \brief Queues incoming messages */
        virtual void deliver(const std::string&, basic_transceiver*);
 
        /* \brief Wait until the next message of a certain type arrives */
        const message_ptr await_message(typename M::type_identifier_t);
 
        /** \brief Send a message */
        inline void send_message(const message&);
 
        /** \brief Pops the first message of the queue */
        inline message_ptr pop_message();
 
        /** \brief Get the first message in the queue */
        inline message& peek_message();

        /** \brief Wait until the first message of type t has arrived */
        inline message_ptr find_message(const typename M::type_identifier_t);
 
        /** \brief Remove a message from the queue */
        inline void remove_message(message_ptr& p);

        /** \brief Returns the number of messages in the queue */
        inline size_t number_of_messages();
 
        /** \brief Set the handler for a type */
        inline void add_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Clears the handlers for a message type */
        inline void clear_handlers(const typename M::type_identifier_t);

        /** \brief Remove a specific handlers for a message type */
        inline void remove_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Destructor */
        ~basic_messenger();
    };

    template < class M >
    inline basic_messenger< M >::basic_messenger() : message_open(false), partially_matched(0) {
    }
  }
}

#endif
