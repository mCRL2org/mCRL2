#ifndef TOOL_MANAGER_H_
#define TOOL_MANAGER_H_

#include <iostream>
#include <ostream>
#include <string>
#include <list>

#include "tool.h"

class ToolExecutor;
class Specification;

/*
 * Updating the tool catalog proceeds atomically:
 *
 * A lock is obtained
 *
 *  1. tool_catalog.xml.out is written.
 *  2. tool_catalog.xml is removed.
 *  3. tool_catalog.xml.out is moved to tool_catalog.xml
 *
 * The lock is released
 */

class ToolManager {
  friend class Specification;

  private:

    static ToolExecutor tool_executor;

    std::list < Tool* > tools;            /* List of known tools */

    /* Write configuration to stream */
    bool Write(std::ostream& = std::cout) const;

    /* Read configuration from file */
    bool Read(std::string);

    bool ToolManager::Execute(unsigned int tool_identifier, std::string arguments, Specification* plan);

  public:

    /* Constructor; with the directory it should use as data directory */
    ToolManager();

    ~ToolManager();

    bool QueryTools() const;

    /* Load tool configuration from the default location */
    bool Load() throw (int);

    /* Store tool configuration to the default location */
    bool Store() const;

    /* Print tool configuration information to stream */
    void Print(std::ostream& stream = std::cerr) const;

    /* Add a new tool to the catalog */
    bool AddTool(std::string name);

    /* Remove a tool from the catalog */
    bool RemoveTool(std::string name);

    /* Get the list of known tools */
    inline const std::list < Tool* >& GetTools() const {
      return (tools);
    }

    /* Get tool number i (where i < tools.size() ) */
    inline const Tool* GetTool(unsigned int index) const {
      std::list < Tool* >::const_iterator i = tools.begin();

      while (0 < index--) {
        ++i;
      }

      return (*i);
    }

    /* Get the number of known tools */
    inline const unsigned int GetNumberOfTools() const {
      return (tools.size());
    }

    /* Have the tool executor terminate all running tools */
    void TerminateAll();
};

#endif
