#include <sip/detail/object.h>

namespace sip {
  const char* object::type_strings[] = {
    "input"      /// \brief object is used for input
   ,"output"     /// \brief object is used for output
   ,0
  };
}

