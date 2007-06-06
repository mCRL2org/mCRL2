#include "utilities/generic_visitor.h"

namespace utility {
  template < typename S, typename R >
  typename abstract_visitor< R >::visitable_type_tree  visitor< S, R >::visitable_types;
}
