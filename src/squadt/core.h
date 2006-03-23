#ifndef UI_CORE_H
#define UI_CORE_H

#include <boost/shared_ptr.hpp>

namespace squadt {
  class settings_manager;
  class tool_manager;

  /** \brief Global Settings Manager component */
  extern boost::shared_ptr < settings_manager > global_settings_manager;

  /** \brief Global Tool Manager component */
  extern boost::shared_ptr < tool_manager >     global_tool_manager;
}

#endif

