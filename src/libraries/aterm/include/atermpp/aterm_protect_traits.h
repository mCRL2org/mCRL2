#ifndef ATERM_PROTECT_TRAITS_H
#define ATERM_PROTECT_TRAITS_H

#ifdef ATERM_DEBUG_PROTECTION
#include <iostream>
#endif // ATERM_DEBUG_PROTECTION

#include "aterm2.h"

namespace atermpp {

   template <class T>
   class aterm_protect_traits
   {
     public:
       static void protect(T t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<default>::protect()" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
       }

       static void unprotect(T t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<default>::unprotect()" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
       }

       static void mark(T t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<default>::mark()" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
       }
   };

   template <>
   class aterm_protect_traits<ATerm>
   {
     public:
       static void protect(ATerm t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATerm>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATprotect(&t);
       }

       static void unprotect(ATerm t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATerm>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATunprotect(&t);
       }

       static void mark(ATerm t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATerm>::mark()" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATmarkTerm(&t);
       }
   };

   template <>
   class aterm_protect_traits<ATermList>
   {
     public:
       static void protect(ATermList t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATermList>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATprotect(reinterpret_cast<ATerm*>(&t));
       }

       static void unprotect(ATermList t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATermList>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATunprotect(reinterpret_cast<ATerm*>(&t));
       }

       static void mark(ATermList t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATermList>::mark()" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATmarkTerm(reinterpret_cast<ATerm*>(&t));
       }
   };

   template <>
   class aterm_protect_traits<ATermAppl>
   {
     public:
       static void protect(ATermAppl t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATermAppl>::protect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATprotect(reinterpret_cast<ATerm*>(&t));
       }

       static void unprotect(ATermAppl t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATermAppl>::unprotect() " << t << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATunprotect(reinterpret_cast<ATerm*>(&t));
       }

       static void mark(ATermAppl t)
       {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "aterm_protect_traits<ATermAppl>::mark()" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
         ATmarkTerm(reinterpret_cast<ATerm*>(&t));
       }
   };

} // namespace atermpp

#endif // ATERM_PROTECT_TRAITS_H
