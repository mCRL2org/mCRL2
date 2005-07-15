// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_init.h
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_INIT_H
#define ATERM_INIT_H

/** @file
  * If this file is included, the ATinit initialization function will be
  * invoked automatically. It is possible to override this behavior by
  * defining ATERM_USER_INITIALIZATION.
  */

#include <iostream>
#include "aterm2.h"

namespace { // unnamed namespace

  template <class T> // note, T is only a dummy
  class aterm_initializer {
      static unsigned int count;
  public:
      aterm_initializer();
      ~aterm_initializer();
  };
  
  static aterm_initializer<int> aterm_init_var;
  static ATerm bottom_of_stack;
  
  template <class T>
  unsigned int aterm_initializer<T>::count = 0;
  
  template <class T>
  aterm_initializer<T>::aterm_initializer() { // default constructor
    if (0 == count++) {
      // perform initialization
      // std::cerr << "ATerm library initialization called." << std::endl;
#ifndef ATERM_USER_INITIALIZATION
      ATinit(0, 0, &bottom_of_stack);
#endif // ATERM_USER_INITIALIZATION
    }
  }
  
  template <class T>
  aterm_initializer<T>::~aterm_initializer() { // destructor
    if (0 == --count) {
      // perform housekeeping
    }
  }

} // unnamed namespace

#endif // ATERM_INIT_H
