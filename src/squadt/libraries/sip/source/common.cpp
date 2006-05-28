#include <sip/detail/configuration.h>

namespace sip {

  /** The optional constraint, option is either not present or only present once */
  const configuration::option_constraint configuration::constrain_optional = {0,1};

  /** The required constraint, option (with possible arguments) must be present */
  const configuration::option_constraint configuration::constrain_required = {1,1};

  namespace datatype {

    std::string boolean::true_string  = "true";
    std::string boolean::false_string = "false";
  }
}

