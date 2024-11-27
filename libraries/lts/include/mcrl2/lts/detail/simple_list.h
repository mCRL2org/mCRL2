// Author(s): David N. Jansen, Institute of Software, Chinese Academy of
// Sciences, Beijing, PR China
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/simple_list.h
///
/// \brief Simple list implementation (with pool allocator)
///
/// \details This file supports partition refinement algorithms by implementing
/// a simple list data structure, possibly with a pool allocator.
///
/// The main difference to std::list<T> is that the simple list does not store
/// the list size.  Also, it requires the stored elements to be trivially
/// destructible, so they can be stored in a pool allocator data structure.
///
/// The pool allocator uses larger blocks to allocate data items, mostly list
/// elements, but also other items can be stored, as long as they are trivially
/// destructible.
///
/// \author David N. Jansen, Institute of Software, Chinese Academy of
/// Sciences, Beijing, PR China

#ifndef SIMPLE_LIST_H
#define SIMPLE_LIST_H

#include <cstddef>     // for std::size_t
#include <new>         // for placement new
#include <type_traits> // for std::is_trivially_destructible<class>
#include <functional>  // for std::less
#include <iterator>    // for std::bidirectional_iterator_tag

// My provisional recommendation is to always use simple lists and pool
// allocators.  Using standard allocation and standard lists is 5-15% slower
// and uses perhaps 0.7% more memory.  Using standard allocation and simple
// lists is 10-20% slower and has no significant effect on memory use.  These
// numbers are based on a small set with not-so-large case studies for the JGKW
// bisimulation minimisation algorithm, none of which includes new bottom
// states.

#define USE_SIMPLE_LIST

#ifndef USE_SIMPLE_LIST
    #include <list>
#endif

#define USE_POOL_ALLOCATOR

namespace mcrl2
{
namespace lts
{
namespace detail
{





/* ************************************************************************* */
/*                                                                           */
/*                     M E M O R Y   M A N A G E M E N T                     */
/*                                                                           */
/* ************************************************************************* */





#ifdef USE_POOL_ALLOCATOR
    #define ONLY_IF_POOL_ALLOCATOR(...) __VA_ARGS__
    #ifndef USE_SIMPLE_LIST
        #error "Using the pool allocator also requires using the simple list"
    #endif

    /// \class pool
    /// \brief a pool allocator class
    /// \details This class allocates a large chunk of memory at once and hands
    /// out smaller parts.  It is supposed to be more efficient than calling
    /// new/delete, in particular because it assumes that T is trivially
    /// destructible, so it won't call destructors.  It allows to store
    /// elements of different sizes.
    ///
    /// Internally, it keeps a (single-linked) list of large chunks of size
    /// BLOCKSIZE. Each chunk contains a data area; for all chunks except the
    /// first one, this area is completely in use.
    ///
    /// There is a free list, a (single-linked) list of elements in the chunks
    /// that have been freed.  However, all elements in the free list have to
    /// have the same size as type T.
    template <class T, std::size_t BLOCKSIZE = 1000>
    class my_pool
    {                                                                           static_assert(std::is_trivially_destructible<T>::value);
      private:                                                                  static_assert(sizeof(void*) <= sizeof(T));
        class pool_block_t
        {
          public:
            char data[BLOCKSIZE - sizeof(pool_block_t*)];
            pool_block_t* next_block;

            pool_block_t(pool_block_t* const new_next_block)
              : next_block(new_next_block)
            {  }
        };                                                                      static_assert(sizeof(T) <= sizeof(pool_block_t::data));

        /// \brief first chunk in list of chunks
        /// \details All chunks except the first one are completely in use.
        pool_block_t* first_block;

        /// \brief start of part in the first chunk that is already in use
        void* begin_used_in_first_block;

        /// \brief first freed element
        void* first_free_T;

        static void*& deref_void(void* addr)
        {
            return *static_cast<void**>(addr);
        }
      public:
        /// \brief constructor
        my_pool()
          : first_block(new pool_block_t(nullptr)),
            begin_used_in_first_block(
                                &first_block->data[sizeof(first_block->data)]),
            first_free_T(nullptr)
        {  }


        /// \brief destructor
        ~my_pool()
        {
            pool_block_t* block(first_block);                                   assert(nullptr != block);
            do
            {
                pool_block_t* next_block = block->next_block;
                delete block;
                block = next_block;
            }
            while(nullptr != block);
        }


