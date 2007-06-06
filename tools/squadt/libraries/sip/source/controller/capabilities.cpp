#include <ostream>
#include <sstream>
#include <utility>

#include <boost/bind.hpp>

#include <sip/controller/capabilities.h>
#include <sip/tool/capabilities.h>
#include <sip/visitors.h>

namespace sip {
  namespace controller {
     /**
     * \brief Operator for writing to stream
     *
     * \param[in,out] s stream to write to
     * \param[in] c the capabilities object to write out
     **/
    inline std::ostream& operator << (std::ostream& s, controller::capabilities const& c) {
      visitors::store(c, s);
 
      return (s);
    }
  }
}
