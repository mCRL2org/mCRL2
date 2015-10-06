// Author(s): Jan Friso Groote, Anton Wijs
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/sized_forward_list_gw.h


#ifndef SIZED_FORWARD_LIST_GH
#define SIZED_FORWARD_LIST_GH
#include <string>
#include <deque>
#include <cassert>
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

typedef size_t statemode_type;

/* The data structure below is intended to recycle forward list
 * elements, such that they do not have to be malloced and deleted
 * which is time consuming. The forward list elements are stored
 * in a deque. There is a list of free elements to which deallocated
 * elements are stored, and from which allocated members are taken
 * when they are needed and available.
 *
 * This list is only intended for the gw bisimulation checker. These
 * forward lists are non copiable, nor can they be constructed from
 * each other.  */

typedef struct two_pointers
{
  protected:
    two_pointers* m_next;
    void* m_content;

  public:
    two_pointers(two_pointers* next, void* content)
     : m_next(next),
       m_content(content)
    {}

    two_pointers* next() const
    {
      return m_next;
    }

    void* content() const
    {
      return m_content;
    }

    void set_next(two_pointers* next)
    {
      m_next=next;
    }

    void set_content(void* content)
    {
      m_content=content;
    }

} two_pointers;

inline std::deque<two_pointers>& local_forward_list_storage()
{
  static std::deque<two_pointers> m_storage;
  return m_storage;
}

inline two_pointers*& local_forward_list_storage_free_list_elements()
{
  static two_pointers* m_free_list_elements;
  return m_free_list_elements;
}

// forward_list with size counter and pointer to last element.
// This forward list stores pointers to objects of class T.
template < class T>
class sized_forward_list 
{
  protected:
    size_t m_list_size;
    // The first field is the link to the first node of the list.
    // The second field is used as a pointer to the last node.
    two_pointers* m_node_before_begin;

    two_pointers* allocate_node(two_pointers* next, void* val)
    {
      if (local_forward_list_storage_free_list_elements()==NULL)
      {
        local_forward_list_storage().push_back(two_pointers(next,val));
        return &local_forward_list_storage().back();
      }
      else
      {
        two_pointers* result=local_forward_list_storage_free_list_elements();
        local_forward_list_storage_free_list_elements()=local_forward_list_storage_free_list_elements()->next();
        result->set_next(next);
        result->set_content(val);
        return result;
      }
    }

    void deallocate_node(two_pointers* node)
    {
      node->set_next(local_forward_list_storage_free_list_elements());
      local_forward_list_storage_free_list_elements()=node;
    }

    // Get a pointer to the first node.
    two_pointers* first() const
    {
      return m_node_before_begin->next();
    }

    // Get a poiner to the last node.
    two_pointers* last() const
    {
      return reinterpret_cast<two_pointers*>(m_node_before_begin->content());
    }

    // set the first node of this list.
    void set_first(two_pointers* node)
    {
      m_node_before_begin->set_next(node);
    }

    // set the last node of this list.
    void set_last(two_pointers* node)
    {
      m_node_before_begin->set_content(node);
    }

    // Copy constructor is not possible
    sized_forward_list(const sized_forward_list&)
    {
      assert(0);
    }

    // Assignment of lists is not possible
    sized_forward_list operator=(const sized_forward_list&)
    {
      assert(0);
    }

  public:
    sized_forward_list()
      :  m_list_size(0),
         m_node_before_begin(allocate_node(NULL,NULL))
    {
      set_last(m_node_before_begin);
    }

    ~sized_forward_list()
    {
      last()->set_next(local_forward_list_storage_free_list_elements());
      local_forward_list_storage_free_list_elements()=m_node_before_begin;
    }

    void swap(sized_forward_list& other)
    {
      const size_t tmp1=m_list_size;
      m_list_size=other.m_list_size;
      other.m_list_size=tmp1;
      two_pointers* tmp2=m_node_before_begin;
      m_node_before_begin=other.m_node_before_begin;
      other.m_node_before_begin=tmp2;
    }