      private:
        /// \brief allocate and construct a new element of the same size as the
        /// free list
        template <class U, class... Args>
        U* construct_samesize(Args&&... args)
        {                                                                       static_assert(sizeof(T) == sizeof(U));
            void* new_el;                                                       assert(nullptr != first_block);
            if (first_block->data + sizeof(U) <= begin_used_in_first_block)
            {
                begin_used_in_first_block =
                    new_el = static_cast<char*>(begin_used_in_first_block) -
                                                                     sizeof(U);
            }
            else if (nullptr != first_free_T)
            {
                // free list is tested afterwards because I expect that there
                // won't be too many elements in the free list.
                new_el = first_free_T;
                first_free_T = deref_void(new_el);
            }
            else
            {
                first_block = new pool_block_t(first_block);
                begin_used_in_first_block =
                    new_el = &first_block->data[sizeof(first_block->data) -
                                                                    sizeof(U)];
            }
            return new(new_el) U(std::forward<Args>(args)...);
        }


        /// \brief allocate and construct a new element of a size that doesn't
        /// fit the free list
        template <class U, class... Args>
        U* construct_othersize(Args&&... args)
        {                                                                       static_assert(sizeof(U) != sizeof(T));
            void* new_el;                                                       assert(nullptr != first_block);
            if (first_block->data + sizeof(U) <= begin_used_in_first_block)
            {
                begin_used_in_first_block =
                    new_el = static_cast<char*>(begin_used_in_first_block) -
                                                                     sizeof(U);
            }
            else
            {
                if constexpr (sizeof(T) * 2 < sizeof(U))
                {
                    // There may be space for several T-elements,
                    // extend the free list accordingly
                    while (first_block->data + sizeof(T) <=
                                                     begin_used_in_first_block)
                    {
                        begin_used_in_first_block = static_cast<char*>
                                       (begin_used_in_first_block) - sizeof(T);
                        deref_void(begin_used_in_first_block) = first_free_T;
                        first_free_T = begin_used_in_first_block;
                    }
                }
                else if constexpr (sizeof(T) < sizeof(U))
                {
                    // There may be space for one T-element (but not more),
                    // extend the free list accordingly
                    if (first_block->data + sizeof(T) <=
                                                     begin_used_in_first_block)
                    {
                        begin_used_in_first_block = static_cast<char*>
                                       (begin_used_in_first_block) - sizeof(T);
                        deref_void(begin_used_in_first_block) = first_free_T;
                        first_free_T = begin_used_in_first_block;
                    }
                }                                                               assert(first_block->data + sizeof(T) > begin_used_in_first_block);
                first_block = new pool_block_t(first_block);
                begin_used_in_first_block =
                    new_el = &first_block->data[sizeof(first_block->data) -
                                                                    sizeof(U)];
            }
            return new(new_el) U(std::forward<Args>(args)...);
        }
      public:
        /// \brief allocate and construct a new element (of any type)
        template <class U, class... Args>
        U* construct(Args&&... args)
        {                                                                       static_assert(std::is_trivially_destructible<U>::value);
            if constexpr (sizeof(U) == sizeof(T))
            {
                return construct_samesize<U>(std::forward<Args>(args)...);
            }
            else
            {                                                                   static_assert(sizeof(U) <= sizeof(first_block->data));
                return construct_othersize<U>(std::forward<Args>(args)...);
            }
        }


        /// \brief destroy and delete some element
        /// \details destroy() is only allowed if the destructor of U is
        /// trivial.  This ensures that in my_pool::~my_pool() we do not have
        /// to test whether some element has been freed before we destroy it.
        /// Also, the size of U has to be the same size as the size of T, so
        /// each entry in the free list has the same size.
        template <class U>
        void destroy(U* const old_el)
        {                                                                       static_assert(sizeof(T) == sizeof(U));
            old_el->~U();                                                       static_assert(std::is_trivially_destructible<U>::value);
                                                                                #ifndef NDEBUG
                                                                                    // ensure that old_el points to an element in some block
                                                                                    static std::less<const void*> const total_order;
                                                                                    for (const pool_block_t* block(first_block);
                                                                                                assert(nullptr != block),
                                                                                                total_order(old_el, block->data) ||
                                                                                                total_order(&block->data[sizeof(block->data)], old_el + 1);
                                                                                                                                    block = block->next_block )
                                                                                    {  }
                                                                                #endif
            deref_void(old_el) = first_free_T;
            first_free_T = static_cast<void*>(old_el);
        }
    };
#else
    #define ONLY_IF_POOL_ALLOCATOR(...)
#endif // #define USE_POOL_ALLOCATOR

#ifdef USE_SIMPLE_LIST
    /// \class simple_list
    /// \brief a simple implementation of lists
    /// \details This class simplifies lists:  It assumes that list entries are
    /// trivially destructible, and it does not store the size of a list.
    /// Therefore, the destructor, erase() and splice() can be simplified.
    /// Also, the simple_list object itself is trivially destructible if the
    /// pool allocator is used; therefore, destroying a block_t object becomes
    /// trivial as well.
    template <class T>
    class simple_list
    {
      private:
        /// \brief empty entry, used for the sentinel
        class empty_entry
        {
          protected:
            /// \brief pointer to the next element in the list
            empty_entry* next;

