#ifndef BASIC_MESSENGER_H
#define BASIC_MESSENGER_H

#include <deque>
#include <iosfwd>
#include <map>
#include <set>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/function_equal.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <transport/transporter.h>
#include <utility/print_logger.h>

#include <sip/detail/message.h>

namespace transport {
  class transporter;
}

namespace sip {

  namespace messaging {
    template < class M > class basic_messenger;

    /**
     * \brief Abstract communicator class that divides an incoming data stream in messages
     *
     * M is the type of a messenger::message or derived type
     */
    template < class M >
    class basic_messenger : public transport::transporter {

      private:

        /** \brief Convenience type for handlers */
        typedef boost::function < void (boost::shared_ptr < M > const&) > handler_type;

        /** \brief Strict weak order on handler_type (for use with std::set) */
        class compare_handlers {
          public:

            inline bool operator()(handler_type const&, handler_type const&);
        };

      public:

        /** \brief Convenience type for messages of type M */
        typedef M                                             message;

        /** \brief Convenience type for shared pointers */
        typedef boost::shared_ptr < M >                       message_ptr;

        /** \brief Convenience type for shared pointers */
        typedef boost::weak_ptr < basic_messenger < M > >     wptr;

        /** \brief Set of handlers */
        typedef std::set < handler_type, compare_handlers >   handler_set;
 
      private:

        /** \brief Monitor synchronisation construct */
        class waiter_data {

          private:

            /** boost::mutex synchronisation primitive */
            boost::mutex                               mutex;

            /** boost::condition synchronisation primitive */
            boost::condition                           condition;

            /** pointers to local message variables */
            std::vector < boost::shared_ptr < M >* >   pointers;

          public:

            /** \brief Constructor */
            waiter_data(boost::shared_ptr < M >&);

            /** \brief Block until the next message has been delivered, or the object is destroyed */
            void wait(boost::function < void () >);

            /** \brief Wake up all treads that are blocked via wait(), and delivers a message */
            void wait(boost::function < void () >, long const&);

            /** \brief Wake up all treads that are blocked via wait(), and delivers a message */
            void wake(boost::shared_ptr < M > const&);

            /** \brief Wake up all treads that are blocked via wait() */
            void wake();

            /** \brief Destructor */
            ~waiter_data();
        };

        /** \brief Type for the map used to associate a handler to a message type */
        typedef std::map < typename M::type_identifier_t, handler_set >                                     handler_map;

        /** \brief Type for the map used to associate a handler to a lock primitive */
        typedef std::map < typename M::type_identifier_t, boost::shared_ptr < waiter_data > >               waiter_map;

        /** \brief Type for the message queue */
        typedef std::deque < boost::shared_ptr < M > >                                                      message_queue_t;

        /** \brief The XML-like tag used for wrapping the content */
        static const std::string                                                                            tag_open;

        /** \brief The XML-like tag used for wrapping the content */
        static const std::string                                                                            tag_close;

        /** \brief Standard (clog) logging component */
        static utility::print_logger                                                                        standard_error_logger;

      private:

        /** \brief Handlers based on message types */
        handler_map                handlers;
 
        /** \brief For blocking until delivery (used with function await_message) */
        waiter_map                 waiters;
 
        /** \brief Used to ensure any element t (in M::type_identifier_t) in waiters is assigned to at most once */
        boost::recursive_mutex     waiter_lock;

        /** \brief The current task queue (messages to be delivered) */
        message_queue_t            task_queue;

        /** \brief The current message queue (unhandled messages end up here) */
        message_queue_t            message_queue;

        /** \brief Buffer that holds content until a message is complete */
        std::string                buffer;
 
        /** \brief Whether a message start tag has been matched after the most recent message end tag */
        bool                       message_open;

        /** \brief The number of tag elements (of message::tag) that have been matched at the last delivery */
        unsigned char              partially_matched;

      protected:

        /** \brief The component used for logging */
        utility::logger*           logger;

      private:

        /** \brief Helper function that services the handlers */
        inline void   service_handlers();

      public:

        /** \brief Default constructor */
        basic_messenger(utility::logger* = &standard_error_logger);

        /** \brief Destroys all connections */
        void disconnect();
 
        /** \brief Queues incoming messages */
        virtual void deliver(std::istream&, typename M::end_point);
 
        /** \brief Queues incoming messages */
        virtual void deliver(const std::string&, typename M::end_point);
 
        /* \brief Wait until the next message of a certain type arrives */
        const boost::shared_ptr < M > await_message(typename M::type_identifier_t);
 
        /* \brief Wait until the next message of a certain type arrives */
        const boost::shared_ptr < M > await_message(typename M::type_identifier_t, long const&);
 
        /** \brief Send a message */
        void send_message(const message&);
 
        /** \brief Pops the first message of the queue */
        inline boost::shared_ptr < M > pop_message();
 
        /** \brief Get the first message in the queue */
        inline message& peek_message();

        /** \brief Wait until the first message of type t has arrived */
        boost::shared_ptr < M > find_message(const typename M::type_identifier_t);
 
        /** \brief Remove a message from the queue */
        inline void remove_message(boost::shared_ptr < M >& p);

        /** \brief Returns the number of messages in the queue */
        inline size_t number_of_messages();
 
        /** \brief Set the handler for a type */
        void add_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Clears the handlers for a message type */
        void clear_handlers(const typename M::type_identifier_t);

        /** \brief Remove a specific handlers for a message type */
        void remove_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Gets the associated logger object */
        utility::logger* get_logger();

        /** \brief Gets the associated logger object */
        static utility::logger* get_standard_error_logger();

        /** \brief Destructor */
        ~basic_messenger();
    };

    /**
     * @param[in] l a logger object used to write logging messages to
     *
     * \pre l != 0
     **/
    template < class M >
    inline basic_messenger< M >::basic_messenger(utility::logger* l) : message_open(false), partially_matched(0), logger(l) {
    }
  }
}

#endif
