#include "exception.h"

namespace squadt {

  /**
   * @param[in] t the type identifier of the exception
   * @param[in] a an argument for the error
   **/
  template < typename T >
  inline exception::exception(const exception_type t, const T a) : type(t), argument(a) {
  }

  inline const char* exception::what() const throw () {
    return (boost::str(boost::format(descriptions[type]) % argument).c_str());
  }

  inline exception::~exception() throw () {
  }
}