            /// \brief pointer to the previous element in the list
            empty_entry* prev;

            empty_entry(empty_entry*const new_next, empty_entry*const new_prev)
              : next(new_next),
                prev(new_prev)
            {  }

            friend class simple_list;
        };

        /// \brief sentinel, i.e. element that provides pointers to the
        /// beginning and the end of the list
        empty_entry sentinel;

      public:
        /// \brief list entry
        /// \details If the list is to use the pool allocator, its designated
        /// type must be `simple_list::entry` so elements can be erased.
        class entry : public empty_entry
        {
          private:
            T data;

            friend class simple_list;
          public:
            template <class... Args>
            entry(empty_entry* const new_next, empty_entry* const new_prev,
                                                                Args&&... args)
              : empty_entry(new_next, new_prev),
                data(std::forward<Args>(args)...)
            {  }
        };

        /// \brief constant iterator class for simple_list
        class const_iterator
        {
          public:
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef T value_type;
            typedef std::ptrdiff_t difference_type;
            typedef T* pointer;
            typedef T& reference;
          protected:
            empty_entry* ptr;

            const_iterator(const empty_entry* const new_ptr)
              : ptr(const_cast<empty_entry*>(new_ptr))
            {  }

            friend class simple_list;
          public:
            const_iterator() = default;
            const_iterator(const const_iterator& other) = default;
            const_iterator& operator=(const const_iterator& other) = default;
            const_iterator& operator++()  {  ptr = ptr->next;  return *this;  }
            const_iterator& operator--()  {  ptr = ptr->prev;  return *this;  }
            const T& operator*() const
            {
                return  static_cast<const entry*>(ptr)->data;
            }
            const T* operator->() const
            {
                return &static_cast<const entry*>(ptr)->data;
            }
            bool operator==(const const_iterator& other) const
            {
                return ptr == other.ptr;
            }
            bool operator!=(const const_iterator& other) const
            {
                return !operator==(other);
            }
        };

        /// \brief iterator class for simple_list
        class iterator : public const_iterator
        {
          public:
            typedef typename const_iterator::iterator_category
                                                             iterator_category;
            typedef typename const_iterator::value_type value_type;
            typedef typename const_iterator::difference_type difference_type;
            typedef typename const_iterator::pointer pointer;
            typedef typename const_iterator::reference reference;
          protected:
            iterator(empty_entry*const new_ptr) : const_iterator(new_ptr)  {  }

            friend class simple_list;
          public:
            iterator() = default;
            iterator(const iterator& other) = default;
            iterator& operator=(const iterator& other) = default;
            iterator& operator++(){const_iterator::operator++(); return *this;}
            iterator& operator--(){const_iterator::operator--(); return *this;}
            T& operator*() const
            {
                return  static_cast<entry*>(const_iterator::ptr)->data;
            }
            T* operator->() const
            {
                return &static_cast<entry*>(const_iterator::ptr)->data;
            }
        };

        /// \brief class that stores either an iterator or a null value
        /// \details We cannot use C++14's ``null forward iterators'', as they
        /// are not guaranteed to compare unequal to valid iterators.  We also
        /// need to compare null iterators with non-null ones.
        class iterator_or_null : public iterator
        {
          public:
            typedef typename iterator::iterator_category iterator_category;
            typedef typename iterator::value_type value_type;
            typedef typename iterator::difference_type difference_type;
            typedef typename iterator::pointer pointer;
            typedef typename iterator::reference reference;
            iterator_or_null() : iterator()  {  }
            iterator_or_null(std::nullptr_t) : iterator()
            {
                 const_iterator::ptr = nullptr;
            }
            iterator_or_null(const iterator& other) : iterator(other)  {  }
            bool is_null() const  {  return nullptr == const_iterator::ptr;  }
            T& operator*() const
            {                                                                   assert(!is_null());
                return iterator::operator*();
            }
            T* operator->() const
            {                                                                   assert(!is_null());
                return iterator::operator->();
            }
            bool operator==(const const_iterator& other) const
            {
                return const_iterator::ptr == other.ptr;
            }
            bool operator!=(const const_iterator& other) const
            {
                return !operator==(other);
            }
            bool operator==(const T* const other) const
            {                                                                   assert(nullptr != other);
                // It is allowed to call this even if is_null().  Therefore, we
                // cannot use iterator_or_null::operator->().
                return const_iterator::operator->() == other;
            }
            bool operator!=(const T* const other) const
            {
                return !operator==(other);
            }

