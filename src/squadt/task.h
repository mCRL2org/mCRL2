#ifndef BASIC_PROCESSOR_H
#define BASIC_PROCESSOR_H

#include <sip/controller.h>

#include "process_listener.h"

namespace squadt {

  class tool_manager;
 
  namespace execution {

    /**
     * \brief Base class for tasks that provide tool execution via a tool manager
     **/
    class task : public sip::controller::communicator, public execution::process_listener {
      friend class squadt::tool_manager;
 
      public:
 
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < task > ptr;
    };
  }
}

#endif
