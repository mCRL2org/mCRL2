#ifndef ATERM_ALLOCATOR_H
#define ATERM_ALLOCATOR_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#ifdef ATERM_DEBUG_ALLOCATOR
#include <iostream>
#endif // ATERM_DEBUG_ALLOCATOR
#include "atermpp/atermpp.h"

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

   // general case; works only for T = ATerm, ATermList, ...
   template <class T>
   void on_allocate(T* p, std::size_t num)
   {
     assert(num >= 1);
#ifndef NDEBUG // circumvent ATerm Library safety check in debug mode
     std::fill(p, p + num, static_cast<ATerm>(0));
#endif
     ATprotectArray(p, num);
   }

   template <class T>
   void on_deallocate(T* p, std::size_t num)
   {
     assert(num >= 1);
     ATunprotectArray(p);
   }

   template <class T>
   void on_construct(T* p)
   {
   }

   template <class T>
   void on_destroy(T* p)
   {
   }

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

       // rebind aterm_allocator to type U
       template <class U>
       struct rebind {
           typedef aterm_allocator<U> other;
       };

       // return address of values
       pointer address (reference value) const {
           return &value;
       }
       const_pointer address (const_reference value) const {
           return &value;
       }

       /* constructors and destructor
        * - nothing to do because the aterm_allocator has no state
        */
       aterm_allocator() throw() {
       }

       aterm_allocator(const aterm_allocator&) throw() {
       }

       template <class U>
         aterm_allocator (const aterm_allocator<U>&) throw() {
       }

       ~aterm_allocator() throw() {
       }

       // return maximum number of elements that can be allocated
       size_type max_size () const throw() {
           return std::numeric_limits<std::size_t>::max() / sizeof(T);
       }

       // allocate but don't initialize num elements of type T
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

       // initialize elements of allocated storage p with value value
       void construct (pointer p, const T& value) {
#ifdef ATERM_DEBUG_ALLOCATOR
static int n = 0;
std::cout << "aterm_allocator.construct() " << ++n << " elements constructed" << std::endl;
#endif // ATERM_DEBUG_ALLOCATOR
           on_construct(p);
           // initialize memory with placement new
           new((void*)p)T(value);
       }

       // destroy elements of initialized storage p
       void destroy (pointer p) {
#ifdef ATERM_DEBUG_ALLOCATOR
static int n = 0;
std::cout << "aterm_allocator.destroy() " << ++n << " elements destroyed" << std::endl;
#endif // ATERM_DEBUG_ALLOCATOR
           on_destroy(p);
           // destroy objects by calling their destructor
           p->~T();
       }

       // deallocate storage p of deleted elements
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

   // return that all specializations of this aterm_allocator are interchangeable
   template <class T1, class T2>
   bool operator== (const aterm_allocator<T1>&,
                    const aterm_allocator<T2>&) throw() {
       return true;
   }
   template <class T1, class T2>
   bool operator!= (const aterm_allocator<T1>&,
                    const aterm_allocator<T2>&) throw() {
       return false;
   }

} // namespace atermpp

#endif // ATERM_ALLOCATOR_H
