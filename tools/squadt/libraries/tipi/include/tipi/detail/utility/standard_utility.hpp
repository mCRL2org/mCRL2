#ifndef __STANDARD_UTILITY_HPP__
#define __STANDARD_UTILITY_HPP__

/**
 * \brief Collection of utility functionality for use with STL/Boost functionality
 **/
namespace utility {
  /**
   * Trivial deleter for use with boost::shared_ptr 
   **/
  template < typename T >
  class trivial_deleter {

    public:

      inline void operator() (T*) {
      }
  };
}

#endif