            void operator=(std::nullptr_t)
            {
                const_iterator::ptr = nullptr;
            }
        };

        /// \brief constructor
        simple_list()
          : sentinel(&sentinel, &sentinel)
        {                                                                       static_assert(std::is_trivially_destructible<entry>::value);
        }

        #ifndef USE_POOL_ALLOCATOR
            /// \brief destructor
            ~simple_list()
            {
                for (iterator iter = begin(); end() != iter; )
                {
                    iterator next = std::next(iter);
                    delete static_cast<entry*>(iter.ptr);
                    iter = next;
                }
            }
        #endif

        /// \brief return an iterator to the first element of the list
        iterator begin()  {  return iterator(sentinel.next);  }

        /// \brief return an iterator past the last element of the list
        iterator end()    {  return iterator(&sentinel);      }

        /// \brief return a constant iterator to the first element of the list
        const_iterator cbegin() const { return const_iterator(sentinel.next); }

        /// \brief return a constant iterator past the last element of the list
        const_iterator cend()   const { return const_iterator(&sentinel);     }

        /// \brief return a constant iterator to the first element of the list
        const_iterator begin() const  {  return cbegin();  }

        /// \brief return a constant iterator past the last element of the list
        const_iterator end()   const  {  return cend();    }

        /// \brief return a reference to the first element of the list
        T& front()
        {                                                                       assert(!empty());
            return static_cast<entry*>(sentinel.next)->data;
        }

        /// \brief return a reference to the last element of the list
        T& back()
        {                                                                       assert(!empty());
            return static_cast<entry*>(sentinel.prev)->data;
        }

        /// \brief return true iff the list is empty
        bool empty() const  {  return sentinel.next == &sentinel;  }

        /// \brief construct a new list entry before pos
        template<class... Args>
        static iterator emplace(
                ONLY_IF_POOL_ALLOCATOR( my_pool<entry>& pool, )
                                                  iterator pos, Args&&... args)
        {
            entry* const new_entry(
                #ifdef USE_POOL_ALLOCATOR
                    pool.template construct<entry>
                #else
                    new entry
                #endif
                        (pos.ptr, pos.ptr->prev, std::forward<Args>(args)...));
            pos.ptr->prev->next = new_entry;
            pos.ptr->prev = new_entry;
            return iterator(new_entry);
        }

        /// \brief construct a new list entry after pos
        template<class... Args>
        static iterator emplace_after(
                ONLY_IF_POOL_ALLOCATOR( my_pool<entry>& pool, )
                                                  iterator pos, Args&&... args)
        {
            entry* const new_entry(
                #ifdef USE_POOL_ALLOCATOR
                    pool.template construct<entry>
                #else
                    new entry
                #endif
                        (pos.ptr->next, pos.ptr, std::forward<Args>(args)...));
            pos.ptr->next->prev = new_entry;
            pos.ptr->next = new_entry;
            return iterator(new_entry);
        }

        /// \brief construct a new list entry at the beginning
        template<class... Args>
        iterator emplace_front(
                ONLY_IF_POOL_ALLOCATOR( my_pool<entry>& pool, )
                                                                Args&&... args)
        {
            entry* const new_entry(
                #ifdef USE_POOL_ALLOCATOR
                    pool.template construct<entry>
                #else
                    new entry
                #endif
                      (sentinel.next, &sentinel, std::forward<Args>(args)...));
            sentinel.next->prev = new_entry;
            sentinel.next = new_entry;
            return iterator(new_entry);
        }

        /// \brief construct a new list entry at the end
        template<class... Args>
        iterator emplace_back(
                ONLY_IF_POOL_ALLOCATOR( my_pool<entry>& pool, )
                                                                Args&&... args)
        {
            entry* const new_entry(
                #ifdef USE_POOL_ALLOCATOR
                    pool.template construct<entry>
                #else
                    new entry
                #endif
                      (&sentinel, sentinel.prev, std::forward<Args>(args)...));
            sentinel.prev->next = new_entry;
            sentinel.prev = new_entry;
            return iterator(new_entry);
        }

