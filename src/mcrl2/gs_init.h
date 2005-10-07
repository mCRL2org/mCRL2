#ifndef GS_INIT_H
#define GS_INIT_H

/** @file
  * If this file is included, the gsEnableConstructorFunctions initialization
  * function will be invoked automatically. It is possible to override this behavior
  * by defining GS_USER_INITIALIZATION.
  */

#include <iostream>
#include "gsfunc.h"

namespace { // unnamed namespace

  template <class T> // note, T is only a dummy
  class gs_initializer {
      static unsigned int count;
  public:
      gs_initializer();
      ~gs_initializer();
  };
  
  static gs_initializer<int> gs_init_var;
  
  template <class T>
  unsigned int gs_initializer<T>::count = 0;
  
  template <class T>
  gs_initializer<T>::gs_initializer() { // default constructor
    if (0 == count++) {
      // perform initialization
#ifndef GS_USER_INITIALIZATION
      gsEnableConstructorFunctions();
#endif // GS_USER_INITIALIZATION
    }
  }
  
  template <class T>
  gs_initializer<T>::~gs_initializer() { // destructor
    if (0 == --count) {
      // perform housekeeping
    }
  }

} // unnamed namespace

#endif // GS_INIT_H
