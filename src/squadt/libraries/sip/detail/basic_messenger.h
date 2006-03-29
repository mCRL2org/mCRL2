#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <deque>
#include <iosfwd>
#include <map>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/barrier.hpp>

#include <transport/transporter.h>

/* Braindead macro defined in one of the system headers included by transport.h */
#undef barrier

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

        /** Convenience type for messages of type M */
        typedef M                                                           message;

        /** Convenience type for pointers to messages using a boost shared pointer */
        typedef boost::shared_ptr < message >                               message_ptr;

        /** Convenience type for handlers */
        typedef boost::function2 < void, message_ptr&, basic_transceiver* > handler_type;
 
      private:

        /** Convenience type for pointers to wait locks using a boost scoped pointer */
        typedef boost::shared_ptr< boost::barrier >                        barrier_ptr;

        /** Type for the map used to associate a handler to a message type */
        typedef std::map < typename M::type_identifier_t, handler_type >   handler_map;

        /** Type for the map used to associate a handler to a lock primitive */
        typedef std::map < typename M::type_identifier_t, barrier_ptr >    waiter_map;

        /** Type for the message queue */
        typedef std::deque < message_ptr >                                 message_queue_t;

        /** The XML-like tag used for wrapping the content */
        static const std::string   tag_open;

        /** The XML-like tag used for wrapping the content */
        static const std::string   tag_close;

      private:

        /** Handlers based on message types */
        handler_map                handlers;
 
        /** For barrier synchronisation (used with function await_message) */
        waiter_map                 waiters;
 
        /** The current message queue (unhandled messages end up here) */
        message_queue_t            message_queue;

        /** Buffer that holds content until a message is complete */
        std::string                buffer;
 
        /** Whether a message start tag has been matched after the most recent message end tag */
        bool                       message_open;

        /** The number of tag elements (of message::tag) that have been matched at the last delivery */
        unsigned char              partially_matched;

      private:

        /** Helper function that delivers an incoming message directly to a waiter */
        static inline void deliver_to_waiter(message_ptr&, basic_transceiver*, message_ptr&);

        /** Helper function that delivers an incoming message directly to a waiter */
        static inline void deliver_to_waiter(message_ptr&, basic_transceiver*, message_ptr&, handler_type);

      public:

        /** Default constructor */
        basic_messenger();
 
        /** Queues incoming messages */
        virtual void deliver(std::istream&, basic_transceiver*);
 
        /** Queues incoming messages */
        virtual void deliver(const std::string&, basic_transceiver*);
 
        /* Wait until the next message arrives */
        const message_ptr await_message(typename M::type_identifier_t);
 
        /** Send a message */
        inline void send_message(const message&);
 
        /** Pops the first message of the queue */
        inline message_ptr pop_message();
 
        /** Get the first message in the queue */
        inline message& peek_message();

        /** Wait until the first message of type t has arrived */
        inline message_ptr find_message(const typename M::type_identifier_t);
 
        /** Remove a message from the queue */
        inline void remove_message(message_ptr& p);

        /** Returns the number of messages in the queue */
        inline size_t number_of_messages();
 
        /** Set the handler for a type */
        inline void set_handler(handler_type, const typename M::type_identifier_t = static_cast < typename M::type_identifier_t > (0));

        /** Unset the handler for a type */
        inline void unset_handler(const typename M::type_identifier_t);
    };

    template < class M >
    inline basic_messenger< M >::basic_messenger() : message_open(false), partially_matched(0) {
    }
  }
}

#endif
