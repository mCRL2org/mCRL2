#ifndef PARAMETER_H__
#define PARAMETER_H__

#include <sip/utility/generic_visitor.h>

namespace sip {

  /** \brief Base class for elements of a sip::configuration */
  class parameter : public utility::visitable {

    public:

      /** \brief Destructor */
      virtual ~parameter() {
      }
  };
}

#endif
