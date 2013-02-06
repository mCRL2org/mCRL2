// Author(s): Howard Hinnant, see http://home.roadrunner.com/~hinnant/stack_alloc.html
//
/// \file mcrl2/utilities/short_alloc.h

#ifndef MCRL2_UTILITIES_STACK_ALLOC_H
#define MCRL2_UTILITIES_STACK_ALLOC_H

#define HAS_TYPE_TRAITS 0

#if HAS_TYPE_TRAITS
#include <type_traits>
#else
#include <tr1/type_traits>
#endif

#include <cstddef>

namespace mcrl2
{

namespace utilities
{

template <class T, std::size_t N> class stack_alloc;

template <std::size_t N>
class stack_alloc<void, N>
{
public:
    typedef const void*       const_pointer;
    typedef void              value_type;
};

template <class T, std::size_t N>
class stack_alloc
{
public:
    typedef std::size_t       size_type;
    typedef T                 value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;

private:
#if HAS_TYPE_TRAITS
    typename std::aligned_storage<sizeof(T) * N, 16>::type buf_;
#else
    typename std::tr1::aligned_storage<sizeof(T) * N, 16>::type buf_;
#endif
    pointer ptr_;

public:
    stack_alloc() throw() : ptr_((pointer)&buf_) {}
    stack_alloc(const stack_alloc&) throw() : ptr_((pointer)&buf_) {}
    template <class U> stack_alloc(const stack_alloc<U, N>&) throw()
         : ptr_((pointer)&buf_) {}

    template <class U> struct rebind {typedef stack_alloc<U, N> other;};
private:
    stack_alloc& operator=(const stack_alloc&);
public:
    pointer allocate(size_type n, typename stack_alloc<void, N>::const_pointer = 0)
    {
        if ((pointer)&buf_ + N - ptr_ >= n)
        {
            pointer r = ptr_;
            ptr_ += n;
            return r;
        }
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }
    void deallocate(pointer p, size_type n)
    {
        if ((pointer)&buf_ <= p && p < (pointer)&buf_ + N)
        {
            if (p + n == ptr_)
                ptr_ = p;
        }
        else
            ::operator delete(p);
    }
    size_type max_size() const throw() {return size_type(~0) / sizeof(T);}

    void destroy(T* p) {p->~T();}

        void
        construct(pointer p)
        {
            ::new((void*)p) T();
        }

    template <class A0>
        void
        construct(pointer p, const A0& a0)
        {
            ::new((void*)p) T(a0);
        }

    bool operator==(stack_alloc& a) const {return &buf_ == &a.buf_;}
    bool operator!=(stack_alloc& a) const {return &buf_ != &a.buf_;}
};

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_STACK_ALLOC_H

