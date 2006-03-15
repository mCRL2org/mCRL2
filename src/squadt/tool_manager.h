#ifndef TOOL_MANAGER_H_
#define TOOL_MANAGER_H_

#include <iosfwd>
#include <ostream>
#include <string>
#include <list>

#include "tool.h"
#include "executor.h"

namespace squadt {

  using namespace tool;

  class executor;
  class Specification;

  class ToolManager {
    friend class Specification;
 
    private:
 
      static executor tool_executor;
 
      std::list < Tool* > tools;            /* List of known tools */
 
      /* Write configuration to stream */
      bool Write(std::ostream& = std::cout) const;
 
      /* Read configuration from file */
      bool Read(const std::string&);
 
      void Execute(unsigned int tool_identifier, std::string arguments, Specification* p = 0) const;

    public:
 
      /* Constructor; with the directory it should use as data directory */
      ToolManager();
 
      ~ToolManager();
 
      bool QueryTools() const;
 
      /** Load tool configuration from the default location */
      bool Load() throw ();
 
      /** Store tool configuration to the default location */
      bool Store() const;
 
      /** Print tool configuration information to stream */
      void Print(std::ostream& stream = std::cerr) const;
 
      /** Add a new tool to the catalog */
      bool AddTool(std::string name);
 
      /** Remove a tool from the catalog */
      bool RemoveTool(std::string name);
 
      /** Get the list of known tools */
      const std::list < Tool* >& GetTools() const;
 
      /** Get tool number i (where i < tools.size() ) */
      const Tool* GetTool(unsigned int index) const;
 
      /** Get the number of known tools */
      const unsigned int GetNumberOfTools() const;
 
      /** Have the tool executor terminate all running tools */
      void TerminateAll();
  };

  inline const std::list < Tool* >& ToolManager::GetTools() const {
    return (tools);
  }

  inline const Tool* ToolManager::GetTool(unsigned int index) const {
    std::list < Tool* >::const_iterator i = tools.begin();

    while (0 < index--) {
      ++i;
    }

    return (*i);
  }

  inline const unsigned int ToolManager::GetNumberOfTools() const {
    return (tools.size());
  }
}

#endif