    class iterator
    {
      protected:
        friend class sized_forward_list;
        two_pointers* m_iter;

      public:
        iterator()
         : m_iter(NULL)
        {}

        iterator(two_pointers* iter)
         : m_iter(iter)
        {}

        iterator operator++(int)
        {
          assert(m_iter!=NULL);
          iterator old=m_iter;
          m_iter=m_iter->next();
          return old;
        }

        iterator operator++()
        {
          assert(m_iter!=NULL);
          m_iter=m_iter->next();
          return m_iter;
        }

        T* operator*() const
        {
          assert(m_iter!=NULL);
          return reinterpret_cast<T*>(m_iter->content());
        }

        T* operator->() const
        {
          assert(m_iter!=NULL);
          return reinterpret_cast<T*>(m_iter->content());
        }

        bool operator==(iterator other) const
        {
          return m_iter==other.m_iter;
        }

        bool operator!=(iterator other) const
        {
          return m_iter!=other.m_iter;
        }
    };

    iterator begin() const
    {
      return iterator(first());
    }

    iterator before_begin() 
    {
      return iterator(m_node_before_begin);
    }

    iterator end() const
    {
      return iterator(NULL);
    }

    T* front() const
    {
      assert(check_integrity_of_sized_forward_list("FRONT",false));
      return reinterpret_cast<T*>(first()->content());
    }

    iterator insert(T* obj)
    {
      assert(check_integrity_of_sized_forward_list("INSERT1",false));
      two_pointers* node=allocate_node(m_node_before_begin->next(),obj);
      m_node_before_begin->set_next(node);
      m_list_size++;
      if (m_list_size == 1) 
      {
        set_last(node);
      }
      assert(check_integrity_of_sized_forward_list("INSERT2",false));
      return iterator(node);
    }

    void insert_back(T* obj)
    {
      assert(check_integrity_of_sized_forward_list("INSERT_BACK",false));
      two_pointers* node=allocate_node(NULL,obj);
      last()->set_next(node);
      set_last(node);
      m_list_size++;
      assert(check_integrity_of_sized_forward_list("INSERT_BACK2",false));
    }
  
    // in 'linked' methods, the object to be inserted has an iterator 'ptr_in_list' which needs to refer to the position
    // preceding the one where obj is inserted. This allows constant time lookup of objects in lists.
    iterator insert_linked(T* obj)
    {
      assert(check_integrity_of_sized_forward_list("INSERT LINKED1"));
      iterator it=insert(obj);
      obj->ptr_in_list = iterator(m_node_before_begin);

      if (m_list_size > 1) 
      {
        iterator itnext = it;
        itnext++;
        itnext->ptr_in_list = it;
      }
      assert(check_integrity_of_sized_forward_list("INSERT LINKED2"));
      return it;
    }
  
    // insert element at the back of the list
    void insert_linked_back(T* obj) 
    {
      assert(check_integrity_of_sized_forward_list("LINKDED BACK1"));
      obj->ptr_in_list = last();
      insert_back(obj);
      assert(check_integrity_of_sized_forward_list("LINKED BACK2"));
    }

    // special method for inserting states
    void insert_state_linked(T* obj, statemode_type t)
    {
      assert(check_integrity_of_sized_forward_list());
      // make sure that element pointing before current first element will point to the newly inserted element
      // (the new position before its element)
      insert_linked(obj);
      obj->type=t;
      assert(check_integrity_of_sized_forward_list());
    }
  
    iterator remove_after(const iterator position)
    {
      assert(is_in_sized_forward_list(position));
      assert(check_integrity_of_sized_forward_list("REMOVE AFTER1",false));
      two_pointers* node=position.m_iter;
      two_pointers* node_to_remove=node->next();
      assert(node_to_remove!=NULL);
      node->set_next(node_to_remove->next());
      m_list_size--;
      if (node_to_remove==last())
      {
        set_last(node);
      }
      deallocate_node(node_to_remove);
      assert(check_integrity_of_sized_forward_list("REMOVE AFTER2",false));
      return node->next();
    }

