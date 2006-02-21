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

  namespace communicator {
    class basic_messenger;

    /* Abstract communicator class that divides an incoming data stream in messages */
    class basic_messenger : public transport::transporter {
      public:

        typedef message < sip_message_types, unknown >  message;

        /** Convenience type for pointers to messages using a boost shared pointer */
        typedef boost::shared_ptr < message >           message_ptr;

        /** Convenience type for handlers */
        typedef boost::function1 < void, message_ptr& > handler_type;
 
      private:

        /** Convenience type for pointers to wait locks using a boost scoped pointer */
        typedef boost::shared_ptr< boost::barrier >                barrier_ptr;

        /** Type for the map used to associate a handler to a message type */
        typedef std::map < message::type_identifier_t, handler_type >   handler_map;

        /** Type for the map used to associate a handler to a lock primitive */
        typedef std::map < message::type_identifier_t, barrier_ptr >    waiter_map;

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
        void await_message(message::type_identifier_t);
 
        /** Send a message */
        inline void send_message(const message&);
 
        /** Pops the first message of the queue */
        inline message pop_message();
 
        /** Get the first message in the queue */
        inline message& peek_message();
 
        inline size_t number_of_messages();
 
        /** Set the handler for a type */
        inline void set_handler(handler_type, const message::type_identifier_t = (message::type_identifier_t) 0);

        /** Unset the handler for a type */
        inline void unset_handler(const message::type_identifier_t);
    };

    const std::string basic_messenger::tag_open("<message>");

    const std::string basic_messenger::tag_close("</message>");

    inline basic_messenger::basic_messenger() : message_open(false), partially_matched(0) {
    }
 
    /* Send a message */
    inline void basic_messenger::send_message(const message& m) {
      send(m.to_xml());
    }
 
    /* \pre{the message queue is not empty} */
    inline basic_messenger::message basic_messenger::pop_message() {
      message_ptr m = message_queue.front();
 
      message_queue.pop_front();
 
      return (*m);
    }
 
    /* \pre{the message queue is not empty} */
    inline basic_messenger::message& basic_messenger::peek_message() {
      message_ptr m = message_queue.front();
 
      message_queue.front();
 
      return (*m);
    }
 
    inline size_t basic_messenger::number_of_messages() {
      return (message_queue.size());
    }

    /** \pre{there is no waiter for this type} */
    inline void basic_messenger::set_handler(handler_type h, const message::type_identifier_t t) {
      assert(waiters.count(t) == 0);

      handlers[t] = h;
    }

    inline void basic_messenger::unset_handler(const message::type_identifier_t t) {
      handlers.erase(t);
    }
  }
}

#endif
