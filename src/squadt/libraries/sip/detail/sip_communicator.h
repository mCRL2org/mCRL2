#ifndef SIP_COMMUNICATOR_H
#define SIP_COMMUNICATOR_H

#include <iosfwd>

#include <boost/thread/barrier.hpp>
#include <boost/shared_ptr.hpp>

#include <transport/transporter.h>

#include <sip/detail/message.h>

namespace sip {

  namespace communicator {
    class sip_communicator;
 
    /* Abstract communicator class that divides an incoming data stream in messages */
    class sip_communicator : public transport::transporter {
      public:
 
        typedef boost::shared_ptr < message > message_ptr;
 
        sip_communicator();
 
        /** Queues incoming messages */
        void deliver(std::istream&);
 
        /** Queues incoming messages */
        void deliver(std::string&);
 
        /* Wait until the next message arrives */
        void await_message();
 
        /** Send a message */
        inline void send_message(message&);
 
        /** Pops the first message of the queue */
        inline message pop_message();
 
        /** Get the first message in the queue */
        inline message& peek_message();
 
        inline size_t number_of_messages();
 
      protected:
 
        /** The current message queue */
        std::deque < message_ptr > message_queue;
 
        /** Buffer that holds content until a message is complete */
        std::string      buffer;
 
        /** For barrier synchronisation (used with function await_message) */
        boost::barrier   wait_lock;
 
        bool             using_buffer;
        bool             using_lock;
    };
 
    /* Send a message */
    inline void sip_communicator::send_message(message& m) {
      send(m.to_xml());
    }
 
    /* \pre{the message queue is not empty} */
    inline message sip_communicator::pop_message() {
      message_ptr m = message_queue.front();
 
      message_queue.pop_front();
 
      return (*m);
    }
 
    /* \pre{the message queue is not empty} */
    inline message& sip_communicator::peek_message() {
      message_ptr m = message_queue.front();
 
      message_queue.front();
 
      return (*m);
    }
 
    inline size_t sip_communicator::number_of_messages() {
      return (message_queue.size());
    }
  }
}

#endif
