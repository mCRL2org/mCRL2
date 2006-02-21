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

  namespace messenger {
    template < class M > class basic_messenger;

    /**
     * \brief Abstract communicator class that divides an incoming data stream in messages
     *
     * M is the type of a messenger::message or derived type
     */
    template < class M >
    class basic_messenger : public transport::transporter {
      public:

        typedef M                                       message;

        /** Convenience type for pointers to messages using a boost shared pointer */
        typedef boost::shared_ptr < message >           message_ptr;

        /** Convenience type for handlers */
        typedef boost::function1 < void, message_ptr& > handler_type;
 
      private:

        /** Convenience type for pointers to wait locks using a boost scoped pointer */
        typedef boost::shared_ptr< boost::barrier >                     barrier_ptr;

        /** Type for the map used to associate a handler to a message type */
        typedef std::map < typename M::type_identifier_t, handler_type >   handler_map;

        /** Type for the map used to associate a handler to a lock primitive */
        typedef std::map < typename M::type_identifier_t, barrier_ptr >    waiter_map;

        /** Handlers based on message types */
        handler_map                handlers;
 
        /** For barrier synchronisation (used with function await_message) */
        waiter_map                 waiters;
 
        /** The current message queue (unhandled messages end up here) */
        std::deque < message_ptr > message_queue;

        /** Buffer that holds content until a message is complete */
        std::string                buffer;
 
        /** Whether a message start tag has been matched after the most recent message end tag */
        bool                       message_open;

        /** The number of tag elements (of message::tag) that have been matched at the last delivery */
        unsigned char              partially_matched;

        /** The XML-like tags used for wrapping the content */
        static const std::string   tag_open;
        static const std::string   tag_close;

      public:

        basic_messenger();
 
        /** Queues incoming messages */
        virtual void deliver(std::istream&);
 
        /** Queues incoming messages */
        virtual void deliver(std::string&);
 
        /* Wait until the next message arrives */
        void await_message(typename M::type_identifier_t);
 
        /** Send a message */
        inline void send_message(const message&);
 
        /** Pops the first message of the queue */
        inline message pop_message();
 
        /** Get the first message in the queue */
        inline message& peek_message();
 
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
