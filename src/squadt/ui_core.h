#ifndef UI_CORE_H
#define UI_CORE_H

namespace squadt {

  class SettingsManager;
  class ToolManager;
  class Logger;

  /* Global Settings Manager component */
  extern SettingsManager  settings_manager;

  /* Global Tool Manager component */
  extern ToolManager      tool_manager;

  /* Global log component */
  extern Logger*          logger;
}

#endif

