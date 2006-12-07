#ifndef ATERM_DEQUE_H
#define ATERM_DEQUE_H

#include <memory>
#include <deque>
#include "atermpp/aterm_traits.h"
#include "atermextpp.h"          // IProtectedATerm

namespace atermpp {

template <class T, class Allocator = std::allocator<T> >
class deque: public std::deque<T, Allocator>, IProtectedATerm
{
  public:
    deque()
    {
      ATprotectProtectedATerm(this);
    }

    explicit deque(const Allocator& a)
      : std::deque<T, Allocator>(a)
    {
      ATprotectProtectedATerm(this);
    }

    explicit deque(typename std::deque<T, Allocator>::size_type count)
      : std::deque<T, Allocator>(count)
    {
      ATprotectProtectedATerm(this);
    }
    
    deque(typename std::deque<T, Allocator>::size_type count, const T& val)
      : std::deque<T, Allocator>(count, val)
    {
      ATprotectProtectedATerm(this);
    }

    deque(typename std::deque<T, Allocator>::size_type count, const T& val, const Allocator& a)
      : std::deque<T, Allocator>(count, val, a)
    {
      ATprotectProtectedATerm(this);
    }

    deque(const deque& right)
      : std::deque<T, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
        deque(InIt first, InIt last)
      : std::deque<T, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    template<class InIt>
        deque(InIt first, InIt last, const Allocator& a)
      : std::deque<T, Allocator>(first, last, a)
    {
      ATprotectProtectedATerm(this);
    }

    ~deque()
    {
      ATunprotectProtectedATerm(this);
    }

    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::deque.ATprotectTerms() : protecting " << deque<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::deque<T, Allocator>::iterator i = std::deque<T, Allocator>::begin(); i != std::deque<T, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

} // namespace atermpp

#endif // ATERM_DEQUE_H
