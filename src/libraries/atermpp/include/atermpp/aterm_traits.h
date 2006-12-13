#ifndef ATERM_TRAITS_H
#define ATERM_TRAITS_H

#include "aterm2.h"

namespace atermpp {

template <typename T>
struct aterm_traits
{
  typedef void* aterm_type; 
  static void protect(T* t)       {}
  static void unprotect(T* t)     {}
  static void mark(T t)           {}
  static T term(const T& t)       { return t; }
  static const T* ptr(const T& t) { return &t; }
  static T* ptr(T& t)             { return &t; }
};

template <>
struct aterm_traits<ATerm>
{
  typedef ATerm aterm_type;
  static void protect(ATerm* t)       { ATprotect(t); }
  static void unprotect(ATerm* t)     { ATunprotect(t); }
  static void mark(ATerm t)           { ATmarkTerm(t); }
  static ATerm term(ATerm t)          { return t; }
  static ATerm* ptr(ATerm& t)         { return &t; }
};

template <>
struct aterm_traits<ATermList>
{
  typedef ATermList aterm_type;
  static void protect(ATermList* t)   { aterm_traits<ATerm>::protect(reinterpret_cast<ATerm*>(t)); }
  static void unprotect(ATermList* t) { aterm_traits<ATerm>::unprotect(reinterpret_cast<ATerm*>(t)); }
  static void mark(ATermList t)       { aterm_traits<ATerm>::mark(reinterpret_cast<ATerm>(t)); }
  static ATerm term(ATermList t)      { return reinterpret_cast<ATerm>(t); }
  static ATerm* ptr(ATermList& t)     { return reinterpret_cast<ATerm*>(&t); }
};

template <>
struct aterm_traits<ATermAppl>
{
  typedef ATermAppl aterm_type;
  static void protect(ATermAppl* t)   { aterm_traits<ATerm>::protect(reinterpret_cast<ATerm*>(t)); }
  static void unprotect(ATermAppl* t) { aterm_traits<ATerm>::unprotect(reinterpret_cast<ATerm*>(t)); }
  static void mark(ATermAppl t)       { aterm_traits<ATerm>::mark(reinterpret_cast<ATerm>(t)); }
  static ATerm term(ATermAppl t)      { return reinterpret_cast<ATerm>(t); }
  static ATerm* ptr(ATermAppl& t)     { return reinterpret_cast<ATerm*>(&t); }
};                                    
                                      
template <>                           
struct aterm_traits<ATermBlob>        
{                                     
  typedef ATermBlob aterm_type;       
  static void protect(ATermBlob* t)   { aterm_traits<ATerm>::protect(reinterpret_cast<ATerm*>(t)); }
  static void unprotect(ATermBlob* t) { aterm_traits<ATerm>::unprotect(reinterpret_cast<ATerm*>(t)); }
  static void mark(ATermBlob t)       { aterm_traits<ATerm>::mark(reinterpret_cast<ATerm>(t)); }
  static ATerm term(ATermBlob t)      { return reinterpret_cast<ATerm>(t); }
  static ATerm* ptr(ATermBlob& t)     { return reinterpret_cast<ATerm*>(&t); }
};                                    
                                      
template <>                           
struct aterm_traits<ATermReal>        
{                                     
  typedef ATermReal aterm_type;       
  static void protect(ATermReal* t)   { aterm_traits<ATerm>::protect(reinterpret_cast<ATerm*>(t)); }
  static void unprotect(ATermReal* t) { aterm_traits<ATerm>::unprotect(reinterpret_cast<ATerm*>(t)); }
  static void mark(ATermReal t)       { aterm_traits<ATerm>::mark(reinterpret_cast<ATerm>(t)); }
  static ATerm term(ATermReal t)      { return reinterpret_cast<ATerm>(t); }
  static ATerm* ptr(ATermReal& t)     { return reinterpret_cast<ATerm*>(&t); }
};                                    
                                      
template <>                           
struct aterm_traits<ATermInt>         
{                                     
  typedef ATermInt aterm_type;        
  static void protect(ATermInt* t)    { aterm_traits<ATerm>::protect(reinterpret_cast<ATerm*>(t)); }
  static void unprotect(ATermInt* t)  { aterm_traits<ATerm>::unprotect(reinterpret_cast<ATerm*>(t)); }
  static void mark(ATermInt t)        { aterm_traits<ATerm>::mark(reinterpret_cast<ATerm>(t)); }
  static ATerm term(ATermInt t)       { return reinterpret_cast<ATerm>(t); }
  static ATerm* ptr(ATermInt& t)      { return reinterpret_cast<ATerm*>(&t); }
};                                    
                                      
} // namespace atermpp

#endif // ATERM_TRAITS_H
