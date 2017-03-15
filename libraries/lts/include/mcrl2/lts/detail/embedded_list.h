// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/embedded_list.h

#ifndef _EMBEDDED_LIST_H
#define _EMBEDDED_LIST_H

#include <cassert>
#include <vector>

namespace mcrl2
{
namespace lts
{
namespace detail
{

// The list type below embeds a list in existing data structures. 
// For instance if elements are stored in a vector, and should also
// be linked in a particular order in a list this data structure can be 
// used.
//
// For this purpose to each data structure that must be put in an embedded
// list the embedded_list_node must be added to the data structure. 
//
// From that point on the embedded_list data type can be used to construct
// an embedded list. The data structure, once created will not require
// extra memory. 

// prototype.
template < class TYPE > class embedded_list;

template <class TYPE>
class embedded_list_node
{
 
  template < class T > friend class embedded_list;

  protected:
     TYPE* m_next;  // Points to the next element in the list. The nullptr if it does not exist.
     TYPE* m_prev;  // Points to the current element in the list. The nullptr if it does not exist.
};


template < class TYPE >
class embedded_list
{
  protected:
    TYPE* m_first;   // Points to the last element of the list. nulllptr if not valid.
    TYPE* m_last;    // Points to the first element of the list
    std::size_t m_size;   // The number of elements in the list.

  protected:

    bool check_this_embedded_list() const
    {
      std::size_t count_up=0;
      for(TYPE* walker=m_first; walker!=nullptr && count_up<=m_size ; walker=walker->m_next)
      {
        count_up++;
      }

      std::size_t count_down=0;
      for(TYPE* walker=m_last; walker!=nullptr && count_down<=m_size ; walker=walker->m_prev)
      {
        count_down++;
      }

      return m_size==count_up && m_size==count_down;
    }

    bool check_presence(const TYPE& e) const
    {
      for(TYPE* walker=m_first; walker!=nullptr; walker=walker->m_next)
      {
        if (walker==&e)
        {
          return true;
        }
      }
      return false;
    }



  public:
    
    // Constructor.
    embedded_list()
      : m_first(nullptr), m_last(nullptr), m_size(0)
    {}

    // Copy constructor.
    embedded_list(const embedded_list& other) = default;

    // get the size
    std::size_t size() const
    {
      return m_size;
    }

    // Get the first element of the list.
    TYPE& front() 
    {
      assert(m_size>0);
      return *m_first;
    }

    // Get the last element of the list.
    TYPE& back() 
    {
      assert(m_size>0);
      return *m_last;
    }

    // Insert an element at the end of the list. Note that the element e is changed.
    void push_back(TYPE& e)
    {
      if (m_first==nullptr)
      {
        assert(m_last==nullptr && m_size==0);
        m_first= &e;
        e.m_prev= nullptr;
      }
      else 
      {
        assert(m_last!=nullptr && m_size>0);
        e.m_prev=m_last;
        m_last->m_next=&e;
      }
      e.m_next=nullptr;
      m_last= &e;
      m_size++;
      assert(check_this_embedded_list());
    }

    // Insert an element at the begining of the list. Note that the element e is changed.
    void push_front(TYPE& e)
    {
      if (m_last==nullptr)
      {
        assert(m_first==nullptr && m_size==0);
        m_last= &e;
        e.m_next= nullptr;
      }
      else 
      {
        assert(m_first!=nullptr && m_size>0);
        e.m_next=m_first;
        m_first->m_prev=&e;
      }
      e.m_prev=nullptr;
      m_first= &e;
      m_size++;
      assert(check_this_embedded_list());
    }

    // Erase this element from the list. The list is adapted such that it does not contain
    // element e anymore. Postcondition: The previous and next pointer in e are invalid. 
    void erase(TYPE& e)
    {
      assert(check_presence(e));
      if (e.m_next==nullptr)
      {
        assert(&e==m_last);
        m_last=e.m_prev;
      }
      else
      {
        assert(e.m_next->m_prev!=nullptr);
        e.m_next->m_prev = e.m_prev;
      }

      if (e.m_prev==nullptr)
      {
        assert(&e==m_first);
        m_first=e.m_next;
      }
      else
      {
        assert(e.m_prev->m_next!=nullptr);
        e.m_prev->m_next = e.m_next;
      }

      e.m_next= nullptr;
      e.m_prev= nullptr;

      m_size--;

      assert(check_this_embedded_list());
    }

    void clear() 
    {
      m_first=nullptr;
      m_last=nullptr;
      m_size=0;
    }

    /* Append the list l to the current list.  
       After this operation the list l is replaced by the empty list
       to prevent unwanted sharing of lists.  */
       
    void append(embedded_list& l)
    {
      if (l.size()==0)
      {
        return;
      }

      if (m_size==0)
      {
        *this=l;
      }
      else
      {
        m_last->m_next=l.m_first;
        l.m_first->m_prev=m_last;
        m_last=l.m_last;
        m_size=m_size+l.m_size;
      }
      // Explicitly invalidate l.
      l.m_first=nullptr;
      l.m_last=nullptr;
      l.m_size=0;
      
      assert(check_this_embedded_list());
    }

    class iterator 
    {
      protected:
        TYPE* m_ptr;

      public:
        
        iterator(TYPE* p)
         : m_ptr(p)
        {}

        // Prefix increment
        iterator operator++() 
        {
          iterator old=*this;
          m_ptr=m_ptr->m_next;
          return old;
        }
        
        // Postfix increment
        iterator operator++(int) 
        {
          m_ptr=m_ptr->m_next;
          return *this;
        }

        // Dereference of the iterator. 
        TYPE& operator*() 
        {
          return *m_ptr;
        }

        // Dereference of the iterator. 
        TYPE* operator->() 
        {
          return m_ptr;
        }

       // Equality operator on iterators.
       bool operator ==(const iterator& other) const
       {
         return m_ptr==other.m_ptr;
       }
       // Inequality operator on iterators.
       bool operator !=(const iterator& other) const
       {
         return !(*this==other);
       }
        
    };

    iterator begin() const
    {
      assert(check_this_embedded_list());
      return m_first;
    }

    iterator end() const
    {
      return nullptr;
    }
    
};

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
#endif //_EMBEDDED_LIST_H
