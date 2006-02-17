#ifndef SIP_COMMON_H
#define SIP_COMMON_H

#include <list>

#include <sip/detail/report.h>
#include <sip/detail/controller_capabilities.h>
#include <sip/detail/display_layout.h>

/* Interface classes for both the tool and the controller side of the Squadt Interaction Protocol */
namespace sip {

  /** Category name that describes a tools function */
  typedef std::string tool_category;

  /** Storage format for tool input/output configuration specification */
  typedef std::string storage_format;
}

#endif

