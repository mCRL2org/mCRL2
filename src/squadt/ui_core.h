#ifndef UI_CORE_H
#define UI_CORE_H

namespace squadt {

  class settings_manager;
  class ToolManager;
  class Logger;

  /* Convenience type */
  typedef boost::shared_ptr < settings_manager > settings_manager_ptr;

  /* Global Settings Manager component */
  extern settings_manager_ptr global_settings_manager;

  /* Global Tool Manager component */
  extern ToolManager          tool_manager;

  /* Global log component */
  extern Logger*              logger;
}

#endif

