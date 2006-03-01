#ifndef LISTENER_H
#define LISTENER_H

#include <boost/shared_ptr.hpp>

#include <transport/transporter.h>

namespace transport {
  namespace transceiver {
    class transceiver;
  }

  namespace listener {

    /* Base class for listeners */
    class basic_listener {
      protected:
        transport::transporter& owner;

        inline void associate(boost::shared_ptr < transceiver::basic_transceiver >);

      public:

        inline basic_listener(transport::transporter& m);

        virtual void shutdown() = 0;

        virtual inline ~basic_listener();
    };

    inline void basic_listener::associate(boost::shared_ptr < transceiver::basic_transceiver > t) {
      owner.connections.push_back(t);
    }

    inline basic_listener::basic_listener(transporter& m) : owner(m) {
    }

    inline basic_listener::~basic_listener() {
    }

    class exception : transport::exception {
      private:
        basic_listener* l;

      public:
        exception(basic_listener*);
 
        /** Returns the type of the exception */
        exception_type type();
 
        /** Returns the type of the exception */
        basic_listener* originator();
    };
  }
}

#endif

