#ifndef PARAMETER_H__
#define PARAMETER_H__

#include <utility/generic_visitor.h>

namespace sip {

  class parameter : public utility::visitable {

    public:

      virtual ~parameter() {
      }
  };
}

#endif