    iterator remove_after(iterator position, iterator& current)
    {
      if (position.m_iter->next()==current.m_iter)
      {
        current=position;
      }
      return remove_after(position);
    }

    void remove_linked(T* obj)
    {
      assert(check_integrity_of_sized_forward_list("REMOVE LINKED1"));
      assert(is_in_sized_forward_list(obj->ptr_in_list));
      iterator objptrnext = obj->ptr_in_list;
      ++objptrnext;
      assert(objptrnext!=NULL);
      // redirect pointer of next element in list
      ++objptrnext;
      if (objptrnext != end()) 
      {
        objptrnext->ptr_in_list = obj->ptr_in_list;
      }
      remove_after(obj->ptr_in_list);
      assert(check_integrity_of_sized_forward_list("REMOVE LINKED2"));
    }

    void remove_linked(T* obj, iterator& current)
    {
      assert(check_integrity_of_sized_forward_list());
      if (obj->ptr_in_list.m_iter->next()==current.m_iter)
      {
        current=obj->ptr_in_list;
      }
      remove_linked(obj);
    }
  
    size_t size() const
    {
      return m_list_size;
    }
  
    iterator back()
    {
      assert(check_integrity_of_sized_forward_list("BACK",false));
      return iterator(last());
    }

    void clear() 
    {
      last()->set_next(local_forward_list_storage_free_list_elements());
      local_forward_list_storage_free_list_elements()=first();
      set_last(m_node_before_begin);
      set_first(NULL);
      m_list_size = 0;
      assert(check_integrity_of_sized_forward_list("CLEAR"));
    }

  // protected:
  public:
    bool is_in_sized_forward_list(iterator position)
    {
      for(iterator i=before_begin(); i!=end(); ++i)
      {
        if (i==position)
        {
          i++;
          return (i!=end());
        }
      }
      return false;
    }

    bool not_in_free_list(two_pointers* n) const
    {
      for(two_pointers* i=local_forward_list_storage_free_list_elements(); i!=NULL; i=i->next())
      {
        if (i==n)
        {
          return false;
        }
      }
      return true;
    }

    bool check_integrity_of_sized_forward_list(const std::string message="",bool check_ptr_in_list=true) const
    {
// std::cerr << message << "\n";
      size_t length=0;
      // iterator last_seen=before_begin();
      const two_pointers* last_seen=m_node_before_begin;
// std::cerr << "LIST: " << last_seen << "{" << last_seen->next() << "," << last_seen->content() << "} ";
      for(iterator i=begin(); i!=end(); ++i)
      {
// std::cerr << "N: " << i.m_iter << "[" << i.m_iter->next() << "," << i->ptr_in_list.m_iter << "(" << i.m_iter->content() << ")] ";
        if (!not_in_free_list(i.m_iter))
        {
// std::cerr << "\nNODE " << i.m_iter << " IS IN FREE LIST \n";
          std::cerr << "ERROR IN " << message << "\n";
          return false;
        }
        // Check that the ptr_in_list is either NULL or points to the node before the node in the list
        // that refers to this node. 
        if (check_ptr_in_list)
        {
          two_pointers* before_node_i=i->ptr_in_list.m_iter;
          if (before_node_i!=NULL && before_node_i->next()!=i.m_iter)
          {
// std::cerr << "\nBEFORE NODE " << before_node_i << "  NEXT " << before_node_i->next() << "  iter " << i.m_iter << "\n";
            // The ptr_in_list of a node does not point to a node before this node in the list.
            return false;
          }
        }
        last_seen=i.m_iter;
        length++;
        
      }
// std::cerr << "\n";
// std::cerr << "Length actual: " << length << " Size supposed: " << m_list_size << " (" << message << ")\n";
// std::cerr << "Last: " << last() << "  -- " << last_seen << " last.next: " << last()->next() << "\n";
      return length==m_list_size && last()==last_seen && last()->next()==NULL;
    }

};

} // namespace detail

} // namespace lts

} // namespace mcrl2

#endif // SIZED_FORWARD_LIST_GH
