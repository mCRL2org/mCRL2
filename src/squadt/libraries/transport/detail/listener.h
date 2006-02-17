#ifndef LISTENER_H
#define LISTENER_H

#include <boost/shared_ptr.hpp>

#include <transport/transporter.h>

namespace transport {
  namespace transceiver {
    class Transceiver;
  }

  namespace listener {
    using transport::transceiver::Transceiver;

    /* Base class for listeners */
    class Listener {
      protected:
        transport::transporter& owner;

        inline void associate(boost::shared_ptr < Transceiver >);

      public:

        inline Listener(transport::transporter& m);

        virtual void shutdown() = 0;

        virtual inline ~Listener();
    };

    inline void Listener::associate(boost::shared_ptr < Transceiver > t) {
      owner.connections.push_back(t);
    }

    inline Listener::Listener(transporter& m) : owner(m) {
    }

    inline Listener::~Listener() {
    }

    class exception : transport::exception {
      private:
        Listener* l;

      public:
        exception(Listener*);
 
        /** Returns the type of the exception */
        exception_type type();
 
        /** Returns the type of the exception */
        Listener* listener();
    };
  }
}

#endif