        /// \brief move a list entry from one position to another (possibly in
        /// a different list)
        static void splice(iterator const new_pos, simple_list& /* unused */,
                                                        iterator const old_pos)
        {
            old_pos.ptr->prev->next = old_pos.ptr->next;
            old_pos.ptr->next->prev = old_pos.ptr->prev;

            old_pos.ptr->next = new_pos.ptr->prev->next;
            old_pos.ptr->prev = new_pos.ptr->prev;

            old_pos.ptr->prev->next = old_pos.ptr;
            old_pos.ptr->next->prev = old_pos.ptr;
        }

        /// \brief erase an element from a list
        static void erase(
                ONLY_IF_POOL_ALLOCATOR( my_pool<entry>& pool, )
                                                            iterator const pos)
        {
            pos.ptr->prev->next = pos.ptr->next;
            pos.ptr->next->prev = pos.ptr->prev;
            #ifdef USE_POOL_ALLOCATOR
                pool.destroy(static_cast<entry*>(pos.ptr));
            #else
                delete static_cast<entry*>(pos.ptr);
            #endif
        }
    };

    template <class El>
    using iterator_or_null_t = typename simple_list<El>::iterator_or_null;
#else
    #define simple_list std::list

    template <class El>
    union iterator_or_null_t
    {
      public:
        typedef std::list<El>::iterator iterator;
        typedef std::list<El>::const_iterator const_iterator;
      private:
        const void* null;
        iterator iter;
      public:
        /// \brief Construct an uninitialized object
        iterator_or_null_t()
        {
            if constexpr (!std::is_trivially_destructible<iterator>::value)
            {
                // We still have to internally decide whether to construct
                // the iterator or not so the destructor knows what to do.
                null = nullptr;
            }
        }


        /// \brief Construct an object containing a null pointer
        explicit iterator_or_null_t(nullptr_t)
        {
            null = nullptr;
        }


        /// \brief Construct an object containing a valid iterator
        explicit iterator_or_null_t(const iterator& other)
        {
            new (&iter) iterator(other);                                        assert(nullptr != null);
        }


        /// \brief Check whether the object contains a valid iterator
        bool is_null() const  {  return nullptr == null;  }


        /// \brief Destruct an object (whether it contains a valid iterator or
        // not)
        ~iterator_or_null_t()
        {
            if constexpr (!std::is_trivially_destructible<iterator>::value)
            {
                if (!is_null())  iter.~iterator();
            }
        }

        iterator* operator->()
        {                                                                       assert(nullptr != null);
            return iter.operator->();
        }
        iterator& operator*()
        {                                                                       assert(nullptr != null);
            return iter.operator*();
        }

        void operator=(nullptr_t)
        {
            if constexpr (!std::is_trivially_destructible<iterator>::value)
            {
                if (!is_null())  iter.~iterator();
            }
            null = nullptr;
        }


        explicit operator iterator() const
        {                                                                       assert(nullptr != null);
            return iter;
        }


        void operator=(const iterator& other)
        {
            if constexpr (!std::is_trivially_destructible<iterator>::value)
            {
                if (!is_null())  iter.~iterator();
            }
            new (&iter) iterator(other);                                        assert(nullptr != null);
        }

        /// \brief Compare the object with another iterator_or_null_t object
        /// \details The operator could be templated so that iterator_or_null_t
        /// objects of different types can be compared.
        bool operator==(const iterator_or_null_t& other) const
        {
            if constexpr (sizeof(null) == sizeof(iter))
            {
                return &*iter == &*other.iter;
            }
            else
            {
                return (is_null() && other.is_null()) ||
                    (!is_null() && !other.is_null() && &*iter == &*other.iter);
            }
        }


        /// \brief Compare the object with another iterator_or_null_t object
        bool operator!=(const iterator_or_null_t& other) const
        {
            return !operator==(other);
        }


        /// \brief Compare the object with an iterator
        /// \details If the object does not contain a valid iterator, it
        /// compares unequal with the iterator.
        bool operator==(const const_iterator other) const
        {                                                                       // assert(nullptr != &*other); -- generates a warning
            return (sizeof(null) == sizeof(iter) || !is_null()) &&
                                                             &*iter == &*other;
        }


        bool operator!=(const const_iterator other) const
        {
            return !operator==(other);
        }


        /// \brief Compare the object with a non-null pointer
        /// \details If the object does not contain a valid iterator, it
        /// compares unequal with the pointer.
        bool operator==(const El* const other) const
        {                                                                       assert(nullptr != other);
            return (sizeof(null) == sizeof(iter) || !is_null()) &&
                                                               &*iter == other;
        }


        bool operator!=(const El* const other) const
        {
            return !operator==(other);
        }
    };
#endif

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef SIMPLE_LIST_H
