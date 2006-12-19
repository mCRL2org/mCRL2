#ifndef ATERM_SET_H
#define ATERM_SET_H

#include <functional>
#include <memory>
#include <set>
#include "atermpp/aterm_traits.h"
#include "protaterm.h"          // IProtectedATerm

namespace atermpp {

template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
class set: public std::set<T, Compare, Allocator>, IProtectedATerm
{
  public:
    set()
    {
      ATprotectProtectedATerm(this);
    }

    explicit set(const Compare& comp)
      : std::set<T, Compare, Allocator>(comp)
    {
      ATprotectProtectedATerm(this);
    }

    set(const Compare& comp, const Allocator& a)
      : std::set<T, Compare, Allocator>(comp, a)
    {
      ATprotectProtectedATerm(this);
    }

    set(const set& right)
      : std::set<T, Compare, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
    set(InIt first, InIt last)
      : std::set<T, Compare, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
    set(InIt first, InIt last, const Compare& comp)
      : std::set<T, Compare, Allocator>(first, last, comp)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
    set(InIt first, InIt last, const Compare& comp, const Allocator& a)
      : std::set<T, Compare, Allocator>(first, last, comp, a)
    {
      ATprotectProtectedATerm(this);
    }
   
    ~set()
    {
      ATunprotectProtectedATerm(this);
    }

    atermpp::set<T, Compare, Allocator>& operator=(const std::set<T, Compare, Allocator>& other)
    {
      std::set<T, Compare, Allocator>::operator=(other);
      return *this;
    }

    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::set.ATprotectTerms() : protecting " << set<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::set<T, Compare, Allocator>::iterator i = std::set<T, Compare, Allocator>::begin(); i != std::set<T, Compare, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
class multiset: public std::multiset<T, Compare, Allocator>, IProtectedATerm
{
  public:
    multiset()
    {
      ATprotectProtectedATerm(this);
    }

    explicit multiset(const Compare& comp)
      : std::multiset<T, Compare, Allocator>(comp)
    {
      ATprotectProtectedATerm(this);
    }

    multiset(const Compare& comp, const Allocator& a)
      : std::multiset<T, Compare, Allocator>(comp, a)
    {
      ATprotectProtectedATerm(this);
    }

    multiset(const multiset& right)
      : std::multiset<T, Compare, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
    multiset(InIt first, InIt last)
      : std::multiset<T, Compare, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
    multiset(InIt first, InIt last, const Compare& comp)
      : std::multiset<T, Compare, Allocator>(first, last, comp)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
    multiset(InIt first, InIt last, const Compare& comp, const Allocator& a)
      : std::multiset<T, Compare, Allocator>(first, last, comp, a)
    {
      ATprotectProtectedATerm(this);
    }
   
    ~multiset()
    {
      ATunprotectProtectedATerm(this);
    }

    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::multiset.ATprotectTerms() : protecting " << multiset<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::multiset<T, Compare, Allocator>::iterator i = std::multiset<T, Compare, Allocator>::begin(); i != std::multiset<T, Compare, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

} // namespace atermpp

#endif // ATERM_SET_H
