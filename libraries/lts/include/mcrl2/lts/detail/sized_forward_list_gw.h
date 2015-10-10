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

    // Get a pointer to the last node.
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
      return reinterpret_cast<T*>(first()->content());
    }

    iterator insert_node(two_pointers* node)
  {
      m_node_before_begin->set_next(node);
      m_list_size++;
      if (m_list_size == 1) 
      {
        set_last(node);
      }
      assert(check_integrity_of_sized_forward_list("INSERT_NODE",false));
      return iterator(node);
    }

    void insert_node_back(two_pointers* node)
    {
      last()->set_next(node);
      set_last(node);
      m_list_size++;
      assert(check_integrity_of_sized_forward_list("INSERT_NODE_BACK",false));
    }

    iterator insert(T* obj)
    {
      two_pointers* node=allocate_node(m_node_before_begin->next(),obj);
      iterator it = insert_node(node);
      return it;
    }

    void insert_back(T* obj)
    {
      two_pointers* node=allocate_node(NULL,obj);
      insert_node_back(node);
    }
  
    // in 'linked' methods, the object to be inserted has an iterator 'ptr_in_list' which needs to refer to the position
    // preceding the one where obj is inserted. This allows constant time lookup of objects in lists.
    iterator insert_linked(T* obj)
    {
      iterator it=insert(obj);
      obj->ptr_in_list = iterator(m_node_before_begin);

      if (m_list_size > 1) 
      {
        iterator itnext = it;
        itnext++;
        itnext->ptr_in_list = it;
      }
      assert(check_integrity_of_sized_forward_list("INSERT LINKED2",true));
      return it;
    }
  
    // insert element at the back of the list
    void insert_linked_back(T* obj) 
    {
      obj->ptr_in_list = last();
      insert_back(obj);
      assert(check_integrity_of_sized_forward_list("LINKED BACK2",true));
    }

    // special method for inserting states
    void insert_state_linked(T* obj, statemode_type t)
    {
      // make sure that element pointing before current first element will point to the newly inserted element
      // (the new position before its element)
      insert_linked(obj);
      obj->type=t;
    }

    two_pointers* detach_node(const iterator position)
    {
      assert(is_in_sized_forward_list(position));
      two_pointers* node=position.m_iter;
      two_pointers* node_to_remove=node->next();
      assert(node_to_remove!=NULL);
      node->set_next(node_to_remove->next());
      m_list_size--;
      if (node_to_remove==last())
      {
        set_last(node);
      }
      return node_to_remove;
    }
  
    iterator remove_after(const iterator position)
    {
      two_pointers* node_to_remove = detach_node(position);
      deallocate_node(node_to_remove);
      two_pointers* node=position.m_iter;
      return node->next();
    }

    iterator remove_after(const iterator position, iterator& current)
    {
      if (position.m_iter->next()==current.m_iter)
      {
        current=position;
      }
      return remove_after(position);
    }
  
    void rearrange_element_pointers(T* obj)
    {
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
    }

    void remove_linked(T* obj)
    {
      rearrange_element_pointers(obj);
      remove_after(obj->ptr_in_list);
      assert(check_integrity_of_sized_forward_list("REMOVE LINKED",true));
    }

    void remove_linked(T* obj, iterator& current)
    {
      if (obj->ptr_in_list.m_iter->next()==current.m_iter)
      {
        current=obj->ptr_in_list;
      }
      remove_linked(obj);
    }

    // move element from this list to back of other list
    iterator move_from_after_to_back(const iterator position, sized_forward_list& other, iterator& current)
    {
      if (position.m_iter->next()==current.m_iter)
      {
        current=position;
      }
      two_pointers* node_to_move = detach_node(position);
      node_to_move->set_next(NULL);
      other.insert_node_back(node_to_move);
      return position.m_iter->next();
    }
  
    // move element from this list to other list
    iterator move_linked(T* obj, sized_forward_list& other)
    {
      rearrange_element_pointers(obj);
      two_pointers* node_to_move = detach_node(obj->ptr_in_list);
      node_to_move->set_next(other.m_node_before_begin->next());
      iterator it = other.insert_node(node_to_move);
      obj->ptr_in_list = iterator(other.m_node_before_begin);
      if (other.m_list_size > 1)
      {
        iterator itnext = it;
        itnext++;
        itnext->ptr_in_list = it;
      }
      return it;
    }
  
    iterator move_linked(T* obj, sized_forward_list& other, iterator& current)
    {
      if (obj->ptr_in_list.m_iter->next()==current.m_iter)
      {
        current=obj->ptr_in_list;
      }
      return move_linked(obj, other);
    }

    // move element to front of list
    iterator move_to_front_linked(T* obj)
    {
      return move_linked(obj, *this);
    }
  
    // move element to back of list
    void move_to_back_linked(T* obj)
    {
      rearrange_element_pointers(obj);
      two_pointers* node_to_move = detach_node(obj->ptr_in_list);
      node_to_move->set_next(NULL);
      obj->ptr_in_list = iterator(last());
      last()->set_next(node_to_move);
      set_last(node_to_move);
      m_list_size++;
      assert(check_integrity_of_sized_forward_list("MOVE TO BACK2",true));
    }

    void move_to_back_linked(T* obj, iterator& current)
    {
      if (obj->ptr_in_list.m_iter->next()==current.m_iter)
      {
        current=obj->ptr_in_list;
      }
      move_to_back_linked(obj);
    }

    // special method for moving states
    void move_state_linked(T* obj, sized_forward_list& other, statemode_type t)
    {
      move_linked(obj, other);
      obj->type=t;
    }

    void move_state_linked(T* obj, sized_forward_list& other, statemode_type t, iterator& current)
    {
      if (obj->ptr_in_list.m_iter->next()==current.m_iter)
      {
        current=obj->ptr_in_list;
      }
      move_state_linked(obj, other, t);
    }
  
    size_t size() const
    {
      return m_list_size;
    }
  
    iterator back()
    {
      return iterator(last());
    }

    void clear() 
    {
      last()->set_next(local_forward_list_storage_free_list_elements());
      local_forward_list_storage_free_list_elements()=first();
      set_last(m_node_before_begin);
      set_first(NULL);
      m_list_size = 0;
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

    bool check_integrity_of_sized_forward_list(const std::string message, bool check_ptr_in_list) const
    {
      return true; // This check is very time consuming. It must be explicitly switched on.
      size_t length=0;
      const two_pointers* last_seen=m_node_before_begin;
      if (m_list_size == 0 && m_node_before_begin != last())
      {
        std::cerr << "wrong before begin and last values for empty list " << message << " \n";
        return false;
      }
      for(iterator i=begin(); i!=end(); ++i)
      {
        if (!not_in_free_list(i.m_iter))
        {
          std::cerr << "\nNODE " << i.m_iter << " IS IN FREE LIST \n";
          return false;
        }
        // Check that the ptr_in_list is either NULL or points to the node before the node in the list
        // that refers to this node. 
        if (check_ptr_in_list)
        {
          two_pointers* before_node_i=i->ptr_in_list.m_iter;
          if (before_node_i!=NULL && before_node_i->next()!=i.m_iter)
          {
            std::cerr << "\nBEFORE NODE " << before_node_i << "  NEXT " << before_node_i->next() << "  iter " << i.m_iter << "\n";
            // The ptr_in_list of a node does not point to a node before this node in the list.
            return false;
          }
        }
        last_seen=i.m_iter;
        length++;
        
      }
      return length==m_list_size && last()==last_seen && last()->next()==NULL;
    }
};

// wrapper for pool structure
template < class T>
class pooled_sized_forward_list
{
  protected:
    // needed in repository
    pooled_sized_forward_list* r_next;
  public:
    // the actual list
    sized_forward_list<T> list;

    void set_rep_next(pooled_sized_forward_list* e)
    {
      r_next = e;
    }
    pooled_sized_forward_list* rep_next()
    {
      return r_next;
    }
    void rep_init()
    {
      r_next = NULL;
      list.clear();
    }

    pooled_sized_forward_list()
     : r_next(NULL)
    {}

    // Copy constructor
    // Does not copy lists
    pooled_sized_forward_list(const pooled_sized_forward_list& c)
      : r_next(c.r_next)
    {
    }
  
    // Assignment
    pooled_sized_forward_list operator=(const pooled_sized_forward_list& c)
    {
      r_next = c.r_next;
      return *this;
    }
};

} // namespace detail

} // namespace lts

} // namespace mcrl2

#endif // SIZED_FORWARD_LIST_GH
