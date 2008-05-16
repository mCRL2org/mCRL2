// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_allocator.h
/// \brief An allocator class for ATerms. Unfortunately it is not usable,
/// due to incompatibilities between compilers.

#ifndef MCRL2_ATERMPP_ATERM_ALLOCATOR_H
#define MCRL2_ATERMPP_ATERM_ALLOCATOR_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#ifdef ATERM_DEBUG_ALLOCATOR
#include <iostream>
#endif // ATERM_DEBUG_ALLOCATOR
#include "mcrl2/atermpp/atermpp.h"

/* The following code example is taken from the book
 * "The C++ Standard Library - A Tutorial and Reference"
 * by Nicolai M. Josuttis, Addison-Wesley, 1999
 *
 * (C) Copyright Nicolai M. Josuttis 1999.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 * Modified by Wieger Wesselink.
 */

namespace atermpp {

   /// Is called when num elements are allocated at memory location p.
   ///
   // general case; works only for T = ATerm, ATermList, ...
   template <class T>
   void on_allocate(T* p, std::size_t num)
   {
     assert(num >= 1);
     std::fill(p, p + num, static_cast<ATerm>(0));
     ATprotectArray(p, num);
   }

   /// Is called when the memory at location p is freed.
   ///
   template <class T>
   void on_deallocate(T* p, std::size_t num)
   {
     assert(num >= 1);
     ATunprotectArray(p);
   }

   /// Is called when an element is constructed at memory location p.
   ///
   template <class T>
   void on_construct(T* p)
   {
   }

   /// Is called when the element at location p is destroyed.
   ///
   template <class T>
   void on_destroy(T* p)
   {
   }

   /// Allocator for ATerms. To be used with standard library containers.
   ///
   template <class T>
   class aterm_allocator {
     public:
       // type definitions
       typedef std::size_t    size_type;
       typedef std::ptrdiff_t difference_type;
       typedef T*             pointer;
       typedef const T*       const_pointer;
       typedef T&             reference;
       typedef const T&       const_reference;
       typedef T              value_type;

       /// Rebind aterm_allocator to type U.
       ///
       template <class U>
       struct rebind {
           typedef aterm_allocator<U> other;
       };

       /// Return address of value.
       ///
       pointer address (reference value) const {
           return &value;
       }

       /// Return address of value.
       ///
       const_pointer address (const_reference value) const {
           return &value;
       }

       /// Default constructor.
       ///
       aterm_allocator() throw() {
       }

       /// Copy constructor.
       ///
       aterm_allocator(const aterm_allocator&) throw() {
       }

       /// Copy constructor.
       ///
       template <class U>
         aterm_allocator (const aterm_allocator<U>&) throw() {
       }

       /// Default destructor.
       ///
       ~aterm_allocator() throw() {
       }

       /// Returns the maximum number of elements that can be allocated.
       ///
       size_type max_size () const throw() {
           return std::numeric_limits<std::size_t>::max() / sizeof(T);
       }

       /// Allocates but doesn't initialize num elements of type T.
       ///
       pointer allocate (size_type num)
       {
#ifdef ATERM_DEBUG_ALLOCATOR
static int n = 0;
n += num;
std::cout << "aterm_allocator.allocate(" << num << ") " << n << " elements allocated\n";
#endif // ATERM_DEBUG_ALLOCATOR

         // allocate memory with global new
         pointer p = (pointer)(::operator new(num*sizeof(T)));
         on_allocate(p, num);
         return p;
       }

       /// Initialize elements of allocated storage p with value value.
       ///
       void construct (pointer p, const T& value) {
#ifdef ATERM_DEBUG_ALLOCATOR
static int n = 0;
std::cout << "aterm_allocator.construct() " << ++n << " elements constructed" << std::endl;
#endif // ATERM_DEBUG_ALLOCATOR
           on_construct(p);
           // initialize memory with placement new
           new((void*)p)T(value);
       }

       /// Destroy elements of initialized storage p.
       ///
       void destroy (pointer p) {
#ifdef ATERM_DEBUG_ALLOCATOR
static int n = 0;
std::cout << "aterm_allocator.destroy() " << ++n << " elements destroyed" << std::endl;
#endif // ATERM_DEBUG_ALLOCATOR
           on_destroy(p);
           // destroy objects by calling their destructor
           p->~T();
       }

       /// Deallocate storage p of deleted elements.
       ///
       void deallocate (pointer p, size_type num) {
#ifdef ATERM_DEBUG_ALLOCATOR
static int n = 0;
n += num;
std::cout << "aterm_allocator.deallocate(" << num << ") " << n << " elements deallocated\n";
#endif // ATERM_DEBUG_ALLOCATOR
           on_deallocate(p, num);
           // deallocate memory with global delete
           ::operator delete((void*)p);
       }
   };

   /// Return that all specializations of this aterm_allocator are interchangeable.
   ///
   template <class T1, class T2>
   bool operator== (const aterm_allocator<T1>&,
                    const aterm_allocator<T2>&) throw() {
       return true;
   }

   /// Return that not all specializations of this aterm_allocator are interchangeable.
   ///
   template <class T1, class T2>
   bool operator!= (const aterm_allocator<T1>&,
                    const aterm_allocator<T2>&) throw() {
       return false;
   }

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_ALLOCATOR_H
