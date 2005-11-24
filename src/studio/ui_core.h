#ifndef UI_CORE_H
#define UI_CORE_H

class SettingsManager;
class ToolManager;
class ToolExecutor;
class Logger;

/* Global Settings Manager component */
extern SettingsManager  settings_manager;

/* Global Tool Manager component */
extern ToolManager      tool_manager;

/* Global Tool Executor component (TODO run a single instance per machine) */
extern ToolExecutor     tool_executor;

/* Global log component */
extern Logger           logger;

#endif

