// Author(s): Jan Friso Groote (with edits by David N. Jansen)
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_gj.h
///
/// \brief O(m log n)-time branching bisimulation algorithm similar to liblts_bisim_dnj.h
///        which does not use bunches, i.e., partitions of transitions. This algorithm
///        should be slightly faster, but in particular use less memory than liblts_bisim_dnj.h.
///        Otherwise the functionality is exactly the same.

// TODO:
// Merge identifying whether there is a splitter and actual splitting (Done. No performance effect).
// Use BLC lists for the main split.
// Maintain a co-splitter and a splitter to enable a co-split.
// Eliminate two pointers in transition_type (done).
// JFG: Optimise swap. 

#ifndef LIBLTS_BISIM_GJ_H
#define LIBLTS_BISIM_GJ_H

//#include <forward_list>
#include <deque>
#include "mcrl2/utilities/hash_utility.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/check_complexity.h"
#include "mcrl2/lts/detail/fixed_vector.h"

// If CO_SPLITTER_IN_BLC_LIST is defined, the co-splitter belonging to a splitter
// is found by ordering the BLC_indicators lists in a specific way: the co-splitter
// is always placed immediately before the main splitter in the respective list.
// (If CO_SPLITTER_IN_BLC_LIST is not defined, one uses a std::unordered_map to
// store for every pair <source block, action label> a transition to the respective
// co-splitter.)
// It seems that defining this constant is advantageous.
#define CO_SPLITTER_IN_BLC_LIST

// If TRY_EFFICIENT_SWAP is defined, then splitting a block will try to swap more
// efficiently: states that are already in an acceptable position in m_states_in_blocks
// will not be moved. This may require some additional reading but fewer changes
// of pointers.
// However, a test pointed out that it is about 2% slower.  As the code becomes
// more complex with TRY_EFFICIENT_SWAP, I suggest to not use the option.
//#define TRY_EFFICIENT_SWAP

namespace mcrl2
{
namespace lts
{
namespace detail
{

template <class LTS_TYPE> class bisim_partitioner_gj;

namespace bisimulation_gj
{

// Forward declaration.
struct state_type_gj;
struct block_type;
struct transition_type;
struct outgoing_transition_type;

typedef std::size_t state_index;
typedef std::size_t transition_index;
typedef std::size_t block_index;


typedef std::size_t label_index;
typedef std::size_t constellation_index;
typedef fixed_vector<outgoing_transition_type>::iterator outgoing_transitions_it;
typedef fixed_vector<outgoing_transition_type>::const_iterator outgoing_transitions_const_it;

constexpr constellation_index null_constellation=-1;
constexpr transition_index null_transition=-1;
constexpr label_index null_action=-1;
constexpr state_index null_state=-1;
constexpr block_index null_block=-1;
constexpr transition_index undefined=-1;
constexpr transition_index Rmarked=-2;

// The function clear takes care that a container frees memory when it is cleared and it is large.
template <class CONTAINER>
void clear(CONTAINER& c)
{
  if (c.size()>1000) { c=CONTAINER(); } else { c.clear(); }
}

// Private linked list that uses less memory.

// The linked_list type given here is almost a circular list:
// every element points to the next, except the last one, which contains nullptr
// as next pointer.
// The prev pointers are completely circular.
// This allows to find the last element of the list as well.
// Additionally, it simplifies walking forward through the list, as
// end()==nullptr.  (Walking backward is slightly more difficult; to enable it,
// there is an additional iterator before_end().)
template <class T>
struct linked_list_node;

template <class T>
struct linked_list_const_iterator
{
  public:
    typedef std::ptrdiff_t difference_type;
    typedef linked_list_node<T> value_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;
    typedef std::forward_iterator_tag iterator_category;

  protected:
    value_type* m_iterator=nullptr;
  public:

    linked_list_const_iterator()=default;

    linked_list_const_iterator(pointer t)
      : m_iterator(const_cast<value_type*>(t))
    {}

    linked_list_const_iterator& operator ++()
    {
      assert(nullptr!=m_iterator);
      *this=m_iterator->next();
      return *this;
    }

    linked_list_const_iterator operator ++(int)
    {
      linked_list_const_iterator temp=*this;
      operator ++();
      return temp;
    }

    //linked_list_const_iterator& operator --()
    //{
    //  assert(nullptr != m_iterator);
    //  *this=m_iterator->prev();
    //  assert(m_iterator->next()!=nullptr); // -- should not be applied to begin()
    //  return *this;
    //}

    //linked_list_const_iterator operator --(int)
    //{
    //  linked_list_const_iterator temp = *this;
    //  operator --();
    //  return temp;
    //}

    reference operator *() const
    {
      assert(nullptr!=m_iterator);
      return *m_iterator;
    }

    pointer operator ->() const
    {
      assert(nullptr!=m_iterator);
      return m_iterator;
    }

    bool operator !=(const linked_list_const_iterator other) const
    {
      return m_iterator!=other.m_iterator;
    }

    bool operator ==(const linked_list_const_iterator other) const
    {
      return m_iterator==other.m_iterator;
    }
};

template <class T>
struct linked_list_iterator : public linked_list_const_iterator<T>
{
  public:
    typedef typename linked_list_const_iterator<T>::difference_type difference_type;
    typedef typename linked_list_const_iterator<T>::value_type value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef typename linked_list_const_iterator<T>::iterator_category iterator_category;

    linked_list_iterator()=default;

    linked_list_iterator(pointer t)
      : linked_list_const_iterator<T>(t)
    {}

    linked_list_iterator& operator ++()
    {
      return static_cast<linked_list_iterator<T>&>(linked_list_const_iterator<T>::operator ++());
    }

    linked_list_iterator operator ++(int i)
    {
      return static_cast<linked_list_iterator<T> >(linked_list_const_iterator<T>::operator ++(i));
    }

    //linked_list_iterator& operator --()
    //{
    //  return static_cast<linked_list_iterator<T>&>(linked_list_const_iterator<T>::operator --());
    //}

    //linked_list_iterator operator --(int i)
    //{
    //  return static_cast<linked_list_iterator<T> >(linked_list_const_iterator<T>::operator --(i));
    //}

    reference operator *() const
    {
      return const_cast<reference>(linked_list_const_iterator<T>::operator *());
    }

    pointer operator ->() const
    {
      return const_cast<pointer>(linked_list_const_iterator<T>::operator ->());
    }
};

template <class T>
struct linked_list_node: public T
{
  public:
    typedef linked_list_const_iterator<T> const_iterator;
    typedef linked_list_iterator<T> iterator;
  private:
    iterator m_next;
    iterator m_prev;
  public:
    template <class... Args>
    linked_list_node(iterator next, iterator prev, Args&&... args)
     : T(std::forward<Args>(args)...),
       m_next(next),
       m_prev(prev)
    {}

    iterator& next()
    {
      return m_next;
    }

    const const_iterator& next() const
    {
      return m_next;
    }

    iterator& prev()
    {
      return m_prev;
    }

    const const_iterator& prev() const
    {
      return m_prev;
    }

    T& content()
    {
      return static_cast<T&>(*this);
    }

    operator T&()
    {
      return content();
    }

    operator const T&() const
    {
      return static_cast<const T&>(*this);
    }
};

template <class T>
struct global_linked_list_administration
{
  std::deque<linked_list_node<T> > m_content;
  linked_list_iterator<T> m_free_list=nullptr;
};

template <class T>
struct linked_list
{
  typedef linked_list_const_iterator<T> const_iterator;
  typedef linked_list_iterator<T> iterator;

  global_linked_list_administration<T>& glla() { static global_linked_list_administration<T> glla; return glla; }
  iterator m_initial_node=nullptr;

  iterator begin() const
  {
    return m_initial_node;
  }

  static iterator end()
  {
    return nullptr;
  }

  bool empty() const
  {
    return m_initial_node==nullptr;
  }

  iterator before_end() const
  {
    assert(!empty());
    return m_initial_node->prev();
  }

#ifndef NDEBUG
  [[nodiscard]]
  bool check_linked_list() const
  {
    if (empty())
    {
      return true;
    }
    const_iterator i=m_initial_node;
    if (i->prev()==nullptr)
    {
//std::cerr << "Error at the beginning of a linked list\n";
      return false;
    }
    while (i->next()!=nullptr)
    {
      if (i->next()->prev()!=i)
      {
//std::cerr << "Error in the middle of a linked list\n";
        return false;
      }
      ++i;
      assert(i->prev()->next() == i);
    }
//if (m_initial_node->prev()!=i) { std::cerr << "Error at the end of a linked list\n"; }
    return m_initial_node->prev()==i;
  }
#endif

  // Puts a new element before the current element indicated by pos.
  // It is ok to place the new element at the end (i.e. pos==nullptr is allowed).
  template <class... Args>
  iterator emplace(const iterator pos, Args&&... args)
  {
    #ifndef NDEBUG
      assert(pos==nullptr || !empty());
      assert(pos==nullptr || pos->prev()!=nullptr);
      assert(check_linked_list());
      if (pos!=nullptr) { for(iterator i=begin();i!=pos;++i) { assert(i!=end()); } }
    #endif
    const iterator prev = pos==nullptr ? (empty() ? nullptr/* should become ==new_position later */ : before_end()) : pos->prev();
    iterator new_position;
    if (glla().m_free_list==nullptr)
    {
      new_position=&glla().m_content.emplace_back(pos, prev, std::forward<Args>(args)...);
    }
    else
    {
      // Take an element from the free list.
      new_position=glla().m_free_list;
      glla().m_free_list=glla().m_free_list->next();
      new_position->content()=T(std::forward<Args>(args)...);
      new_position->next()=pos;
      new_position->prev()=prev;
    }
    if (pos==m_initial_node)
    {
      // we insert a new element before the current list begin, so the begin should change.
      // This includes the case that the list was empty before.
      m_initial_node=new_position;
    }
    else if (prev!=nullptr)
    {
      // We insert an element not at the current list begin, so it should be reachable from its predecessor.
      prev->next()=new_position;
    }
    if (pos!=nullptr)
    {
      pos->prev()=new_position;
    }
    else
    {
      assert(m_initial_node!=nullptr);
      m_initial_node->prev()=new_position;
    }
    #ifndef NDEBUG
      assert(check_linked_list());
      assert((pos==nullptr ? before_end() : pos->prev())==new_position);
      for(iterator i=begin();i!=new_position;++i) { assert(i!=end()); }
    #endif

    return new_position;
  }

  // Puts a new element at the end.
  template <class... Args>
  iterator emplace_back(Args&&... args)
  {
    return emplace(end(), std::forward<Args>(args)...);
  }

  // Puts a new element after the current element indicated by pos, unless
  // pos==end(), in which it is put in front of the list.
  template <class... Args>
  iterator emplace_after(const iterator pos, Args&&... args)
  {
    #ifndef NDEBUG
      assert(pos==nullptr || !empty());
      assert(pos==nullptr || pos->prev()!=nullptr);
      assert(check_linked_list());
      if (pos!=nullptr) { for (const_iterator i=begin(); i!=pos; ++i) { assert(i!=end()); } }
    #endif
    const iterator next = pos==nullptr ? begin() : pos->next();
    const iterator prev = pos==nullptr ? (empty() ? nullptr/* should become ==new_position later */ : before_end()) : pos;
//std::cerr << "emplace_after(this == " << (const void*)this << ", pos == " << (const void*)(pos==nullptr ? nullptr : &*pos) << ",...): next == " << (const void*)(next==nullptr ? nullptr : &*next) << ", prev == " << (const void*)(prev==nullptr ? nullptr : &*prev);
    iterator new_position;
    if (glla().m_free_list==nullptr)
    {
      new_position=&glla().m_content.emplace_back(next, prev, std::forward<Args>(args)...);
    }
    else
    {
      // Take an element from the free list.
      new_position=glla().m_free_list;
      glla().m_free_list=glla().m_free_list->next();
      new_position->content()=T(std::forward<Args>(args)...);
      new_position->next()=next;
      new_position->prev()=prev;
    }
//std::cerr << ", new_position == " << (const void*)&*new_position << '\n';
    if (pos==nullptr)
    {
      // we insert a new element before the current list begin, so the begin should change.
      // This includes the case that the list was empty before.
      m_initial_node=new_position;
    }
    else
    {
      pos->next()=new_position;
    }
    assert(m_initial_node!=nullptr);
    if (next==nullptr)
    {
      m_initial_node->prev()=new_position;
    }
    else
    {
      next->prev()=new_position;
    }
    #ifndef NDEBUG
//std::cerr << "m_initial_node == " << (const void*)&*m_initial_node << ", new_position->prev() == " << (const void*)&*new_position->prev() << ", new_position->next() == " << (const void*)(new_position->next()==nullptr ? nullptr : &*new_position->next()) << '\n';
      assert(check_linked_list());
      assert((pos==nullptr ? m_initial_node : pos->next())==new_position);
      for (const_iterator i=begin(); i!=new_position; ++i) { assert(i!=end()); }
    #endif

    return new_position;
  }

  template <class... Args>
  iterator emplace_front(Args&&... args)
  {
    return emplace_after(end(), std::forward<Args>(args)...);
  }

  // The function moves the element pointed at by from_pos (that is in the list
  // indicated by the 2nd parameter) just after position to_pos (that is in
  // this list). If to_pos == nullptr, move the element to the beginning of this
  // list.
  void splice_to_after(iterator const to_pos, linked_list<T>& from_list, iterator const from_pos)
  {
    #ifndef NDEBUG
      assert(from_pos!=to_pos);
      assert(to_pos==nullptr || !empty());
      assert(to_pos==nullptr || to_pos->prev()!=nullptr);
      assert(check_linked_list());
      if (to_pos!=nullptr) { for (const_iterator i=begin(); i!=to_pos; ++i) { assert(i!=end()); } }
      assert(from_pos!=nullptr);
      assert(from_pos->prev()!=nullptr);
      assert(!from_list.empty());
      assert(from_list.check_linked_list());
      for (const_iterator i=from_list.begin(); i!=from_pos; ++i) { assert(i!=from_list.end()); }
    #endif
    // remove element from_pos from its original list
    if (from_pos!=from_list.m_initial_node)
    {
      // not the first element in from_list
      assert(from_list.m_initial_node->next()!=nullptr); // there are at least 2 elements in the list
      assert(from_pos == from_pos->prev()->next());
      from_pos->prev()->next() = from_pos->next();
      if (from_pos->next()!=nullptr)
      {
        // not the last element in from_list
        assert(from_pos==from_pos->next()->prev());
        from_pos->next()->prev()=from_pos->prev();
      }
      else
      {
        // last element in from_list
        assert(from_pos==from_list.m_initial_node->prev());
        from_list.m_initial_node->prev()=from_pos->prev();
      }
    }
    else
    {
      // first element in from_list
      assert(from_pos->prev()->next()==nullptr);
      from_list.m_initial_node=from_pos->next();
      if (!from_list.empty())
      {
        // not the last element in from_list
        assert(from_pos==from_pos->next()->prev());
        from_pos->next()->prev()=from_pos->prev();
      }
    }
    // update the pointers of from_pos and insert from_pos into this list
    if (to_pos != nullptr)
    {
      // not the first element in *this
      from_pos->prev()=to_pos;
      from_pos->next() = to_pos->next();
      to_pos->next() = from_pos;
      if (from_pos->next()!=nullptr)
      {
        // not the last element in *this
        assert(to_pos==from_pos->next()->prev());
        from_pos->next()->prev()=from_pos;
      }
      else
      {
        // last element in *this
        m_initial_node->prev()=from_pos;
      }
    }
    else
    {
      // first element in *this
      from_pos->next()=m_initial_node;
      if (!empty())
      {
        // not the last element in *this
        from_pos->prev()=m_initial_node->prev();
        m_initial_node->prev()=from_pos;
      }
      else
      {
        // also last element in *this
        from_pos->prev()=from_pos;
      }
      m_initial_node = from_pos;
    }
    #ifndef NDEBUG
      assert(check_linked_list());
      assert(from_list.check_linked_list());
      assert((to_pos==nullptr ? m_initial_node : to_pos->next())==from_pos);
      for (const_iterator i=begin(); i!=from_pos; ++i) { assert(i!=end()); }
      if (to_pos!=nullptr) { for (const_iterator i=begin(); i!=to_pos; ++i) { assert(i!=end()); } }
    #endif
  }

  // The function moves the element pointed at by from_pos (that is in the list
  // indicated by the 2nd parameter) just before position to_pos (that is in
  // this list). If to_pos == nullptr, move the element to the end of this
  // list.
  void splice(iterator const to_pos, linked_list<T>& from_list, iterator const from_pos)
  {
    #ifndef NDEBUG
      assert(from_pos!=to_pos);
      assert(to_pos==nullptr || !empty());
      assert(to_pos==nullptr || to_pos->prev()!=nullptr);
      assert(check_linked_list());
      if (to_pos!=nullptr) { for (const_iterator i=begin(); i!=to_pos; ++i) { assert(i!=end()); } }
      assert(from_pos!=nullptr);
      assert(from_pos->prev()!=nullptr);
      assert(!from_list.empty());
      assert(from_list.check_linked_list());
      for (const_iterator i=from_list.begin(); i!=from_pos; ++i) { assert(i!=from_list.end()); }
    #endif
    // remove element from_pos from its original list
    if (from_pos!=from_list.m_initial_node)
    {
      // not the first element in from_list
      assert(from_list.m_initial_node->next()!=nullptr); // there are at least 2 elements in the list
      assert(from_pos==from_pos->prev()->next());
      from_pos->prev()->next()=from_pos->next();
      if (from_pos->next()!=nullptr)
      {
        // not the last element in from_list
        assert(from_pos==from_pos->next()->prev());
        from_pos->next()->prev()=from_pos->prev();
      }
      else
      {
        // last element in from_list
        assert(from_pos==from_list.m_initial_node->prev());
        from_list.m_initial_node->prev()=from_pos->prev();
      }
    }
    else
    {
      // first element in from_list
      assert(from_pos->prev()->next()==nullptr);
      from_list.m_initial_node=from_pos->next();
      if (!from_list.empty())
      {
        // not the last element in from_list
        assert(from_pos==from_pos->next()->prev());
        from_pos->next()->prev()=from_pos->prev();
      }
    }
    // update the pointers of from_pos and insert from_pos into this list
    from_pos->next()=to_pos;
    if (to_pos==nullptr)
    {
      // from_pos becomes the last element in *this
      if (empty())
      {
        from_pos->prev()=from_pos;
        m_initial_node=from_pos;
        #ifndef NDEBUG
          assert(check_linked_list());
          assert(from_list.check_linked_list());
        #endif
        return;
      }
      from_pos->prev()=before_end();
      from_pos->prev()->next()=from_pos;
      m_initial_node->prev()=from_pos;
    }
    else
    {
      // from_pos does not become the last element in *this
      assert(!empty());
      from_pos->prev()=to_pos->prev();
      to_pos->prev()=from_pos;
      if (to_pos==m_initial_node)
      {
        // we insert a new element before the current list begin, so the begin should change.
        m_initial_node=from_pos;
      }
      else
      {
        from_pos->prev()->next()=from_pos;
      }
     }
     #ifndef NDEBUG
       assert(check_linked_list());
       assert(from_list.check_linked_list());
       assert((to_pos==nullptr ? before_end() : to_pos->prev())==from_pos);
       for (const_iterator i=begin(); i!=from_pos; ++i) { assert(i!=end()); }
       if (to_pos!=nullptr) { for (const_iterator i=begin(); i!=to_pos; ++i) { assert(i!=end()); } }
     #endif
   }

  void erase(iterator const pos)
  {
    #ifndef NDEBUG
      assert(pos!=nullptr);
      assert(pos->prev()!=nullptr);
      assert(!empty());
      assert(check_linked_list());
      for (const_iterator i=begin(); i!=pos; ++i) { assert(i!=end()); }
    #endif
    if (pos->next()!=nullptr)
    {
      // not the last element in the list
      assert(pos == pos->next()->prev());
      pos->next()->prev()=pos->prev();
    }
    else
    {
      // last element in the list
      assert(pos==m_initial_node->prev());
      m_initial_node->prev()=pos->prev();
    }
    if (pos!=m_initial_node)
    {
      // not the first element in the list
      assert(pos == pos->prev()->next());
      pos->prev()->next()=pos->next();
    }
    else
    {
      // first element in the list
      assert(pos->prev()->next()==nullptr);
      m_initial_node=pos->next();
    }
    pos->next()=glla().m_free_list;
    glla().m_free_list=pos;
#ifndef NDEBUG
    pos->prev()=nullptr;
    assert(check_linked_list());
    for (const_iterator i=begin(); i!=end(); ++i) { assert(i!=pos); }
#endif
  }

  // The function computes the successor of pos in the list.  If pos is the last
  // element of the list, it returns end().  It is an error if pos==end() or
  // if pos is not in the list.
  #ifdef NDEBUG
    static // only in debug mode it accesses data of the list itself
  #endif
  iterator next(iterator pos)
  #ifndef NDEBUG
    const // static functions cannot be const
  #endif
  {
    #ifndef NDEBUG
      assert(pos!=end());
      for (const_iterator i=begin(); i!=pos; ++i) { assert(i!=end()); }
    #endif
    return pos->next();
  }

  // The function computes the predecessor of pos in the list.  If pos is at the
  // beginning of the list, it returns end().  It is an error if pos==end() or
  // if pos is not in the list.
  iterator prev(iterator pos) const
  {
    #ifndef NDEBUG
      assert(pos!=end());
      for (const_iterator i=begin(); i!=pos; ++i) { assert(i!=end()); }
    #endif
    return begin()==pos ? end() : pos->prev();
  }
};

// The struct below facilitates to walk through a LBC_list starting from an arbitrary transition.
typedef transition_index* BLC_list_iterator; // should not be nullptr
typedef transition_index* BLC_list_iterator_or_null; // can be nullptr
typedef const transition_index* BLC_list_const_iterator; // should not be nullptr

struct outgoing_transition_type
{
  BLC_list_iterator ref_BLC_transitions;
  outgoing_transitions_it start_same_saC; // Refers to the last state with the same state, action and constellation,
                                          // unless it is the last, which refers to the first state.

  // The default initialiser does not initialize the fields of this struct.
  outgoing_transition_type()
  {}

  outgoing_transition_type(const BLC_list_iterator t, const outgoing_transitions_it sssaC)
   : ref_BLC_transitions(t),
     start_same_saC(sssaC)
  {}
};

/* struct label_count_sum_triple
// David suggests to call this a pair, not a triple. The triple is not really used.
{
  transition_index label_counter=0;
  transition_index not_investigated=0;

  // The default initialiser does not initialize the fields of this struct.
  label_count_sum_triple()
  {}
};

struct label_count_sum_triple: label_count_sum_triple
{
  transition_index cumulative_label_counter=0;

  // The default initialiser does not initialize the fields of this struct.
  label_count_sum_triple()
  {}
}; */

// a pointer to a state, i.e. a reference to a state
struct state_in_block_pointer
{
  state_in_block_pointer(fixed_vector<state_type_gj>::iterator new_ref_state)
   : ref_state(new_ref_state)
  {}

  state_in_block_pointer()
  {}

  fixed_vector<state_type_gj>::iterator ref_state;

  bool operator==(const state_in_block_pointer& other) const
  {
    return ref_state==other.ref_state;
  }

  bool operator!=(const state_in_block_pointer& other) const
  {
    return ref_state!=other.ref_state;
  }
};

// a vector with an additional (internal) field to indicate how much work has been
// done already on it.
class todo_state_vector
{
  std::size_t m_todo_indicator=0;
  std::vector<state_in_block_pointer> m_vec;

  public:
    //typedef std::vector<state_in_block_pointer>::iterator iterator;
    typedef std::vector<state_in_block_pointer>::const_iterator const_iterator;

    void add_todo(const state_in_block_pointer s)
    {
      assert(!find(s));
      m_vec.push_back(s);
    }

    // Move a state from the todo part to the definitive vector.
    state_in_block_pointer move_from_todo()
    {
      assert(!todo_is_empty());
      assert(m_todo_indicator<m_vec.size());
      state_in_block_pointer result=m_vec[m_todo_indicator];
      m_todo_indicator++;
      return result;
    }

    std::size_t size() const
    {
      return m_vec.size();
    }

    std::size_t todo_is_empty() const
    {
      return m_vec.size()==m_todo_indicator;
    }

    std::size_t empty() const
    {
      return m_vec.empty();
    }

    bool find(const state_in_block_pointer s) const
    {
      return std::find(m_vec.begin(), m_vec.end(), s)!=m_vec.end();
    }

    const_iterator begin() const
    {
      return m_vec.begin();
    }

    const_iterator end() const
    {
      return m_vec.end();
    }

    void clear()
    {
      m_todo_indicator=0;
      bisimulation_gj::clear(m_vec);
    }

    void clear_todo()
    {
      m_todo_indicator=m_vec.size();
    }
};



// Below the four main data structures are listed.
struct state_type_gj
{
  block_index block=0;
  std::vector<transition>::iterator start_incoming_transitions;
  outgoing_transitions_it start_outgoing_transitions;
  fixed_vector<state_in_block_pointer>::iterator ref_states_in_blocks;
  transition_index no_of_outgoing_inert_transitions=0;
  transition_index counter=undefined; // This field is used to store local information while splitting. While set to -1 (undefined)
                                 // it is considered to be undefined.
                                 // When set to -2 (Rmarked) it is considered to be marked for being in R or R_todo.
  #ifndef NDEBUG
    /// \brief print a short state identification for debugging
    template<class LTS_TYPE>
    std::string debug_id_short(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        assert(&partitioner.m_states.front() <= this);
        assert(this <= &partitioner.m_states.back());
        return std::to_string(this - &partitioner.m_states.front());
    }

    /// \brief print a state identification for debugging
    template<class LTS_TYPE>
    std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        return "state " + debug_id_short(partitioner);
    }

    mutable check_complexity::state_gj_counter_t work_counter;
  #endif
};

// The following type gives the start and end indications of the transitions for the same block, label and constellation
// in the array m_BLC_transitions.
struct BLC_indicators
{
  BLC_list_iterator start_same_BLC;

  // If the source block of the BLC_indicator has new bottom states,
  // it is undefined whether the BLC_indicator should be regarded as stable or unstable.
  // Otherwise, the BLC_indicator is regarded as stable if and only if start_marked_BLC is ==nullptr.
  BLC_list_iterator_or_null start_marked_BLC;
  BLC_list_iterator end_same_BLC;

  BLC_indicators(BLC_list_iterator start, BLC_list_iterator end, bool is_stable)
   : start_same_BLC(start),
     start_marked_BLC(is_stable ? nullptr : end),
     end_same_BLC(end)
  {
    assert(nullptr!=start_same_BLC);
    assert(nullptr!=end_same_BLC);
  }

  bool is_stable() const
  {
    assert(nullptr!=start_same_BLC);
    assert(nullptr==start_marked_BLC || start_same_BLC<=start_marked_BLC);
    assert(nullptr==start_marked_BLC || start_marked_BLC<=end_same_BLC);
    assert(nullptr!=end_same_BLC);
    return nullptr==start_marked_BLC;
  }

  void make_stable()
  {
    assert(!is_stable());
    assert(nullptr!=start_same_BLC);
    assert(start_same_BLC<=start_marked_BLC);
    assert(start_marked_BLC<=end_same_BLC);
    assert(nullptr!=end_same_BLC);
    start_marked_BLC=nullptr;
  }

  void make_unstable()
  {
    assert(is_stable());
    assert(nullptr!=start_same_BLC);
    assert(nullptr!=end_same_BLC);
    start_marked_BLC=end_same_BLC;
  }

  #ifndef NDEBUG
    /// \brief print a B_to_C slice identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    template<class LTS_TYPE>
    std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        assert(&*partitioner.m_BLC_transitions.begin()<=start_same_BLC);
        assert(nullptr==start_marked_BLC || start_same_BLC<=start_marked_BLC);
        assert(nullptr==start_marked_BLC || start_marked_BLC<=end_same_BLC);
        assert(start_same_BLC<=end_same_BLC);
        assert(end_same_BLC<=&*partitioner.m_BLC_transitions.end());
        if (start_same_BLC==end_same_BLC)
        {
          return "Empty BLC slice at m_BLC_transitions["+std::to_string(std::distance<BLC_list_const_iterator>(&*partitioner.m_BLC_transitions.begin(), end_same_BLC))+"]";
        }
        std::string result("BLC slice ");
        result += partitioner.m_blocks[partitioner.m_states[partitioner.m_aut.get_transitions()[*start_same_BLC].from()].block].debug_id(partitioner);
        result += " -> ";
        result += partitioner.m_constellations[partitioner.m_blocks[partitioner.m_states[partitioner.m_aut.get_transitions()[*start_same_BLC].to()].block].constellation].debug_id(partitioner);
        result += " containing the ";
        if (std::distance(start_same_BLC, end_same_BLC)>1)
        {
            result+=std::to_string(std::distance(start_same_BLC, end_same_BLC));
            result += " transitions ";
        }
        else
        {
            result += "transition ";
        }
        BLC_list_const_iterator iter = start_same_BLC;
        if (start_marked_BLC == iter)
        {
            result += "| ";
        }
        result += partitioner.m_transitions[*iter].debug_id_short(partitioner);
        if (std::distance(start_same_BLC, end_same_BLC)>4)
        {
            ++iter;
            result += start_marked_BLC == iter ? " | " : ", ";
            result += partitioner.m_transitions[*iter].debug_id_short(partitioner);
            result += std::next(iter) == start_marked_BLC ? " | ..."
                      : (!is_stable() && start_marked_BLC>std::next(iter) && start_marked_BLC<=end_same_BLC-3 ? ", ..|.." : ", ...");
            iter = end_same_BLC-3;
        }
        while (++iter!=end_same_BLC)
        {
            result += start_marked_BLC == iter ? " | " : ", ";
            result += partitioner.m_transitions[*iter].debug_id_short(partitioner);
        }
        if (start_marked_BLC == iter)
        {
            result += " |";
        }
        return result;
    }

    mutable check_complexity::BLC_gj_counter_t work_counter;
  #endif
};

struct transition_type
{
  // The position of the transition type corresponds to m_aut.get_transitions().
  // std::size_t from, label, to are found in m_aut.get_transitions().
  linked_list<BLC_indicators>::iterator transitions_per_block_to_constellation;
  outgoing_transitions_it ref_outgoing_transitions;  // This refers to the position of this transition in m_outgoing_transitions.
                                                     // During initialisation m_outgoing_transitions contains the indices of this
                                                     // transition. After initialisation m_outgoing_transitions refers to the corresponding
                                                     // entry in m_BLC_transitions, of which the field transition contains the index
                                                     // of this transition.

  #ifndef NDEBUG
    /// \brief print a short transition identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    template<class LTS_TYPE>
    std::string debug_id_short(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        assert(&partitioner.m_transitions.front() <= this);
        assert(this <= &partitioner.m_transitions.back());
        const transition& t = partitioner.m_aut.get_transitions()[this - &partitioner.m_transitions.front()];
        return partitioner.m_states[t.from()].debug_id_short(partitioner) + " -" +
               pp(partitioner.m_aut.action_label(t.label())) + "-> " +
               partitioner.m_states[t.to()].debug_id_short(partitioner);
    }

    /// \brief print a transition identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    template<class LTS_TYPE>
    std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        return "transition " + debug_id_short(partitioner);
    }

    mutable check_complexity::trans_gj_counter_t work_counter;
  #endif
};

struct block_type
{
  constellation_index constellation : (sizeof(constellation_index) * CHAR_BIT - 1);
  unsigned contains_new_bottom_states : 1;
  fixed_vector<state_in_block_pointer>::iterator start_bottom_states;
  fixed_vector<state_in_block_pointer>::iterator start_non_bottom_states;
  fixed_vector<state_in_block_pointer>::iterator end_states;
// David thinks that end_states may perhaps be suppressed.
// We need the size of a block in two cases: to choose a small block in a constellation,
// and to decide whether to abort a coroutine early in simple_splitB().
// In both cases it is enough to get an upper bound on the size of the block,
// and if a constellation is a contiguous slice in m_states_in_blocks (as I
// suggested elsewhere), the constellation can provide this upper bound.
  linked_list< BLC_indicators > block_to_constellation;

  block_type(const fixed_vector<state_in_block_pointer>::iterator beginning_of_states, constellation_index c)
    : constellation(c),
      contains_new_bottom_states(false),
      start_bottom_states(beginning_of_states),
      start_non_bottom_states(beginning_of_states),
      end_states(beginning_of_states),
      block_to_constellation()
  {}

  #ifndef NDEBUG
    /// \brief print a block identification for debugging
    template<class LTS_TYPE>
    inline std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        assert(!partitioner.m_blocks.empty());
        assert(&partitioner.m_blocks.front() <= this);
        assert(this <= &partitioner.m_blocks.back());
        assert(partitioner.m_states_in_blocks.begin() <= start_bottom_states);
        assert(start_bottom_states <= start_non_bottom_states);
        assert(start_non_bottom_states <= end_states);
        assert(end_states <= partitioner.m_states_in_blocks.end());
        return "block [" + std::to_string(std::distance<fixed_vector<state_in_block_pointer>::const_iterator>(partitioner.m_states_in_blocks.begin(), start_bottom_states)) + "," + std::to_string(std::distance<fixed_vector<state_in_block_pointer>::const_iterator>(partitioner.m_states_in_blocks.begin(), end_states)) + ")"
                    " (#" + std::to_string(std::distance(&partitioner.m_blocks.front(), this)) + ")";
    }

    mutable check_complexity::block_gj_counter_t work_counter;
  #endif
};

struct constellation_type
{
  fixed_vector<state_in_block_pointer>::iterator start_const_states;
  fixed_vector<state_in_block_pointer>::iterator end_const_states;
  constellation_type(const fixed_vector<state_in_block_pointer>::iterator new_start, const fixed_vector<state_in_block_pointer>::iterator new_end)
    : start_const_states(new_start),
      end_const_states(new_end)
  {}

  #ifndef NDEBUG
    /// \brief print a constellation identification for debugging
    template<class LTS_TYPE>
    inline std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
    {
        assert(&partitioner.m_constellations.front() <= this);
        assert(this <= &partitioner.m_constellations.back());
        return "constellation " + std::to_string(this - &partitioner.m_constellations.front());
    }
  #endif
};

} // end namespace bisimulation_gj


/*=============================================================================
=                                 main class                                  =
=============================================================================*/


using namespace mcrl2::lts::detail::bisimulation_gj;

/// \class bisim_partitioner_gj
/// \brief implements the main algorithm for the branching bisimulation quotient
template <class LTS_TYPE>
class bisim_partitioner_gj
{
  protected:

    typedef std::unordered_set<state_index> set_of_states_type;
    typedef std::unordered_set<transition_index> set_of_transitions_type;
    typedef std::vector<constellation_index> set_of_constellations_type;

    #ifndef CO_SPLITTER_IN_BLC_LIST
      typedef std::unordered_map<std::pair<block_index, label_index>, transition_index> block_label_to_size_t_map;
    #endif

    #ifndef NDEBUG
      public: // needed for the debugging functions, e.g. debug_id().
    #endif
    /// \brief automaton that is being reduced
    LTS_TYPE& m_aut;

    // Generic data structures.
    fixed_vector<state_type_gj> m_states;
    // fixed_vector<transition_index> m_incoming_transitions;
    fixed_vector<outgoing_transition_type> m_outgoing_transitions;
                                                                  // During refining this contains the index in m_BLC_transition, of which
                                                                  // the transition field contains the index of the transition.
    fixed_vector<transition_type> m_transitions;
    fixed_vector<state_in_block_pointer> m_states_in_blocks;
    std::vector<block_type> m_blocks;
    std::vector<constellation_type> m_constellations;
    // David suggests to allocate blocks and constellations in global_linked_list_administration.
    // (There is a pool allocator that can be used like this in liblts_bisim_dnj.h.)
    // Then, one can store pointers to block_type and constellation_type instead of numbers;
    // the type checking of pointers is more strict than the type checking of integer types,
    // so it becomes impossible to assign a constellation number to a block or v.v.
    // Also, it will reduce the complexity of address calculations.
    fixed_vector<transition_index> m_BLC_transitions;
  protected:
    std::vector<block_index> m_blocks_with_new_bottom_states;
    // Below are the two vectors that contain the marked and unmarked states, which
    // are internally split in a part for states to be investigated, and a part for
    // states that belong definitively to this set.
    todo_state_vector m_R, m_U;
    std::vector<state_in_block_pointer> m_U_counter_reset_vector;
    // The following variable contains all non trivial constellations.
    set_of_constellations_type m_non_trivial_constellations;

    #ifdef CO_SPLITTER_IN_BLC_LIST
      std::vector<linked_list<BLC_indicators>::iterator> m_BLC_indicators_to_be_deleted;
    #endif
    // In m_co_splitters_to_be_checked we store pairs (transition, from_block).
    // The transition is in an unstable co-splitter that may have no corresponding
    // main splitter. In that case it should be made stable.
    std::vector<std::pair<BLC_list_iterator, block_index> > m_co_splitters_to_be_checked;

    /// \brief true iff branching (not strong) bisimulation has been requested
    const bool m_branching;

    /// \brief true iff divergence-preserving branching bisimulation has been
    /// requested
    /// \details Note that this field must be false if strong bisimulation has
    /// been requested.  There is no such thing as divergence-preserving strong
    /// bisimulation.
    const bool m_preserve_divergence;

    /// The auxiliary function below can be removed, but is now used to express that the hidden_label_map does not need
    /// to be applied, while still leaving it in the code.
    static typename LTS_TYPE::labels_size_type m_aut_apply_hidden_label_map(typename LTS_TYPE::labels_size_type l)
    {
      return l;
    }

    // The function assumes that m_branching is true and tests whether transition t is inert during initialisation under that condition
    bool is_inert_during_init_if_branching(const transition& t) const
    {
      assert(m_branching);
      return m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())) && (!m_preserve_divergence || t.from() != t.to());
    }

    // The function tests whether transition t is inert during initialisation, i.e. when there is only one source/target block.
    bool is_inert_during_init(const transition& t) const
    {
      return m_branching && is_inert_during_init_if_branching(t);
    }

    // The function calculates the label index of transition t, where tau-self-loops get the special index m_aut.num_action_labels() if divergence needs to be preserved
    label_index label_or_divergence(const transition& t) const
    {
      label_index result = m_aut_apply_hidden_label_map(t.label());
      if (m_preserve_divergence && (assert(m_branching), t.from() == t.to()) && m_aut.is_tau(result))
      {
        return m_aut.num_action_labels();
      }
      return result;
    }

    // This function returns true iff the BLC set ind contains at least one
    // marked transition.
    bool has_marked_transitions(const BLC_indicators& ind) const
    {
      if (ind.is_stable())
      {
        return false;
      }
      assert(ind.start_same_BLC<=ind.start_marked_BLC);
      assert(ind.start_marked_BLC<=ind.end_same_BLC);
      return ind.start_marked_BLC<ind.end_same_BLC;
    }

#ifndef NDEBUG
    // This suppresses many unused variable warnings.

    void check_transitions(const bool check_temporary_complexity_counters, const bool check_block_to_constellation = true) const
    {
      // This routine can only be used after initialisation.
      for(std::size_t ti=0; ti<m_transitions.size(); ++ti)
      {
        const BLC_list_const_iterator btc_ti = m_transitions[ti].ref_outgoing_transitions->ref_BLC_transitions;
        assert(*btc_ti==ti);

        const transition& t=m_aut.get_transitions()[ti];
        assert(&*m_states[t.to()].start_incoming_transitions <= &t);
        assert(&t <= &*std::prev(m_aut.get_transitions().end()));
        if (t.to() + 1 != m_states.size())
        {
          assert(&t <= &*std::prev(m_states[t.to() + 1].start_incoming_transitions));
        }

        assert(m_states[t.from()].start_outgoing_transitions <= m_transitions[ti].ref_outgoing_transitions);
        if (t.from() + 1 == m_states.size())
          assert(m_transitions[ti].ref_outgoing_transitions < m_outgoing_transitions.end());
        else
          assert(m_transitions[ti].ref_outgoing_transitions < m_states[t.from() + 1].start_outgoing_transitions);

        assert(m_transitions[ti].transitions_per_block_to_constellation->start_same_BLC <= btc_ti);
        assert(btc_ti<m_transitions[ti].transitions_per_block_to_constellation->end_same_BLC);

        if (!check_block_to_constellation)
          continue;

        const block_index b=m_states[t.from()].block;

        const label_index t_label = label_or_divergence(t);
        bool found=false;
        for(const BLC_indicators& blc: m_blocks[b].block_to_constellation)
        {
          if (!blc.is_stable())
          {
            assert(blc.start_same_BLC<=blc.start_marked_BLC);
            assert(blc.start_marked_BLC<=blc.end_same_BLC);
          }
          assert(blc.start_same_BLC<blc.end_same_BLC);
          transition& first_t = m_aut.get_transitions()[*blc.start_same_BLC];
          assert(b == m_states[first_t.from()].block);
          if (t_label == label_or_divergence(first_t) &&
              m_blocks[m_states[first_t.to()].block].constellation == m_blocks[m_states[t.to()].block].constellation)
          {
// if (found) { std::cerr << "Found multiple BLC sets with transitions (block " << b << " -" << m_aut.action_label(t.label()) << "-> constellation " << m_blocks[m_states[t.to()].block].constellation << ")\n"; }
            assert(!found);
            assert(blc.start_same_BLC <= btc_ti);
            assert(btc_ti<blc.end_same_BLC);
            assert(&blc == &*m_transitions[ti].transitions_per_block_to_constellation);
            found = true;
          }
        }
        assert(found);
        if (check_temporary_complexity_counters)
        {
          const block_index targetb = m_states[t.to()].block;
          const unsigned max_sourceB = check_complexity::log_n-check_complexity::ilog2(number_of_states_in_block(b));
          const unsigned max_targetC = check_complexity::log_n-check_complexity::ilog2(number_of_states_in_constellation(m_blocks[targetb].constellation));
          const unsigned max_targetB = check_complexity::log_n-check_complexity::ilog2(number_of_states_in_block(targetb));
          mCRL2complexity(&m_transitions[ti], no_temporary_work(max_sourceB, max_targetC, max_targetB,
                  0 == m_states[t.from()].no_of_outgoing_inert_transitions), *this);
        }
      }
    }

    [[nodiscard]]
    bool check_data_structures(const std::string& tag, const bool initialisation=false, const bool check_temporary_complexity_counters=true) const
    {
assert(!initialisation);
      mCRL2log(log::debug) << "Check data structures: " << tag << ".\n";
      assert(m_states.size()==m_aut.num_states());
      assert(m_outgoing_transitions.size()==m_aut.num_transitions());

      // Check that the elements in m_states are well formed.
      for (fixed_vector<state_type_gj>::iterator si=const_cast<fixed_vector<state_type_gj>&>(m_states).begin(); si<m_states.cend(); si++)
      {
        const state_type_gj& s=*si;

        assert(s.counter==undefined);
        assert(m_blocks[s.block].start_bottom_states<m_blocks[s.block].start_non_bottom_states);
        assert(m_blocks[s.block].start_non_bottom_states<=m_blocks[s.block].end_states);

        // In the following line we need that si is an iterator (not a const_iterator)
        assert(std::find(m_blocks[s.block].start_bottom_states, m_blocks[s.block].end_states,state_in_block_pointer(si))!=m_blocks[s.block].end_states);

        // The construction below is added to enable compilation on Windows. 
        const outgoing_transitions_const_it end_it1=std::next(si)>=m_states.end() ? m_outgoing_transitions.cend() : std::next(si)->start_outgoing_transitions;
        for(outgoing_transitions_const_it it=s.start_outgoing_transitions; it!=end_it1; ++it)
        {
          const transition& t=m_aut.get_transitions()
                                [/*initialisation ?it->transition :*/*it->ref_BLC_transitions];
// if (t.from() != si) { std::cerr << m_transitions[*it->ref_BLC_transitions].debug_id(*this) << " is an outgoing transition of state " << si << "!\n"; }
          assert(m_states.cbegin()+t.from()==si);
          assert(!initialisation /*|| m_transitions[it->transition].ref_outgoing_transitions==it*/);
          assert(/*initialisation ||*/ m_transitions[*it->ref_BLC_transitions].ref_outgoing_transitions==it);
          assert((it->start_same_saC>it && it->start_same_saC<m_outgoing_transitions.end() &&
                        ((it+1)->start_same_saC==it->start_same_saC || (it+1)->start_same_saC<=it)) ||
                 (it->start_same_saC<=it && (it+1==m_outgoing_transitions.end() || (it+1)->start_same_saC>it)));
// if (it->start_same_saC < it->start_same_saC->start_same_saC) { std::cerr << "Now checking transitions " << m_transitions[*it->start_same_saC->ref_BLC_transitions].debug_id_short(*this) << " ... " << m_transitions[*it->start_same_saC->start_same_saC->ref_BLC_transitions].debug_id_short(*this) << '\n'; }
          const label_index t_label = label_or_divergence(t);
          // The following for loop is only executed if it is the last transition in the saC-slice.
          for(outgoing_transitions_const_it itt=it->start_same_saC; itt<it->start_same_saC->start_same_saC; ++itt)
          {
            const transition& t1=m_aut.get_transitions()
                                 [/*initialisation?itt->transition:*/*itt->ref_BLC_transitions];
// if (t1.from()!=si) { assert(!initialisation); std::cerr << m_transitions[*itt->ref_BLC_transitions].debug_id(*this) << " does not start in state " << si << '\n'; }
            assert(m_states.cbegin()+t1.from()==si);
            assert(label_or_divergence(t1) == t_label);
            assert(m_blocks[m_states[t.to()].block].constellation==m_blocks[m_states[t1.to()].block].constellation);
          }
        }
        assert(s.ref_states_in_blocks->ref_state==si);

        // Check that for each state the outgoing transitions satisfy the following invariant.
        // First there are inert transitions. Then there are other transitions sorted per label
        // and constellation.
        std::unordered_set<std::pair<label_index, constellation_index> > constellations_seen;

        // The construction below is to enable translation on Windows. 
        const outgoing_transitions_const_it end_it2=std::next(si)>=m_states.end() ? m_outgoing_transitions.cend() : std::next(si)->start_outgoing_transitions;
        for(outgoing_transitions_const_it it=s.start_outgoing_transitions; it!=end_it2; ++it)
        {
          const transition& t=m_aut.get_transitions()[/*initialisation?it->transition:*/ *it->ref_BLC_transitions];
          const label_index label = label_or_divergence(t);
          // Check that if the target constellation, if not new, is equal to the target constellation of the previous outgoing transition.
          const constellation_index t_to_constellation = m_blocks[m_states[t.to()].block].constellation;
          if (constellations_seen.count(std::pair(label,t_to_constellation))>0)
          {
            assert(it!=s.start_outgoing_transitions);
            const transition& old_t=m_aut.get_transitions()[/*initialisation?std::prev(it)->transition:*/ *std::prev(it)->ref_BLC_transitions];
            assert(label_or_divergence(old_t) == label);
            assert(t_to_constellation==m_blocks[m_states[old_t.to()].block].constellation);
          }
          constellations_seen.emplace(label,t_to_constellation);
        }
      }
      // Check that the elements in m_transitions are well formed.
      if (!initialisation)
      {
        check_transitions(check_temporary_complexity_counters);
      }
      // Check that the elements in m_blocks are well formed.
      {
        set_of_transitions_type all_transitions;
        for(block_index bi=0; bi<m_blocks.size(); ++bi)
        {
          const block_type& b=m_blocks[bi];
          const constellation_type& c=m_constellations[b.constellation];
          assert(b.start_bottom_states<m_states_in_blocks.end());
          assert(b.start_non_bottom_states<=m_states_in_blocks.end());
          assert(b.start_non_bottom_states>=m_states_in_blocks.begin());

          assert(m_states_in_blocks.begin() <= c.start_const_states);
          assert(c.start_const_states <= b.start_bottom_states);
          assert(b.start_bottom_states < b.start_non_bottom_states);
          assert(b.start_non_bottom_states <= b.end_states);
          assert(b.end_states <= c.end_const_states);
          assert(c.end_const_states <= m_states_in_blocks.end());

          unsigned max_B = check_complexity::log_n-check_complexity::ilog2(number_of_states_in_block(bi));
          unsigned max_C = check_complexity::log_n-check_complexity::ilog2(number_of_states_in_constellation(b.constellation));
          for(fixed_vector<state_in_block_pointer>::const_iterator is=b.start_bottom_states;
                   is!=b.start_non_bottom_states; ++is)
          {
            assert(is->ref_state->block==bi);
            assert(is->ref_state->no_of_outgoing_inert_transitions==0);
            if (check_temporary_complexity_counters)
            {
              mCRL2complexity(is->ref_state, no_temporary_work(max_B, true), *this);
            }
          }
          for(fixed_vector<state_in_block_pointer>::const_iterator is=b.start_non_bottom_states;
                   is!=b.end_states; ++is)
          {
            assert(is->ref_state->block==bi);
            assert(is->ref_state->no_of_outgoing_inert_transitions>0);
            // Because there cannot be new bottom states among the non-bottom states,
            // we can always check the temporary work of non-bottom states:
            mCRL2complexity(is->ref_state, no_temporary_work(max_B, false), *this);
          }
          // Because a block has no temporary or new-bottom-state-related counters,
          // we can always check its temporary work:
          mCRL2complexity(&b, no_temporary_work(max_C, max_B), *this);

          assert(b.block_to_constellation.check_linked_list());
          for(linked_list< BLC_indicators >::iterator ind=b.block_to_constellation.begin();
                     ind!=b.block_to_constellation.end(); ++ind)
          {
            assert(ind->start_same_BLC<ind->end_same_BLC);
            const transition& first_transition=m_aut.get_transitions()[*(ind->start_same_BLC)];
            const label_index first_transition_label = label_or_divergence(first_transition);
            for(BLC_list_const_iterator i=ind->start_same_BLC; i<ind->end_same_BLC; ++i)
            {
              const transition& t=m_aut.get_transitions()[*i];
              assert(m_transitions[*i].transitions_per_block_to_constellation == ind);
              all_transitions.emplace(*i);
              assert(m_states[t.from()].block==bi);
              assert(m_blocks[m_states[t.to()].block].constellation==
                               m_blocks[m_states[first_transition.to()].block].constellation);
              assert(label_or_divergence(t) == first_transition_label);
              if (is_inert_during_init(t) && m_blocks[m_states[t.to()].block].constellation==b.constellation)
              {
                // The inert transitions should be in the first element of block_to_constellation:
                assert(b.block_to_constellation.begin()==ind);
              }
            }
            if (check_temporary_complexity_counters)
            {
              mCRL2complexity(ind, no_temporary_work(max_C, check_complexity::log_n-check_complexity::ilog2(number_of_states_in_constellation(m_blocks[m_states[first_transition.to()].block].constellation))), *this);
            }
          }
        }
        assert(initialisation || all_transitions.size()==m_transitions.size());
        // destruct all_transitions here
      }

      // TODO: Check that the elements in m_constellations are well formed.
      {
        std::unordered_set<block_index> all_blocks;
        for(constellation_index ci=0; ci<m_constellations.size(); ci++)
        {
          for (fixed_vector<state_in_block_pointer>::const_iterator constln_it=m_constellations[ci].start_const_states; constln_it<m_constellations[ci].end_const_states; )
          {
            const block_index bi=constln_it->ref_state->block;
            assert(bi<m_blocks.size());
            assert(all_blocks.emplace(bi).second);  // Block is not already present. Otherwise a block occurs in two constellations.
            constln_it = m_blocks[bi].end_states;
          }
        }
        assert(all_blocks.size()==m_blocks.size());
        // destruct all_blocks here
      }

      // Check that the states in m_states_in_blocks refer to with ref_states_in_block to the right position.
      // and that a state is correctly designated as a (non-)bottom state.
      for(fixed_vector<state_in_block_pointer>::const_iterator si=m_states_in_blocks.begin(); si!=m_states_in_blocks.end(); ++si)
      {
        assert(si==si->ref_state->ref_states_in_blocks);
      }

      // Check that the blocks in m_blocks_with_new_bottom_states are bottom states.
      for(const block_index bi: m_blocks_with_new_bottom_states)
      {
        assert(m_blocks[bi].contains_new_bottom_states);
      }

      // Check that the non-trivial constellations are non trivial.
      for(const constellation_index ci: m_non_trivial_constellations)
      {
        // There are at least two blocks in a non-trivial constellation.
        const block_index first_bi=m_constellations[ci].start_const_states->ref_state->block;
        const block_index last_bi=std::prev(m_constellations[ci].end_const_states)->ref_state->block;
        assert(first_bi != last_bi);
      }
      return true;
    }

    /// Checks the following invariant:
    ///     If a block has a constellation-non-inert transition, then every
    ///     bottom state has a constellation-non-inert transition with the same
    ///     label to the same target constellation.
    /// It is assumed that the BLC data structure is correct, so we conveniently
    /// use that to verify the invariant.
    ///
    /// The function can also check a partial invariant while stabilisation has
    /// not yet finished. If calM != nullptr, then we have:
    ///     The above invariant may be violated for BLC sets that are still to
    ///     be stabilized, as given by the main splitters in calM.
    ///     (calM_elt indicates how far stabilization has handled calM already.)
    ///     (block_label_to_cotransition indicates the co-splitters that belong
    ///     to the main splitters in calM.)
    ///     It may also be violated for blocks that contain new bottom states,
    ///     as indicated by m_blocks_with_new_bottom_states.
    ///
    /// Additionally, the function ensures that only transitions in BLC sets
    /// satisfying the above conditions are marked:
    ///     Transitions may only be marked in BLC sets that are still to be
    ///     stabilized, as given by calM (including co-splitters); they may
    ///     also be marked if they start in new bottom states, as indicated by
    ///     m_blocks_with_new_bottom_states, or if they start in a singleton
    ///     block.
    [[nodiscard]]
    bool check_stability(const std::string& tag,
                         const std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> >* calM = nullptr,
                         const std::pair<BLC_list_iterator, BLC_list_iterator>* calM_elt = nullptr,
                         #ifdef CO_SPLITTER_IN_BLC_LIST
                           const constellation_index old_constellation = null_constellation
                         #else
                           const block_label_to_size_t_map* const block_label_to_cotransition = nullptr
                         #endif
                         ) const
    {
      mCRL2log(log::debug) << "Check stability: " << tag << ".\n";
      for(block_index bi=0; bi<m_blocks.size(); ++bi)
      {
        const block_type& b=m_blocks[bi];
        for(linked_list< BLC_indicators >::iterator ind=b.block_to_constellation.begin();
                     ind!=b.block_to_constellation.end(); ++ind)
        {
          set_of_states_type all_source_bottom_states;

          assert(ind->start_same_BLC<ind->end_same_BLC);
          const transition& first_t = m_aut.get_transitions()[*ind->start_same_BLC];
          const label_index first_t_label = label_or_divergence(first_t);
          const bool all_transitions_in_BLC_are_inert = is_inert_during_init(first_t) &&
                                                        m_blocks[m_states[first_t.to()].block].constellation == b.constellation;
          assert(!all_transitions_in_BLC_are_inert || b.block_to_constellation.begin() == ind);
          for(BLC_list_const_iterator i=ind->start_same_BLC; i<ind->end_same_BLC; ++i)
          {
            assert(&*m_BLC_transitions.begin()<=i);
            assert(i<&*m_BLC_transitions.end());
            const transition& t=m_aut.get_transitions()[*i];
// if (m_states[t.from()].block != bi) { std::cerr << m_transitions[*ind->start_same_BLC].debug_id(*this) << " should start in block " << bi << '\n'; }
            assert(m_states[t.from()].block == bi);
            assert(label_or_divergence(t) == first_t_label);
            assert(m_blocks[m_states[t.to()].block].constellation == m_blocks[m_states[first_t.to()].block].constellation);
            if (is_inert_during_init(t) && m_blocks[m_states[t.to()].block].constellation==b.constellation)
            {
              assert(all_transitions_in_BLC_are_inert);
            }
            else
            {
              // This is a constellation-non-inert transition.
              assert(!all_transitions_in_BLC_are_inert);
              if (0 == m_states[t.from()].no_of_outgoing_inert_transitions)
              {
                assert(b.start_bottom_states <= m_states[t.from()].ref_states_in_blocks);
                assert(m_states[t.from()].ref_states_in_blocks < b.start_non_bottom_states);
                all_source_bottom_states.emplace(t.from());
              }
              else
              {
                assert(b.start_non_bottom_states <= m_states[t.from()].ref_states_in_blocks);
                assert(m_states[t.from()].ref_states_in_blocks < b.end_states);
              }
            }
          }
          assert(all_source_bottom_states.size() <= static_cast<std::size_t>(std::distance(b.start_bottom_states, b.start_non_bottom_states)));
          // check that every bottom state has a transition in this BLC entry:
          bool eventual_instability_is_ok = true;
          bool eventual_marking_is_ok = true;
          if (!all_transitions_in_BLC_are_inert &&
              all_source_bottom_states.size()!=static_cast<std::size_t>(std::distance(b.start_bottom_states, b.start_non_bottom_states)))
          {
            // only splitters should be instable.
            mCRL2log(log::debug) << "Not all " << std::distance(b.start_bottom_states, b.start_non_bottom_states)
                << (m_branching ? " bottom states have a transition in the " : " states have a transition in the ")
                << ind->debug_id(*this) << ": transitions found from states";
            for (set_of_states_type::iterator asbc_it = all_source_bottom_states.begin() ; asbc_it != all_source_bottom_states.end() ; ++asbc_it) { mCRL2log(log::debug) << ' ' << *asbc_it; }
            mCRL2log(log::debug) << '\n';
            eventual_instability_is_ok = false;
          }
          if (!ind->is_stable())
          {
            // only splitters should contain marked transitions.
            mCRL2log(log::debug) << ind->debug_id(*this) << " contains " << std::distance(ind->start_marked_BLC, ind->end_same_BLC) << " marked transitions.\n";
            eventual_marking_is_ok = false;
          }
          if (b.contains_new_bottom_states)
          {
            /* I would like the following to check more closely because in a
               block with new bottom states, one should have...
            if (!eventual_marking_is_ok)
            {
              eventual_marking_is_ok = true;
              for (BLC_list_const_iterator i=ind->start_marked_BLC; i<ind->end_same_BLC; ++i)
              {
                const state_index from = m_aut.get_transitions()[*i].from();
                // assert(m_states[from].block == bi); -- already checked earlier
                if (0 != m_states[from].no_of_outgoing_inert_transitions)
                {
                  // the state is a non-bottom state
                  eventual_marking_is_ok = false;
                  break;
                }
              }
              if (eventual_marking_is_ok)
              {
                mCRL2log(log::debug) << "  This is ok because all marked transitions begin in new bottom states of block " << bi << ".\n";
                eventual_instability_is_ok = true;
              }
            } */
            if (!(eventual_instability_is_ok && eventual_marking_is_ok))
            {
              mCRL2log(log::debug) << "  This is ok because block " << bi << " contains new bottom states.\n";
              eventual_instability_is_ok = true;
              eventual_marking_is_ok = true;
            }
          }
          if (!(eventual_instability_is_ok && eventual_marking_is_ok) && nullptr != calM && calM->begin() != calM->end())
          {
            std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> >::const_iterator calM_iter = calM->begin();
            if (nullptr != calM_elt)
            {
              for(;;)
              {
                assert(calM->end() != calM_iter);
                if (calM_iter->first <= calM_elt->first && calM_elt->second <= calM_iter->second)
                {
                  break;
                }
                ++calM_iter;
              }
              if (calM_elt->first<=ind->start_same_BLC && ind->end_same_BLC<=calM_elt->second)
              {
                mCRL2log(log::debug) << "  This is ok because the BLC set (block " << bi << " -" << m_aut.action_label(first_t.label()) << "-> constellation " << m_blocks[m_states[first_t.to()].block].constellation << ") is soon going to be a main splitter.\n";
                eventual_instability_is_ok = true;
                eventual_marking_is_ok = true;
              }
              else
              {
                #ifdef CO_SPLITTER_IN_BLC_LIST
                  if (old_constellation == m_blocks[m_states[first_t.to()].block].constellation)
                  {
                    const linked_list<BLC_indicators>::const_iterator main_splitter=b.block_to_constellation.next(ind);
                    if (main_splitter!=b.block_to_constellation.end())
                    {
                      assert(main_splitter->start_same_BLC < main_splitter->end_same_BLC);
                      const transition& main_t = m_aut.get_transitions()[*main_splitter->start_same_BLC];
                      assert(m_states[main_t.from()].block == bi);
                      if (label_or_divergence(first_t) == label_or_divergence(main_t) &&
                          m_blocks[m_states[main_t.to()].block].constellation == m_constellations.size() - 1)
                      {
//std::cerr << "Corresponding main splitter: " << main_splitter->debug_id(*this) << '\n';
                        if (calM_elt->first<=main_splitter->start_same_BLC && main_splitter->end_same_BLC<=calM_elt->second)
                        {
                          assert(m_constellations.size() - 1 == m_blocks[m_states[main_t.to()].block].constellation);
                          mCRL2log(log::debug) << "  This is ok because the BLC set (block " << bi << " -" << m_aut.action_label(first_t.label()) << "-> constellation " << old_constellation << ") is soon going to be a co-splitter.\n";
                          eventual_instability_is_ok = true;
                          eventual_marking_is_ok = true;
                        }
//else { std::cerr << "Main splitter is not in calM_elt = [" << std::distance(m_BLC_transitions.begin(), calM_elt->first) << ',' << std::distance(m_BLC_transitions.begin(), calM_elt->second) << ")\n" };
                      }
                    }
                  }
                #else
                  if (nullptr != block_label_to_cotransition)
                  {
                    for (BLC_list_const_iterator ind_iter = calM_elt->second; ind_iter > calM_elt->first; )
                    {
                      assert(m_BLC_transitions.begin()<ind_iter);
                      assert(m_transitions[*std::prev(ind_iter)].transitions_per_block_to_constellation->start_same_BLC<m_transitions[*std::prev(ind_iter)].transitions_per_block_to_constellation->end_same_BLC);
                      ind_iter = m_transitions[*std::prev(ind_iter)].transitions_per_block_to_constellation->start_same_BLC;
                      assert(ind_iter<m_BLC_transitions.end());
                      const transition& t = m_aut.get_transitions()[*ind_iter];
                      block_label_to_size_t_map::const_iterator co_iter = block_label_to_cotransition->find(std::pair(m_states[t.from()].block, label_or_divergence(t)));
                      if (block_label_to_cotransition->end() != co_iter && null_transition != co_iter->second && m_transitions[co_iter->second].transitions_per_block_to_constellation == ind)
                      {
                        const transition& co_t = m_aut.get_transitions()[co_iter->second];
                        mCRL2log(log::debug) << "  This is ok because the BLC set (block " << bi << " -" << m_aut.action_label(co_t.label()) << "-> constellation " << m_blocks[m_states[co_t.to()].block].constellation << ") is soon going to be a co-splitter.\n";
                        eventual_instability_is_ok = true;
                        eventual_marking_is_ok = true;
                        break;
                      }
                    }
                  }
                #endif
              }
              ++calM_iter;
            }
            for(; !(eventual_instability_is_ok && eventual_marking_is_ok) && calM->end() != calM_iter; ++calM_iter)
            {
              if (calM_iter->first<=ind->start_same_BLC && ind->end_same_BLC<=calM_iter->second)
              {
                mCRL2log(log::debug) << "  This is ok because the BLC set (block " << bi << " -" << m_aut.action_label(first_t.label()) << "-> constellation " << m_blocks[m_states[first_t.to()].block].constellation << ") is going to be a main splitter later.\n";
                eventual_instability_is_ok = true;
                eventual_marking_is_ok = true;
              }
              else
              {
                #ifdef CO_SPLITTER_IN_BLC_LIST
                  if (old_constellation == m_blocks[m_states[first_t.to()].block].constellation)
                  {
                    const linked_list<BLC_indicators>::const_iterator main_splitter=b.block_to_constellation.next(ind);
                    if (main_splitter != b.block_to_constellation.end())
                    {
                      assert(main_splitter->start_same_BLC < main_splitter->end_same_BLC);
                      const transition& main_t = m_aut.get_transitions()[*main_splitter->start_same_BLC];
                      assert(m_states[main_t.from()].block == bi);
                      if (label_or_divergence(first_t) == label_or_divergence(main_t) &&
                          m_blocks[m_states[main_t.to()].block].constellation == m_constellations.size() - 1)
                      {
                        if (calM_iter->first<=main_splitter->start_same_BLC && main_splitter->end_same_BLC<=calM_iter->second)
                        {
                          assert(m_constellations.size() - 1 == m_blocks[m_states[main_t.to()].block].constellation);
                          mCRL2log(log::debug) << "  This is ok because the BLC set (block " << bi << " -" << m_aut.action_label(first_t.label()) << "-> constellation " << old_constellation << ") is going to be a co-splitter later.\n";
                          eventual_instability_is_ok = true;
                          eventual_marking_is_ok = true;
                        }
//else { std::cerr << "Main splitter is not in calM_iter = [" << std::distance(m_BLC_transitions.begin(), calM_iter->first) << ',' << std::distance(m_BLC_transitions.begin(), calM_iter->second) << ")\n" };
                      }
                    }
                  }
                #else
                  if (nullptr != block_label_to_cotransition)
                  {
                    for (BLC_list_const_iterator ind_iter = calM_iter->second; ind_iter > calM_iter->first; )
                    {
                      assert(m_BLC_transitions.begin()<ind_iter);
                      assert(m_transitions[*std::prev(ind_iter)].transitions_per_block_to_constellation->start_same_BLC<m_transitions[*std::prev(ind_iter)].transitions_per_block_to_constellation->end_same_BLC);
                      ind_iter = m_transitions[*std::prev(ind_iter)].transitions_per_block_to_constellation->start_same_BLC;
                      assert(ind_iter<m_BLC_transitions.end());
                      const transition& t = m_aut.get_transitions()[*ind_iter];
                      block_label_to_size_t_map::const_iterator co_iter = block_label_to_cotransition->find(std::pair(m_states[t.from()].block, label_or_divergence(t)));
                      if (block_label_to_cotransition->end() != co_iter && null_transition != co_iter->second && m_transitions[co_iter->second].transitions_per_block_to_constellation == ind)
                      {
                        const transition& co_t = m_aut.get_transitions()[co_iter->second];
                        mCRL2log(log::debug) << "  This is ok because the BLC set (block " << bi << " -" << m_aut.action_label(co_t.label()) << "-> constellation " << m_blocks[m_states[co_t.to()].block].constellation << ") is going to be a co-splitter later.\n";
                        eventual_instability_is_ok = true;
                        eventual_marking_is_ok = true;
                        break;
                      }
                    }
                  }
                #endif
              }
            }
          }
          #ifndef CO_SPLITTER_IN_BLC_LIST
            if (!(eventual_instability_is_ok && eventual_marking_is_ok) && nullptr != block_label_to_cotransition)
            {
              block_label_to_size_t_map::const_iterator co_iter = block_label_to_cotransition->find(std::pair(m_states[first_t.from()].block, label_or_divergence(first_t)));
              if (block_label_to_cotransition->end() != co_iter && null_transition != co_iter->second && m_transitions[co_iter->second].transitions_per_block_to_constellation == ind)
              {
                mCRL2log(log::debug) << "  (This BLC set is registered as co-splitter but there is no corresponding main splitter.)\n";
              }
            }
          #endif
          if (!eventual_marking_is_ok && 1 >= number_of_states_in_block(bi))
          {
            mCRL2log(log::debug) << "  (This is ok because the source block contains only 1 state.)\n";
            eventual_marking_is_ok = true;
          }
          assert(eventual_marking_is_ok);
          assert(eventual_instability_is_ok);
        }
      }
      mCRL2log(log::debug) << "Check stability finished: " << tag << ".\n";
      return true;
    }

    void display_BLC_list(const block_index bi
            #ifndef CO_SPLITTER_IN_BLC_LIST
              , const block_label_to_size_t_map* const block_label_to_cotransition = nullptr
            #endif
            ) const
    {
      mCRL2log(log::debug) << "\n  BLC_List\n";
      for(const BLC_indicators& blc_it: m_blocks[bi].block_to_constellation)
      {
        mCRL2log(log::debug) << "\n    BLC_sublist:  " << std::distance<BLC_list_const_iterator>(&*m_BLC_transitions.begin(),blc_it.start_same_BLC) << " -- "
                             << std::distance<BLC_list_const_iterator>(&*m_BLC_transitions.begin(), blc_it.end_same_BLC) << "\n";
        for (BLC_list_const_iterator i=blc_it.start_same_BLC; ; ++i)
        {
          if (i == blc_it.start_marked_BLC)
          {
            mCRL2log(log::debug) << "        (The BLC set is unstable, and the following transitions are marked.)\n";
          }
          if (i>=blc_it.end_same_BLC)
          {
            break;
          }
          const transition& t=m_aut.get_transitions()[*i];
          mCRL2log(log::debug) << "        " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to();
          if (is_inert_during_init(t) && m_states[t.from()].block == m_states[t.to()].block)
          {
            mCRL2log(log::debug) << " (block-inert)";
          }
          else if (is_inert_during_init(t) && m_blocks[m_states[t.from()].block].constellation == m_blocks[m_states[t.to()].block].constellation)
          {
            mCRL2log(log::debug) << " (constellation-inert)";
          }
          else if (m_preserve_divergence && t.from() == t.to() && m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())))
          {
            mCRL2log(log::debug) << " (divergent self-loop)";
          }
          else
          {
            mCRL2log(log::debug) << " (to constellation " << m_blocks[m_states[t.to()].block].constellation << ')';
          }
          #ifndef CO_SPLITTER_IN_BLC_LIST
            if (nullptr != block_label_to_cotransition)
            {
              const block_label_to_size_t_map::const_iterator co_tr = block_label_to_cotransition->find(std::pair(bi, label_or_divergence(t)));
              if (co_tr != block_label_to_cotransition->end() && co_tr->second == *i)
              {
                mCRL2log(log::debug) << " (co-splitter transition for <block " << bi << ", label " << m_aut.action_label(t.label()) << ">)";
              }
            }
          #endif
          mCRL2log(log::debug) << '\n';
        }
      }
      mCRL2log(log::debug) << "  BLC_List end\n";
    }


    void print_data_structures(const std::string& header,
                               #ifndef CO_SPLITTER_IN_BLC_LIST
                                 const block_label_to_size_t_map* const block_label_to_cotransition = nullptr,
                               #endif
                               const bool initialisation=false) const
    {
      assert(!initialisation);
      if (!mCRL2logEnabled(log::debug))  return;
      mCRL2log(log::debug) << "========= PRINT DATASTRUCTURE: " << header << " =======================================\n";
      mCRL2log(log::debug) << "++++++++++++++++++++  States     ++++++++++++++++++++++++++++\n";
      for(state_index si=0; si<m_states.size(); ++si)
      {
        mCRL2log(log::debug) << "State " << si <<" (Block: " << m_states[si].block <<"):\n";
        mCRL2log(log::debug) << "  #Inert outgoing transitions: " << m_states[si].no_of_outgoing_inert_transitions << '\n';

        mCRL2log(log::debug) << "  Incoming transitions:\n";
        std::vector<transition>::const_iterator end=(si+1==m_states.size()?m_aut.get_transitions().end():m_states[si+1].start_incoming_transitions);
        for(std::vector<transition>::const_iterator it=m_states[si].start_incoming_transitions; it!=end; ++it)
        {
           mCRL2log(log::debug) << "  " << ptr(*it) << "\n";
        }

        mCRL2log(log::debug) << "  Outgoing transitions:\n";
        for(outgoing_transitions_const_it it=m_states[si].start_outgoing_transitions;
                        it!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_outgoing_transitions);
                     ++it)
        {
           const transition& t=m_aut.get_transitions()[/*initialisation?it->transition:*/ *it->ref_BLC_transitions];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";;
        }
        mCRL2log(log::debug) << "  Ref states in blocks: " << std::distance<fixed_vector<state_type_gj>::const_iterator>(m_states.cbegin(), m_states[si].ref_states_in_blocks->ref_state) << ". Must be " << si <<".\n";
        mCRL2log(log::debug) << "---------------------------------------------------\n";
      }
      mCRL2log(log::debug) << "++++++++++++++++++++ Transitions ++++++++++++++++++++++++++++\n";
      for(state_index ti=0; ti<m_transitions.size(); ++ti)
      {
        const transition& t=m_aut.get_transitions()[ti];
        mCRL2log(log::debug) << "Transition " << ti <<": " << t.from()
                                              << " -" << m_aut.action_label(t.label()) << "-> "
                                              << t.to() << "\n";
      }

      mCRL2log(log::debug) << "++++++++++++++++++++ Blocks ++++++++++++++++++++++++++++\n";
      for(block_index bi=0; bi<m_blocks.size(); ++bi)
      {
        mCRL2log(log::debug) << "  Block " << bi << " (const: " << m_blocks[bi].constellation
                             << (m_branching ? "):\n  Bottom states: " : "):\n  States: ");
        for(fixed_vector<state_in_block_pointer>::const_iterator sit=m_blocks[bi].start_bottom_states;
                        sit!=m_blocks[bi].start_non_bottom_states; ++sit)
        {
          mCRL2log(log::debug) << std::distance<fixed_vector<state_type_gj>::const_iterator>(m_states.cbegin(), sit->ref_state) << "  ";
        }
        if (m_branching)
        {
          mCRL2log(log::debug) << "\n  Non-bottom states: ";
          for(fixed_vector<state_in_block_pointer>::const_iterator sit=m_blocks[bi].start_non_bottom_states;
                                 sit!=m_blocks[bi].end_states; ++sit)
          {
            mCRL2log(log::debug) << std::distance<fixed_vector<state_type_gj>::const_iterator>(m_states.cbegin(), sit->ref_state) << "  ";
          }
        }
        else
        {
          assert(m_blocks[bi].start_non_bottom_states == m_blocks[bi].end_states);
        }
        if (!initialisation)
        {
          display_BLC_list(bi
                    #ifndef CO_SPLITTER_IN_BLC_LIST
                      , block_label_to_cotransition
                    #endif
                    );
        }
        mCRL2log(log::debug) << "\n";
      }

      mCRL2log(log::debug) << "++++++++++++++++++++ Constellations ++++++++++++++++++++++++++++\n";
      for(constellation_index ci=0; ci<m_constellations.size(); ++ci)
      {
        mCRL2log(log::debug) << "  Constellation " << ci << ":\n";
        mCRL2log(log::debug) << "    Blocks in constellation:";
        for (fixed_vector<state_in_block_pointer>::const_iterator constln_it=m_constellations[ci].start_const_states; constln_it<m_constellations[ci].end_const_states; )
        {
          const block_index bi=constln_it->ref_state->block;
          mCRL2log(log::debug) << " " << bi;
          constln_it = m_blocks[bi].end_states;
        }
        mCRL2log(log::debug) << "\n";
      }
      mCRL2log(log::debug) << "Non-trivial constellations:";
      for(const constellation_index ci: m_non_trivial_constellations)
      {
        mCRL2log(log::debug) << " " << ci;
      }

      mCRL2log(log::debug) << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      mCRL2log(log::debug) << "Outgoing transitions:\n";

      for(outgoing_transitions_const_it pi = m_outgoing_transitions.begin(); pi < m_outgoing_transitions.end(); ++pi)
      {
        const transition& t=m_aut.get_transitions()[/*initialisation?pi->transition:*/ *pi->ref_BLC_transitions];
        mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to();
        if (m_outgoing_transitions.begin() <= pi->start_same_saC && pi->start_same_saC < m_outgoing_transitions.end())
        {
          const transition& t1=m_aut.get_transitions()[/*initialisation?pi->start_same_saC->transition:*/ *pi->start_same_saC->ref_BLC_transitions];
          mCRL2log(log::debug) << "  \t(same saC: " << t1.from() << " -" << m_aut.action_label(t1.label()) << "-> " << t1.to();
          const label_index t_label = label_or_divergence(t);
          if (pi->start_same_saC->start_same_saC == pi)
          {
            // Transition t must be the beginning and/or the end of a saC-slice
            if (pi->start_same_saC >= pi && pi > m_outgoing_transitions.begin())
            {
              // Transition t must be the beginning of a saC-slice
              const transition& prev_t = m_aut.get_transitions()[/*initialisation ? std::prev(pi)->transition :*/ *std::prev(pi)->ref_BLC_transitions];
              if (prev_t.from() == t.from() &&
                  label_or_divergence(prev_t) == t_label &&
                  m_blocks[m_states[prev_t.to()].block].constellation == m_blocks[m_states[t.to()].block].constellation)
              {
                mCRL2log(log::debug) << " -- error: not the beginning of a saC-slice";
              }
            }
            if (pi->start_same_saC <= pi && std::next(pi) < m_outgoing_transitions.end())
            {
              // Transition t must be the end of a saC-slice
              const transition& next_t = m_aut.get_transitions()[/*initialisation ? std::next(pi)->transition :*/ *std::next(pi)->ref_BLC_transitions];
              if (next_t.from() == t.from() &&
                  label_or_divergence(next_t) == t_label &&
                  m_blocks[m_states[next_t.to()].block].constellation == m_blocks[m_states[t.to()].block].constellation)
              {
                mCRL2log(log::debug) << " -- error: not the end of a saC-slice";
              }
            }
          }
          else if (pi->start_same_saC > pi ? pi->start_same_saC->start_same_saC > pi : pi->start_same_saC->start_same_saC < pi)
          {
            mCRL2log(log::debug) << " -- error: not in its own saC-slice";
          }
          mCRL2log(log::debug) << ')';
        }
        mCRL2log(log::debug) << '\n';
      }

      mCRL2log(log::debug) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      mCRL2log(log::debug) << "New bottom blocks to be investigated:";

      for(block_index bi: m_blocks_with_new_bottom_states)
      {
        mCRL2log(log::debug) << " " << bi;
      }

      mCRL2log(log::debug) << "\n========= END PRINT DATASTRUCTURE: " << header << " =======================================\n";
    }
#endif // ifndef NDEBUG

  public:
    /// \brief constructor
    /// \details The constructor constructs the data structures and immediately
    /// calculates the partition corresponding with the bisimulation quotient.
    /// It destroys the transitions on the LTS (to save memory) but does not
    /// adapt the LTS to represent the quotient's transitions.
    /// It is assumed that there are no tau-loops in aut.
    /// \param aut                 LTS that needs to be reduced
    /// \param branching           If true branching bisimulation is used,
    ///                                otherwise strong bisimulation is
    ///                                applied.
    /// \param preserve_divergence If true and branching is true, preserve
    ///                                tau loops on states.
    bisim_partitioner_gj(LTS_TYPE& aut,
                         const bool branching = false,
                         const bool preserve_divergence = false)
      : m_aut(aut),
        m_states(aut.num_states()),
        m_outgoing_transitions(aut.num_transitions()),
        m_transitions(aut.num_transitions()),
        m_states_in_blocks(aut.num_states()),
        m_blocks(1,{m_states_in_blocks.begin(),0}),
        m_constellations(1,constellation_type(m_states_in_blocks.begin(), m_states_in_blocks.end())),   // Algorithm 1, line 1.2.
        m_BLC_transitions(aut.num_transitions()),
        m_branching(branching),
        m_preserve_divergence(preserve_divergence)
    {
//log::logger::set_reporting_level(log::debug);

      assert(m_branching || !m_preserve_divergence);
      mCRL2log(log::verbose) << "Start initialisation.\n";
      create_initial_partition();
      mCRL2log(log::verbose) << "After initialisation there are " << m_blocks.size() << " equivalence classes. Start refining. \n";
      refine_partition_until_it_becomes_stable();
      assert(check_data_structures("READY"));
    }


    /// \brief Calculate the number of equivalence classes
    /// \details The number of equivalence classes (which is valid after the
    /// partition has been constructed) is equal to the number of states in the
    /// bisimulation quotient.
    std::size_t num_eq_classes() const
    {
      return m_blocks.size();
    }


    /// \brief Get the equivalence class of a state
    /// \details After running the minimisation algorithm, this function
    /// produces the number of the equivalence class of a state.  This number
    /// is the same as the number of the state in the minimised LTS to which
    /// the original state is mapped.
    /// \param s state whose equivalence class needs to be found
    /// \returns sequence number of the equivalence class of state s
    state_index get_eq_class(const state_index si) const
    {
      assert(si<m_states.size());
      return m_states[si].block;
    }


    /// \brief Adapt the LTS after minimisation
    /// \details After the efficient branching bisimulation minimisation, the
    /// information about the quotient LTS is only stored in the partition data
    /// structure of the partitioner object.  This function exports the
    /// information back to the LTS by adapting its states and transitions:  it
    /// updates the number of states and adds those transitions that are
    /// mandated by the partition data structure.  If desired, it also creates
    /// a vector containing an arbritrary (example) original state per
    /// equivalence class.
    ///
    /// The main parameter and return value are implicit with this function: a
    /// reference to the LTS was stored in the object by the constructor.
    void finalize_minimized_LTS()
    {
      // The transitions are most efficiently directly extracted from the block_to_constellation lists in blocks.
      std::vector<transition> T;
      for(block_index bi=0; bi<m_blocks.size(); ++bi)
      {
        const block_type& B=m_blocks[bi];
        //mCRL2complexity(&B, add_work(..., 1), *this);
            // Because every block is touched exactly once, we do not store a physical counter for this.
        for(const BLC_indicators blc_ind: B.block_to_constellation)
        {
          // mCRL2complexity(&blc_ind, add_work(..., 1), *this);
              // Because every BLC set is touched exactly once, we do not store a physical counter for this.
          assert(blc_ind.start_same_BLC<blc_ind.end_same_BLC);
          const transition& t= m_aut.get_transitions()[*blc_ind.start_same_BLC];
          const transition_index new_to=get_eq_class(t.to());
          if (!is_inert_during_init(t) || bi!=new_to)
          {
            T.emplace_back(bi, t.label(), new_to);
          }
        }
      }
      m_aut.clear_transitions();
      for (const transition& t: T)
      {
        //mCRL2complexity(..., add_work(..., 1), *this);
            // we do not add a counter because every transition has been generated by one of the above iterations.
        m_aut.add_transition(t);
      }
      //
      // Merge the states, by setting the state labels of each state to the
      // concatenation of the state labels of its equivalence class.

      if (m_aut.has_state_info())   /* If there are no state labels this step is not needed */
      {
        /* Create a vector for the new labels */
        std::vector<typename LTS_TYPE::state_label_t> new_labels(num_eq_classes());


        for(std::size_t i=0; i<m_aut.num_states(); ++i)
        {
          //mCRL2complexity(&m_states[i], add_work(..., 1), *this);
              // Because every state is touched exactly once, we do not store a physical counter for this.
          const state_index new_index(get_eq_class(i));
          new_labels[new_index]=new_labels[new_index]+m_aut.state_label(i);
        }

        m_aut.set_num_states(num_eq_classes());
        for (std::size_t i=0; i<num_eq_classes(); ++i)
        {
          // mCRL2complexity(&m_blocks[i], add_work(check_complexity::finalize_minimized_LTS__set_labels_of_block, 1), *this);
              // Because every block is touched exactly once, we do not store a physical counter for this.
          m_aut.set_state_label(i, new_labels[i]);
        }
      }
      else
      {
        m_aut.set_num_states(num_eq_classes());
      }

      m_aut.set_initial_state(get_eq_class(m_aut.initial_state()));
    }


    /// \brief Check whether two states are in the same equivalence class.
    /// \param s first state that needs to be compared.
    /// \param t second state that needs to be compared.
    /// \returns true iff the two states are in the same equivalence class.
    bool in_same_class(state_index const s, state_index const t) const
    {
      return get_eq_class(s) == get_eq_class(t);
    }
  protected:

    std::string ptr(const transition& t) const
    {
      return std::to_string(t.from()) + " -" + pp(m_aut.action_label(t.label())) + "-> " + std::to_string(t.to());
    }

    std::string ptr(const transition_index ti) const
    {
      const transition& t=m_aut.get_transitions()[ti];
      return ptr(t);
    }

    /*--------------------------- main algorithm ----------------------------*/

    /*----------------- splitB -- Algorithm 3 of [GJ 2024] -----------------*/

    state_index number_of_states_in_block(const block_index B) const
    {
      assert(m_blocks[B].start_bottom_states < m_blocks[B].end_states);
      return std::distance(m_blocks[B].start_bottom_states, m_blocks[B].end_states);
    }

    state_index number_of_states_in_constellation(const constellation_index C) const
    {
      assert(m_constellations[C].start_const_states < m_constellations[C].end_const_states);
      return std::distance(m_constellations[C].start_const_states, m_constellations[C].end_const_states);
    }

    void swap_states_in_states_in_block_never_equal(
              fixed_vector<state_in_block_pointer>::iterator pos1,
              fixed_vector<state_in_block_pointer>::iterator pos2)
    {
      assert(m_states_in_blocks.begin() <= pos1);
      assert(pos1 < m_states_in_blocks.end());
      assert(m_states_in_blocks.begin() <= pos2);
      assert(pos2 < m_states_in_blocks.end());
      assert(pos1!=pos2);
      std::swap(*pos1,*pos2);
      pos1->ref_state->ref_states_in_blocks=pos1;
      pos2->ref_state->ref_states_in_blocks=pos2;
    }

    void swap_states_in_states_in_block(
              fixed_vector<state_in_block_pointer>::iterator pos1,
              fixed_vector<state_in_block_pointer>::iterator pos2)
    {
      if (pos1!=pos2)
      {
        swap_states_in_states_in_block_never_equal(pos1, pos2);
      }
    }

    // Move pos1 to pos2, pos2 to pos3 and pos3 to pos1;
    // The function requires that pos3 lies in between pos1 and pos2.
    // It also requires that pos2 and pos3 are different.
    void swap_states_in_states_in_block_23_never_equal(
              fixed_vector<state_in_block_pointer>::iterator pos1,
              fixed_vector<state_in_block_pointer>::iterator pos2,
              fixed_vector<state_in_block_pointer>::iterator pos3)
    {
      assert(m_states_in_blocks.begin()<=pos2);
      assert(pos2<pos3); assert(pos3<=pos1);
      assert(pos1<m_states_in_blocks.end());
      if (pos1==pos3)
      {
        std::swap(*pos1,*pos2);
      }
      else
      {
        const state_in_block_pointer temp=*pos1;
        *pos1=*pos3;
        *pos3=*pos2;
        *pos2=temp;

        pos3->ref_state->ref_states_in_blocks=pos3;
      }
      pos1->ref_state->ref_states_in_blocks=pos1;
      pos2->ref_state->ref_states_in_blocks=pos2;
    }

    // Move pos1 to pos2, pos2 to pos3 and pos3 to pos1;
    // The function requires that pos3 lies in between pos1 and pos2.
    void swap_states_in_states_in_block(
              fixed_vector<state_in_block_pointer>::iterator pos1,
              fixed_vector<state_in_block_pointer>::iterator pos2,
              fixed_vector<state_in_block_pointer>::iterator pos3)
    {
      if (pos2==pos3)
      {
        swap_states_in_states_in_block(pos1,pos2);
      }
      else
      {
        swap_states_in_states_in_block_23_never_equal(pos1,pos2,pos3);
      }
/*    assert(m_states_in_blocks.begin() <= pos2);
      assert(pos2 <= pos3); assert(pos3 <= pos1);
      assert(pos1 < m_states_in_blocks.end());
      if (pos1 == pos3)
      {
        if (pos1 == pos2)
        {
          return;
        }
        std::swap(*pos1,*pos2);
      }
      else if (pos2 == pos3)
      {
        std::swap(*pos1,*pos2);
      }
      else
      {
        const state_index temp=*pos1;
        *pos1=*pos3;
        *pos3=*pos2;
        *pos2=temp;

        m_states[*pos3].ref_states_in_blocks=pos3;
      }
      m_states[*pos1].ref_states_in_blocks=pos1;
      m_states[*pos2].ref_states_in_blocks=pos2; */
    }

    // Swap the range [pos1, pos1 + count) with the range [pos2, pos2 + count).
    // pos1 must come before pos2.
    // (If the ranges overlap, only swap the non-overlapping part.)
    // The function requires count > 0 and pos1 < pos2
    // (this is sufficient for how it's used below: to swap new bottom states into their proper places;
    // also, the work counters assume that [pos2, pos2 + count) is assigned the work.)
    void multiple_swap_states_in_states_in_block(
              fixed_vector<state_in_block_pointer>::iterator pos1,
              fixed_vector<state_in_block_pointer>::iterator pos2,
              state_index count
              #ifndef NDEBUG
                , const state_index max_B
              #endif
              )
    {
      assert(count <= m_states_in_blocks.size());
      // if (pos1 > pos2)  std::swap(pos1, pos2);
      assert(m_states_in_blocks.begin() <= pos1);
      assert(pos1 < pos2); // in particular, they are not allowed to be equal
      assert(pos2 <= m_states_in_blocks.end() - count);
      {
        std::make_signed<state_index>::type overlap = std::distance(pos2, pos1 + count);
        if (overlap > 0)
        {
          count -= overlap;
          pos2 += overlap;
        }
      }
      assert(0 < count);
      state_in_block_pointer temp=*pos1;
      while (--count > 0)
      {
        mCRL2complexity(pos2->ref_state, add_work(check_complexity::multiple_swap_states_in_block__swap_state_in_small_block, max_B), *this);
        *pos1 = *pos2;
        pos1->ref_state->ref_states_in_blocks=pos1;
        ++pos1;
        *pos2 = *pos1;
        pos2->ref_state->ref_states_in_blocks=pos2;
        ++pos2;
      }
      *pos1 = *pos2;
      pos1->ref_state->ref_states_in_blocks=pos1;
      *pos2 = temp;
      pos2->ref_state->ref_states_in_blocks=pos2;

      #ifndef NDEBUG
        for (fixed_vector<state_type_gj>::const_iterator si = m_states.cbegin(); si < m_states.cend(); ++si)
        {
          assert(si==si->ref_states_in_blocks->ref_state);
        }
      #endif
    }

    // marks the transition indicated by out_pos.
    // (We use an outgoing_transitions_it because it points to the m_BLC_transitions entry that needs to be updated.)
    void mark_BLC_transition(const outgoing_transitions_it out_pos)
    {
      BLC_list_iterator old_pos = out_pos->ref_BLC_transitions;
      linked_list<BLC_indicators>::iterator ind = m_transitions[*old_pos].transitions_per_block_to_constellation;
      assert(ind->start_same_BLC <= old_pos);
      assert(old_pos<&*m_BLC_transitions.end());
      assert(old_pos<ind->end_same_BLC);
      assert(!ind->is_stable());
      if (old_pos < ind->start_marked_BLC)
      {
        // The transition is not marked
        assert(ind->start_same_BLC < ind->start_marked_BLC);
        BLC_list_iterator new_pos = std::prev(ind->start_marked_BLC);
        assert(ind->start_same_BLC <= new_pos);
        assert(new_pos<&*m_BLC_transitions.end());
        assert(new_pos<ind->end_same_BLC);
        if (old_pos < new_pos)
        {
          std::swap(*old_pos, *new_pos);
          m_transitions[*old_pos].ref_outgoing_transitions->ref_BLC_transitions = old_pos;
          assert(out_pos == m_transitions[*new_pos].ref_outgoing_transitions);
          out_pos->ref_BLC_transitions = new_pos;
        }
        ind->start_marked_BLC--;
      }

      #ifndef NDEBUG
        for (BLC_list_const_iterator it=&*m_BLC_transitions.cbegin(); it<&*m_BLC_transitions.cend(); ++it)
        {
          assert(m_transitions[*it].ref_outgoing_transitions->ref_BLC_transitions == it);
          assert(m_transitions[*it].transitions_per_block_to_constellation->start_same_BLC <= it);
          assert(it<m_transitions[*it].transitions_per_block_to_constellation->end_same_BLC);
        }
      #endif
    }

    // Split the block B by moving the elements in R to the front in m_states, and add a
    // new element B_new at the end of m_blocks referring to R. Adapt B.start_bottom_states,
    // B.start_non_bottom_states and B.end_states, and do the same for B_new.
    block_index split_block_B_into_R_and_BminR(
                     const block_index B,
                     fixed_vector<state_in_block_pointer>::iterator first_bottom_state_in_R,
                     fixed_vector<state_in_block_pointer>::iterator last_bottom_state_in_R,
                     const todo_state_vector& R
                     #ifdef TRY_EFFICIENT_SWAP
                       , const transition_index marking_value
                     #endif
                     )
    {
//std::cerr << "block_index split_block_B_into_R_and_BminR(" << m_blocks[B].debug_id(*this)
//<< ",&m_states_in_blocks[" << std::distance(m_states_in_blocks.begin(), first_bottom_state_in_R)
//<< "..." << std::distance(m_states_in_blocks.begin(), last_bottom_state_in_R)
//<< "),R = {";
//for(auto s:R){ std::cerr << ' ' << std::distance(m_states.begin(), s.ref_state); }
//std::cerr << " }," << static_cast<std::make_signed<transition_index>::type>(marking_value) << ")\n";
//std::cerr << "SPLIT BLOCK " << B << " by removing"; for(auto s = first_bottom_state_in_R; s < last_bottom_state_in_R; ++s){ std::cerr << ' ' << std::distance(m_states.begin(), s->ref_state);} for(auto s:R){ std::cerr << ' ' << std::distance(m_states.begin(), s.ref_state); } std::cerr << '\n';
      assert(m_blocks[B].start_bottom_states <= first_bottom_state_in_R);
      assert(first_bottom_state_in_R <= last_bottom_state_in_R);
      assert(last_bottom_state_in_R <= m_blocks[B].start_non_bottom_states);
      // Basic administration. Make a new block and add it to the current constellation.
      const block_index B_new=m_blocks.size();
      const constellation_index ci = m_blocks[B].constellation;
      m_blocks.emplace_back(m_blocks[B].start_bottom_states,ci);
      #ifndef NDEBUG
        m_blocks[B_new].work_counter = m_blocks[B].work_counter;
      #endif
      if (m_constellations[ci].start_const_states->ref_state->block==std::prev(m_constellations[ci].end_const_states)->ref_state->block) // This constellation is trivial.
      {
        // This constellation is trivial, as it will be split add it to the non trivial constellations.
        assert(std::find(m_non_trivial_constellations.begin(),
                         m_non_trivial_constellations.end(),
                         ci)==m_non_trivial_constellations.end());
        m_non_trivial_constellations.emplace_back(ci);
      }

      // Carry out the split.
      #ifndef NDEBUG
        // The size of the new block is not yet fixed.
        const state_index max_B = check_complexity::log_n - check_complexity::ilog2(std::distance(first_bottom_state_in_R, last_bottom_state_in_R) + R.size());
      #endif
      if (m_blocks[B].start_bottom_states < first_bottom_state_in_R)
      {
        multiple_swap_states_in_states_in_block(m_blocks[B].start_bottom_states, first_bottom_state_in_R,
                    std::distance(first_bottom_state_in_R, last_bottom_state_in_R)
                    #ifndef NDEBUG
                      , max_B
                    #endif
                    );
        last_bottom_state_in_R = m_blocks[B].start_bottom_states + std::distance(first_bottom_state_in_R, last_bottom_state_in_R);
        first_bottom_state_in_R = m_blocks[B].start_bottom_states;
      }

      assert(m_blocks[B_new].start_bottom_states==first_bottom_state_in_R);
      m_blocks[B_new].start_non_bottom_states=last_bottom_state_in_R;
      // Update the block pointers for R-bottom states:
      for(fixed_vector<state_in_block_pointer>::iterator s_it=first_bottom_state_in_R; s_it<last_bottom_state_in_R; ++s_it)
      {
        mCRL2complexity(s_it->ref_state, add_work(check_complexity::split_block_B_into_R_and_BminR__carry_out_split,
                max_B), *this);
//std::cerr << "MOVE STATE TO NEW BLOCK: " << std::distance(m_states.begin(), s_it->ref_state) << "\n";
        assert(B==s_it->ref_state->block);
        s_it->ref_state->block=B_new;
        assert(s_it->ref_state->ref_states_in_blocks==s_it);
      }
      // Now the R bottom states are in the correct position, and we don't have to look into them any more.
      #ifdef TRY_EFFICIENT_SWAP
        // (We could perhaps extend the efficient swap to include the R bottom states,
        // but I think that is too complicated for me to think through.)
        const fixed_vector<state_in_block_pointer>::iterator BminR_start_bottom_states=last_bottom_state_in_R+R.size();
        const fixed_vector<state_in_block_pointer>::iterator BminR_start_non_bottom_states=m_blocks[B].start_non_bottom_states+R.size();

        if (R.size()>0)
        {
          fixed_vector<state_in_block_pointer>::iterator move_next_R_non_bottom_state_to=last_bottom_state_in_R;
          fixed_vector<state_in_block_pointer>::iterator move_next_R_non_bottom_state_to_end=BminR_start_bottom_states;
          // Move BminR bottom states out of the way.
          fixed_vector<state_in_block_pointer>::iterator move_next_BminR_bottom_state_to=m_blocks[B].start_non_bottom_states;
          if (move_next_BminR_bottom_state_to<move_next_R_non_bottom_state_to_end)
          {
            // there are many R-non-bottom states, so we will need to move all BminR bottom states.
            // (Later, the remaining R-non-bottom states will be placed.)
            move_next_R_non_bottom_state_to_end=move_next_BminR_bottom_state_to;
            move_next_BminR_bottom_state_to=BminR_start_bottom_states;
          }
          if (R.size()*2>=static_cast<state_index>(std::distance(m_blocks[B].start_non_bottom_states, m_blocks[B].end_states)))
          {
            // At least half the non-bottom states go into R.
            // It is more economical to disregard R completely and look only at ....counter==marking_value.

            // Move BminR bottom states out of the way.
            fixed_vector<state_in_block_pointer>::iterator take_next_R_non_bottom_state_from=BminR_start_non_bottom_states;
            #ifndef NDEBUG
              todo_state_vector::const_iterator account_for_skipped_BminR_states=R.begin();
            #endif
            for (; move_next_R_non_bottom_state_to<move_next_R_non_bottom_state_to_end;
                   ++move_next_R_non_bottom_state_to, ++move_next_BminR_bottom_state_to)
            {
              // Now the BminR-bottom state at *move_next_R_non_bottom_state_to should move to *move_next_BminR_bottom_state_to.
              // Find some R-non-bottom state that can move to *move_next_R_non_bottom_state_to:
              if (marking_value==move_next_BminR_bottom_state_to->ref_state->counter)
              {
                // There is a R-non-bottom state at *move_next_BminR_bottom_state_to already:
                swap_states_in_states_in_block_never_equal(move_next_R_non_bottom_state_to, move_next_BminR_bottom_state_to);
              }
              else
              {
                // *move_next_BminR_bottom_state_to now contains a BminR-non-bottom state.
                // Find a place where to move it to, namely some R-non-bottom state that needs to be moved.
                while (assert(take_next_R_non_bottom_state_from<m_blocks[B].end_states),
                       marking_value!=take_next_R_non_bottom_state_from->ref_state->counter)
                {
                  #ifndef NDEBUG
                    // assign the work in this loop to some R non-bottom state.
                    // (This is possible because there are no fewer R non-bottom states than BminR non-bottom states.)
                    assert(account_for_skipped_BminR_states<R.end());
                    mCRL2complexity(account_for_skipped_BminR_states->ref_state, add_work(check_complexity::split_block_B_into_R_and_BminR__skip_over_state, max_B), *this);
                    ++account_for_skipped_BminR_states;
                  #endif
                  ++take_next_R_non_bottom_state_from;
                }
                swap_states_in_states_in_block_23_never_equal(take_next_R_non_bottom_state_from, move_next_R_non_bottom_state_to, move_next_BminR_bottom_state_to);
                ++take_next_R_non_bottom_state_from;
              }
              assert(marking_value==move_next_R_non_bottom_state_to->ref_state->counter);
              assert(B==move_next_R_non_bottom_state_to->ref_state->block);
              move_next_R_non_bottom_state_to->ref_state->block=B_new;
              mCRL2complexity(move_next_R_non_bottom_state_to->ref_state,
                                 add_work(check_complexity::split_block_B_into_R_and_BminR__carry_out_split, max_B),
                                 *this);
            }
            for (; move_next_R_non_bottom_state_to<BminR_start_bottom_states; ++move_next_R_non_bottom_state_to)
            {
              // This loop moves R non-bottom states to places where there were BminR non-bottom states before;
              // it is executed if there are more R non-bottom states that BminR bottom states.

              // Try to find a R non-bottom state to move to *move_next_R_non_bottom_state_to.
              if (marking_value==move_next_R_non_bottom_state_to->ref_state->counter)
              {
                // There is already a suitable state at this position. Do nothing.
              }
              else
              {
                // *move_next_R_non_bottom_state_to now contains a BminR-non-bottom state.
                // Find a place where to move it to, namely some R-non-bottom state that needs to be moved.
                while (assert(take_next_R_non_bottom_state_from<m_blocks[B].end_states),
                       marking_value!=take_next_R_non_bottom_state_from->ref_state->counter)
                {
                  #ifndef NDEBUG
                    // assign the work in this loop to some R non-bottom state.
                    // (This is possible because there are no fewer R non-bottom states than BminR non-bottom states.)
                    assert(account_for_skipped_BminR_states<R.end());
                    mCRL2complexity(account_for_skipped_BminR_states->ref_state, add_work(check_complexity::split_block_B_into_R_and_BminR__skip_over_state, max_B), *this);
                    ++account_for_skipped_BminR_states;
                  #endif
                  ++take_next_R_non_bottom_state_from;
                }
                swap_states_in_states_in_block_never_equal(take_next_R_non_bottom_state_from, move_next_R_non_bottom_state_to);
                ++take_next_R_non_bottom_state_from;
                assert(marking_value==move_next_R_non_bottom_state_to->ref_state->counter);
              }
              assert(B==move_next_R_non_bottom_state_to->ref_state->block);
              move_next_R_non_bottom_state_to->ref_state->block=B_new;
              mCRL2complexity(move_next_R_non_bottom_state_to->ref_state,
                                 add_work(check_complexity::split_block_B_into_R_and_BminR__carry_out_split, max_B),
                                 *this);
            }
          }
          else
          {
            // Less than half of the non-bottom states go to R.
            // We have to ensure that we assign every move to some R non-bottom state
            // (and do not look at too many BminR states).
            todo_state_vector::const_iterator take_next_R_non_bottom_state_from=R.begin();
            for (; move_next_R_non_bottom_state_to<move_next_R_non_bottom_state_to_end;
               ++move_next_R_non_bottom_state_to, ++move_next_BminR_bottom_state_to)
            {
              // Now the BminR-bottom state at *move_next_R_non_bottom_state_to should move to *move_next_BminR_bottom_state_to.
              // Find some R-non-bottom state that can move to *move_next_R_non_bottom_state_to:
              if (marking_value==move_next_BminR_bottom_state_to->ref_state->counter)
              {
                // There is a R-non-bottom state at *move_next_BminR_bottom_state_to already:
                swap_states_in_states_in_block_never_equal(move_next_R_non_bottom_state_to, move_next_BminR_bottom_state_to);
              }
              else
              {
                // *move_next_BminR_bottom_state_to now contains a BminR-non-bottom state.
                // Find a place where to move it to, namely some R-non-bottom state that needs to be moved.
                while (assert(take_next_R_non_bottom_state_from<R.end()),
                   take_next_R_non_bottom_state_from->ref_state->ref_states_in_blocks<BminR_start_non_bottom_states)
                {
                  mCRL2complexity(take_next_R_non_bottom_state_from->ref_state, add_work(check_complexity::split_block_B_into_R_and_BminR__skip_over_state, max_B), *this);
                  assert(marking_value==take_next_R_non_bottom_state_from->ref_state->counter);
                  assert((take_next_R_non_bottom_state_from->ref_state->ref_states_in_blocks<move_next_R_non_bottom_state_to
                      ? B_new : B)==take_next_R_non_bottom_state_from->ref_state->block);
                  ++take_next_R_non_bottom_state_from;
                }
                swap_states_in_states_in_block_23_never_equal(take_next_R_non_bottom_state_from->ref_state->ref_states_in_blocks, move_next_R_non_bottom_state_to, move_next_BminR_bottom_state_to);
                ++take_next_R_non_bottom_state_from;
              }
              assert(marking_value==move_next_R_non_bottom_state_to->ref_state->counter);
              assert(B==move_next_R_non_bottom_state_to->ref_state->block);
              move_next_R_non_bottom_state_to->ref_state->block=B_new;
              mCRL2complexity(move_next_R_non_bottom_state_to->ref_state,
                             add_work(check_complexity::split_block_B_into_R_and_BminR__carry_out_split, max_B),
                             *this);
            }
            #ifndef NDEBUG
              if (move_next_R_non_bottom_state_to<BminR_start_bottom_states)
              {
                // There are still some R-non-bottom states that may not be in their correct positions.
                assert(move_next_R_non_bottom_state_to==m_blocks[B].start_non_bottom_states);
              }
              else
              {
                // All states are in their correct positions.
                assert(BminR_start_non_bottom_states==move_next_BminR_bottom_state_to);
              }
            #endif
            for (; move_next_R_non_bottom_state_to<BminR_start_bottom_states; ++move_next_R_non_bottom_state_to)
            {
              // This loop moves R non-bottom states to places where there were BminR non-bottom states before;
              // it is executed if there are more R non-bottom states that BminR bottom states.

              // Try to find a R non-bottom state to move to *move_next_R_non_bottom_state_to.
              if (marking_value==move_next_R_non_bottom_state_to->ref_state->counter)
              {
                // There is already a suitable state at this position. Do nothing.
              }
              else
              {
                // *move_next_R_non_bottom_state_to now contains a BminR-non-bottom state.
                // Find a place where to move it to, namely some R-non-bottom state that needs to be moved.
                while (assert(R.begin()<=take_next_R_non_bottom_state_from),
                   assert(take_next_R_non_bottom_state_from<R.end()),
                   take_next_R_non_bottom_state_from->ref_state->ref_states_in_blocks<BminR_start_non_bottom_states)
                {
                  // Actually the state at *take_next_R_non_bottom_state_from is already at an acceptable position.
                  // Leave it there and try the next state.
                  mCRL2complexity(take_next_R_non_bottom_state_from->ref_state, add_work(check_complexity::split_block_B_into_R_and_BminR__skip_over_state, max_B), *this);
                  assert(marking_value==take_next_R_non_bottom_state_from->ref_state->counter);
                  assert((take_next_R_non_bottom_state_from->ref_state->ref_states_in_blocks<move_next_R_non_bottom_state_to
                      ? B_new : B)==take_next_R_non_bottom_state_from->ref_state->block);
                  ++take_next_R_non_bottom_state_from;
                }
                swap_states_in_states_in_block_never_equal(take_next_R_non_bottom_state_from->ref_state->ref_states_in_blocks, move_next_R_non_bottom_state_to);
                ++take_next_R_non_bottom_state_from;
                assert(marking_value==move_next_R_non_bottom_state_to->ref_state->counter);
              }
              assert(B==move_next_R_non_bottom_state_to->ref_state->block);
              move_next_R_non_bottom_state_to->ref_state->block=B_new;
              mCRL2complexity(move_next_R_non_bottom_state_to->ref_state,
                             add_work(check_complexity::split_block_B_into_R_and_BminR__carry_out_split, max_B),
                             *this);
            }
          }
        }
        m_blocks[B_new].end_states=BminR_start_bottom_states;
        m_blocks[B].start_bottom_states=BminR_start_bottom_states;
        m_blocks[B].start_non_bottom_states=BminR_start_non_bottom_states;
        assert(static_cast<std::make_signed<state_index>::type>(R.size())==std::distance(m_blocks[B_new].start_non_bottom_states, m_blocks[B_new].end_states));
      #else
        m_blocks[B_new].end_states=last_bottom_state_in_R;
        m_blocks[B].start_bottom_states=last_bottom_state_in_R;

        // Move the non-bottom states to their correct positions:
        for(state_in_block_pointer s: R)
        {
          mCRL2complexity(s.ref_state, add_work(check_complexity::split_block_B_into_R_and_BminR__carry_out_split,
                max_B), *this);
//std::cerr << "MOVE STATE TO NEW BLOCK: " << s << "\n";
          assert(B==s.ref_state->block);
          s.ref_state->block=B_new;
          fixed_vector<state_in_block_pointer>::iterator pos=s.ref_state->ref_states_in_blocks;
          assert(pos>=m_blocks[B].start_non_bottom_states); // the state is a non bottom state.
            // pos --> B.start_bottom_states --> B.start_non_bottom_states --> pos.
          swap_states_in_states_in_block(pos, m_blocks[B].start_bottom_states, m_blocks[B].start_non_bottom_states);
          m_blocks[B].start_non_bottom_states++;
          m_blocks[B].start_bottom_states++;
          m_blocks[B_new].end_states++;
          assert(m_blocks[B].start_bottom_states<=m_blocks[B].start_non_bottom_states);
          assert(m_blocks[B_new].start_bottom_states<m_blocks[B_new].end_states);
        }
      #endif
      return B_new;
    }

    // Move the content if i1 to i2, i2 to i3 and i3 to i1.
    void swap_three_iterators_and_update_m_transitions(const BLC_list_iterator i1, const BLC_list_iterator i2, const BLC_list_iterator i3)
    {
      assert(i3<=i2);
      assert(i2<=i1);
      if (i1==i3)
      {
        return;
      }
      if ((i1==i2)||(i2==i3))
      {
        std::swap(*i1,*i3);
        m_transitions[*i1].ref_outgoing_transitions->ref_BLC_transitions = i1;
        m_transitions[*i3].ref_outgoing_transitions->ref_BLC_transitions = i3;
      }
      else  // swap all three elements.
      {
        transition_index temp = *i1;
        *i1=*i2;
        *i2=*i3;
        *i3=temp;
        m_transitions[*i1].ref_outgoing_transitions->ref_BLC_transitions = i1;
        m_transitions[*i2].ref_outgoing_transitions->ref_BLC_transitions = i2;
        m_transitions[*i3].ref_outgoing_transitions->ref_BLC_transitions = i3;
      }
    }

    // It is assumed that the new block is located precisely before the old_block in m_BLC_transitions.
    // This routine can not be used in the initialisation phase. It can only be used during refinement.
    // The routine returns true if the last element of old_BLC_block has been removed.
    //
    // This variant of the swap routine assumes that transition ti is unmarked,
    // or at least that it is not relevant if it is marked.
    [[nodiscard]]
    bool swap_in_the_doubly_linked_list_LBC_in_blocks_new_constellation(
               const transition_index ti,
               linked_list<BLC_indicators>::iterator new_BLC_block,
               linked_list<BLC_indicators>::iterator old_BLC_block)
    {
      assert(new_BLC_block->is_stable());
      BLC_list_iterator old_position = m_transitions[ti].ref_outgoing_transitions->ref_BLC_transitions;
      assert(old_BLC_block->start_same_BLC <= old_position);
      assert(old_position<old_BLC_block->end_same_BLC);
      assert(new_BLC_block->end_same_BLC==old_BLC_block->start_same_BLC);
      assert(m_transitions[ti].transitions_per_block_to_constellation == old_BLC_block);
      assert(ti == *old_position);

      if (!old_BLC_block->is_stable())
      {
        assert(old_BLC_block->start_same_BLC<=old_BLC_block->start_marked_BLC);
        assert(old_BLC_block->start_marked_BLC<=old_BLC_block->end_same_BLC);
        if (old_BLC_block->start_marked_BLC<=old_position)
        {
          assert(m_blocks[m_states[m_aut.get_transitions()[ti].from()].block].contains_new_bottom_states ||
                 number_of_states_in_block(m_states[m_aut.get_transitions()[ti].from()].block)<=1);
          // It's ok to unmark transitions because they start in blocks with new bottom states,
          // or in blocks with only 1 state.
          // (However, it may happen that unmarked transitions from other blocks
          // are moved, even if there are marked transitions in the same BLC set---
          // namely for the co-splitters. So we can only unmark transitions if
          // transition ti itself is marked.)
          old_BLC_block->start_marked_BLC=old_BLC_block->end_same_BLC;
        }
      }
      if (old_position!=old_BLC_block->start_same_BLC)
      {
        std::swap(*old_position,*old_BLC_block->start_same_BLC);
        m_transitions[*old_position].ref_outgoing_transitions->ref_BLC_transitions=old_position;
        m_transitions[*old_BLC_block->start_same_BLC].ref_outgoing_transitions->ref_BLC_transitions = old_BLC_block->start_same_BLC;
      }
      new_BLC_block->end_same_BLC=++old_BLC_block->start_same_BLC;
      m_transitions[ti].transitions_per_block_to_constellation=new_BLC_block;
//std::cerr << " to new " << new_BLC_block->debug_id(*this) << '\n';
      return old_BLC_block->start_same_BLC==old_BLC_block->end_same_BLC;
    }

    // Move the transition t with transition index ti to a new
    // LBC list as the target state switches to a new constellation.
    // Returns true if a new BLC-entry for non-inert transitions has been created,
    // and there are still transitions in the old BLC-entry.
    // (Otherwise there is no need to split the block.)
    [[nodiscard]]
    bool update_the_doubly_linked_list_LBC_new_constellation(
               const block_index index_block_B,
               const transition& t,
               const transition_index ti)
    {
      assert(m_states[t.to()].block==index_block_B);
      assert(&m_aut.get_transitions()[ti] == &t);
      const block_index from_block=m_states[t.from()].block;
      assert(m_blocks[from_block].block_to_constellation.check_linked_list());
      bool new_block_created = false;
      linked_list<BLC_indicators>::iterator this_block_to_constellation=
                           m_transitions[ti].transitions_per_block_to_constellation;
      #ifndef NDEBUG
        // Check whether this_block_to_constellation is in the corresponding list
        for (linked_list<BLC_indicators>::const_iterator i=m_blocks[from_block].block_to_constellation.begin();
             i!=this_block_to_constellation; ++i)
        {
          assert(i!=m_blocks[from_block].block_to_constellation.end());
        }
      #endif
      assert(this_block_to_constellation!=m_blocks[from_block].block_to_constellation.end());
      assert(this_block_to_constellation->start_same_BLC <= m_transitions[ti].ref_outgoing_transitions->ref_BLC_transitions);
      // the transition is never marked:
      assert(m_blocks[from_block].contains_new_bottom_states ||
             number_of_states_in_block(from_block)<=1 ||
             this_block_to_constellation->is_stable() ||
             m_transitions[ti].ref_outgoing_transitions->ref_BLC_transitions<this_block_to_constellation->start_marked_BLC);
      linked_list<BLC_indicators>::iterator next_block_to_constellation;
      // if this transition is inert, it is inserted in a block in front. Otherwise, it is inserted after
      // the current element in the list.
      if (is_inert_during_init(t) && from_block==index_block_B)
      {
        next_block_to_constellation=m_blocks[from_block].block_to_constellation.begin();
        assert(next_block_to_constellation->start_same_BLC<next_block_to_constellation->end_same_BLC);
        assert(m_states[m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)].from()].block==index_block_B);
        assert(m_aut.is_tau(m_aut_apply_hidden_label_map(m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)].label())));
        if (next_block_to_constellation==this_block_to_constellation)
        {
          // Make a new entry in the list block_to_constellation, at the beginning;
//std::cerr << "Creating new BLC set for inert " << m_transitions[ti].debug_id(*this) << ": ";

          next_block_to_constellation=
                  m_blocks[from_block].block_to_constellation.
                           emplace_front(//first_block_to_constellation,
                                   this_block_to_constellation->start_same_BLC,
                                   this_block_to_constellation->start_same_BLC, true);
          #ifndef NDEBUG
            next_block_to_constellation->work_counter = this_block_to_constellation->work_counter;
          #endif
        }
        else
        {
//std::cerr << "Extending existing BLC set for inert " << m_transitions[ti].debug_id(*this) << ": ";
          assert(m_states[m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)].to()].block==index_block_B);
        }
      }
      else
      {
        // The transition is not constellation-inert.
        // The transition will be placed in a BLC set immediately after the BLC set it came from,
        // so that main splitters (with transitions to the new constellation)
        // come after co-splitters (with transitions to the old constellation).

        // This method also ensures that transitions from the old constellation
        // to the old constellation will remain at the beginning of their respective BLC set.
        next_block_to_constellation=m_blocks[from_block].block_to_constellation.next(this_block_to_constellation);
        const transition* first_t;
        if (next_block_to_constellation==m_blocks[from_block].block_to_constellation.end() ||
            (first_t = &m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)],
             assert(m_states[first_t->from()].block==from_block),
             m_states[first_t->to()].block!=index_block_B) ||
            (label_or_divergence(*first_t) != label_or_divergence(t)))
        {
//std::cerr << "Creating new BLC set for " << m_transitions[ti].debug_id(*this) << ": ";
          // Make a new entry in the list next_block_to_constellation, after the current list element.
          next_block_to_constellation=
                  m_blocks[from_block].block_to_constellation.
                           emplace_after(this_block_to_constellation,
                                         this_block_to_constellation->start_same_BLC,
                                         this_block_to_constellation->start_same_BLC, true);
          // The entry will be marked as unstable later
          #ifndef NDEBUG
            next_block_to_constellation->work_counter = this_block_to_constellation->work_counter;
          #endif
          new_block_created = true;
        }
//else { std::cerr << "Extending existing BLC set for " << m_transitions[ti].debug_id(*this) << ": "; }
      }

      if (swap_in_the_doubly_linked_list_LBC_in_blocks_new_constellation(ti,
                    next_block_to_constellation, this_block_to_constellation))
      {
        m_blocks[from_block].block_to_constellation.erase(this_block_to_constellation);
      }
      #ifndef NDEBUG
        check_transitions(false, false);
      #endif
      return new_block_created;
    }

    // It is assumed that the new block is located precisely before the old_block in m_BLC_transitions.
    // This routine can not be used in the initialisation phase. It can only be used during refinement.
    // The routine returns true if the last element of old_BLC_block has been removed.
    // There is no requirement on stability of old and new BLC set.
    // However, the procedure is only called for different stability states if ti
    // is an inert transition and some OTHER inert transitions have been marked for being no longer block-inert.
    // (So, it cannot happen that ti is marked and the new BLC set is stable.)
    [[nodiscard]]
    bool swap_in_the_doubly_linked_list_LBC_in_blocks_new_block(
               const transition_index ti,
               linked_list<BLC_indicators>::iterator new_BLC_block,
               linked_list<BLC_indicators>::iterator old_BLC_block)
    {
      BLC_list_iterator old_position = m_transitions[ti].ref_outgoing_transitions->ref_BLC_transitions;
      assert(old_BLC_block->start_same_BLC <= old_position);
      assert(old_position<old_BLC_block->end_same_BLC);
      if (!old_BLC_block->is_stable())
      {
        assert(old_BLC_block->start_same_BLC<=old_BLC_block->start_marked_BLC);
        assert(old_BLC_block->start_marked_BLC<=old_BLC_block->end_same_BLC);
      }
      if (!new_BLC_block->is_stable())
      {
        assert(new_BLC_block->start_same_BLC<=new_BLC_block->start_marked_BLC);
        assert(new_BLC_block->start_marked_BLC<=new_BLC_block->end_same_BLC);
      }
      assert(new_BLC_block->end_same_BLC==old_BLC_block->start_same_BLC);
      assert(m_transitions[ti].transitions_per_block_to_constellation == old_BLC_block);
      assert(m_blocks.size()-1==m_states[m_aut.get_transitions()[ti].from()].block);
      assert(ti == *old_position);
      if (old_BLC_block->is_stable() || old_position<old_BLC_block->start_marked_BLC)
      {
//std::cerr << "Moving unmarked " << m_transitions[*old_position].debug_id(*this);
        assert(old_BLC_block->start_same_BLC <= old_position);
        if (new_BLC_block->is_stable())
        {
          #ifndef NDEBUG
            if (!old_BLC_block->is_stable())
            {
              // different stability status: check that ti is inert
              const transition& t=m_aut.get_transitions()[ti];
              assert(is_inert_during_init(t) && m_blocks[m_states[t.from()].block].constellation==m_blocks[m_states[t.to()].block].constellation);
            }
          #endif
          if (old_position!=old_BLC_block->start_same_BLC)
          {
            std::swap(*old_position, *old_BLC_block->start_same_BLC);
            m_transitions[*old_position].ref_outgoing_transitions->ref_BLC_transitions = old_position;
            m_transitions[*old_BLC_block->start_same_BLC].ref_outgoing_transitions->ref_BLC_transitions = old_BLC_block->start_same_BLC;
          }
        }
        else
        {
          #ifndef NDEBUG
            if (old_BLC_block->is_stable())
            {
              // different stability status: check that ti is inert
              const transition& t=m_aut.get_transitions()[ti];
              assert(is_inert_during_init(t) && m_blocks[m_states[t.from()].block].constellation==m_blocks[m_states[t.to()].block].constellation);
            }
          #endif
          assert(new_BLC_block->start_marked_BLC<=old_BLC_block->start_same_BLC);
          swap_three_iterators_and_update_m_transitions(old_position, old_BLC_block->start_same_BLC, new_BLC_block->start_marked_BLC++);
        }
      }
      else
      {
//std::cerr << "Moving marked " << m_transitions[*old_position].debug_id(*this);
        assert(old_BLC_block->start_marked_BLC <= old_position);
        assert(old_BLC_block->start_same_BLC <= old_BLC_block->start_marked_BLC);
        assert(!new_BLC_block->is_stable());
        swap_three_iterators_and_update_m_transitions(old_position, old_BLC_block->start_marked_BLC++, old_BLC_block->start_same_BLC);
      }
      m_transitions[ti].transitions_per_block_to_constellation=new_BLC_block;
      new_BLC_block->end_same_BLC=++old_BLC_block->start_same_BLC;
//std::cerr << " to new " << new_BLC_block->debug_id(*this) << '\n';
      return old_BLC_block->start_same_BLC==old_BLC_block->end_same_BLC;
    }

    // Update the LBC list of a transition, when the from state of the transition moves
    // from block old_bi to new_bi.
    // If the transition was part of a stable BLC set, the new BLC set where it goes to
    // is also stable, and vice versa.
    // If the transition is part of an unstable BLC set, main/co-splitter order
    // is maintained (if CO_SPLITTER_IN_BLC_LIST).

    // A problem that may appear is the following:
    // When splitting a co-splitter, it may occasionally happen that one part of
    // the co-splitter has no corresponding main splitter. (Of course, if it is
    // the co-splitter of the current main splitter, this will soon be corrected,
    // but a similar phenomenon can appear for other main/co-splitter pairs.)
    // That is: if B is split into old_bi and new_bi, then it may happen that
    // for some OTHER main splitter S', all transitions start in one of the subblocks,
    // but the transitions of the co-splitter corresponding to S' start in both
    // subblocks.
    // Then the co-splitter part that loses its correspondence with S' should
    // be changed to stable.
    // This can happen both ways: the co-splitter part may belong to old_bi or to new_bi.
    // - If the co-splitter part that loses its correspondence with S' belongs
    //   to old_bi (i.e. S' only contains transitions from new_bi):
    //   the old S' is deleted at some point; at that point, one should turn the
    //   co-splitter part stable.
    // - If the co-splitter part that loses its correspondence with S' belongs
    //   to new_bi (i.e. S' only contains transitions from old_bi):
    //   the new co-splitter part should be created as stable and become unstable
    //   as soon as a transition of S' moves to new_bi.

    // So, the resolution is:
    // - when a new co-splitter BLC set is created, it is always regarded as stable.
    // - when a new main splitter BLC set is created, its stability is the same as the original.
    //   If it already has a corresponding new co-splitter, the co-splitter is made unstable
    //   (and moved to a suitable position).
    // - when an old main splitter BLC set is deleted, its corresponding co-splitter is made stable
    //   (if it still contains a marked transition, it will be moved to the new co-splitter set later---
    //   in fact, while still handling the same state's transitions.)
    // - when an old co-splitter BLC set loses its last marked transition, and it has already lost its corresponding main splitter,
    //   it is made stable.
    // - when a stable new co-splitter BLC set should get a marked transition, it is made unstable
    //   (but this situation does not appear in our implementation
    //   because we handle the main splitter transitions from a state
    //   before the co-splitter transitions).
    // But perhaps a simpler solution is:
    // - we keep a list of co-splitters that need to be checked afterwards
    //   (actually, a list of m_BLC_transition iterators that are in the respective co-splitters [if it still exists],
    //   together with an indication whether it is the old or the new co-splitter,
    //   and possibly time complexity information)
    // - if a new unstable main splitter is created but the new co-splitter does not yet exist,
    //   the old co-splitter may need to be checked afterwards.
    //   (The new co-splitter is safe, as it will always have a corresponding main splitter.)
    // - if a new unstable co-splitter is created but the new main splitter does not yet exist,
    //   the *old and* new co-splitters may need to be checked afterwards.
    // The check is: if the co-splitter has no corresponding main splitter, it becomes stable.
    // This check can be accounted for by the creation of the new co-splitter/main splitter.
    transition_index update_the_doubly_linked_list_LBC_new_block(
               const block_index old_bi,
               const block_index new_bi,
               const transition_index ti
               #ifdef CO_SPLITTER_IN_BLC_LIST
                 , constellation_index old_constellation = null_constellation
                 // used to maintain the order of BLC sets:
                 // main splitter BLC sets (target constellation == new constellation) follow immediately
                 // after co-splitter BLC sets (target constellation == old_constellation) in the BLC sets
               #endif
               )
    {
//std::cerr << "update_the_doubly_linked_list_LBC_new_block(old_bi = " << old_bi << ", new_bi = " << new_bi << ", transition_index = " << ti << " = " << m_transitions[ti].debug_id_short(*this) << ")\n";
      const transition& t=m_aut.get_transitions()[ti];
      assert(m_blocks[old_bi].block_to_constellation.check_linked_list());
      assert(m_blocks[new_bi].block_to_constellation.check_linked_list());

      assert(m_states[t.from()].block==new_bi);

      linked_list<BLC_indicators>::iterator this_block_to_constellation=
                           m_transitions[ti].transitions_per_block_to_constellation;
//std::cerr << "This transition is in " << this_block_to_constellation->debug_id(*this) << '\n';
      #ifndef NDEBUG
        // Check whether this_block_to_constellation is in the corresponding list
        for (linked_list<BLC_indicators>::const_iterator i=m_blocks[old_bi].block_to_constellation.begin();
             i!=this_block_to_constellation; ++i)
        {
          assert(i!=m_blocks[old_bi].block_to_constellation.end());
        }
      #endif
      const label_index a = label_or_divergence(t);
      const constellation_index to_constln = m_blocks[m_states[t.to()].block].constellation;
      linked_list<BLC_indicators>::iterator new_BLC_block;
      const bool t_is_inert=is_inert_during_init(t);
      if (t_is_inert && to_constln==m_blocks[new_bi].constellation)
      {
          // Before correcting the BLC lists, we already inserted an empty BLC_indicator
          // into the list to take the inert transitions.
          assert(this_block_to_constellation==m_blocks[old_bi].block_to_constellation.begin());
          new_BLC_block=m_blocks[new_bi].block_to_constellation.begin();
          assert(!m_blocks[new_bi].block_to_constellation.empty());
          #ifndef NDEBUG
            if (new_BLC_block->start_same_BLC<new_BLC_block->end_same_BLC)
            {
              const transition& inert_t=m_aut.get_transitions()[*new_BLC_block->start_same_BLC];
              assert(new_bi==m_states[inert_t.from()].block);
              assert(a==label_or_divergence(inert_t));
              assert(to_constln==m_blocks[m_states[inert_t.to()].block].constellation);
            }
          #endif
          assert(this_block_to_constellation->start_same_BLC==new_BLC_block->end_same_BLC);
      }
      else
      {
        transition_index perhaps_new_BLC_block_transition;
        const transition* perhaps_new_BLC_t;
        if (this_block_to_constellation->start_same_BLC!=&*m_BLC_transitions.begin() &&
            (perhaps_new_BLC_block_transition=*std::prev(this_block_to_constellation->start_same_BLC),
             perhaps_new_BLC_t=&m_aut.get_transitions()[perhaps_new_BLC_block_transition],
             m_states[perhaps_new_BLC_t->from()].block==new_bi &&
             a == label_or_divergence(*perhaps_new_BLC_t) &&
             to_constln == m_blocks[m_states[perhaps_new_BLC_t->to()].block].constellation))
        {
          // Found the entry where the transition should go to
          // Move the current transition to the next list indicated by the iterator it.
          new_BLC_block=m_transitions[perhaps_new_BLC_block_transition].transitions_per_block_to_constellation;
          #ifndef NDEBUG
            if (this_block_to_constellation->is_stable()) { assert(new_BLC_block->is_stable()); }
            else { assert(!new_BLC_block->is_stable()); }
          #endif
        }
        else
        {
          // Make a new entry in the list next_block_to_constellation;

          // We first calculate the position where the new BLC set should go to in new_position.
          // Default position: at the beginning.
          linked_list<BLC_indicators>::iterator new_position=m_blocks[new_bi].block_to_constellation.end();
          BLC_list_iterator_or_null old_co_splitter_end=nullptr;
          assert(!(is_inert_during_init(t) && to_constln==m_blocks[new_bi].constellation));
          if (this_block_to_constellation->is_stable())
          {
            if (m_branching)
            {
              // We always inserted a new BLC set for the inert transitions,
              // so we can place the BLC set after that one.
              new_position=m_blocks[new_bi].block_to_constellation.begin();
            }
//std::cerr << "Transition is in a stable BLC set.\n";
            #ifdef CO_SPLITTER_IN_BLC_LIST
              // Because the BLC set is stable, one does not need to keep main splitter and co-splitter together.
              // If the BLC set becomes empty, one can immediately delete it.
              old_constellation=null_constellation;
            #endif
          }
          else
          {
            if (m_blocks[new_bi].block_to_constellation.empty())
            {
              // This is the first transition that is moved.
              assert(!m_branching);
              assert(m_blocks[new_bi].block_to_constellation.end()==new_position);
//std::cerr << "This is the first transition that is moved.\n";
            }
            else
            {
              // default position: place it at the end of the list
              new_position=m_blocks[new_bi].block_to_constellation.before_end();
            }
            const constellation_index new_constellation=m_constellations.size()-1;
            assert(!t_is_inert || to_constln!=old_constellation || m_blocks[new_bi].constellation!=new_constellation);
                // < If the formerly inert transition goes from the new constellation to the old constellation,
                // it is in a co-splitter without (unstable) main splitter, and this co-splitter was handled
                // as the first splitting action.
                // Therefore, it must be stable, and the situation does not appear.
            if (t_is_inert && to_constln==new_constellation && m_blocks[new_bi].constellation==old_constellation)
            {
              // The transition goes from the old constellation to the splitter
              // and was inert earlier.
              // It is in a main splitter without (unstable) co-splitter.
              // We do not need to find the co-splitter, and we do not need to maintain the relationship with it.
              old_constellation=null_constellation;
//std::cerr << "This transition was inert earlier, so we do not need to find a co-splitter.\n";
            }
            else
            {
              #ifdef CO_SPLITTER_IN_BLC_LIST
                if (null_constellation!=old_constellation)
                {
                  // The following comments are all formulated for the case that this_block_to_constellation is a main splitter (except when indicated explicitly).
                  assert(old_constellation<new_constellation);
                  linked_list<BLC_indicators>::const_iterator old_co_splitter;
                  if ((old_constellation==to_constln /* i.e. this_block_to_constellation is a co-splitter */ &&
                       (old_co_splitter = m_blocks[old_bi].block_to_constellation.next(this_block_to_constellation),
//(std::cerr << "Transition is originally in a co-splitter; "),
                        true)) ||
                      (new_constellation == to_constln /* i.e. this_block_to_constellation is a main splitter */ &&
                       (old_co_splitter = m_blocks[old_bi].block_to_constellation.prev(this_block_to_constellation),
//(std::cerr << "Transition is originally in a main splitter; "),
                        true)))
                  {
                    if (m_blocks[old_bi].block_to_constellation.end()!=old_co_splitter)
                    {
//std::cerr << (old_constellation == to_constln ? "Current old main splitter candidate: " : "Current old co-splitter candidate: ") << old_co_splitter->debug_id(*this);
                      // If the co-splitter belonging to this_block_to_constellation exists,
                      // then it is old_co_splitter.
                      // (but if there is no such co-splitter, old_co_splitter could be a different main splitter,
                      // a different co-splitter without main splitter, or a completely unrelated splitter).

                      // Try to find out whether there is already a corresponding co-splitter in m_blocks[new_bi].block_to_constellation.
                      // This co-splitter would be just before old_co_splitter in m_BLC_transitions.
                      if (m_blocks[new_bi].block_to_constellation.end()!=new_position && // i.e. this is not the first transition
                          &*m_BLC_transitions.begin()<old_co_splitter->start_same_BLC)
                      {
                        // Check the transition in the potential corresponding new co-splitter:
                        const transition_index perhaps_new_co_spl_transition=*std::prev(old_co_splitter->start_same_BLC);
                        const transition& perhaps_new_co_spl_t=m_aut.get_transitions()[perhaps_new_co_spl_transition];
                        const constellation_index co_to_constln=to_constln^old_constellation^new_constellation;
                        if (new_bi==m_states[perhaps_new_co_spl_t.from()].block &&
                            a==label_or_divergence(perhaps_new_co_spl_t) &&
                            co_to_constln==m_blocks[m_states[perhaps_new_co_spl_t.to()].block].constellation)
                        {
                          // perhaps_new_co_spl_transition is in the corresponding new co-splitter; place the new BLC set immediately after this co-splitter in the list m_blocks[new_bi].block_to_constellation.
                          new_position=m_transitions[perhaps_new_co_spl_transition].transitions_per_block_to_constellation;
                          if (old_constellation==to_constln)
                          {
                            // (this_block_to_constellation was a co-splitter:) perhaps_new_co_spl_transition is in the new main splitter; place the new BLC set immediately before this main splitter in the list m_blocks[new_bi].block_to_constellation.
                            new_position = m_blocks[new_bi].block_to_constellation.prev(new_position);
//std::cerr << ". This is a real old main splitter.\n";
                          }
//else { std::cerr << ". This is a real old co-splitter.\n"; }
                          // The new co-splitter was found, and old_co_splitter must have been the old co-splitter.
                          #ifndef NDEBUG
                            if (old_co_splitter->start_same_BLC<old_co_splitter->end_same_BLC)
                            {
                              const transition& co_t=m_aut.get_transitions()[*old_co_splitter->start_same_BLC];
                              assert(old_bi==m_states[co_t.from()].block || new_bi==m_states[co_t.from()].block);
                              assert(a==label_or_divergence(co_t));
                              assert(co_to_constln==m_blocks[m_states[co_t.to()].block].constellation);
                            }
                          #endif
                          // Now the new main splitter is about to be created.
                          // In this case it is ok to delete this_block_to_constellation when it becomes empty;
                          // therefore we set old_constellation in a way that it's going to delete it immediately:
                          old_constellation=null_constellation;
                          // We should not use old_constellation for anything else after this point.
                        }
                        else if (new_constellation==to_constln)
                        {
                          assert(old_co_splitter->start_same_BLC<old_co_splitter->end_same_BLC);
                          const transition& co_t=m_aut.get_transitions()[*old_co_splitter->start_same_BLC];
                          assert(old_bi==m_states[co_t.from()].block || new_bi==m_states[co_t.from()].block);
                          if (a==label_or_divergence(co_t) &&
                              old_constellation==m_blocks[m_states[co_t.to()].block].constellation)
                          {
//std::cerr << ". The transition just before it in m_BLC_transitions (" << m_transitions[perhaps_new_co_spl_transition].debug_id_short(*this) << ") does not belong to the new co-splitter.\n";
                            old_co_splitter_end=old_co_splitter->end_same_BLC;
                          }
                          else
                          {
//std::cerr << ". In fact, it is not a true candidate.\n";
                            // As there is no old co-splitter, we do not need to keep an empty old main splitter.
                            old_constellation=null_constellation;
                          }
                        }
                        else
                        {
                          assert(old_constellation==to_constln);
//std::cerr << ". The transition just before it in m_BLC_transitions (" << m_transitions[perhaps_new_co_spl_transition].debug_id_short(*this) << ") does not belong to the new main splitter.\n";
                        }
                      }
                      else if (new_constellation==to_constln)
                      {
                        assert(old_co_splitter->start_same_BLC<old_co_splitter->end_same_BLC);
                        const transition& co_t=m_aut.get_transitions()[*old_co_splitter->start_same_BLC];
                        assert(old_bi==m_states[co_t.from()].block || new_bi==m_states[co_t.from()].block);
                        if (a==label_or_divergence(co_t) &&
                            old_constellation==m_blocks[m_states[co_t.to()].block].constellation)
                        {
//std::cerr << ". This candidate is at the beginning of m_BLC_transitions, or it is the first transition. There is no new co-splitter yet.\n";
                          old_co_splitter_end=old_co_splitter->end_same_BLC;
                        }
                        else
                        {
//std::cerr << ". In fact, it is a false candidate.\n";
                          // As there is no old co-splitter, we do not need to keep an empty old main splitter.
                          old_constellation=null_constellation;
                        }
                      }
                      else
                      {
                        assert(old_constellation==to_constln);
//std::cerr << ". This candidate is at the beginning of m_BLC_transitions, or it is the first transition. There is no new main splitter yet.\n";
                      }
                    }
                    else
                    {
                      // this_block_to_constellation is a main splitter
                      // but it has no corresponding co-splitter.
                      // If it becomes empty, one can immediately delete it.
                      old_constellation=null_constellation;
//std::cerr << (old_constellation == to_constln ? "There is no candidate old main splitter.\n" : "There is no candidate old co-splitter.\n");
                    }
                  }
                  else
                  {
                    // this_block_to_constellation is neither a main splitter nor a co-splitter.
                    // If it becomes empty, one can immediately delete it.
                    old_constellation=null_constellation;
                  }
                }
              #else
                #error "old_co_splitter_end should be calculated here"
              #endif
            }
          }
          assert(!m_branching || m_blocks[new_bi].block_to_constellation.end()!=new_position);
          const BLC_list_iterator old_BLC_start=this_block_to_constellation->start_same_BLC;
          new_BLC_block=m_blocks[new_bi].block_to_constellation.emplace_after(new_position, old_BLC_start, old_BLC_start, this_block_to_constellation->is_stable());
          #ifndef NDEBUG
            new_BLC_block->work_counter=this_block_to_constellation->work_counter;
          #endif
          if (null_constellation!=old_constellation)
          {
            // This is a new main splitter without new co-splitter, or a new co-splitter without new main splitter
            if (old_constellation==to_constln)
            {
              // this is a new co-splitter but we haven't yet found the new main splitter.
              // Perhaps we will have to stabilize it later.
              m_co_splitters_to_be_checked.emplace_back(new_BLC_block->start_same_BLC, new_bi);
//std::cerr << "We will check the new co-splitter later.\n";
              // We may also have to stabilize the old co-splitter later.
              assert(&*m_BLC_transitions.begin()<this_block_to_constellation->end_same_BLC);
              m_co_splitters_to_be_checked.emplace_back(std::prev(this_block_to_constellation->end_same_BLC), old_bi);
            }
            else
            {
              // this is a new main splitter but we haven't yet found the new co-splitter.
              // Perhaps we will have to stabilize the old co-splitter later.
              assert(m_constellations.size()-1==to_constln);
              assert(nullptr!=old_co_splitter_end);
              assert(&*m_BLC_transitions.begin()<old_co_splitter_end);
              assert(old_co_splitter_end<=&*m_BLC_transitions.end());
              m_co_splitters_to_be_checked.emplace_back(std::prev(old_co_splitter_end), old_bi);
//std::cerr << "We will check the old co-splitter later.\n";
            }
          }
        }
      }
      const bool last_element_removed=swap_in_the_doubly_linked_list_LBC_in_blocks_new_block(ti,
                    new_BLC_block, this_block_to_constellation);

      transition_index remaining_transition=null_transition;
      if (last_element_removed)
      {
//std::cerr << "This was the last element in the old BLC set.\n";
        #ifdef CO_SPLITTER_IN_BLC_LIST
          if (null_constellation != old_constellation)
          {
            // Sometimes we could still remove this_block_to_constellation immediately
            // (namely if the new main splitter and the new co-splitter already exist,
            // or if the old co-splitter does not exist at all).
            // A few small such cases are handled above,
            // but other cases would require additional, possibly extensive, checks:
            // if (co_block_found) {
            //   copy more or less the code from above that decides
            //   whether this_block_to_constellation is a main splitter
            //   that has an old co-splitter but not a new co-splitter or vice versa.
            // }
            m_BLC_indicators_to_be_deleted.push_back(this_block_to_constellation);
          }
          else
        #endif
        {
          // Remove this element.
          m_blocks[old_bi].block_to_constellation.erase(this_block_to_constellation);
        }
      }
      else
      {
        #ifndef CO_SPLITTER_IN_BLC_LIST
          remaining_transition=*(this_block_to_constellation->start_same_BLC);
        #endif
      }
      #ifndef NDEBUG
        check_transitions(false, false);
        assert(m_blocks[old_bi].block_to_constellation.check_linked_list());
        assert(m_blocks[new_bi].block_to_constellation.check_linked_list());
      #endif

      return remaining_transition;
    }

    // Set m_states[s].counter:=undefined for all s in m_R and m_U.
    void clear_state_counters(bool restrict_to_R=false)
    {
      for(const state_in_block_pointer si: m_R)
      {
        assert(Rmarked == si.ref_state->counter); // this allows us to charge the work in this loop to setting the counter to Rmarked
        si.ref_state->counter=undefined;
      }
      if (restrict_to_R)
      {
        assert(m_U_counter_reset_vector.empty());
        return;
      }
      for(const state_in_block_pointer si: m_U_counter_reset_vector)
      {
        // this work is charged to adding a value to m_U_counter_reset_vector
        assert(undefined!=si.ref_state->counter || m_R.find(si));
        assert(Rmarked!=si.ref_state->counter);
        si.ref_state->counter=undefined;
      }
      clear(m_U_counter_reset_vector);
    }

    // Calculate the states R in block B that can inertly reach a state with a transition in splitter
    // and split B in R and B\R. The complexity is conform the smaller block, either R or B\R.
    // The LBC_list and bottom states are not updated.
    // Provide the index of subblock R as a result.

    // The function assumes that R has already been pre-filled with the
    // sources of the marked transitions in the splitter, and therefore a
    // split is always needed.  Bottom states in R are those in the range
    // [m_blocks[B].start_bottom_states, first_unmarked_bottom_state),
    // non-bottom states in R are already in m_R.  (Unmarked transitions
    // in the splitter may need to be checked still, but they can only add
    // non-bottom states to R.)

    // Occasionally it is necessary to split using only a subset of a splitter.
    // Then, splitter_end_unmarked_BLC can be used to indicate that this is the case.
    // The function assumes that the *marked* transitions in the splitter have
    // already been used to separate the bottom states in block B. This is indicated
    // using first_unmarked_bottom_state, an iterator into m_states_in_blocks.

    // Return value: the index of subblock R
    block_index simple_splitB(const block_index B,
                              linked_list<BLC_indicators>::const_iterator splitter,
                              const fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state,
                              // const bool initialisation,
                              const BLC_list_iterator splitter_end_unmarked_BLC)
    {
//std::cerr << "simple_splitB(block " << B << ",...)\n";
      typedef enum { initializing, state_checking, aborted, aborted_after_initialisation,
                     incoming_inert_transition_checking, outgoing_action_constellation_check } status_type;
      status_type U_status=state_checking;
      status_type R_status=initializing;
      BLC_list_iterator M_it = splitter->start_same_BLC;
      std::vector<transition>::iterator current_U_incoming_transition_iterator;
      std::vector<transition>::iterator current_U_incoming_transition_iterator_end;
      state_in_block_pointer current_U_outgoing_state;
      outgoing_transitions_it current_U_outgoing_transition_iterator;
      outgoing_transitions_it current_U_outgoing_transition_iterator_end;
      std::vector<transition>::iterator current_R_incoming_transition_iterator;
      std::vector<transition>::iterator current_R_incoming_transition_iterator_end;
      fixed_vector<state_in_block_pointer>::iterator current_R_incoming_bottom_state_iterator = m_blocks[B].start_bottom_states;
      fixed_vector<state_in_block_pointer>::iterator current_U_incoming_bottom_state_iterator = first_unmarked_bottom_state;

      assert(1 < number_of_states_in_block(B));
      assert(!m_blocks[B].contains_new_bottom_states);
      assert(m_U.empty());
      assert(m_U_counter_reset_vector.empty());
      // assert(splitter_end_unmarked_BLC == old value of splitter->start_marked_BLC || splitter_end_unmarked_BLC == splitter->start_same_BLC);
      assert(&*m_BLC_transitions.begin()<=splitter->start_same_BLC);
      assert(M_it <= splitter_end_unmarked_BLC);
      assert(splitter_end_unmarked_BLC<=splitter->end_same_BLC);
      assert(splitter->is_stable());
      assert(splitter->start_same_BLC<splitter->end_same_BLC);
      assert(splitter->end_same_BLC<=&*m_BLC_transitions.end());
      assert(!has_marked_transitions(*splitter));
      assert(m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].block == B);
      assert(current_R_incoming_bottom_state_iterator <= first_unmarked_bottom_state);
      assert(current_R_incoming_bottom_state_iterator < first_unmarked_bottom_state || !m_R.empty() || M_it < splitter_end_unmarked_BLC);
      assert(current_U_incoming_bottom_state_iterator < m_blocks[B].start_non_bottom_states);

      const std::make_signed<state_index>::type max_R_nonbottom_size=number_of_states_in_block(B)/2-std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state); // can underflow
      if (max_R_nonbottom_size < static_cast<std::make_signed<state_index>::type>(m_R.size()))
      {
        assert(number_of_states_in_block(B)/2<std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state)+m_R.size());
        R_status = (M_it == splitter_end_unmarked_BLC) ? aborted_after_initialisation : aborted;
      }
      else
      {
        assert(number_of_states_in_block(B)/2>=std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state)+m_R.size());
        if (m_blocks[B].start_non_bottom_states==m_blocks[B].end_states)
        {
          // There are no non-bottom states, hence we do not need to carry out the tau closure.
          // Also, there cannot be any new bottom states.
          assert(m_blocks[B].start_bottom_states<first_unmarked_bottom_state);
          assert(std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state)<=std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states));
          assert(m_R.empty()); // m_R.clear(); is superfluous
          assert(m_U.empty()); // m_U.clear(); is superfluous;
          assert(m_U_counter_reset_vector.empty()); // clear_state_counters(); is superfluous
          // split_block B into R and B\R.
          return split_block_B_into_R_and_BminR(B, m_blocks[B].start_bottom_states, first_unmarked_bottom_state, m_R
                                                #ifdef TRY_EFFICIENT_SWAP
                                                  , Rmarked
                                                #endif
                                                );
        }
        else if (M_it==splitter_end_unmarked_BLC)
        {
          // There are no more transitions in the splitter whose source states need to be added to R.
          //assert(m_blocks[B].start_bottom_states < first_unmarked_bottom_state || !m_R.empty());
          R_status=state_checking;
        }
      }
      const std::make_signed<state_index>::type max_U_nonbottom_size=number_of_states_in_block(B)/2-std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states); // can underflow
      if (max_U_nonbottom_size<0)
      {
        assert(static_cast<std::make_signed<state_index>::type>(number_of_states_in_block(B)/2)<std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states));
        assert(aborted != R_status); assert(aborted_after_initialisation != R_status);
        U_status=aborted_after_initialisation;
      }
      else
      {
        assert(static_cast<std::make_signed<state_index>::type>(number_of_states_in_block(B)/2)>=std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states));
        if (m_blocks[B].start_non_bottom_states==m_blocks[B].end_states)
        {
          // There are no non-bottom states, hence we do not need to carry out the tau closure.
          // Also, there cannot be any new bottom states.
          assert(first_unmarked_bottom_state<m_blocks[B].start_non_bottom_states);
          assert(std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states)<=std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state));
          assert(m_R.empty()); // m_R.clear(); is superfluous
          assert(m_U.empty()); // m_U.clear(); is superfluous;
          assert(m_U_counter_reset_vector.empty()); // clear_state_counters(); is superfluous
          // split_block B into U and B\U.
          split_block_B_into_R_and_BminR(B, first_unmarked_bottom_state, m_blocks[B].end_states, m_U
                                         #ifdef TRY_EFFICIENT_SWAP
                                           , 0
                                         #endif
                                         );
          return B;
        }
      }
      assert(m_blocks[B].start_non_bottom_states < m_blocks[B].end_states);
      assert(splitter->start_same_BLC<splitter->end_same_BLC);
      const transition& first_t = m_aut.get_transitions()[*splitter->start_same_BLC];
      const label_index a = label_or_divergence(first_t);
      const constellation_index C = m_blocks[m_states[first_t.to()].block].constellation;
      #ifndef NDEBUG
        if (initializing == R_status || aborted == R_status)
        {
          // For constellation-inert splitters (which only happens for the special split to separate new from old bottom states), one has to mark all transitions in the splitter.
          assert(!m_aut.is_tau(a) || m_blocks[B].constellation != C);
        }
      #endif

      // Algorithm 3, line 3.2 left.

      // start coroutines. Each co-routine handles one state, and then gives control
      // to the other co-routine. The coroutines can be found sequentially below surrounded
      // by a while loop.

//std::cerr << "simple_splitB() before while\n";
      while (true)
      {
        assert(U_status!=aborted_after_initialisation || (R_status!=aborted && R_status!=aborted_after_initialisation));
#ifndef NDEBUG
        for(state_in_block_pointer si=state_in_block_pointer(m_states.begin()); si.ref_state<m_states.end(); ++si.ref_state)
        {
          if (si.ref_state->block!=B || 0==si.ref_state->no_of_outgoing_inert_transitions)
          {
            assert(undefined==si.ref_state->counter);
            assert(!m_R.find(si));
            assert(!m_U.find(si));
            assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
          }
          else
          {
            switch(si.ref_state->counter)
            {
            case undefined:  assert(!m_U.find(si)); assert(!m_R.find(si));
                             assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
                             break;
            case Rmarked:    assert( m_R.find(si)); assert(!m_U.find(si));
                             // It can happen that the state has a tau-transition to a U-state, then it will be in the m_U_counter_reset_vector.
                             break;
            case 0:          assert(!m_R.find(si)); // It can happen that the state is in U or is not in U
                             #ifdef TRY_EFFICIENT_SWAP
                               assert(m_U.find(si) ||
                                      (si==current_U_outgoing_state && outgoing_action_constellation_check==U_status));
                             #endif
                             assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                             break;
            default:         assert(!m_R.find(si)); assert(!m_U.find(si));
                             assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                             break;
            }
          }
        }
#endif
        // The code for the right co-routine.
        if (incoming_inert_transition_checking==R_status) // 18178 times (large 1394-fin.lts example: 372431 times)
        {
          assert(current_R_incoming_transition_iterator<current_R_incoming_transition_iterator_end);
          mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), current_R_incoming_transition_iterator)],
                        add_work(check_complexity::simple_splitB_R__handle_transition_to_R_state, 1), *this);
          assert(m_aut.is_tau(m_aut_apply_hidden_label_map(current_R_incoming_transition_iterator->label())));
          assert(m_states[current_R_incoming_transition_iterator->to()].block==B);

          const transition& tr=*current_R_incoming_transition_iterator;
          if (m_states[tr.from()].block==B && !(m_preserve_divergence && tr.from() == tr.to()))
          {
            if (m_states[tr.from()].counter!=Rmarked)
            {
              assert(!m_R.find(m_states.begin() + tr.from()));
//std::cerr << "R_todo2 insert: " << tr.from() << "\n";
              m_R.add_todo(m_states.begin() + tr.from());
              m_states[tr.from()].counter=Rmarked;

              // Algorithm 3, line 3.10 and line 3.11, right.
              if (m_R.size()>static_cast<state_index>(max_R_nonbottom_size))
              {
                assert(aborted_after_initialisation!=U_status);
                R_status=aborted_after_initialisation;
                goto R_handled_and_is_not_state_checking;
              }
            }
            else assert(m_R.find(m_states.begin() + tr.from()));
            assert(!m_U.find(m_states.begin() + tr.from()));
          }
          ++current_R_incoming_transition_iterator;
          if (current_R_incoming_transition_iterator!=current_R_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map(current_R_incoming_transition_iterator->label())))
          {
            goto R_handled_and_is_not_state_checking;
          }
          R_status=state_checking;
        }
        else if (state_checking==R_status) // 18014 times (large 1394-fin.lts example: 331708 times)
        {
          const state_in_block_pointer s=(current_R_incoming_bottom_state_iterator<first_unmarked_bottom_state
                                    ? *current_R_incoming_bottom_state_iterator++
                                    : m_R.move_from_todo());
          mCRL2complexity(s.ref_state, add_work(check_complexity::simple_splitB_R__find_predecessors, 1), *this);
//std::cerr << "R insert: " << s << "\n";
          assert(s.ref_state->block==B);
          if (std::next(s.ref_state)==m_states.end())
          {
            current_R_incoming_transition_iterator_end=m_aut.get_transitions().end();
          }
          else
          {
            current_R_incoming_transition_iterator_end=std::next(s.ref_state)->start_incoming_transitions;
          }
          current_R_incoming_transition_iterator=s.ref_state->start_incoming_transitions;
          if (current_R_incoming_transition_iterator!=current_R_incoming_transition_iterator_end &&
                  m_aut.is_tau(m_aut_apply_hidden_label_map(current_R_incoming_transition_iterator->label())))
          {
            R_status=incoming_inert_transition_checking;
            goto R_handled_and_is_not_state_checking;
          }
        }
        else if (initializing!=R_status)
        {
          assert(aborted_after_initialisation==R_status || // 10280 times (large 1394-fin.lts example: 550 times)
                 aborted==R_status); // 663 times (large 1394-fin.lts example: 584 times)
          goto R_handled_and_is_not_state_checking;
        }
        else // initializing==R_status: 2742 times (large 1394-fin.lts example: 1200 times)
        {
            // Algorithm 3, line 3.3, right.
          assert(M_it<splitter_end_unmarked_BLC);
          const state_in_block_pointer si(m_states.begin() + m_aut.get_transitions()[*M_it].from());
          mCRL2complexity(&m_transitions[*M_it], add_work(check_complexity::simple_splitB_R__handle_transition_from_R_state, 1), *this);
          assert(si.ref_state->block==B);
          assert(!is_inert_during_init(m_aut.get_transitions()[*M_it]) || m_blocks[B].constellation!=m_blocks[m_states[m_aut.get_transitions()[*M_it].to()].block].constellation);
          ++M_it;
          if (0==si.ref_state->no_of_outgoing_inert_transitions)
          {
            // The state is a bottom state, it should be in R already
            assert(m_blocks[B].start_bottom_states<=si.ref_state->ref_states_in_blocks);
            assert(si.ref_state->ref_states_in_blocks<first_unmarked_bottom_state);
            assert(!m_R.find(si));
          }
          else if (si.ref_state->counter!=Rmarked)
          {
            // The state is a nonbottom state that is not yet in R
            assert(m_blocks[B].start_non_bottom_states<=si.ref_state->ref_states_in_blocks);
            assert(si.ref_state->ref_states_in_blocks<m_blocks[B].end_states);
            m_R.add_todo(si);
            si.ref_state->counter=Rmarked;
//std::cerr << "R_todo1 insert: " << si << "\n";
            if (m_R.size()>static_cast<state_index>(max_R_nonbottom_size))
            {
              assert(aborted_after_initialisation!=U_status);
              R_status=aborted;
              goto R_handled_and_is_not_state_checking;
            }
          }
          else assert(m_R.find(si));
          assert(!m_U.find(si));
          if (M_it!=splitter_end_unmarked_BLC)
          {
            goto R_handled_and_is_not_state_checking;
          }
          assert(m_blocks[B].start_non_bottom_states<m_blocks[B].end_states);
          R_status=state_checking;
        }
        assert(state_checking==R_status);
        if (current_R_incoming_bottom_state_iterator==first_unmarked_bottom_state && m_R.todo_is_empty())
        {
//std::cerr << "R empty: " << "\n";
          // split_block B into R and B\R.
          assert(0 < std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state) + m_R.size());
          assert(std::distance(m_blocks[B].start_bottom_states, first_unmarked_bottom_state) + m_R.size() <= number_of_states_in_block(B)/2);
          const block_index block_index_of_R = split_block_B_into_R_and_BminR(B, m_blocks[B].start_bottom_states, first_unmarked_bottom_state, m_R
                                    #ifdef TRY_EFFICIENT_SWAP
                                      , Rmarked
                                    #endif
                                    );
          clear_state_counters();
          m_R.clear();
          m_U.clear();
          return block_index_of_R;
        }
    R_handled_and_is_not_state_checking:
        assert(state_checking!=R_status || current_R_incoming_bottom_state_iterator!=first_unmarked_bottom_state || !m_R.todo_is_empty());

#ifndef NDEBUG
        for(state_in_block_pointer si=state_in_block_pointer(m_states.begin()); si.ref_state<m_states.end(); ++si.ref_state)
        {
          if (si.ref_state->block!=B || 0==si.ref_state->no_of_outgoing_inert_transitions)
          {
            assert(undefined==si.ref_state->counter);
            assert(!m_R.find(si));
            assert(!m_U.find(si));
            assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
          }
          else
          {
            switch(si.ref_state->counter)
            {
            case undefined:  assert(!m_U.find(si)); assert(!m_R.find(si));
                             assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
                             break;
            case Rmarked:    assert( m_R.find(si)); assert(!m_U.find(si));
                             // It can happen that the state has a tau-transition to a U-state, then it will be in the m_U_counter_reset_vector.
                             break;
            case 0:          assert(!m_R.find(si)); // It can happen that the state is in U or is not in U
                             #ifdef TRY_EFFICIENT_SWAP
                               assert(m_U.find(si) ||
                                      (si==current_U_outgoing_state && outgoing_action_constellation_check==U_status));
                             #endif
                             assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                             break;
            default:         assert(!m_R.find(si)); assert(!m_U.find(si));
                             assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                             break;
            }
          }
        }
#endif
        // The code for the left co-routine.
        if (incoming_inert_transition_checking==U_status) // 20299 times (large 1394-fin.lts example: 360327 times)
        {
//std::cerr << "U_incoming_inert_transition_checking\n";
            // Algorithm 3, line 3.8, left.
          assert(current_U_incoming_transition_iterator<current_U_incoming_transition_iterator_end);
          assert(m_aut.is_tau(m_aut_apply_hidden_label_map(current_U_incoming_transition_iterator->label())));
            // Check one incoming transition.
            // Algorithm 3, line 3.12, left.
          mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), current_U_incoming_transition_iterator)], add_work(check_complexity::simple_splitB_U__handle_transition_to_U_state, 1), *this);
          current_U_outgoing_state=state_in_block_pointer(m_states.begin()+current_U_incoming_transition_iterator->from());
          assert(m_states[current_U_incoming_transition_iterator->to()].block==B);
          current_U_incoming_transition_iterator++;
//std::cerr << "FROM " << std::distance(m_states.begin(), current_U_outgoing_state.ref_state) << "\n";
          if (current_U_outgoing_state.ref_state->block==B && !(m_preserve_divergence && std::prev(current_U_incoming_transition_iterator)->from()==std::prev(current_U_incoming_transition_iterator)->to()))
          {
            assert(!m_U.find(current_U_outgoing_state));
            if (current_U_outgoing_state.ref_state->counter!=Rmarked)
            {
              if (current_U_outgoing_state.ref_state->counter==undefined) // count(current_U_outgoing_state) is undefined;
              {
                  // Algorithm 3, line 3.13, left.
                  // Algorithm 3, line 3.15 and 3.18, left.
                current_U_outgoing_state.ref_state->counter=current_U_outgoing_state.ref_state->no_of_outgoing_inert_transitions-1;
                m_U_counter_reset_vector.push_back(current_U_outgoing_state);
              }
              else
              {
                  // Algorithm 3, line 3.18, left.
                assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), current_U_outgoing_state) != m_U_counter_reset_vector.end());
                assert(current_U_outgoing_state.ref_state->counter>0);
                current_U_outgoing_state.ref_state->counter--;
              }
//std::cerr << "COUNTER " << current_U_outgoing_state.ref_state->counter << "\n";
                // Algorithm 3, line 3.19, left.
              if (current_U_outgoing_state.ref_state->counter==0)
              {
                if (initializing==R_status || aborted==R_status)
                {
                    // Start searching for an outgoing transition with action a to constellation C.
                  current_U_outgoing_transition_iterator=current_U_outgoing_state.ref_state->start_outgoing_transitions;
                  assert(m_outgoing_transitions.begin()<=current_U_outgoing_transition_iterator);
                  assert(current_U_outgoing_transition_iterator<m_outgoing_transitions.end());
                  current_U_outgoing_transition_iterator_end=(std::next(current_U_outgoing_state.ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(current_U_outgoing_state.ref_state)->start_outgoing_transitions);
                  assert(current_U_outgoing_transition_iterator<current_U_outgoing_transition_iterator_end);
                  assert(m_states.begin()+m_aut.get_transitions()[*current_U_outgoing_transition_iterator->ref_BLC_transitions].from()==current_U_outgoing_state.ref_state);
                  U_status=outgoing_action_constellation_check;
                  goto U_handled_and_is_not_state_checking;
                }
                    // The state can be added to U_todo immediately.
                #ifndef NDEBUG
                      // check that the state has no transition in the splitter
                  for (outgoing_transitions_it out_it=current_U_outgoing_state.ref_state->start_outgoing_transitions;
                           out_it<m_outgoing_transitions.end() && (std::next(current_U_outgoing_state.ref_state)>=m_states.end() || out_it<std::next(current_U_outgoing_state.ref_state)->start_outgoing_transitions); ++out_it)
                  {
                    assert(m_outgoing_transitions.begin()<=out_it);
                    assert(out_it<m_outgoing_transitions.end());
                    assert(&*m_BLC_transitions.begin()<=out_it->ref_BLC_transitions);
                    assert(out_it->ref_BLC_transitions<&*m_BLC_transitions.end());
                    assert(0<=*out_it->ref_BLC_transitions);
                    assert(*out_it->ref_BLC_transitions<m_aut.num_transitions());
                    assert(m_transitions[*out_it->ref_BLC_transitions].ref_outgoing_transitions==out_it);
                    const transition& t=m_aut.get_transitions()[/*initialisation?current_U_outgoing_transition_iterator->transition:*/ *out_it->ref_BLC_transitions];
                    assert(m_states.begin()+t.from()==current_U_outgoing_state.ref_state);
                    if (a==label_or_divergence(t) && C==m_blocks[m_states[t.to()].block].constellation)
                    {
                          // The transition is in the splitter, so it must be in the part of the splitter that is disregarded.
//std::cerr << "State " << std::distance(m_states.begin(), current_U_outgoing_state.ref_state) << " has a transition in the splitter, namely " << m_transitions[*out_it->ref_BLC_transitions].debug_id_short(*this) << '\n';
                      assert(out_it->ref_BLC_transitions>=splitter_end_unmarked_BLC);
                      assert(splitter->start_same_BLC<=out_it->ref_BLC_transitions);
                      assert(out_it->ref_BLC_transitions<splitter->end_same_BLC);
                    }
                  }
                #endif
                m_U.add_todo(current_U_outgoing_state);
                    // Algorithm 3, line 3.10 and line 3.11 left.
                if (m_U.size()>static_cast<state_index>(max_U_nonbottom_size))
                {
                  assert(aborted!=R_status); assert(aborted_after_initialisation!=R_status);
                  U_status=aborted_after_initialisation;
                  goto U_handled_and_is_not_state_checking;
                }
              }
            }
            else assert(m_R.find(current_U_outgoing_state));
          }
          if (current_U_incoming_transition_iterator!=current_U_incoming_transition_iterator_end &&
                m_aut.is_tau(m_aut_apply_hidden_label_map(current_U_incoming_transition_iterator->label())))
          {
            assert(incoming_inert_transition_checking==U_status);
            goto U_handled_and_is_not_state_checking;
          }
          U_status=state_checking;
        }
        else if (state_checking==U_status) // 18920 times (large 1394-fin.lts example: 340893 times)
        {
//std::cerr << "U_state_checking\n";

            // Algorithm 3, line 3.23 and line 3.24, left.
          const state_in_block_pointer s=(current_U_incoming_bottom_state_iterator<m_blocks[B].start_non_bottom_states
                               ? *current_U_incoming_bottom_state_iterator++
                               : m_U.move_from_todo());
          assert(!m_R.find(s));
          mCRL2complexity(s.ref_state, add_work(check_complexity::simple_splitB_U__find_predecessors, 1), *this);
//std::cerr << "U insert/ U_todo_remove: " << s << "\n";
          current_U_incoming_transition_iterator=s.ref_state->start_incoming_transitions;
          current_U_incoming_transition_iterator_end=(std::next(s.ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s.ref_state)->start_incoming_transitions);
          if (current_U_incoming_transition_iterator!=current_U_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map(current_U_incoming_transition_iterator->label())))
          {
            U_status=incoming_inert_transition_checking;
            goto U_handled_and_is_not_state_checking;
          }
        }
        else if (aborted_after_initialisation==U_status) // 6284 times (large 1394-fin.lts example: 2500 times)
        {
          goto U_handled_and_is_not_state_checking;
        }
        else
        {
          assert(outgoing_action_constellation_check==U_status); // 911 times (large 1394-fin.lts example: 912 times)
//std::cerr << "U_outgoing_action_constellation_check\n";
          assert(current_U_outgoing_transition_iterator!=current_U_outgoing_transition_iterator_end);
            // will only be used if the transitions are not constellation-inert:
          assert(!m_aut.is_tau(a) || m_blocks[B].constellation!=C);
            // assert(splitter_end_unmarked_BLC == old value of splitter->start_marked_BLC); -- can no longer be checked
          #ifndef NDEBUG
            mCRL2complexity((&m_transitions[/*initialisation?current_U_outgoing_transition_iterator->transition:*/ *current_U_outgoing_transition_iterator->ref_BLC_transitions]), add_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, 1), *this);
              // This is one step in the coroutine, so we should assign the work to exactly one transition.
              // But to make sure, we also mark the other transitions that we skipped in the optimisation.
            for (outgoing_transitions_it out_it=current_U_outgoing_transition_iterator; out_it<current_U_outgoing_transition_iterator->start_same_saC; )
            {
              ++out_it;
              mCRL2complexity(&m_transitions[/*initialisation?out_it->transition:*/ *out_it->ref_BLC_transitions], add_work_notemporary(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, 1), *this);
            }
          #endif
          const transition& t_local=m_aut.get_transitions()
                          [/*initialisation
                               ?current_U_outgoing_transition_iterator->transition
                               :*/ *current_U_outgoing_transition_iterator->ref_BLC_transitions];
          current_U_outgoing_transition_iterator=current_U_outgoing_transition_iterator->start_same_saC; // This is an optimisation.
          ++current_U_outgoing_transition_iterator;

          assert(m_states.begin()+t_local.from()==current_U_outgoing_state.ref_state);
          assert(m_branching);
          if (m_blocks[m_states[t_local.to()].block].constellation==C &&
                label_or_divergence(t_local) == a)
          {
                // This state must be blocked.
            #ifdef TRY_EFFICIENT_SWAP
              if(Rmarked!=current_U_outgoing_state.ref_state->counter)
              {
                current_U_outgoing_state.ref_state->counter=std::numeric_limits<std::make_signed<transition_index>::type>::max();
                assert(0!=current_U_outgoing_state.ref_state->counter);
                assert(undefined!=current_U_outgoing_state.ref_state->counter);
                assert(Rmarked!=current_U_outgoing_state.ref_state->counter);
              }
            #endif
          }
          else if (current_U_outgoing_transition_iterator==current_U_outgoing_transition_iterator_end)
          {
            assert(!m_U.find(current_U_outgoing_state));
//std::cerr << "U_todo4 insert: " << std::distance(m_states.begin(), current_U_outgoing_state.ref_state) << "   " << m_U.size() << "    " << number_of_states_in_block(B) << "\n";
            m_U.add_todo(current_U_outgoing_state);
              // Algorithm 3, line 3.10 and line 3.11 left.
            if (m_U.size()>static_cast<state_index>(max_U_nonbottom_size))
            {
              assert(aborted!=R_status); assert(aborted_after_initialisation!=R_status);
              U_status=aborted_after_initialisation;
              goto U_handled_and_is_not_state_checking;
            }
          }
          else
          {
            goto U_handled_and_is_not_state_checking;
          }

          if (current_U_incoming_transition_iterator!=current_U_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map(current_U_incoming_transition_iterator->label())))
          {
            U_status = incoming_inert_transition_checking;
            goto U_handled_and_is_not_state_checking;
          }
          U_status=state_checking;
        }
        assert(state_checking==U_status);
        if (current_U_incoming_bottom_state_iterator==m_blocks[B].start_non_bottom_states && m_U.todo_is_empty())
        {
          // split_block B into U and B\U.
//std::cerr << "U_todo empty:\n";
          assert(0 < std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states) + m_U.size());
          assert(std::distance(first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states) + m_U.size() <= number_of_states_in_block(B)/2);
          split_block_B_into_R_and_BminR(B, first_unmarked_bottom_state, m_blocks[B].start_non_bottom_states, m_U
                                         #ifdef TRY_EFFICIENT_SWAP
                                           , 0
                                         #endif
                                         );
          clear_state_counters();
          m_R.clear();
          m_U.clear();
          return B;
        }
    U_handled_and_is_not_state_checking:
        assert(state_checking!=U_status || current_U_incoming_bottom_state_iterator!=m_blocks[B].start_non_bottom_states || !m_U.todo_is_empty());
      }
      assert(0);
    }

    // Make this transition non-inert.
    // The transition must go from one block to another but it cannot be constellation-inert yet.
    void make_transition_non_inert(const transition& t)
    {
//std::cerr << "Transition " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << " becomes non-inert.\n";
      assert(is_inert_during_init(t));
      assert(m_states[t.to()].block!=m_states[t.from()].block);
      assert(m_blocks[m_states[t.to()].block].constellation == m_blocks[m_states[t.from()].block].constellation);
      m_states[t.from()].no_of_outgoing_inert_transitions--;
    }

    void change_non_bottom_state_to_bottom_state(const fixed_vector<state_type_gj>::iterator si)
    {
      assert(m_states.begin()<=si);
      assert(si<m_states.end());
      // Move this former non-bottom state to the bottom states.
      // The block of si is not yet inserted into the set of blocks with new bottom states.
      block_index bi = si->block;
//std::cerr << si->debug_id(*this) << " becomes a new bottom state of " << m_blocks[bi].debug_id(*this) << ".\n";
      assert(0 == si->no_of_outgoing_inert_transitions);
      assert(!m_blocks[bi].contains_new_bottom_states);
      swap_states_in_states_in_block(si->ref_states_in_blocks, m_blocks[bi].start_non_bottom_states);
      m_blocks[bi].start_non_bottom_states++;
    }

    // Split block B in R, being the inert-tau transitive closure of M contains
    // states that must be in block, and M\R. M_nonmarked, minus those in unmarked_blocker, are those in the other block.
    // The splitting is done in time O(min(|R|,|B\R|). Returns the block index of the R-block.
    // The splitter is marked as stable and moved to a suitable position
    // at or near the beginning of the list of BLC sets.
    // (If !split_off_new_bottom_states, it was already the first BLC set.)
    // This means that the order of main splitter/co-splitter is not maintained;
    // the recommended course of actions for the caller is:
    // first remember in which slice of m_BLC_transitions the co-splitter is stored,
    // then call splitB(), then recover the co-splitter from this slice.
    // (The co-splitter part that starts in the U-subblock is actually already stable
    // and just needs to be marked so;
    // the part that starts in the R-subblock may lead to another call of splitB().)
    block_index splitB(linked_list<BLC_indicators>::iterator splitter,
                       fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state,
                       const BLC_list_iterator splitter_end_unmarked_BLC /* = splitter.start_marked_BLC -- but this default argument is not allowed */,
                       #ifdef CO_SPLITTER_IN_BLC_LIST
                         constellation_index old_constellation = null_constellation,
                       #else
                         std::function<void(const block_index, const block_index, const transition_index, const transition_index)>
                                                 update_block_label_to_cotransition = [](const block_index, const block_index, const transition_index, const transition_index){},
                       #endif
                       // const bool initialisation=false,
                       //std::function<void(const transition_index, const transition_index, const block_index)> process_transition=
                       //                                 [](const transition_index, const transition_index, const block_index){},
                       const bool split_off_new_bottom_states = true)
    {
//std::cerr << "splitB(splitter = " << splitter->debug_id(*this) << ", first_unmarked_bottom_state = " << first_unmarked_bottom_state->ref_state->debug_id(*this) << ", splitter_end_unmarked_BLC = "
//<< (split_off_new_bottom_states && splitter_end_unmarked_BLC == splitter->start_marked_BLC ? "start_marked_BLC" : (splitter_end_unmarked_BLC == splitter->start_same_BLC ? "start_same_BLC" : "?")) << ", ..., split_off_new_bottom_states = " << split_off_new_bottom_states << ")\n";
      const block_index B = m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].block;
//std::cerr << (m_branching ? "Marked bottom states:" : "Marked states:"); for (fixed_vector<state_in_block_pointer>::const_iterator it=m_blocks[B].start_bottom_states; it!=first_unmarked_bottom_state; ++it) { std::cerr << ' ' << std::distance(m_states.begin(), it->ref_state); }
//std::cerr << (m_branching ? "\nUnmarked bottom states:" : "\nUnmarked states:"); for (fixed_vector<state_in_block_pointer>::const_iterator it=first_unmarked_bottom_state; it!=m_blocks[B].start_non_bottom_states; ++it) { std::cerr << ' ' << std::distance(m_states.begin(), it->ref_state); } std::cerr << '\n';
//if (m_branching) { std::cerr << "Additionally, " << m_R.size() << " non-bottom states have been marked.\n"; }
      assert(splitter->is_stable()); // The splitter has already been declared stable and moved to the beginning of its list of BLC sets by not_all_bottom_states_are_touched().
      if (1 >= number_of_states_in_block(B))
      {
        mCRL2log(log::debug) << "Trying to split up singleton block " << B << '\n';
        clear_state_counters();
        return null_block;
      }
      block_index R_block = simple_splitB(B, splitter, first_unmarked_bottom_state, splitter_end_unmarked_BLC);
      const block_index bi = m_blocks.size() - 1;
//std::cerr << "Split block of size " << number_of_states_in_block(B) + number_of_states_in_block(bi) << " taking away " << number_of_states_in_block(bi) << " states\n";
      assert(bi == R_block || B == R_block);
      assert(number_of_states_in_block(B) >= number_of_states_in_block(bi));

      // Because we visit all states of block bi and almost all their incoming and outgoing transitions,
      // we subsume all this bookkeeping in a single block counter:
      mCRL2complexity(&m_blocks[bi], add_work(check_complexity::splitB__update_BLC_of_smaller_subblock, check_complexity::log_n - check_complexity::ilog2(number_of_states_in_block(bi))), *this);
      // Update the LBC_list, and bottom states, and invariant on inert transitions.
      const fixed_vector<state_in_block_pointer>::iterator start_new_bottom_states=m_blocks[R_block].start_non_bottom_states;
      linked_list<BLC_indicators>::iterator R_to_U_tau_splitter = m_blocks[R_block].block_to_constellation.end();
      assert(m_blocks[bi].block_to_constellation.empty());
      if (m_branching)
      {
        // insert an empty BLC set into m_blocks[bi].block_to_constellation
        // for the inert transitions out of m_blocks[bi],
        // to avoid the need to check whether such a BLC set already exists
        // in update_the_doubly_linked_list_LBC_new_block().
        // This set, if it remains empty, will need to be deleted
        // after updating the BLC sets.
        // (It may happen that there are actually no inert transitions. Then
        // the new list will contain some dummy empty element, which should
        // remain empty until it is deleted again later.)
        assert(!m_blocks[B].block_to_constellation.empty());
        linked_list<BLC_indicators>::const_iterator perhaps_inert_ind=m_blocks[B].block_to_constellation.begin();
        m_blocks[bi].block_to_constellation.emplace_front(perhaps_inert_ind->start_same_BLC, perhaps_inert_ind->start_same_BLC, true);
      }
      bool skip_transitions_in_splitter = false;
      #ifdef CO_SPLITTER_IN_BLC_LIST
        assert(m_BLC_indicators_to_be_deleted.empty());
      #endif
      assert(split_off_new_bottom_states || m_blocks[B].block_to_constellation.begin()==splitter);
      if (split_off_new_bottom_states && bi == R_block)
      {
        // move splitter as a whole from its current list to the new list where it will be part of:
        // (This only works if the whole splitter BLC set has moved, i.e. only if split_off_new_bottom_states == true.)
        assert(!m_branching || !m_blocks[R_block].block_to_constellation.empty());
        m_blocks[R_block].block_to_constellation.splice_to_after(
                                m_branching ? m_blocks[R_block].block_to_constellation.begin()
                                            : m_blocks[R_block].block_to_constellation.end(),
                                m_blocks[B].block_to_constellation, splitter);
        skip_transitions_in_splitter = true;
      }
      // Recall new LBC positions.
      assert(m_co_splitters_to_be_checked.empty());
      for(fixed_vector<state_in_block_pointer>::iterator ssi=m_blocks[bi].start_bottom_states;
                                                      ssi!=m_blocks[bi].end_states;
                                                      ++ssi)
      {
        state_type_gj& s=*ssi->ref_state;
        assert(s.ref_states_in_blocks == ssi);
        // mCRL2complexity(s, add_work(..., max_bi_counter), *this);
            // is subsumed in the above call
        assert(s.block == bi);

        const outgoing_transitions_it end_it=(std::next(ssi->ref_state)==m_states.end()) ? m_outgoing_transitions.end() : std::next(ssi->ref_state)->start_outgoing_transitions;
        // if (!initialisation)  // update the BLC_lists.
        {
          for(outgoing_transitions_it ti=s.start_outgoing_transitions; ti!=end_it; ti++)
          {
            assert(m_states.begin()+m_aut.get_transitions()[*ti->ref_BLC_transitions].from()==ssi->ref_state);
            // mCRL2complexity(&m_transitions[*ti->ref_BLC_transitions], add_work(..., max_bi_counter), *this);
                // is subsumed in the above call
            transition_index old_remaining_transition;
            if (!skip_transitions_in_splitter || m_transitions[*ti->ref_BLC_transitions].transitions_per_block_to_constellation != splitter)
            {
              old_remaining_transition=update_the_doubly_linked_list_LBC_new_block(B, bi, *ti->ref_BLC_transitions
                            #ifdef CO_SPLITTER_IN_BLC_LIST
                              , old_constellation
                            #endif
                            );
            }
            else
            {
              // No need to move the splitter transitions individually.
              old_remaining_transition = null_transition;
            }
            //process_transition(*ti->ref_BLC_transitions, old_remaining_transition, B);
            #ifdef CO_SPLITTER_IN_BLC_LIST
              (void) old_remaining_transition; // avoid unused variable warning
            #else
              update_block_label_to_cotransition(B, bi, *ti->ref_BLC_transitions, old_remaining_transition);
            #endif
          }
        }

        // Situation below is only relevant if the new block contains the R-states:
        if (bi == R_block && 0 < s.no_of_outgoing_inert_transitions)
        {
          assert(ssi >= m_blocks[R_block].start_non_bottom_states);
          // si is a non_bottom_state in the smallest block containing M..
          bool non_bottom_state_becomes_bottom_state = true;

          for(outgoing_transitions_it ti=s.start_outgoing_transitions; ti!=end_it; ti++)
          {
            // mCRL2complexity(&m_transitions[*ti->ref_BLC_transitions], add_work(..., max_bi_counter), *this);
                // is subsumed in the above call
            const transition& t=m_aut.get_transitions()[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions];
            assert(m_states.begin()+t.from()==ssi->ref_state);
            if (is_inert_during_init_if_branching(t))
            {
              if (m_states[t.to()].block==B)
              {
                // This is a transition that has become non-block-inert.
                // (However, it is still constellation-inert.)
                make_transition_non_inert(t);
                if (split_off_new_bottom_states)
                {
                  const linked_list<BLC_indicators>::iterator new_splitter=m_transitions[*ti->ref_BLC_transitions].transitions_per_block_to_constellation;
                  assert(m_blocks[R_block].block_to_constellation.begin()==new_splitter);
                  if (R_to_U_tau_splitter==m_blocks[R_block].block_to_constellation.end())
                  {
                    R_to_U_tau_splitter=new_splitter;
                    R_to_U_tau_splitter->make_unstable();
                  }
                  else
                  {
                    assert(R_to_U_tau_splitter==new_splitter);
                    assert(!R_to_U_tau_splitter->is_stable());
                  }
                  // immediately mark this transition, in case we get new bottom states:
                  mark_BLC_transition(ti);
                }
              }
              else if (m_states[t.to()].block==R_block)
              {
                non_bottom_state_becomes_bottom_state=false; // There is an outgoing inert tau. State remains non-bottom.
              }
            }
          }
          if (non_bottom_state_becomes_bottom_state)
          {
            // The state at si has become a bottom_state.
            assert(!split_off_new_bottom_states || m_blocks[R_block].block_to_constellation.end()!=R_to_U_tau_splitter);
            change_non_bottom_state_to_bottom_state(ssi->ref_state);
          }
        }

        // Investigate the incoming formerly inert tau transitions.
        if (bi != R_block && m_blocks[R_block].start_non_bottom_states < m_blocks[R_block].end_states)
        {
          const std::vector<transition>::iterator it_end = std::next(ssi->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(ssi->ref_state)->start_incoming_transitions;
          for(std::vector<transition>::iterator it=s.start_incoming_transitions;
                        it!=it_end; it++)
          {
            const transition& t=*it;
            // mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), it)], add_work(..., max_bi_counter), *this);
                // is subsumed in the above call
            assert(m_states.begin()+t.to()==ssi->ref_state);
            if (!m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())))
            {
              break; // All tau transitions have been investigated.
            }

            const fixed_vector<state_type_gj>::iterator from(m_states.begin()+t.from());
            if (from->block==R_block && !(m_preserve_divergence && from==ssi->ref_state))
            {
              // This transition did become non-block-inert.
              // (However, it is still constellation-inert.)
              make_transition_non_inert(t);
              if (split_off_new_bottom_states)
              {
                const linked_list<BLC_indicators>::iterator new_splitter=m_transitions[std::distance(m_aut.get_transitions().begin(), it)].transitions_per_block_to_constellation;
                assert(m_blocks[R_block].block_to_constellation.begin()==new_splitter);
                if (R_to_U_tau_splitter==m_blocks[R_block].block_to_constellation.end())
                {
                  R_to_U_tau_splitter=new_splitter;
                  R_to_U_tau_splitter->make_unstable();
                }
                else
                {
                  assert(R_to_U_tau_splitter==new_splitter);
                  assert(!R_to_U_tau_splitter->is_stable());
                }
                // immediately mark this transition, in case we get new bottom states:
                mark_BLC_transition(m_transitions[std::distance(m_aut.get_transitions().begin(), it)].ref_outgoing_transitions);
              }
              // Check whether from is a new bottom state.
              if (from->no_of_outgoing_inert_transitions==0)
              {
                // This state has no more outgoing inert transitions. It becomes a bottom state.
                change_non_bottom_state_to_bottom_state(from);
              }
            }
          }
        }
      }
      assert(m_blocks[R_block].start_bottom_states < m_blocks[R_block].start_non_bottom_states);

      if (m_branching)
      {
        // Before the loop we inserted an empty BLC set for the inert transitions
        // into m_blocks[bi].block_to_constellation.
        // If it is still empty, we have to remove it again.
        assert(!m_blocks[bi].block_to_constellation.empty());
        linked_list<BLC_indicators>::iterator perhaps_inert_ind=m_blocks[bi].block_to_constellation.begin();
        if (perhaps_inert_ind->start_same_BLC==perhaps_inert_ind->end_same_BLC)
        {
          assert(perhaps_inert_ind->is_stable());
          m_blocks[bi].block_to_constellation.erase(perhaps_inert_ind);
        }
        else
        {
          // assert(perhaps_inert_ind->is_stable()); -- it may be unstable if there are tau-transitions from R_block==bi to U
          #ifndef NDEBUG
            const transition& inert_t=m_aut.get_transitions()[*perhaps_inert_ind->start_same_BLC];
            assert(is_inert_during_init(inert_t));
            assert(bi==m_states[inert_t.from()].block);
            assert(m_blocks[bi].constellation==m_blocks[m_states[inert_t.to()].block].constellation);
          #endif
        }
      }
      #ifdef CO_SPLITTER_IN_BLC_LIST
        for (std::vector<linked_list<BLC_indicators>::iterator>::iterator it = m_BLC_indicators_to_be_deleted.begin(); it < m_BLC_indicators_to_be_deleted.end(); ++it)
        {
          assert((*it)->start_same_BLC==(*it)->end_same_BLC);
          m_blocks[B].block_to_constellation.erase(*it);
        }
        clear(m_BLC_indicators_to_be_deleted);
        #ifndef NDEBUG
          for (const BLC_indicators& B_it : m_blocks[B].block_to_constellation)
          {
            assert(B_it.start_same_BLC < B_it.end_same_BLC);
          }
          for (const BLC_indicators& bi_it : m_blocks[bi].block_to_constellation)
          {
            assert(bi_it.start_same_BLC < bi_it.end_same_BLC);
          }
        #endif
      #endif
      for (const std::pair<BLC_list_iterator, block_index>& co_splitter_check: m_co_splitters_to_be_checked)
      {
        assert(&*m_BLC_transitions.begin()<=co_splitter_check.first);
        assert(co_splitter_check.first<&*m_BLC_transitions.end());
        const transition_index co_transition=*co_splitter_check.first;
        const block_index from_block=co_splitter_check.second;
        assert(B==from_block || bi==from_block);
        const transition& co_t=m_aut.get_transitions()[co_transition];
        assert(old_constellation==m_blocks[m_states[co_t.to()].block].constellation);
        if (m_states[co_t.from()].block==from_block)
        {
          linked_list<BLC_indicators>::iterator co_splitter=m_transitions[co_transition].transitions_per_block_to_constellation;
//std::cerr << "Checking co-splitter " << co_splitter->debug_id(*this);
          assert(co_splitter->start_same_BLC<=co_splitter_check.first);
          assert(co_splitter_check.first<co_splitter->end_same_BLC);
          assert(!co_splitter->is_stable());
          const transition& sp_t=m_aut.get_transitions()[*splitter->start_same_BLC];
          assert(R_block==m_states[sp_t.from()].block);
          if (label_or_divergence(sp_t)==label_or_divergence(co_t) && m_constellations.size()-1==m_blocks[m_states[sp_t.to()].block].constellation)
          {
            // This is actually (a part of) the co-splitter belonging to the current main splitter
            // So we leave the co-splitter as it is.
//std::cerr << ": belongs to the main splitter used in the current split\n";
          }
          else
          {
            linked_list<BLC_indicators>& btc=m_blocks[from_block].block_to_constellation;
            linked_list<BLC_indicators>::const_iterator main_splitter=btc.next(co_splitter);
            const transition* t;
            if (m_blocks[from_block].block_to_constellation.end()==main_splitter ||
                (t=&m_aut.get_transitions()[*main_splitter->start_same_BLC],
                 assert(m_states[t->from()].block==from_block),
                 label_or_divergence(co_t)!=label_or_divergence(*t)) ||
                m_constellations.size()-1!=m_blocks[m_states[t->to()].block].constellation)
            {
              // co_splitter does not have a corresponding main splitter
//std::cerr << ": does not have a corresponding main splitter\n";
              assert(!has_marked_transitions(*co_splitter));
              co_splitter->make_stable();
              if (btc.begin()!=co_splitter)
              {
                linked_list<BLC_indicators>::iterator move_to=btc.end();
                if (m_branching)
                {
                  t=&m_aut.get_transitions()[*btc.begin()->start_same_BLC];
                  assert(m_states[t->from()].block==from_block);
                  if (is_inert_during_init_if_branching(*t) &&
                      m_blocks[from_block].constellation==m_blocks[m_states[t->to()].block].constellation)
                  {
                    move_to=btc.begin();
                  }
                }
                btc.splice_to_after(move_to, btc, co_splitter);
              }
            }
//else { std::cerr << ": its main splitter is " << main_splitter->debug_id(*this) << '\n'; }
          }
        }
        else
        {
          assert(bi==m_states[co_t.from()].block);
        }
      }
      clear(m_co_splitters_to_be_checked);

      #ifndef NDEBUG
        unsigned const max_block(check_complexity::log_n - check_complexity::ilog2(number_of_states_in_block(bi)));
        if (bi == R_block)
        {
          // account for the work in R
          for (fixed_vector<state_in_block_pointer>::iterator s=m_blocks[bi].start_bottom_states;
                              s!=m_blocks[bi].end_states; ++s)
          {
            mCRL2complexity(s->ref_state, finalise_work(check_complexity::simple_splitB_R__find_predecessors, check_complexity::simple_splitB__find_predecessors_of_R_or_U_state, max_block), *this);
            // incoming tau-transitions of s
            const std::vector<transition>::iterator in_ti_end = std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
            for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
            {
              if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
              mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], finalise_work(check_complexity::simple_splitB_R__handle_transition_to_R_state, check_complexity::simple_splitB__handle_transition_to_R_or_U_state, max_block), *this);
            }
            // outgoing transitions of s
            const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
            for (outgoing_transitions_it ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
            {
              mCRL2complexity(&m_transitions[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions], finalise_work(check_complexity::simple_splitB_R__handle_transition_from_R_state, check_complexity::simple_splitB__handle_transition_from_R_or_U_state, max_block), *this);
              // We also need to cancel the work on outgoing transitions of U-state candidates that turned out to be new bottom states:
              mCRL2complexity(&m_transitions[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions], cancel_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state), *this);
            }
          }
          // ensure not too much work has been done on U
          for (fixed_vector<state_in_block_pointer>::iterator s=m_blocks[B].start_bottom_states;
                              s!=m_blocks[B].end_states; ++s)
          {
            mCRL2complexity(s->ref_state, cancel_work(check_complexity::simple_splitB_U__find_bottom_state), *this);
            mCRL2complexity(s->ref_state, cancel_work(check_complexity::simple_splitB_U__find_predecessors), *this);
            // incoming tau-transitions of s
            const std::vector<transition>::iterator in_ti_end=std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
            for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
            {
              if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
              mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], cancel_work(check_complexity::simple_splitB_U__handle_transition_to_U_state), *this);
            }
            // outgoing transitions of s
            const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
            for (outgoing_transitions_it ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
            {
              mCRL2complexity(&m_transitions[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions], cancel_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state), *this);
            }
          }
        }
        else
        {
          // account for the work in U
          for (fixed_vector<state_in_block_pointer>::iterator s=m_blocks[bi].start_bottom_states;
                              s!=m_blocks[bi].end_states; ++s)
          {
            mCRL2complexity(s->ref_state, finalise_work(check_complexity::simple_splitB_U__find_bottom_state, check_complexity::simple_splitB__find_bottom_state, max_block), *this);
            mCRL2complexity(s->ref_state, finalise_work(check_complexity::simple_splitB_U__find_predecessors, check_complexity::simple_splitB__find_predecessors_of_R_or_U_state, max_block), *this);
            // incoming tau-transitions of s
            const std::vector<transition>::iterator in_ti_end = std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
            for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
            {
              if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
              mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], finalise_work(check_complexity::simple_splitB_U__handle_transition_to_U_state, check_complexity::simple_splitB__handle_transition_to_R_or_U_state, max_block), *this);
            }
            // outgoing transitions of s
            const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
            for (outgoing_transitions_it ti = s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
            {
              mCRL2complexity(&m_transitions[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions], finalise_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, check_complexity::simple_splitB__handle_transition_from_R_or_U_state, max_block), *this);
            }
          }
          // ensure not too much work has been done on R
          for (fixed_vector<state_in_block_pointer>::iterator s=m_blocks[B].start_bottom_states;
                              s!=m_blocks[B].end_states; ++s)
          {
            mCRL2complexity(s->ref_state, cancel_work(check_complexity::simple_splitB_R__find_predecessors), *this);
            // incoming tau-transitions of s
            const std::vector<transition>::iterator in_ti_end = std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
            for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
            {
              if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
              mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], cancel_work(check_complexity::simple_splitB_R__handle_transition_to_R_state), *this);
            }
            // outgoing transitions of s
            const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
            for (outgoing_transitions_it ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
            {
              mCRL2complexity(&m_transitions[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions], cancel_work(check_complexity::simple_splitB_R__handle_transition_from_R_state), *this);
              // We also need to move the work on outgoing transitions of U-state candidates that turned out to be new bottom states:
              mCRL2complexity(&m_transitions[/*initialisation?ti->transition:*/ *ti->ref_BLC_transitions], finalise_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, check_complexity::simple_splitB__test_outgoing_transitions_found_new_bottom_state,
                      0==s->ref_state->no_of_outgoing_inert_transitions ? 1 : 0), *this);
            }
          }
        }
        check_complexity::check_temporary_work();
      #endif // ifndef NDEBUG

      if (!split_off_new_bottom_states)
      {
        assert(m_blocks[R_block].block_to_constellation.end()==R_to_U_tau_splitter);
      }
      else if (start_new_bottom_states<m_blocks[R_block].start_non_bottom_states)
      {
        // There are new bottom states, and we have to separate them immediately from the old bottom states.
        // This will make sure that the next call of stabilizeB() will not encounter blocks with both old and new bottom states.
        assert(!m_blocks[R_block].block_to_constellation.empty());
        assert(m_blocks[R_block].block_to_constellation.begin()==R_to_U_tau_splitter);
        assert(has_marked_transitions(*R_to_U_tau_splitter));

        block_index new_bottom_block;
        if (start_new_bottom_states == m_blocks[R_block].start_bottom_states)
        {
          // all bottom states in this block are new
          R_to_U_tau_splitter->make_stable();
          new_bottom_block = R_block;
          R_block = null_block;
        }
        else
        {
          assert(number_of_states_in_block(R_block) > 1);
          // Some tau-transitions from R to U may come out of states that are not (yet) new bottom states.
          // Therefore we still have to go through the movement of constructing m_R:
          const BLC_list_iterator splitter_end_unmarked_BLC = R_to_U_tau_splitter->start_same_BLC;
          first_unmarked_bottom_state = not_all_bottom_states_are_touched(R_to_U_tau_splitter
                      #ifndef NDEBUG
                        , splitter_end_unmarked_BLC
                      #endif
                      );
          assert(std::distance(start_new_bottom_states, m_blocks[R_block].start_non_bottom_states) ==
                 std::distance(m_blocks[R_block].start_bottom_states, first_unmarked_bottom_state));
          assert(m_blocks[R_block].start_bottom_states < first_unmarked_bottom_state);
          assert(std::distance(start_new_bottom_states, m_blocks[R_block].start_non_bottom_states)==
                 std::distance(m_blocks[R_block].start_bottom_states, first_unmarked_bottom_state));
          new_bottom_block = splitB(R_to_U_tau_splitter, first_unmarked_bottom_state,
                                    splitter_end_unmarked_BLC,
                                    #ifdef CO_SPLITTER_IN_BLC_LIST
                                      old_constellation,
                                    #else
                                      update_block_label_to_cotransition,
                                    #endif
                                    //process_transition,
                                    false);
          if (R_block == new_bottom_block)
          {
            R_block = m_blocks.size() - 1;
          }
        }
        assert(0 <= new_bottom_block);  assert(new_bottom_block < m_blocks.size());
        assert(!m_blocks[new_bottom_block].contains_new_bottom_states);
//std::cerr << "new_bottom_block = " << new_bottom_block << ", R_block = " << static_cast<std::make_signed<block_index>::type>(R_block) << '\n';
        m_blocks[new_bottom_block].contains_new_bottom_states = true;
        m_blocks_with_new_bottom_states.push_back(new_bottom_block);
      }
      else
      {
        // No new bottom states found.
        // Still there might be some transitions that became non-inert.
        if (m_blocks[R_block].block_to_constellation.end()!=R_to_U_tau_splitter)
        {
          // unmark the transitions from R to U.
          // We do not need to stabilize the (formerly inert) tau-transitions from R to U,
          // because they are still in the same constellation.
          R_to_U_tau_splitter->make_stable();
          assert(m_blocks[R_block].block_to_constellation.begin()==R_to_U_tau_splitter);
        }
      }

      return R_block;
    }

    /* void accumulate_entries_into_not_investigated(std::vector<label_count_sum_triple>& action_counter,
                            const std::vector<block_index>& todo_stack)
    {
      transition_index sum=0;
      for(block_index index: todo_stack)
      {
        // The caller has to account for this work.
        // Typically, it can be accounted for because todo_stack has been filled
        // by actions that are counted.
        action_counter[index].not_investigated=sum;
        sum=sum+action_counter[index].label_counter;
      }
    }

    void accumulate_entries(std::vector<transition_index>& counter)
    {
      transition_index sum=0;
      for(transition_index& index: counter)
      {
        transition_index n=index;
        index=sum;
        sum=sum+n;
      }
    }

    void reset_entries(std::vector<label_count_sum_triple>& action_counter,
                       std::vector<block_index>& todo_stack)
    {
      for(block_index index: todo_stack)
      {
        // To account for this work, we need to ensure that todo_stack has only
        // been filled by actions that have been accounted for already.
        // it is not necessary to reset the cumulative_label_counter;
        action_counter[index].label_counter=0;
      }
      todo_stack.clear();
    } */

    transition_index accumulate_entries(std::vector<transition_index>& action_counter,
                                   const std::vector<label_index>& todo_stack) const
    {
      transition_index sum=0;
      for(label_index index: todo_stack)
      {
        transition_index n=sum;
        sum=sum+action_counter[index];
        action_counter[index]=n;
      }
      return sum;
    }

/*    // Group the elements from begin up to end, using a range from [0,number of blocks),
    // where each transition pinpointed by the iterator has as value its source block.

    void group_in_situ(const std::vector<transition_index>::iterator begin,
                       const std::vector<transition_index>::iterator end,
                       std::vector<block_index>& todo_stack,
                       std::vector<label_count_sum_triple>& value_counter)
    {
      // Initialise the action counter.
      // The work in reset_entries() can be subsumed under mCRL2complexity calls
      // that have been issued in earlier executions of group_in_situ().
      reset_entries(value_counter, todo_stack);
      #ifndef NDEBUG
        const constellation_index new_constellation = m_constellations.size()-1;
        const unsigned max_C = check_complexity::log_n - check_complexity::ilog2(number_of_states_in_constellation(new_constellation));
      #endif
      for(std::vector<transition_index>::iterator i=begin; i!=end; ++i)
      {
        const transition& t = m_aut.get_transitions()[*i];
        assert(m_blocks[m_states[t.to()].block].constellation == new_constellation);
        mCRL2complexity(&m_transitions[*i], add_work(check_complexity::group_in_situ__count_transitions_per_block, max_C), *this);
        const block_index n=m_states[t.from()].block;

        if (value_counter[n].label_counter==0)
        {
          todo_stack.push_back(n);
        }
        value_counter[n].label_counter++;
      }

      // The work in accumulate_entries_into_not_investigated() can be subsumed
      // under the above call to mCRL2complexity, because an entry in todo_stack
      // is only made if there is at least one transition from that block.
      accumulate_entries_into_not_investigated(value_counter, todo_stack);

      std::vector<block_index>::iterator current_value=todo_stack.begin();
      for(std::vector<transition_index>::iterator i=begin; i!=end; )
      {
        mCRL2complexity(&m_transitions[*i], add_work(check_complexity::group_in_situ__swap_transition, max_C), *this);
        block_index n=m_states[m_aut.get_transitions()[*i].from()].block;
        if (n==*current_value)
        {
          value_counter[n].label_counter--;
          value_counter[n].not_investigated++;
          ++i;
          while (assert(current_value!=todo_stack.end()), value_counter[n].label_counter==0)
          {
            #ifndef NDEBUG
              // This work needs to be assigned to some transition from block n.
              // Just to make sure we assign it to all such transitions:
              std::vector<transition_index>::iterator work_i = i;
              assert(begin != work_i);
              --work_i;
              assert(m_states[m_aut.get_transitions()[*work_i].from()].block == n);
              do
              {
                mCRL2complexity(&m_transitions[*work_i], add_work(check_complexity::group_in_situ__skip_to_next_block, max_C), *this);
              }
              while (begin != work_i && m_states[m_aut.get_transitions()[*--work_i].from()].block == n);
            #endif
            current_value++;
            if (current_value!=todo_stack.end())
            {
              n = *current_value;
              i=begin+value_counter[n].not_investigated; // Jump to the first non investigated action.
            }
            else
            {
              assert(i == end);
              break; // exit the while and the for loop.
            }
          }
        }
        else
        {
          // Find the first transition with a different label than t.label to swap with.
          std::vector<transition_index>::iterator new_position=begin+value_counter[n].not_investigated;
          while (m_states[m_aut.get_transitions()[*new_position].from()].block==n)
          {
            mCRL2complexity(&m_transitions[*new_position], add_work(check_complexity::group_in_situ__swap_transition, max_C), *this);
            value_counter[n].not_investigated++;
            value_counter[n].label_counter--;
            new_position++;
            assert(new_position!=end);
          }
          assert(value_counter[n].label_counter>0);
          std::swap(*i, *new_position);
          value_counter[n].not_investigated++;
          value_counter[n].label_counter--;
        }
      }
    } */

//================================================= Create initial partition ========================================================
    void create_initial_partition()
    {
      mCRL2log(log::verbose) << "An O(m log n) "
           << (m_branching ? (m_preserve_divergence
                                         ? "divergence-preserving branching "
                                         : "branching ")
                         : "")
           << "bisimulation partitioner created for " << m_aut.num_states()
           << " states and " << m_aut.num_transitions() << " transitions (using the experimental algorithm GJ2024).\n";
      // Initialisation.
      #ifndef NDEBUG
        check_complexity::init(2 * m_aut.num_states()); // we need ``2*'' because there is one additional call to splitB during initialisation
      #endif

      // Initialise m_incoming_(non-)inert-transitions, m_outgoing_transitions, and m_states[si].no_of_outgoing_transitions
      //group_transitions_on_label(m_aut.get_transitions(),
      //                          [](const transition& t){ return m_aut_apply_hidden_label_map(t.label()); },
      //                          m_aut.num_action_labels(), m_aut.tau_label_index()); // sort on label. Tau transitions come first.
      // group_transitions_on_label(m_aut.get_transitions(),
      //                           [](const transition& t){ return t.from(); },
      //                           m_aut.num_states(), 0); // sort on label. Tau transitions come first.
      // group_transitions_on_label_tgt(m_aut.get_transitions(), m_aut.num_action_labels(), m_aut.tau_label_index(), m_aut.num_states()); // sort on label. Tau transitions come first.
      // group_transitions_on_tgt(m_aut.get_transitions(), m_aut.num_action_labels(), m_aut.tau_label_index(), m_aut.num_states()); // sort on label. Tau transitions come first.
      // sort_transitions(m_aut.get_transitions(), lbl_tgt_src);
// David suggests: I think it is enough to sort according to tgt_lbl.
// JFG answers: Agreed. But I believe this will cost performance. For 1394-fin-vvlarge this saves 1 second to sort, but
//                      requires five more seconds to carry out the splitting. Apparently, there is benefit to have src together.
//                      This may have been measured on an older version of the code.
      group_transitions_on_tgt_label(m_aut);
      // sort_transitions(m_aut.get_transitions(), m_aut.hidden_label_set(), tgt_lbl_src); // THIS IS NOW ESSENTIAL.
      // sort_transitions(m_aut.get_transitions(), src_lbl_tgt);
      // sort_transitions(m_aut.get_transitions(), tgt_lbl);
      // sort_transitions(m_aut.get_transitions(), target);

      // Count the number of occurring action labels.
      assert((unsigned) m_preserve_divergence <= 1);

      mCRL2log(log::verbose) << "Start initialisation of the BLC list in the initialisation, after sorting.\n";
      {
        std::vector<label_index> todo_stack_actions;
        std::vector<transition_index> count_transitions_per_action(m_aut.num_action_labels() + (unsigned) m_preserve_divergence, 0);
// David suggests: The above allocation may take time up to O(|Act|).
// This is a place where the number of actions plays a role.
// JFG answers: I think we should accept that the algorithm has a complexity of .... + O(|Act|). Act can be assumed to be smaller
// than m, and n can be assumed to be bigger than 1. In that case O(|Act|) will be subsumed. 
        if (m_branching)
        {
          todo_stack_actions.push_back(m_aut.tau_label_index()); // ensure that inert transitions come first
          count_transitions_per_action[m_aut.tau_label_index()] = 1; // set the number of transitions to a nonzero value so it doesn't trigger todo_stack_actions.push_back(...) in the loop
        }
        for(transition_index ti=0; ti<m_aut.num_transitions(); ++ti)
        {
          const transition& t=m_aut.get_transitions()[ti];
          // mCRL2complexity(&m_transitions[ti], add_work(..., 1), *this);
            // Because every transition is touched exactly once, we do not store a physical counter for this.
          const label_index label = label_or_divergence(t);
          transition_index& c=count_transitions_per_action[label];
          if (c==0)
          {
            todo_stack_actions.push_back(label);
          }
          c++;
        }
        if (m_branching)
        {
          assert(m_aut.is_tau(todo_stack_actions.front()));
          --count_transitions_per_action[m_aut.tau_label_index()];
        }
//std::cerr << "COUNT_TRANSITIONS PER ACT1    "; for(auto s: count_transitions_per_action){ std::cerr << s << "  "; } std::cerr << "\n";
        accumulate_entries(count_transitions_per_action, todo_stack_actions);
//std::cerr << "COUNT_TRANSITIONS PER ACT2    "; for(auto s: count_transitions_per_action){ std::cerr << s << "  "; } std::cerr << "\n";
        for(transition_index ti=0; ti<m_aut.num_transitions(); ++ti)
        {
          // mCRL2complexity(&m_transitions[ti], add_work(..., 1), *this);
            // Because every transition is touched exactly once, we do not store a physical counter for this.
          const transition& t=m_aut.get_transitions()[ti];
          const label_index label = label_or_divergence(t);
          transition_index& c=count_transitions_per_action[label];
          assert(0 <= c); assert(c < m_aut.num_transitions());
          m_BLC_transitions[c]=ti;
          c++;
        }

        // create BLC_indicators for every action label:
        BLC_list_iterator start_index=&*m_BLC_transitions.begin();
        for(label_index a: todo_stack_actions)
        {
          // mCRL2complexity(..., add_work(..., 1), *this);
              // not needed because the inner loop is always executed (except possibly for 1 iteration)
//std::cerr << "  Initialising m_BLC_transitions for action " << (m_aut.num_action_labels() == a ? "(tau-self-loops)" : pp(m_aut.action_label(a))) << '\n';
          const BLC_list_iterator end_index=&*m_BLC_transitions.begin()+count_transitions_per_action[a];
          assert(end_index<=&*m_BLC_transitions.end());
          if (start_index == end_index)
          {
            assert(m_branching); assert(m_aut.is_tau(a));
          }
          else
          {
            assert(start_index < end_index);
            // create a BLC_indicator and insert it into the list...
            typename linked_list<BLC_indicators>::iterator current_BLC= m_blocks[0].block_to_constellation.emplace_after(m_blocks[0].block_to_constellation.begin(), start_index, end_index, true);
            if (!is_inert_during_init(m_aut.get_transitions()[*start_index]))
            {
              current_BLC->make_unstable();
              current_BLC->start_marked_BLC = start_index; // mark all states in this BLC_indicator for the initial stabilization
            }
            do
            {
              // mCRL2complexity(&m_transitions[*start_index], add_work(..., 1), *this);
                  // Because every transition is touched exactly once, we do not store a physical counter for this.
              m_transitions[*start_index].transitions_per_block_to_constellation = current_BLC;
            }
            while (++start_index < end_index);
          }
        }
        assert(start_index==&*m_BLC_transitions.end());

        // destroy and deallocate todo_stack_actions and count_transitions_per_action here.
      }

      // Group transitions per outgoing state.
      mCRL2log(log::verbose) << "Start setting outgoing transitions\n";
      {

        fixed_vector<transition_index> count_outgoing_transitions_per_state(m_aut.num_states(), 0);
        for(const transition& t: m_aut.get_transitions())
        {
          // mCRL2complexity(&m_transitions[std::distance(&*m_aut.get_transitions.begin(), &t)], add_work(..., 1), *this);
            // Because every transition is touched exactly once, we do not store a physical counter for this.
          count_outgoing_transitions_per_state[t.from()]++;
          if (is_inert_during_init(t))
          {
            m_states[t.from()].no_of_outgoing_inert_transitions++;
          }
        }

        // We now set the outgoing transition per state pointer to the first non-inert transition.
        // The counters for outgoing transitions calculated above are reset to 0
        // and will later contain the number of transitions already stored.
        // Every time an inert transition is stored, the outgoing transition per state pointer is reduced by one.
        outgoing_transitions_it current_outgoing_transitions = m_outgoing_transitions.begin();

        // place transitions and set the pointers to incoming/outgoing transitions
        for (state_index s = 0; s < m_aut.num_states(); ++s)
        {
          // mCRL2complexity(&m_states[s], add_work(..., 1), *this);
            // Because every state is touched exactly once, we do not store a physical counter for this.
          m_states[s].start_outgoing_transitions = current_outgoing_transitions + m_states[s].no_of_outgoing_inert_transitions;
          current_outgoing_transitions += count_outgoing_transitions_per_state[s];
          count_outgoing_transitions_per_state[s] = 0; // meaning of this counter changes to: number of outgoing transitions already stored
        }
        assert(current_outgoing_transitions == m_outgoing_transitions.end());

        mCRL2log(log::verbose) << "Moving incoming and outgoing transitions\n";

        for (BLC_list_iterator ti=&*m_BLC_transitions.begin(); ti<&*m_BLC_transitions.end(); ++ti)
        {
          // mCRL2complexity(&m_transitions[*ti], add_work(..., 1), *this);
            // Because every transition is touched exactly once, we do not store a physical counter for this.
          const transition& t=m_aut.get_transitions()[*ti];
          if (is_inert_during_init(t))
          {
            m_transitions[*ti].ref_outgoing_transitions = --m_states[t.from()].start_outgoing_transitions;
          }
          else
          {
            m_transitions[*ti].ref_outgoing_transitions = m_states[t.from()].start_outgoing_transitions + count_outgoing_transitions_per_state[t.from()];
          }
          m_transitions[*ti].ref_outgoing_transitions->ref_BLC_transitions = ti;
          ++count_outgoing_transitions_per_state[t.from()];
        }
        // destroy and deallocate count_outgoing_transitions_per_state here.
      }

      state_index current_state=null_state;
      assert(current_state + 1 == 0);
      // bool tau_transitions_passed=true;
      // TODO: This should be combined with another pass through all transitions.
      for(std::vector<transition>::iterator it=m_aut.get_transitions().begin(); it!=m_aut.get_transitions().end(); it++)
      {
        // mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), it)], add_work(..., 1), *this);
            // Because every transition is touched exactly once, we do not store a physical counter for this.
        const transition& t=*it;
        if (t.to()!=current_state)
        {
          for (state_index i=current_state+1; i<=t.to(); ++i)
          {
            // ensure that every state is visited at most once:
            mCRL2complexity(&m_states[i], add_work(check_complexity::create_initial_partition__set_start_incoming_transitions, 1), *this);
//std::cerr << "SET start_incoming_transitions for state " << i << "\n";
            m_states[i].start_incoming_transitions=it;
          }
          current_state=t.to();
        }
      }
      for (state_index i=current_state+1; i<m_states.size(); ++i)
      {
        mCRL2complexity(&m_states[i], add_work(check_complexity::create_initial_partition__set_start_incoming_transitions, 1), *this);
//std::cerr << "SET residual start_incoming_transitions for state " << i << "\n";
        m_states[i].start_incoming_transitions=m_aut.get_transitions().end();
      }

      // Set the start_same_saC fields in m_outgoing_transitions.
      outgoing_transitions_it it = m_outgoing_transitions.end();
      if (m_outgoing_transitions.begin() < it)
      {
        --it;
        const transition& t = m_aut.get_transitions()[*it->ref_BLC_transitions];
        state_index current_state = t.from();
        label_index current_label = label_or_divergence(t);
        outgoing_transitions_it current_end_same_saC = it;
        while (m_outgoing_transitions.begin() < it)
        {
          --it;
          // mCRL2complexity(&m_transitions[*it->ref_BLC_transitions], add_work(..., 1), *this);
              // Because every transition is touched exactly once, we do not store a physical counter for this.
          const transition& t = m_aut.get_transitions()[*it->ref_BLC_transitions];
          const label_index new_label = label_or_divergence(t);
          if (current_state == t.from() && current_label == new_label)
          {
            // We encounter a transition with the same saC. Let it refer to the end.
            it->start_same_saC = current_end_same_saC;
          }
          else
          {
            // We encounter a transition with a different saC.
            current_state = t.from();
            current_label = new_label;
            current_end_same_saC->start_same_saC = std::next(it);
            current_end_same_saC = it;
          }
        }
        current_end_same_saC->start_same_saC = m_outgoing_transitions.begin();
      }

//std::cerr << "Start filling states_in_blocks\n";
      assert(m_states_in_blocks.size()==m_aut.num_states());
      fixed_vector<state_in_block_pointer>::iterator lower_i=m_states_in_blocks.begin(), upper_i=m_states_in_blocks.end();
      for (fixed_vector<state_type_gj>::iterator i=m_states.begin(); i<m_states.end(); ++i)
      {
        // mCRL2complexity(&m_states[i], add_work(..., 1), *this);
            // Because every state is touched exactly once, we do not store a physical counter for this.
        if (0<i->no_of_outgoing_inert_transitions)
        {
          --upper_i;
          upper_i->ref_state=i;
          i->ref_states_in_blocks=upper_i;
        }
        else
        {
          lower_i->ref_state=i;
          i->ref_states_in_blocks=lower_i;
          ++lower_i;
        }
      }
      assert(lower_i == upper_i);
      m_blocks[0].start_bottom_states=m_states_in_blocks.begin();
      m_blocks[0].start_non_bottom_states = lower_i;
      m_blocks[0].end_states=m_states_in_blocks.end();

      // The data structures have now been completely initialized.

      //print_data_structures("After initial reading before splitting in the initialisation");
      assert(check_data_structures("After initial reading before splitting in the initialisation"));

      // The initial partition has been constructed. Continue with the initialisation.
      mCRL2log(log::verbose) << "Start refining in the initialisation\n";

//std::cerr << "COUNT_STATES PER ACT     "; for(auto s: count_transitions_per_action){ std::cerr << s << "  "; } std::cerr << "\n";
//std::cerr << "STATES PER ACTION LABEL  "; for(transition_index ti: m_BLC_transitions){ std::cerr << ti << "  "; } std::cerr << "\n";
//std::cerr << "STATES PER ACTION LABELB "; for(transition_index ti: m_BLC_transitions){ std::cerr << m_states[m_aut.get_transitions()[ti].from()].block << "  "; } std::cerr << "\n";

      // In principle, we now stabilize for every BLC_indicator.
      // All transitions have already been marked in every BLC_indicator, so we can just run normal stabilisations.
      std::pair<BLC_list_iterator, BLC_list_iterator> stabilize_pair(&*m_BLC_transitions.begin(), &*m_BLC_transitions.end());

      #ifndef NDEBUG
        const std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> > transition_array_for_check_stability(1, stabilize_pair);
      #endif
      while (stabilize_pair.first < stabilize_pair.second)
      {
        #ifndef NDEBUG
          assert(check_stability("Initialisation loop", &transition_array_for_check_stability, &stabilize_pair));
        #endif
        linked_list<BLC_indicators>::iterator splitter = m_transitions[*std::prev(stabilize_pair.second)].transitions_per_block_to_constellation;
        // mCRL2complexity(...);
            // not needed, as the splitter has marked transitions and we are allowed to visit each marked transition a fixed number of times.
        stabilize_pair.second = splitter->start_same_BLC;
        assert(stabilize_pair.first <= stabilize_pair.second);
        assert(splitter->start_same_BLC<splitter->end_same_BLC);

        const transition& first_t = m_aut.get_transitions()[*splitter->start_same_BLC];
        if (number_of_states_in_block(m_states[first_t.from()].block) <= 1)
        {
          // a block with 1 state does not need to be split.
          //unmark_all_transitions(*splitter);
        }
        else if (m_blocks[m_states[first_t.from()].block].contains_new_bottom_states)
        {
          // This slice contains new bottom states and therefore should not be stabilized
//std::cerr << splitter->debug_id(*this) << " is skipped because it starts in a block with new bottom states\n";
        }
        else if (is_inert_during_init(first_t))
        {
          // This slice contains constellation-inert transitions;
          // we do not need to stabilize under it.
          assert(!has_marked_transitions(*splitter));
//std::cerr << splitter->debug_id(*this) << " is skipped because it contains constellation-inert transitions\n";
        }
        else
        {
//std::cerr << "Now stabilizing under " << splitter->debug_id(*this) << '\n';
          assert(splitter->start_marked_BLC == splitter->start_same_BLC);
          const block_index source_block = m_states[first_t.from()].block;
          const BLC_list_iterator splitter_end_unmarked_BLC = splitter->start_marked_BLC;
          const fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state=not_all_bottom_states_are_touched(splitter
                    #ifndef NDEBUG
                      , splitter_end_unmarked_BLC
                    #endif
                    );

          if (first_unmarked_bottom_state < m_blocks[source_block].start_non_bottom_states)
          {
            splitB(splitter, first_unmarked_bottom_state, splitter_end_unmarked_BLC);
          }
        }
      }

      // Algorithm 1, line 1.4 is implicitly done in the call to splitB above.

      // Algorithm 1, line 1.5.
      //print_data_structures("End initialisation");
      assert(check_stability("End initialisation"));
      mCRL2log(log::verbose) << "Start stabilizing in the initialisation\n";
      stabilizeB();
      assert(check_data_structures("End initialisation"));
    }

    // Algorithm 4. Stabilize the current partition with respect to the current constellation
    // given that the blocks in m_blocks_with_new_bottom_states do contain new bottom states.
    // Stabilisation is always called after initialisation, i.e., m_incoming_transitions[ti].transition refers
    // to a position in m_BLC_transitions, where the transition index of this transition can be found.

    void stabilizeB()
    {
      if (m_blocks_with_new_bottom_states.empty())
      {
        return;
      }
      bool initial_stabilization=true;
      // Qhat contains the slices of BLC transitions that still need stabilization
      std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> > Qhat;
      #ifndef NDEBUG
        std::vector<std::pair<BLC_list_const_iterator, BLC_list_const_iterator> > initialize_qhat_work_to_assign_later;
        std::vector<std::pair<BLC_list_const_iterator, BLC_list_const_iterator> > stabilize_work_to_assign_later;
      #endif

      for (;;)
      {
        // mCRL2complexity(all bottom states, add_work(..., 1), *this);
            // not necessary, as the inner loop is always executed

        assert(!m_blocks_with_new_bottom_states.empty());
        for(const block_index bi: m_blocks_with_new_bottom_states)
        {
          #ifndef NDEBUG
//std::cerr << "Block " << bi << " has new bottom states.\n";
            // The work in this loop is assigned to the (new) bottom states in bi
            // It cannot be assigned to the block bi because there may be more new bottom states later.
            fixed_vector<state_in_block_pointer>::iterator new_bott_it = m_blocks[bi].start_bottom_states;
            assert(new_bott_it < m_blocks[bi].start_non_bottom_states);
            do
            {
              mCRL2complexity(new_bott_it->ref_state, add_work(check_complexity::stabilizeB__prepare_block, 1), *this);
            }
            while (++new_bott_it < m_blocks[bi].start_non_bottom_states);
          #endif
          assert(m_blocks[bi].contains_new_bottom_states);
          m_blocks[bi].contains_new_bottom_states = false;
          if (1 >= number_of_states_in_block(bi))
          {
            // blocks with only 1 state do not need to be stabilized further
//std::cerr << "    But it has only 1 state.\n";
            continue;
          }

          typename linked_list<BLC_indicators>::iterator ind = m_blocks[bi].block_to_constellation.begin();
          #ifndef NDEBUG
            assert(!m_blocks[bi].block_to_constellation.empty());
            assert(ind->start_same_BLC<ind->end_same_BLC);
            const transition& first_t = m_aut.get_transitions()[*ind->start_same_BLC];
            assert(m_states[first_t.from()].block == bi);
            assert(is_inert_during_init(first_t) &&
                   m_blocks[bi].constellation==m_blocks[m_states[first_t.to()].block].constellation);
            assert(ind->is_stable());
          #endif
              // The first BLC-set is constellation-inert, so skip it
          ++ind;
          for (; m_blocks[bi].block_to_constellation.end()!=ind; ++ind)
          {
            if (!initial_stabilization && !ind->is_stable())
            {
              // This is a new bottom block that was found during stabilizeB().
              // Therefore, the subsequent BLC sets are already somewhere in
              // Qhat, and stabilizing for them two times is not needed.
              #ifndef NDEBUG
                // Check that all other BLC sets are already unstable
                while (++ind!=m_blocks[bi].block_to_constellation.end())
                {
                  assert(!ind->is_stable());
                  // marked transitions would start in new bottom states found
                  // earlier:
                  assert(!has_marked_transitions(*ind));
                }
              #endif
              break;
            }
//std::cerr << "    Preparing to stabilize under " << ind->debug_id(*this) << '\n';
            ind->start_marked_BLC=ind->end_same_BLC;
            #ifndef NDEBUG
              assert(!has_marked_transitions(*ind));
              assert(ind->start_same_BLC<ind->end_same_BLC);
              const transition& first_t = m_aut.get_transitions()[*ind->start_same_BLC];
              assert(m_states[first_t.from()].block == bi);
              assert(!is_inert_during_init(first_t) ||
                     m_blocks[bi].constellation != m_blocks[m_states[first_t.to()].block].constellation);
            #endif
              // BLC set transitions are not constellation-inert, so we need to stabilize under them
              Qhat.emplace_back(ind->start_same_BLC, ind->end_same_BLC);

            #ifndef NDEBUG
                // The work is assigned to the transitions out of new bottom states in ind.
                // Try to find a new bottom state to which to assign it.
                bool work_assigned = false;
                // assign the work to the transitions out of bottom states in this BLC-set
                for (BLC_list_const_iterator work_it = ind->start_same_BLC; work_it<ind->end_same_BLC; ++work_it)
                {
                  // assign the work to this transition
                  if (0 == m_states[m_aut.get_transitions()[*work_it].from()].no_of_outgoing_inert_transitions)
                  {
                    mCRL2complexity(&m_transitions[*work_it], add_work(check_complexity::stabilizeB__initialize_Qhat, 1), *this);
                    work_assigned = true;
                  }
                }
                if (!work_assigned)
                {
                  // We register that we still have to find a transition from a new bottom state in this slice.
                  // we should do the following:
                  // - store the range [ind->start_same_BLC, ind->end_same_BLC) in a temporary variable
                  //   (We cannot use the ind pointer here because the source block
                  //   might be split up later, in a part that contains only old
                  //   bottom states and a part that does contain new bottom states.
                  //   Then, we should decide which part[s] contain new bottom states...)
                  // - expect that one of the source states of this range will become
                  //   a new bottom state in m_blocks_with_new_bottom_states
                  //   later in the main loop of stabilizeB().
                  // - towards the end of the main loop, go through the ranges stored in the temporary variable
                  //   and mark the transitions that start in a bottom state in m_blocks_with_new_bottom_states.
                  //   For every range, we must mark at least one transition.
//std::cerr << "Haven't yet found a transition from a new bottom state in " << ind->debug_id(*this) << " to assign the initialization of Qhat to\n";
                  initialize_qhat_work_to_assign_later.emplace_back(ind->start_same_BLC, ind->end_same_BLC);
                }
            #endif
          }

// 2. Administration: Mark all transitions out of (new) bottom states
          fixed_vector<state_in_block_pointer>::iterator si=m_blocks[bi].start_bottom_states;
          assert(si<m_blocks[bi].start_non_bottom_states);
          do
          {
            mCRL2complexity(si->ref_state, add_work(check_complexity::stabilizeB__distribute_states_over_Phat, 1), *this);
            assert(si->ref_state->block==bi);
            outgoing_transitions_it end_it = std::next(si->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(si->ref_state)->start_outgoing_transitions;
            for (outgoing_transitions_it ti=si->ref_state->start_outgoing_transitions; ti<end_it; ++ti)
            {
              // mCRL2complexity(&m_transitions[m_BLC_transitions[ti->transition]], add_work(..., 1), *this);
                  // subsumed under the above counter
              const transition& t = m_aut.get_transitions()[*ti->ref_BLC_transitions];
              assert(m_states.begin()+t.from()==si->ref_state);
              if (!is_inert_during_init_if_branching(t) ||
                  m_blocks[bi].constellation != m_blocks[m_states[t.to()].block].constellation)
              {
                // the transition is not constellation-inert, so mark it
                mark_BLC_transition(ti);
              }
              else
              {
                // skip all other constellation-inert transitions (this is an optimization)
                assert(ti <= ti->start_same_saC);
                ti = ti->start_same_saC;
              }
            }
            ++si;
          }
          while (si<m_blocks[bi].start_non_bottom_states);
        }
        clear(m_blocks_with_new_bottom_states);
        initial_stabilization=false;

// 3. As long as there are registered slices in m_BLC_transitions, select any one of them.
//    Take the first BLC_indicator that has transitions in this slice; remove it from the slice;
//    if the slice is now empty remove it from the register.
//    Do a normal splitB() under this splitter.
//    If more new bottom states are created, store them in the new m_blocks_with_new_bottom_states.

        // Algorithm 4, line 4.8.
        // inner loop to be executed until further new bottom states are found:
        do
        {
          if (Qhat.empty())
          {
            // nothing needs to be stabilized any more.

            // Therefore, it is impossible that further new bottom states are
            // found in these rounds.  So all work must have been accounted for:
            assert(initialize_qhat_work_to_assign_later.empty());
            assert(stabilize_work_to_assign_later.empty());

            assert(check_data_structures("End of stabilizeB()"));
            assert(check_stability("End of stabilizeB()"));
            return;
          }
          // Algorithm 4, line 4.9.
          // mCRL2complexity(..., add_work(..., max_C), *this);
              // not needed as the inner loop is always executed at least once.
          //print_data_structures("New bottom state loop");
          assert(check_data_structures("New bottom state loop", false, false));
          assert(check_stability("New bottom state loop", &Qhat));

          std::pair<BLC_list_iterator, BLC_list_iterator>& Qhat_elt=Qhat.back();
          assert(Qhat_elt.first < Qhat_elt.second);
          const linked_list<BLC_indicators>::iterator splitter=m_transitions[*std::prev(Qhat_elt.second)].transitions_per_block_to_constellation;
//std::cerr << "Now stabilizing under " << splitter->debug_id(*this) << '\n';
          assert(splitter->end_same_BLC==Qhat_elt.second);
          Qhat_elt.second = splitter->start_same_BLC;
          assert(Qhat_elt.first<=Qhat_elt.second);
          if (Qhat_elt.first==Qhat_elt.second)
          {
            Qhat.pop_back(); // invalidates Qhat_elt
          }

          assert(splitter->start_same_BLC<splitter->end_same_BLC);
          const transition& first_t=m_aut.get_transitions()[*splitter->start_same_BLC];
          const block_type& from_block=m_blocks[m_states[first_t.from()].block];
          #ifndef NDEBUG
            assert(!from_block.contains_new_bottom_states);
                // The work is assigned to the transitions out of new bottom states in splitter.
            bool work_assigned=false;
            for (BLC_list_const_iterator work_it=splitter->start_same_BLC; work_it<splitter->end_same_BLC; ++work_it)
            {
              // assign the work to this transition
              if (0==m_states[m_aut.get_transitions()[*work_it].from()].no_of_outgoing_inert_transitions)
              {
                mCRL2complexity(&m_transitions[*work_it], add_work(check_complexity::stabilizeB__main_loop, 1), *this);
                work_assigned=true;
              }
            }
            if (!work_assigned)
            {
              // We register that we still have to find a transition from a new bottom state in this slice.
//std::cerr << "Haven't yet found a transition from a new bottom state in " << splitter->debug_id(*this) << " to assign the main loop work to\n";
              stabilize_work_to_assign_later.emplace_back(splitter->start_same_BLC, splitter->end_same_BLC);
            }
          #endif
          if (std::distance(from_block.start_bottom_states, from_block.end_states)<=1)
          {
            // a block with 1 state does not need to be split
            //splitter->make_stable();
//std::cerr << "No stabilization is needed because the source block contains only 1 state.\n";
          }
          else
          {
            assert(!is_inert_during_init(first_t) ||
                     from_block.constellation != m_blocks[m_states[first_t.to()].block].constellation);

            // Algorithm 4, line 4.10.
            const BLC_list_iterator splitter_end_unmarked_BLC=splitter->start_marked_BLC;
            fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state=not_all_bottom_states_are_touched(splitter
                        #ifndef NDEBUG
                          , splitter_end_unmarked_BLC
                        #endif
                        );
            if (first_unmarked_bottom_state<from_block.start_non_bottom_states)
            {
//std::cerr << "PERFORM A NEW BOTTOM STATE SPLIT\n";
              // Algorithm 4, line 4.11, and implicitly 4.12, 4.13 and 4.18.
              splitB(splitter, first_unmarked_bottom_state, splitter_end_unmarked_BLC);
            }
//else { std::cerr << "No split is needed because every bottom state has a transition in the splitter.\n"; }
          }
        }
        while (m_blocks_with_new_bottom_states.empty());

        #ifndef NDEBUG
          // Further new bottom states have been found, so we now have a chance at assigning
          // the initialization of Qhat that had not yet been assigned earlier.
          for (std::vector<std::pair<BLC_list_const_iterator, BLC_list_const_iterator> >::iterator qhat_it = initialize_qhat_work_to_assign_later.begin();
                        qhat_it!=initialize_qhat_work_to_assign_later.end(); )
          {
            bool new_bottom_state_with_transition_found = false;
            for (BLC_list_const_iterator work_it = qhat_it->first; work_it < qhat_it->second; ++work_it)
            {
              const state_index t_from = m_aut.get_transitions()[*work_it].from();
              if (0 == m_states[t_from].no_of_outgoing_inert_transitions)
              {
                // t_from is a new bottom state, so we can assign the work to this transition
                mCRL2complexity(&m_transitions[*work_it], add_work(check_complexity::stabilizeB__initialize_Qhat_afterwards, 1), *this);
                new_bottom_state_with_transition_found = true;
              }
            }
            if (new_bottom_state_with_transition_found)
            {
              // The work has been assigned successfully, so we can replace this
              // entry of initialize_qhat_work_to_assign_later with the last one.
              #ifndef NDEBUG
                bool at_end=std::next(qhat_it)==initialize_qhat_work_to_assign_later.end();
              #endif
              *qhat_it=initialize_qhat_work_to_assign_later.back();
              initialize_qhat_work_to_assign_later.pop_back();
              assert(at_end == (qhat_it==initialize_qhat_work_to_assign_later.end()));
            }
            else
            {
              ++qhat_it;
            }
          }

          // We shall also try and find further new bottom states to which to assign
          // the main loop iterations that had not yet been assigned earlier.
          for (std::vector<std::pair<BLC_list_const_iterator, BLC_list_const_iterator> >::iterator stabilize_it = stabilize_work_to_assign_later.begin();
                        stabilize_it!=stabilize_work_to_assign_later.end(); )
          {
            bool new_bottom_state_with_transition_found = false;
            for (BLC_list_const_iterator work_it = stabilize_it->first; work_it < stabilize_it->second; ++work_it)
            {
              const state_index t_from = m_aut.get_transitions()[*work_it].from();
              if (0 == m_states[t_from].no_of_outgoing_inert_transitions)
              {
                // t_from is a new bottom state, so we can assign the work to this transition
                mCRL2complexity(&m_transitions[*work_it], add_work(check_complexity::stabilizeB__main_loop_afterwards, 1), *this);
                new_bottom_state_with_transition_found = true;
              }
            }
            if (new_bottom_state_with_transition_found)
            {
              // The work has been assigned successfully, so we can replace this
              // entry of stabilize_work_to_assign_later with the last one.
              #ifndef NDEBUG
                bool at_end=std::next(stabilize_it)==stabilize_work_to_assign_later.end();
              #endif
              *stabilize_it=stabilize_work_to_assign_later.back();
              stabilize_work_to_assign_later.pop_back();
              assert(at_end == (stabilize_it==stabilize_work_to_assign_later.end()));
            }
            else
            {
              ++stabilize_it;
            }
          }
        #endif
      }
      assert(0); // unreachable
    }

#ifndef CO_SPLITTER_IN_BLC_LIST
    void maintain_block_label_to_cotransition(
                   const block_index old_block,
                   const block_index new_block,
                   const transition_index moved_transition,
                   const transition_index alternative_transition,
                   block_label_to_size_t_map& block_label_to_cotransition,
                   const constellation_index old_constellation) const
    {
      const transition& t_move=m_aut.get_transitions()[moved_transition];
      const label_index lbl = label_or_divergence(t_move);
      assert(m_states[t_move.from()].block == new_block);
      if (null_transition != alternative_transition)
      {
        assert(old_block == m_states[m_aut.get_transitions()[alternative_transition].from()].block ||
               new_block == m_states[m_aut.get_transitions()[alternative_transition].from()].block);
        assert(lbl == label_or_divergence(m_aut.get_transitions()[alternative_transition]));
        assert(m_blocks[m_states[t_move.to()].block].constellation == m_blocks[m_states[m_aut.get_transitions()[alternative_transition].to()].block].constellation);
      }
      if (m_blocks[m_states[t_move.to()].block].constellation==old_constellation &&
          (!is_inert_during_init(t_move) || m_blocks[m_states[t_move.from()].block].constellation!=old_constellation))
      {
        // This is a non-constellation-inert transition to the current co-constellation.

        typename block_label_to_size_t_map::iterator bltc_it=
                       block_label_to_cotransition.find(std::pair(old_block,lbl));
        if (bltc_it!=block_label_to_cotransition.end())
        {
          if (bltc_it->second==moved_transition)
          {
            // This transition is being moved. Find a replacement in block_label_to_cotransition.
            bltc_it->second=alternative_transition;
          }
        }

        // Check whether there is a representation for the new_block in block_label_to_cotransition.
        bltc_it=block_label_to_cotransition.find(std::pair(new_block,lbl));
        if (bltc_it==block_label_to_cotransition.end())
        {
          // No such transition exists as yet. Give moved transition this purpose.
          block_label_to_cotransition[std::pair(new_block,lbl)]=moved_transition;
        }
      }
    }
#endif

    // finds a splitter for the tau-transitions from block B (= the new constellation) to old_constellation,
    // if such a splitter exists.
    // It uses the fact that the first element block_to_constellation contains the inert transitions (if there are any),
    // and just after splitting the new constellation off from the old one,
    // the element immediately after that the tau-transitions from the new to the old constellation.
    linked_list<BLC_indicators>::iterator find_inert_co_transition_for_block(const block_index index_block_B, const constellation_index old_constellation)
    {
      linked_list< BLC_indicators >::iterator btc_it= m_blocks[index_block_B].block_to_constellation.begin();
      if (btc_it == m_blocks[index_block_B].block_to_constellation.end())
      {
        // The new constellation has no outgoing transitions at all.
        return m_blocks[index_block_B].block_to_constellation.end();
      }
      assert(btc_it->start_same_BLC<btc_it->end_same_BLC);
      const transition& btc_t=m_aut.get_transitions()[*(btc_it->start_same_BLC)];
      if (!is_inert_during_init_if_branching(btc_t))
      {
        // The new constellation has no outgoing tau-transitions at all (except possibly tau-self-loops, for divergence-preserving branching bisimulation).
        return m_blocks[index_block_B].block_to_constellation.end();
      }
      if (m_blocks[m_states[btc_t.to()].block].constellation==old_constellation)
      {
        // The new constellation has no inert transitions but it does have tau-transitions to the old constellation
        // (which were inert before).
        return &*btc_it;
      }
      if (m_blocks[m_states[btc_t.to()].block].constellation != m_constellations.size() - 1)
      {
        // The new constellation, before it was separated from the old one,
        // had no constellation-inert outgoing transitions.
        return m_blocks[index_block_B].block_to_constellation.end();
      }
      // *btc_it is the BLC_indicator for the inert transitions of the new constellation.
      // Try the second element in the list:
      btc_it=m_blocks[index_block_B].block_to_constellation.next(btc_it);
      if (btc_it == m_blocks[index_block_B].block_to_constellation.end())
      {
        // The new constellation has no other outgoing transitions.
        return m_blocks[index_block_B].block_to_constellation.end();
      }
      assert(btc_it->start_same_BLC<btc_it->end_same_BLC);
      const transition& btc2_t=m_aut.get_transitions()[*(btc_it->start_same_BLC)];
      if (!is_inert_during_init_if_branching(btc2_t) ||
          m_blocks[m_states[btc2_t.to()].block].constellation != old_constellation) // The new constellation has no tau-transitions to the old constellation.
      {
        return m_blocks[index_block_B].block_to_constellation.end();
      }
      return &*btc_it;
    }

/*
    // This routine can only be called after initialisation, i.e., when m_outgoing_transitions[t].transition refers to
    // a position in m_BLC_transitions.
    bool state_has_outgoing_co_transition(const transition_index transition_to_bi, const constellation_index old_constellation)
    {
      // i1 refers to the position of the transition_to_bi;
      outgoing_transitions_it i1=m_transitions[transition_to_bi].ref_outgoing_transitions;
      // i2 refers to the last position, unless i1 is the last, then i2 is the first of all transitions with the same s, a and C.
      outgoing_transitions_it i2=i1->start_same_saC;
      if (i2<=i1)
      {
        // So, i1 is the last element with the same s, a, C.
        i2=i1+1;
      }
      else
      {
        // In this case i2 refers to the last element with the same s, a, C.
        i2++;
      }
      if (i2==m_outgoing_transitions.end())
      {
        return false;
      }
      const transition& t1=m_aut.get_transitions()[transition_to_bi];
      const transition& t2=m_aut.get_transitions()[*i2->ref_BLC_transitions];
      return t1.from()==t2.from() && m_aut_apply_hidden_label_map(t1.label())==m_aut_apply_hidden_label_map(t2.label()) && m_blocks[m_states[t2.to()].block].constellation==old_constellation;
    }

    // This function determines whether all bottom states in B have outgoing co-transitions. If yes false is reported.
    // If no, true is reported and the source states with outgoing co-transitions are added to m_R and those without outgoing
    // co-transitions are added to m_u. The counters of these states are set to Rmarked or to 0. This already initialises
    // these states for the splitting process.
    // This routine is called after initialisation.
    bool some_bottom_state_has_no_outgoing_co_transition(block_index B,
                                                         fixed_vector<transition_index>::iterator transitions_begin,
                                                         fixed_vector<transition_index>::iterator transitions_end,
                                                         const constellation_index old_constellation,
                                                         fixed_vector<state_index>::iterator& first_unmarked_bottom_state)
    {
      first_unmarked_bottom_state = m_blocks[B].start_bottom_states;
      #ifndef NDEBUG
        const constellation_index new_constellation = m_constellations.size()-1;
        const unsigned max_C = check_complexity::log_n - check_complexity::ilog2(number_of_states_in_constellation(new_constellation));
      #endif
      for(fixed_vector<transition_index>::iterator ti=transitions_begin; ti!=transitions_end; ++ti)
      {
        const transition& t=m_aut.get_transitions()[*ti];
//std::cerr << "INSPECT TRANSITION  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
        assert(new_constellation==m_blocks[m_states[t.to()].block].constellation);
        mCRL2complexity(&m_transitions[*ti], add_work(check_complexity::some_bottom_state_has_no_outgoing_co_transition__handle_transition, max_C), *this);
        const state_index s = t.from();
        assert(m_states[s].ref_states_in_blocks>=m_blocks[B].start_bottom_states);
        assert(m_states[s].ref_states_in_blocks<m_blocks[B].end_states);
        if (m_states[s].ref_states_in_blocks<m_blocks[B].start_non_bottom_states &&
            m_states[s].counter==undefined)
        {
            if (state_has_outgoing_co_transition(*ti,old_constellation))
            {
              m_states[s].counter=Rmarked;
              m_R.add_todo(s);
              const fixed_vector<state_index>::iterator pos_s=m_states[s].ref_states_in_blocks;
              assert(pos_s < m_blocks[B].start_non_bottom_states);
              assert(first_unmarked_bottom_state <= pos_s);
              swap_states_in_states_in_block(first_unmarked_bottom_state, pos_s); // Move marked states to the front.
              first_unmarked_bottom_state++;
            }
            else
            {
              // We need to register that we added this state to U
              // because it might have other transitions to new_constellation,
              // so it will be revisited
              m_U_counter_reset_vector.push_back(t.from());
              m_states[t.from()].counter=0;
              m_U.add_todo(t.from());

            }
        }
      }

      assert(m_R.size() + m_U.size() == static_cast<std::size_t>(std::distance(m_blocks[B].start_bottom_states, m_blocks[B].start_non_bottom_states)));
      if (first_unmarked_bottom_state != m_blocks[B].start_non_bottom_states)
      {
        return true; // A split can commence.
      }
      // Otherwise, reset the marks.
      clear_state_counters();
      m_R.clear();
      m_U.clear();
      return false;
    }

    // Check if there is a state in W that has no outgoing a transition to some constellation.
    // If so, return false, but set in m_R and m_U whether those states in W have or have no
    // outgoing transitions. Set m_states[s].counter accordingly.
    // If all states in W have outgoing transitions with the label and constellation, leave
    // m_R, m_U, m_states[s].counters and m_U_counter_reset vector untouched.

    bool W_empty(const set_of_states_type& W, const set_of_states_type& aux,
                 #ifndef NDEBUG
                   const label_index a,
                   const constellation_index C,
                 #endif
                 fixed_vector<state_index>::iterator& first_unmarked_bottom_state
                 )
    {
      bool W_empty=true;
//std::cerr << "W: "; for(auto s: W) { std::cerr << s << " "; } std::cerr << "\n";
      #ifndef NDEBUG
        assert(m_aut_apply_hidden_label_map(a) == a);
        const block_type& B = m_blocks[m_states[*first_unmarked_bottom_state].block];
        assert(B.start_bottom_states == first_unmarked_bottom_state);
        // assert(static_cast<std::ptrdiff_t>(std::distance(B.start_bottom_states, B.start_non_bottom_states)) == W.size());
      #endif
      for(const state_index si: W)
      {
        assert(0 == m_states[si].no_of_outgoing_inert_transitions);
        assert(m_states[*first_unmarked_bottom_state].block == m_states[si].block);
        if (aux.count(si)==0)
        {
          // I do not know how to account for the work in this case.
          W_empty=false;
          m_U.add_todo(si);
        }
        else
        {
          add_work_to_same_saC(false, si, a, C, check_complexity::W_empty__find_new_bottom_state_in_R, 1);
          assert(undefined == m_states[si].counter);
          m_states[si].counter=Rmarked;
          m_R.add_todo(si);
          const fixed_vector<state_index>::iterator pos_s=m_states[si].ref_states_in_blocks;
          assert(first_unmarked_bottom_state <= pos_s);
          assert(pos_s < B.start_non_bottom_states);
          swap_states_in_states_in_block(first_unmarked_bottom_state, pos_s); // Move marked states to the front.
          first_unmarked_bottom_state++;
        }
      }
      if (!W_empty)
      {
        return false; // A split can commence.
      }
      // Otherwise, reset the marks.
      clear_state_counters();
      m_R.clear();
      m_U.clear();
      return true;
    } */

    // Mark all states that are sources of the marked transitions in splitter
    // and move them to the beginning.
    // (Marking of bottom states is done by moving them to the beginning of the
    // bottom states. Marking of non-bottom states is done by adding them to m_R
    // and setting m_states[*].counter = Rmarked.)
    // Also make the splitter stable and move it to the beginning of the respective
    // list of BLC sets.
    // If all bottom states are marked, reset markers and m_R.
    // Otherwise, if not all
    // bottom states are touched, leave the marked non-bottom states in m_R and leave
    // the markers in m_states[*].counter in place.
    // The marked bottom states are moved to the front in m_states_in_blocks and
    // the return value indicates the position (in m_states_in_blocks)
    // of the first non-marked bottom state.
    [[nodiscard]]
    fixed_vector<state_in_block_pointer>::iterator not_all_bottom_states_are_touched(linked_list<BLC_indicators>::iterator splitter
            #ifndef NDEBUG
              , const BLC_list_const_iterator splitter_end_unmarked_BLC /* = splitter->start_marked_BLC -- but this default argument is not allowed */
            #endif
            )
    {
//std::cerr << "not_all_bottom_states_are_touched(" << splitter->debug_id(*this) << ")\n";
      const block_index bi = m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].block;
      block_type& B=m_blocks[bi];
      assert(m_R.empty());
      assert(m_U.empty());
      assert(1 < number_of_states_in_block(bi));
      // If the above assertion is false, one can just: return B.end_states;
      assert(!B.contains_new_bottom_states);
      fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state=B.start_bottom_states;
      assert(!splitter->is_stable());
      BLC_list_iterator marked_t_it = splitter->start_marked_BLC;
      for(; marked_t_it<splitter->end_same_BLC; ++marked_t_it)
      {
        const transition& t = m_aut.get_transitions()[*marked_t_it];
        const state_in_block_pointer s(m_states.begin()+t.from());
        assert(s.ref_state->block==bi);
        // mCRL2complexity(&m_transitions[*i], add_work(...), *this);
            // not needed because this work can be attributed to the marking of the transition
        const fixed_vector<state_in_block_pointer>::iterator pos_s=s.ref_state->ref_states_in_blocks;
        assert(B.start_bottom_states <= pos_s);
        assert(pos_s < B.end_states);
        if (first_unmarked_bottom_state <= pos_s)
        {
          if (0==s.ref_state->no_of_outgoing_inert_transitions)
          {
            assert(pos_s<B.start_non_bottom_states);
            assert(undefined==s.ref_state->counter);
            swap_states_in_states_in_block(first_unmarked_bottom_state, pos_s); // Move marked states to the front.
            first_unmarked_bottom_state++;
          }
          else if (Rmarked!=s.ref_state->counter)
          {
            assert(B.start_non_bottom_states<=pos_s);
            assert(undefined==s.ref_state->counter);
            m_R.add_todo(s);
            s.ref_state->counter=Rmarked;
          }
        }
      }
      #ifndef NDEBUG
        // ensure that the unmarked transitions do not add any bottom states.
        assert(splitter->start_same_BLC <= splitter_end_unmarked_BLC);
        for(BLC_list_const_iterator i = splitter->start_same_BLC; i < splitter_end_unmarked_BLC; ++i)
        {
          assert(splitter_end_unmarked_BLC == splitter->start_marked_BLC);
          const transition& t = m_aut.get_transitions()[*i];
          const state_in_block_pointer s(m_states.begin()+t.from());
          assert(s.ref_state->block == bi);
          const fixed_vector<state_in_block_pointer>::const_iterator pos_s=s.ref_state->ref_states_in_blocks;
          assert(*pos_s==s);
          assert(B.start_bottom_states <= pos_s);
          assert(pos_s < B.end_states);
          if (0==s.ref_state->no_of_outgoing_inert_transitions)
          {
            // State s is a bottom state. It should already have been marked.
            assert(pos_s < first_unmarked_bottom_state);
          }
          else
          {
            assert(B.start_non_bottom_states <= pos_s);
          }
        }
      #endif
      // make splitter stable and move it to the beginning of the list:
      splitter->make_stable();
      linked_list<BLC_indicators>& btc=B.block_to_constellation;
      assert(!btc.empty());
      if (splitter!=btc.begin())
      {
        linked_list<BLC_indicators>::iterator move_splitter_after=btc.end();
        if (m_branching)
        {
          const transition& perhaps_inert_t=m_aut.get_transitions()[*btc.begin()->start_same_BLC];
          if (is_inert_during_init_if_branching(perhaps_inert_t) &&
              m_blocks[m_states[perhaps_inert_t.to()].block].constellation==B.constellation)
          {
            move_splitter_after=btc.begin();
          }
        }
        btc.splice_to_after(move_splitter_after, btc, splitter);
      }
      if (first_unmarked_bottom_state==B.start_non_bottom_states)
      {
        // All bottom states are marked. No splitting is possible. Reset m_R, m_states[s].counter for s in m_R.
        clear_state_counters(true);
        m_R.clear();
      }
      return first_unmarked_bottom_state;
    }

    // The following is an alternative implementation of not_all_bottom_states_are_touched()
    // that might be more efficient. However, it hasn't been tested fully.
    /* {
//std::cerr << "not_all_bottom_states_are_touched(" << splitter->debug_id(*this) << ")\n";
      const block_index bi = m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].block;
      block_type& B=m_blocks[bi];
      assert(m_R.empty());
      assert(1 < number_of_states_in_block(bi));
      // If the above assertion is false, one can just: return B.end_states;
      fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state=B.start_bottom_states;
      assert(!splitter->is_stable());
      const BLC_list_iterator splitter_start_marked_BLC = splitter->start_marked_BLC;
      for (BLC_list_iterator marked_t_it = splitter_start_marked_BLC; marked_t_it<splitter->end_same_BLC; ++marked_t_it)
      {
        const transition& t = m_aut.get_transitions()[*marked_t_it];
        // mCRL2complexity(&m_transitions[*marked_t_it], add_work(...), *this);
            // not needed because this work can be attributed to the marking of the transition
        const state_in_block_pointer s(m_states.begin()+t.from());
        assert(s.ref_state->block==bi);
        if (Rmarked!=s.ref_state->counter)
        {
          s.ref_state->counter=Rmarked;
          if (0==s.ref_state->no_of_outgoing_inert_transitions)
          {
            ++first_unmarked_bottom_state;
          }
          else
          {
            m_R.add_todo(s);
          }
        }
      }
      #ifndef NDEBUG
        if (splitter->start_same_BLC<splitter_end_unmarked_BLC)
        {
          assert(splitter_end_unmarked_BLC == splitter->start_marked_BLC);
        }
      #endif
      // make splitter stable and move it to the beginning of the list:
      splitter->make_stable();
      linked_list<BLC_indicators>& btc=B.block_to_constellation;
      assert(!btc.empty());
      if (splitter!=btc.begin())
      {
        linked_list<BLC_indicators>::iterator move_splitter_after=btc.end();
        if (m_branching)
        {
          const transition& perhaps_inert_t=m_aut.get_transitions()[*btc.begin()->start_same_BLC];
          if (is_inert_during_init_if_branching(perhaps_inert_t) &&
              m_blocks[m_states[perhaps_inert_t.to()].block].constellation==B.constellation)
          {
            move_splitter_after=btc.begin();
          }
        }
        btc.splice_to_after(move_splitter_after, btc, splitter);
      }
      // now decide what to do further with the block:
      if (first_unmarked_bottom_state==B.start_non_bottom_states)
      {
        // all bottom states are marked.
        // reset all counters of bottom states
        for(fixed_vector<state_in_block_pointer>::iterator s_it=B.start_non_bottom_states; s_it<first_unmarked_bottom_state; ++s_it)
        {
          // mCRL2complexity(s_it->ref_state, ...)
              // not needed because it can be ascribed to the marked transition(s) of s_it->ref_state
          assert(Rmarked==s_it->ref_state->counter);
          s_it->ref_state->counter=undefined;
        }
        // reset all counters of non-bottom states
        assert(m_U.empty());
        clear_state_counters(true);
        m_R.clear();
        return first_unmarked_bottom_state;
      }
#if 0
      if (std::distance(B.start_bottom_states, first_unmarked_bottom_state)>std::distance(first_unmarked_bottom_state, B.start_non_bottom_states))
      {
        // the majority of bottom states is marked
        // so go through the bottom states from the end and swap in those few unmarked bottom states there
        std::vector<state_in_block_pointer>::iterator take_U_bottom_state_from=B.start_bottom_states;
        for (fixed_vector<state_in_block_pointer>::iterator s_it=B.start_non_bottom_states;
             s_it>first_unmarked_bottom_state; )
        {
          --s_it;
          // mCRL2complexity(s_it->ref_state, ...)
              // not needed because it can instead be ascribed to a transition from a marked bottom state
              // (As there are few unmarked bottom states, enough states of the above kind exist.)
          if (Rmarked==s_it->ref_state->counter)
          {
            s_it->ref_state->counter=undefined;
            for (; Rmarked==take_U_bottom_state_from->ref_state->counter; ++take_U_bottom_state_from)
            {
              // mCRL2complexity(take_U_bottom_state_from->ref_state, ...)
                  // not needed because it can be ascribed to the marked transition(s) of take_U_bottom_state_from->ref_state
              take_U_bottom_state_from->ref_state->counter=undefined;
              assert(take_U_bottom_state_from<first_unmarked_bottom_state);
            }
            swap_states_in_states_in_block(take_U_bottom_state_from, s_it);
            ++take_U_bottom_state_from;
            if (take_U_bottom_state_from>=first_unmarked_bottom_state)
            {
              break;
            }
          }
        }
        for (; take_U_bottom_state_from<first_unmarked_bottom_state; ++take_U_bottom_state_from)
        {
          assert(Rmarked==take_U_bottom_state_from->ref_state->counter);
          // mCRL2complexity(take_U_bottom_state_from->ref_state, ...)
              // not needed because it can be ascribed to the marked transition(s) of take_U_bottom_state_from->ref_state
          take_U_bottom_state_from->ref_state->counter=undefined;
        }
      }
      else
#endif
      {
        // few bottom states are marked
        // so go through the marked transitions again and swap the marked bottom states to the beginning
        first_unmarked_bottom_state=B.start_bottom_states;
        for (BLC_list_iterator marked_t_it = splitter_start_marked_BLC; marked_t_it<splitter->end_same_BLC; ++marked_t_it)
        {
          const transition& t = m_aut.get_transitions()[*marked_t_it];
          const state_in_block_pointer s(m_states.begin()+t.from());
          assert(s.ref_state->block==bi);
          // mCRL2complexity(&m_transitions[*i], add_work(...), *this);
              // not needed because this work can be attributed to the marking of the transition
          const fixed_vector<state_in_block_pointer>::iterator pos_s=s.ref_state->ref_states_in_blocks;
          assert(B.start_bottom_states<=pos_s);
          assert(pos_s<B.end_states);
          if (first_unmarked_bottom_state<=pos_s)
          {
            if (0==s.ref_state->no_of_outgoing_inert_transitions)
            {
              assert(pos_s<B.start_non_bottom_states);
              assert(Rmarked==s.ref_state->counter);
              s.ref_state->counter=undefined;
              swap_states_in_states_in_block(first_unmarked_bottom_state, pos_s); // Move marked states to the front.
              first_unmarked_bottom_state++;
            }
          }
          else
          {
            #ifdef NDEBUG
              if (0==s.ref_state->no_of_outgoing_inert_transitions)
              {
                assert(undefined==s.ref_state->counter);
              }
              else
              {
                assert(Rmarked==s.ref_state->counter);
              }
            #endif
          }
        }
      }
      #ifndef NDEBUG
        // ensure that the unmarked transitions do not add any bottom states.
        assert(splitter->start_same_BLC <= splitter_end_unmarked_BLC);
        for(BLC_list_const_iterator i = splitter->start_same_BLC; i < splitter_end_unmarked_BLC; ++i)
        {
          const transition& t = m_aut.get_transitions()[*i];
          const state_in_block_pointer s(m_states.begin()+t.from());
          assert(s.ref_state->block == bi);
          const fixed_vector<state_in_block_pointer>::const_iterator pos_s=s.ref_state->ref_states_in_blocks;
          assert(*pos_s==s);
          assert(B.start_bottom_states <= pos_s);
          assert(pos_s < B.end_states);
          if (0==s.ref_state->no_of_outgoing_inert_transitions)
          {
            // State s is a bottom state. It should already have been marked.
            assert(pos_s < first_unmarked_bottom_state);
          }
          else
          {
            assert(B.start_non_bottom_states <= pos_s);
          }
        }
      #endif
      return first_unmarked_bottom_state;
    } */

/*
    // This routine can only be called after initialisation.
    bool hatU_does_not_cover_B_bottom(const block_index index_block_B,
                                      const constellation_index old_constellation,
                                      fixed_vector<state_index>::iterator first_unmarked_bottom_state)
    {
      mCRL2complexity(&m_blocks[index_block_B], add_work(check_complexity::hatU_does_not_cover_B_bottom__handle_bottom_states_and_their_outgoing_transitions_in_splitter, check_complexity::log_n - check_complexity::ilog2(number_of_states_in_block(index_block_B))), *this);
      assert(m_branching);
      first_unmarked_bottom_state = m_blocks[index_block_B].start_bottom_states;
      for(fixed_vector<state_index>::iterator si=m_blocks[index_block_B].start_bottom_states;
                        si!=m_blocks[index_block_B].start_non_bottom_states;
                      ++si)
      {
        // mCRL2complexity(&m_states[*si], add_work(..., max_C), *this);
            // subsumed by the above call
        bool found=false;
        const outgoing_transitions_it end_it=((*si)+1>=m_states.size())?m_outgoing_transitions.end():m_states[(*si)+1].start_outgoing_transitions;
        for(outgoing_transitions_it tti=m_states[*si].start_outgoing_transitions;
                                     !found && tti!=end_it;
                                     ++tti)
        {
          // mCRL2complexity(&m_transitions[*tti->ref_BLC_transitions], add_work(..., max_C), *this);
          // subsumed by the above call
          const transition& t=m_aut.get_transitions()[*tti->ref_BLC_transitions];
          assert(t.from() == *si);
          if (m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())) && m_blocks[m_states[t.to()].block].constellation==old_constellation)
          {
            found =true;
          }
          assert(tti <= tti->start_same_saC);
          tti = tti->start_same_saC;
        }
        if (!found)
        {
          // This state has no constellation-inert tau transition to the old constellation.
          m_U.add_todo(*si);
        }
        else
        {
          // The state *si has a tau transition to the old constellation that has just become constellation-non-inert.
          m_R.add_todo(*si);
          m_states[*si].counter=Rmarked;
          assert(si == m_states[*si].ref_states_in_blocks);
          assert(first_unmarked_bottom_state <= si);
          assert(si < m_blocks[index_block_B].start_non_bottom_states);
          swap_states_in_states_in_block(first_unmarked_bottom_state, si); // Move marked states to the front.
          first_unmarked_bottom_state++;
        }
      }
      if (first_unmarked_bottom_state != m_blocks[index_block_B].start_non_bottom_states)
      {
        // Splitting can commence.
        return true;
      }
      else
      {
        // Splitting is not possible. Reset the counter in m_states.
        m_U.clear();
        clear_state_counters(true);
        m_R.clear();
        return false;
      }
    } */


    // Select a block that is not the largest block in the constellation.
    // It is advantageous to select the smallest block.
    // The constellation ci is returned.
    block_index select_and_remove_a_block_in_a_non_trivial_constellation()
    {
      // Do the minimal checking, i.e., only check two blocks in a constellation.
      const constellation_index ci=m_non_trivial_constellations.back();
      block_index index_block_B=m_constellations[ci].start_const_states->ref_state->block;           // The first block.
      block_index second_block_B=std::prev(m_constellations[ci].end_const_states)->ref_state->block; // The last block.

      if (number_of_states_in_block(index_block_B)<=number_of_states_in_block(second_block_B))
      {
        m_constellations[ci].start_const_states = m_blocks[index_block_B].end_states;
      }
      else
      {
        m_constellations[ci].end_const_states = m_blocks[second_block_B].start_bottom_states;
        index_block_B=second_block_B;
      }
      return index_block_B;
    }

// =================================================================================================================================
//
//   refine_partition_until_it_becomes_stable.
//
// =================================================================================================================================

    void refine_partition_until_it_becomes_stable()
    {
      // This represents the while loop in Algorithm 1 from line 1.6 to 1.25.

      // The instruction below has complexity O(|Act|);
      // calM will contain the m_BLC_transitions slices that need stabilization:
      std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> > calM;
      // Algorithm 1, line 1.6.
      while (!m_non_trivial_constellations.empty())
      {
        /* static time_t last_log_time=time(nullptr)-1;
        time_t new_log_time = 0;
        if (time(&new_log_time)>last_log_time)
        {
          mCRL2log(log::verbose) << "Refining. There are " << m_blocks.size() << " blocks and " << m_constellations.size() << " constellations.\n";
          last_log_time=last_log_time = new_log_time;
        } */
        //print_data_structures("MAIN LOOP");
        assert(check_data_structures("MAIN LOOP"));
        assert(check_stability("MAIN LOOP"));

        // Algorithm 1, line 1.7.
        block_index index_block_B=select_and_remove_a_block_in_a_non_trivial_constellation();
        const constellation_index old_constellation=m_blocks[index_block_B].constellation;
//std::cerr << "REMOVE BLOCK " << index_block_B << " from constellation " << old_constellation << "\n";

        // Algorithm 1, line 1.8.
        if (m_constellations[old_constellation].start_const_states->ref_state->block==std::prev(m_constellations[old_constellation].end_const_states)->ref_state->block)
        {
          // Constellation has become trivial.
          assert(m_non_trivial_constellations.back()==old_constellation);
          m_non_trivial_constellations.pop_back();
        }
        m_constellations.emplace_back(m_blocks[index_block_B].start_bottom_states, m_blocks[index_block_B].end_states);
        const constellation_index new_constellation=m_constellations.size()-1;
        // Block index_block_B is moved to the new constellation but we shall not yet assign
        // m_blocks[index_block_B].constellation = new_constellation.
        #ifndef NDEBUG
          // m_constellations[new_constellation].work_counter = m_constellations[old_constellation].work_counter;
          const unsigned max_C = check_complexity::log_n - check_complexity::ilog2(number_of_states_in_constellation(new_constellation));
          mCRL2complexity(&m_blocks[index_block_B], add_work(check_complexity::refine_partition_until_it_becomes_stable__find_splitter, max_C), *this);
        #endif
        // Here the variables block_to_constellation and the doubly linked list L_B->C in blocks must be still be updated.
        // This happens further below.

        #ifndef CO_SPLITTER_IN_BLC_LIST
          // Algorithm 1, line 1.9.
          block_label_to_size_t_map block_label_to_cotransition;
        #endif

        for(fixed_vector<state_in_block_pointer>::iterator i=m_blocks[index_block_B].start_bottom_states;
                                                        i!=m_blocks[index_block_B].end_states; ++i)
        {
          // mCRL2complexity(m_states[*i], add_work(check_complexity::..., max_C), *this);
              // subsumed under the above counter
          // and visit the incoming transitions.
          const std::vector<transition>::iterator end_it=
                          (std::next(i->ref_state)==m_states.end()) ? m_aut.get_transitions().end()
                                                   :std::next(i->ref_state)->start_incoming_transitions;
          for(std::vector<transition>::iterator j=i->ref_state->start_incoming_transitions; j!=end_it; ++j)
          {
            const transition& t=*j;
            const transition_index t_index = std::distance(m_aut.get_transitions().begin(),j);
            // mCRL2complexity(&m_transitions[t_index], add_work(check_complexity::..., max_C), *this);
                // subsumed under the above counter

            // Update the state-action-constellation (saC) references in m_outgoing_transitions.
            const outgoing_transitions_it old_pos = m_transitions[t_index].ref_outgoing_transitions;
            const outgoing_transitions_it end_same_saC = old_pos->start_same_saC < old_pos ? old_pos : old_pos->start_same_saC;
            const outgoing_transitions_it new_pos = end_same_saC->start_same_saC;
            assert(m_states[t.from()].start_outgoing_transitions <= new_pos);
            assert(new_pos <= old_pos);
            if (old_pos != new_pos)
            {
              std::swap(old_pos->ref_BLC_transitions,new_pos->ref_BLC_transitions);
              m_transitions[*old_pos->ref_BLC_transitions].ref_outgoing_transitions = old_pos;
              m_transitions[*new_pos->ref_BLC_transitions].ref_outgoing_transitions = new_pos;
            }
            if (new_pos < end_same_saC)
            {
              end_same_saC->start_same_saC = std::next(new_pos);
            }
            // correct start_same_saC provisionally: make them at least point at each other.
            // In the new saC-slice, all transitions point to the first one, except the first one: that shall point at the last one.
            new_pos->start_same_saC = new_pos;
            if (m_states[t.from()].start_outgoing_transitions < new_pos)
            {
              // Check if t is the first transition in the new saC slice:
              const transition& prev_t = m_aut.get_transitions()[*std::prev(new_pos)->ref_BLC_transitions];
              assert(prev_t.from() == t.from());
              if (m_states[prev_t.to()].block == index_block_B && label_or_divergence(prev_t) == label_or_divergence(t))
              {
                // prev_t also belongs to the new saC slice.
                new_pos->start_same_saC = std::prev(new_pos)->start_same_saC;
                assert(m_states[t.from()].start_outgoing_transitions <= new_pos->start_same_saC);
                assert(new_pos->start_same_saC < new_pos);
                assert(std::prev(new_pos) == new_pos->start_same_saC->start_same_saC);
                new_pos->start_same_saC->start_same_saC = new_pos;
              }
            }
          }
        }
        calM.clear();

        // Walk through all states in block B
        for(fixed_vector<state_in_block_pointer>::iterator i=m_blocks[index_block_B].start_bottom_states;
                                                        i!=m_blocks[index_block_B].end_states; ++i)
        {
          // mCRL2complexity(m_states[*i], add_work(check_complexity::..., max_C), *this);
              // subsumed under the above counter

          // and visit the incoming transitions.
          const std::vector<transition>::iterator end_it=
                          (std::next(i->ref_state)==m_states.end()) ? m_aut.get_transitions().end()
                                                   : std::next(i->ref_state)->start_incoming_transitions;
          for(std::vector<transition>::iterator j=i->ref_state->start_incoming_transitions; j!=end_it; ++j)
          {
            const transition& t=*j;
            const transition_index t_index=std::distance(m_aut.get_transitions().begin(),j);
            assert(m_states[t.to()].block == index_block_B);
            bool source_block_is_singleton = (1 >= number_of_states_in_block(m_states[t.from()].block));
            // mCRL2complexity(&m_transitions[t_index], add_work(check_complexity::..., max_C), *this);
                // subsumed under the above counter

            // Give the saC slice of this transition its final correction
            const outgoing_transitions_it out_pos = m_transitions[t_index].ref_outgoing_transitions;
            const outgoing_transitions_it start_new_saC = out_pos->start_same_saC;
            if (start_new_saC < out_pos)
            {
              // not the first transition in the saC-slice
              if (out_pos < start_new_saC->start_same_saC)
              {
                // not the last transition in the saC-slice
                out_pos->start_same_saC = start_new_saC->start_same_saC;
              }
            }

            // mark one cotransition from the same source state (if it exists)
            // If the main transitions are constellation-inert (i.e. they go
            // from index_block_B to index_block_B), we shall not stabilize
            // under them, so we shall also not mark these transitions.
            // (This is also needed to ensure that the BLC set for the special
            // new-bottom-state split does not contain any spurious markings.)
            // If the main transitions start in the old constellation, the
            // co-transitions are constellation-inert and we do not need to
            // stabilize under them.

            else if (!m_blocks[m_states[t.from()].block].contains_new_bottom_states &&
                     !source_block_is_singleton &&
                     (!is_inert_during_init(t) || (/* m_states[t.from()].block != index_block_B && */
                                                   assert(m_blocks[index_block_B].constellation == old_constellation),
                                                   m_blocks[m_states[t.from()].block].constellation != old_constellation)))
            {
              // This is the first transition in the saC-slice
              // It is not inert, and it has not just become non-inert.
              // (Note that the constellation of index_block_B has not yet been changed.)
              // Now we set the pointer to a co-splitter.
              const outgoing_transitions_it start_old_saC = std::next(start_new_saC);
              if (start_old_saC < (t.from()+1 >= m_states.size()
                                   ? m_outgoing_transitions.end()
                                   : m_states[t.from()+1].start_outgoing_transitions))
              {
                assert(start_old_saC <= start_old_saC->start_same_saC);
                const transition& old_t = m_aut.get_transitions()[*start_old_saC->ref_BLC_transitions];
                assert(old_t.from() == t.from());
                if (label_or_divergence(t) == label_or_divergence(old_t) &&
                    old_constellation == m_blocks[m_states[old_t.to()].block].constellation)
                {
//std::cerr << "Marking " << m_transitions[*start_old_saC->ref_BLC_transitions].debug_id(*this) << " as a cotransition.\n";
                  BLC_indicators& ind=*m_transitions[t_index].transitions_per_block_to_constellation;
                  if (ind.is_stable())
                  {
                    ind.make_unstable();
                    // The co-splitter will be moved to its position at the end of the BLC list later.
                  }
                  mark_BLC_transition(start_old_saC);
                }
              }
            }

            #ifndef CO_SPLITTER_IN_BLC_LIST
              // Update the block_label_to_cotransition map.
              if (!source_block_is_singleton && block_label_to_cotransition.find(std::pair(m_states[t.from()].block,label_or_divergence(t))) == block_label_to_cotransition.end())
              {
                // Not found. Add a transition from the LBC_list to block_label_to_cotransition
                // that goes to C\B, or the null_transition if no such transition exists, which prevents searching
                // the list again. Except if t.from is in C\B and a=tau, because in that case it is a (former) constellation-inert transition.
                bool found=false;

                if (!is_inert_during_init(t) || m_blocks[m_states[t.from()].block].constellation!=old_constellation)
                {
                  const BLC_indicators& ind = *m_transitions[t_index].transitions_per_block_to_constellation;
                  BLC_list_iterator transition_walker=ind.start_same_BLC;

                  assert(ind.start_same_BLC <= transition_walker);
                  assert(transition_walker<ind.end_same_BLC);
                  do
                  {
                    const transition& tw=m_aut.get_transitions()[*transition_walker];
                    assert(m_states[tw.from()].block == m_states[t.from()].block);
                    assert(label_or_divergence(tw) == label_or_divergence(t));
                    // The following assertion holds because we set m_blocks[index_block_B].constellation = new_constellation only later.
                    assert(m_blocks[m_states[tw.to()].block].constellation == old_constellation);
                    if (m_states[tw.to()].block != index_block_B)
                    {
                      found=true;
                      block_label_to_cotransition[std::pair(m_states[t.from()].block,label_or_divergence(t))] = *transition_walker;
                      break;
                    }
                    mCRL2complexity(&m_transitions[*transition_walker], add_work(check_complexity::refine_partition_until_it_becomes_stable__find_cotransition, max_C), *this);
                    ++transition_walker;
                  }
                  while (transition_walker<ind.end_same_BLC);
                }
                if (!found)
                {
                  block_label_to_cotransition[std::pair(m_states[t.from()].block,label_or_divergence(t))] = null_transition;
                }
              }
            #endif
            // Update the doubly linked list L_B->C in blocks as the constellation is split in B and C\B.
            if (update_the_doubly_linked_list_LBC_new_constellation(index_block_B, t, t_index) &&
                !source_block_is_singleton &&
                (!is_inert_during_init(t) || index_block_B!=m_states[t.from()].block))
            {
              // a new BLC set has been constructed, insert its start position into calM.
              // (unless the source block is a singleton)
              BLC_list_iterator BLC_pos = m_transitions[t_index].ref_outgoing_transitions->ref_BLC_transitions;
              assert(t_index == *BLC_pos);
              calM.emplace_back(BLC_pos, BLC_pos);
//std::cerr << "This transition is in a main splitter.\n";
              // The end-position (the second element in the pair) will need to be corrected later.
            }
          }
        }
        m_blocks[index_block_B].constellation=new_constellation;

        // mark all states in main splitters and correct the end-positions of calM entries
        for (std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> >::iterator calM_elt=calM.begin(); calM_elt!=calM.end(); )
        {
          linked_list <BLC_indicators>::iterator ind=m_transitions[*calM_elt->first].transitions_per_block_to_constellation;
//std::cerr << "Checking whether " << ind->debug_id(*this) << " is a splitter: ";
          mCRL2complexity(ind, add_work(check_complexity::refine_partition_until_it_becomes_stable__correct_end_of_calM, max_C), *this);
          assert(ind->start_same_BLC==calM_elt->first);
          assert(!has_marked_transitions(*ind));
          // check if all transitions were moved to the new constellation,
          // or some transitions to the old constellation have remained:
          const transition& last_t=m_aut.get_transitions()[*std::prev(ind->end_same_BLC)];
          assert(m_blocks[m_states[last_t.to()].block].constellation==new_constellation);
          const transition* next_t=nullptr;
          if ((is_inert_during_init(last_t) && m_blocks[m_states[last_t.from()].block].constellation==old_constellation &&
               (assert(m_states[last_t.from()].block!=index_block_B),
//std::cerr << "yes, it was constellation-inert earlier but is no more\n",
                                                                       true)) ||
              (ind->end_same_BLC<&*m_BLC_transitions.end() &&
               (next_t=&m_aut.get_transitions()[*ind->end_same_BLC],
                m_states[last_t.from()].block==m_states[next_t->from()].block &&
                label_or_divergence(last_t)==label_or_divergence(*next_t) &&
                m_blocks[m_states[next_t->to()].block].constellation==old_constellation
//&& (std::cerr << "yes, there are transitions in the corresponding co-splitter\n", true)
                                                                                       )))
          {
            // there are some transitions to the corresponding co-splitter,
            // so we will have to stabilize the block
            calM_elt->second = ind->end_same_BLC;
            ind->make_unstable();
            ind->start_marked_BLC = ind->start_same_BLC;
            // The mCRL2complexity call above assigns work to every transition in ind, so we are allowed to mark all transitions at once.
            // now splice ind to the end of its list, together with its co-splitter:
            linked_list<BLC_indicators>& btc=m_blocks[m_states[last_t.from()].block].block_to_constellation;
            if (nullptr!=next_t)
            {
              // the co-splitter has already been marked as unstable if there are transitions from bottom states,
              // but it now needs to move to the end of the list of BLC sets
              const linked_list<BLC_indicators>::iterator co_ind=m_transitions[*ind->end_same_BLC].transitions_per_block_to_constellation;
              if (co_ind->is_stable())
              {
                co_ind->make_unstable();
              }
              else
              {
                assert(has_marked_transitions(*co_ind));
              }
              assert(co_ind->start_same_BLC==ind->end_same_BLC);
              btc.splice(btc.end(), btc, co_ind);
            }
            if (btc.before_end()!=ind)
            {
              btc.splice(btc.end(), btc, ind);
            }
            ++calM_elt;
          }
          else
          {
//std::cerr << "no, all transitions in the old BLC set have moved to the new BLC set\n";
            // all transitions in the old BLC set have moved to the new BLC set;
            // as the old BLC set was stable, so is the new one.
            // We can skip this element.
            #ifndef CO_SPLITTER_IN_BLC_LIST
              #ifndef NDEBUG
                const block_index from_block=m_states[last_t.from()].block;
                typename block_label_to_size_t_map::iterator bltc_it=block_label_to_cotransition.find(std::pair(from_block, label_or_divergence(last_t)));
                assert(bltc_it==block_label_to_cotransition.end() || bltc_it->second==null_transition);
              #endif
              /*if (bltc_it!=block_label_to_cotransition.end() && bltc_it->second!=null_transition)
              {
                linked_list<BLC_indicators>::iterator co_ind = m_transitions[bltc_it->second].transitions_per_block_to_constellation;
                #ifndef NDEBUG
                  const transition& co_t=m_aut.get_transitions()[co_ind->start_same_BLC];
                  assert(m_states[co_t.from()].block==from_block);
                  assert(label_or_divergence(co_t)==label_or_divergence(last_t));
                  assert(m_blocks[m_states[co_t.to()].block].constellation==old_constellation);
                #endif
                if (co_ind->is_stable())
                {
                  assert(is_inert_during_init(co_t) && m_blocks[from_block].constellation==old_constellation);
                }
                else
                {
                  assert(!(is_inert_during_init(co_t) && m_blocks[from_block].constellation==old_constellation));
                  co_ind->make_stable();
                  linked_list<BLC_indicators>& btc=m_blocks[from_block].block_to_constellation;
                  assert(!btc.empty());
                  const transition& first_t=m_aut.get_transitions()[btc.begin()->start_same_BLC];
                  assert(m_states[first_t.from()].block==from_block);
                  btc.splice_after(is_inert_during_init(first_t) && m_blocks[m_states[first_t.to()].block].constellation==m_blocks[from_block].constellation
                                   ? btc.begin() : btc.end(),
                                   btc, co_ind);
                }
              }*/
            #endif
            #ifndef NDEBUG
              bool at_end=std::next(calM_elt)==calM.end();
            #endif
            calM_elt->first=calM.back().first;
            calM.pop_back();
            assert(at_end == (calM_elt==calM.end()));
          }
        }

        // ---------------------------------------------------------------------------------------------
        // First carry out a co-split of B with respect to C\B and an action tau.
        if (m_branching &&
            number_of_states_in_block(index_block_B) > 1)
        {
          linked_list<BLC_indicators>::iterator tau_co_splitter = find_inert_co_transition_for_block(index_block_B, old_constellation);

          // Algorithm 1, line 1.19.
          if (m_blocks[index_block_B].block_to_constellation.end() != tau_co_splitter)
          {
            tau_co_splitter->make_unstable();
            tau_co_splitter->start_marked_BLC = tau_co_splitter->start_same_BLC;
            // We have to give credit for marking all transitions in the splitter at once:
            mCRL2complexity(tau_co_splitter, add_work(check_complexity::refine_partition_until_it_becomes_stable__prepare_cosplit, max_C), *this);
            // The routine below has a side effect, as it sets m_R for all bottom states of block B.
            const BLC_list_iterator splitter_end_unmarked_BLC = tau_co_splitter->start_marked_BLC;
            fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state=not_all_bottom_states_are_touched(tau_co_splitter
                            #ifndef NDEBUG
                              , splitter_end_unmarked_BLC
                            #endif
                            );
            assert(tau_co_splitter->is_stable()); // not_all_bottom_states_are_touched() should have cleaned up
            if (first_unmarked_bottom_state < m_blocks[index_block_B].start_non_bottom_states)
            {
            // Algorithm 1, line 1.10.

//std::cerr << "DO A TAU CO SPLIT " << old_constellation << "\n";
              splitB(tau_co_splitter, first_unmarked_bottom_state,
                        splitter_end_unmarked_BLC,
                        #ifdef CO_SPLITTER_IN_BLC_LIST
                          old_constellation // needed, because index_block_B might be split again later under other labels.
                        #else
                          [&block_label_to_cotransition, old_constellation, this]
                          (const block_index old_block,
                           const block_index new_block,
                           const transition_index moved_transition,
                           const transition_index alternative_transition)
                          {
                            maintain_block_label_to_cotransition(
                                    old_block,
                                    new_block,
                                    moved_transition,
                                    alternative_transition, block_label_to_cotransition,
                                    old_constellation);
                          }
                        #endif
                        );
            }
          }
        }
        // Algorithm 1, line 1.10.
        for (std::pair<BLC_list_iterator, BLC_list_iterator> calM_elt: calM)
        {
          // mCRL2complexity(..., add_work(..., max_C), *this);
              // not needed as the inner loop is always executed at least once.
          #ifdef CO_SPLITTER_IN_BLC_LIST
            //print_data_structures("Main loop");
            assert(check_stability("Main loop", &calM, &calM_elt, old_constellation));
          #else
            //print_data_structures("Main loop", &block_label_to_cotransition);
            assert(check_stability("Main loop", &calM, &calM_elt, &block_label_to_cotransition));
          #endif
          assert(check_data_structures("Main loop", false, false));
          // Algorithm 1, line 1.11.
          assert(calM_elt.first < calM_elt.second);
          do
          {
            linked_list<BLC_indicators>::iterator splitter = m_transitions[*std::prev(calM_elt.second)].transitions_per_block_to_constellation;
            // mCRL2complexity(splitter, add_work(..., max_C), *this);
                // not needed, as all transitions in calM are transitions into the small new constellation.
            assert(splitter->start_same_BLC < calM_elt.second);
            assert(splitter->end_same_BLC==calM_elt.second);
            assert(splitter->start_same_BLC<=splitter->start_marked_BLC);
            assert(splitter->start_marked_BLC<=splitter->end_same_BLC);
            calM_elt.second = splitter->start_same_BLC;

            assert(splitter->start_same_BLC<splitter->end_same_BLC);
            const transition& first_t = m_aut.get_transitions()[*splitter->start_same_BLC];
            const label_index a = label_or_divergence(first_t);
            assert(m_blocks[m_states[first_t.to()].block].constellation == new_constellation);
            block_index Bpp = m_states[first_t.from()].block;
//std::cerr << "INVESTIGATE ACTION " << (m_aut.num_action_labels()==a ? "(tau-self-loops)" : pp(m_aut.action_label(a))) << ", source block==" << Bpp << ", target constellation==" << new_constellation << ", splitter==" << splitter->debug_id(*this) << '\n';
            assert(m_blocks[Bpp].constellation!=new_constellation || !is_inert_during_init(first_t));
            if (number_of_states_in_block(Bpp) <= 1)
            {
              // a block with 1 state does not need to be split
//std::cerr << "No main/co-split is needed because the block contains only 1 state.\n";
              //splitter->make_stable();
              // The following would require to calculate the co-splitter even in this case:
              //if (m_blocks[Bpp].block_to_constellation.end()!=co_splitter)
              //{
              //  co_splitter->make_stable();
              //}
            }
            else if (m_blocks[Bpp].contains_new_bottom_states)
            {
              // The block Bpp contains new bottom states, and it is not necessary to spend any work on it now.
              // We will later stabilize it in stabilizeB().
//std::cerr << "No main/co-split is needed because the block contains new bottom states.\n";
              //splitter->make_stable();
              // The following would require to calculate the co-splitter even in this case:
              //if (m_blocks[Bpp].block_to_constellation.end()!=co_splitter)
              //{
              //  co_splitter->make_stable();
              //}
            }
            else
            {
              assert(has_marked_transitions(*splitter));
              assert(splitter->start_marked_BLC == splitter->start_same_BLC);
              // Check whether the bottom states of Bpp are not all included in Mleft.
              const BLC_list_iterator splitter_end_unmarked_BLC = splitter->start_marked_BLC;
              #ifdef CO_SPLITTER_IN_BLC_LIST
                linked_list<BLC_indicators>::iterator co_splitter = m_blocks[Bpp].block_to_constellation.prev(splitter);
                if (m_blocks[Bpp].block_to_constellation.end()!=co_splitter)
                {
//std::cerr << "Current co-splitter candidate is: " << co_splitter->debug_id(*this);
                  assert(co_splitter->start_same_BLC<co_splitter->end_same_BLC);
                  const transition& co_t=m_aut.get_transitions()[*co_splitter->start_same_BLC];
                  assert(m_states[co_t.from()].block==Bpp);
                  if (a!=label_or_divergence(co_t) ||
                      old_constellation!=m_blocks[m_states[co_t.to()].block].constellation)
                  {
                    // This is not the real co-splitter.
                    co_splitter=m_blocks[Bpp].block_to_constellation.end();
//std::cerr << " -- not the true co-splitter";
                  }
                  else if (is_inert_during_init(co_t) && m_blocks[Bpp].constellation==old_constellation)
                  {
                    // the co-splitter contains constellation-inert
                    // transitions, so no stabilisation under this co-splitter is needed.
                    assert(co_splitter->is_stable());
                    co_splitter=m_blocks[Bpp].block_to_constellation.end();
//std::cerr << " -- constellation-inert, so no co-split is needed";
                  }
//std::cerr << '\n';
                }
              #else
                linked_list<BLC_indicators>::iterator co_splitter=m_blocks[Bpp].block_to_constellation.end();
                typename block_label_to_size_t_map::const_iterator bltc_it=block_label_to_cotransition.find(std::pair(Bpp,a));

                if (bltc_it!=block_label_to_cotransition.end() &&
                    bltc_it->second!=null_transition)
                {
                  #ifndef NDEBUG
//std::cerr << "CO-TRANSITION  " << ptr(bltc_it->second) << "\n";
                    const transition& co_t=m_aut.get_transitions()[bltc_it->second];
                    assert(m_states[co_t.from()].block==Bpp);
                    assert(m_blocks[m_states[co_t.to()].block].constellation==old_constellation);
                    assert(!(is_inert_during_init(co_t) && m_blocks[Bpp].constellation==old_constellation));
                    assert(m_states[m_aut.get_transitions()[*m_transitions[bltc_it->second].transitions_per_block_to_constellation->start_same_BLC].from()].block==Bpp);
                  #endif
                  co_splitter=m_transitions[bltc_it->second].transitions_per_block_to_constellation;
                }
              #endif
              const BLC_list_iterator co_splitter_begin=m_blocks[Bpp].block_to_constellation.end()==co_splitter ? &*m_BLC_transitions.end() : co_splitter->start_same_BLC;
                    BLC_list_iterator co_splitter_end  =m_blocks[Bpp].block_to_constellation.end()==co_splitter ? &*m_BLC_transitions.end() : co_splitter->end_same_BLC;

              fixed_vector<state_in_block_pointer>::iterator first_unmarked_bottom_state=not_all_bottom_states_are_touched(splitter
                        #ifndef NDEBUG
                          , splitter_end_unmarked_BLC
                        #endif
                        );
              assert(splitter->is_stable()); // not_all_bottom_states_are_touched() should have cleaned up
              if (first_unmarked_bottom_state < m_blocks[Bpp].start_non_bottom_states)
              {
//std::cerr << "PERFORM A MAIN SPLIT\n";
              // Algorithm 1, line 1.12.
                Bpp = splitB(splitter,
                                        first_unmarked_bottom_state,
                                        splitter_end_unmarked_BLC,
                                        #ifdef CO_SPLITTER_IN_BLC_LIST
                                          old_constellation
                                        #else
                                          [&block_label_to_cotransition, old_constellation, this]
                                          (const block_index old_block,
                                           const block_index new_block,
                                           const transition_index moved_transition,
                                           const transition_index alternative_transition)
                                          {
                                            maintain_block_label_to_cotransition(
                                                    old_block,
                                                    new_block,
                                                    moved_transition,
                                                    alternative_transition,
                                                    block_label_to_cotransition,
                                                    old_constellation);
                                          }
                                        #endif
                                        );
                if (linked_list<BLC_indicators>::end()!=co_splitter)
                {
                  // The former co-splitter has been separated into up to three parts:
                  // one with transitions starting in the R-subblock (that will be the true co-splitter);
                  // one with transitions starting in the U-subblock (that actually is already stable);
                  // one with transitions starting in new bottom states and their predecessors.
                  co_splitter=m_blocks[Bpp].block_to_constellation.end();
                  assert(co_splitter_begin<co_splitter_end);
                  #ifndef NDEBUG
                    int number_of_iterations=0;
                  #endif
                  do
                  {
                    #ifndef NDEBUG
                      // ensure that there are at most 3 iterations in this loop (to keep time complexity within O(1)).
                      ++number_of_iterations; assert(number_of_iterations<=3);
                    #endif
                    linked_list<BLC_indicators>::iterator candidate=m_transitions[*std::prev(co_splitter_end)].transitions_per_block_to_constellation;
                    co_splitter_end=candidate->start_same_BLC;
                    const transition& t=m_aut.get_transitions()[*candidate->start_same_BLC];
                    assert(m_blocks[m_states[t.to()].block].constellation==old_constellation);
                    if (m_states[t.from()].block==Bpp)
                    {
                      // This is the part that starts in the R-subblock
                      assert(null_block!=Bpp);
                      assert(m_blocks[Bpp].block_to_constellation.end()==co_splitter);
                      co_splitter=candidate;
//std::cerr << "Co-splitter updated to " << co_splitter->debug_id(*this) << '\n';
                    }
                    else if (m_blocks[m_states[t.from()].block].contains_new_bottom_states)
                    {
                      // This is the part that starts in the new bottom block. Don't do anything
//std::cerr << candidate->debug_id(*this) << " was (part of) the co-splitter but now starts in the new bottom block.\n";
                      //candidate->make_stable(); // forget state markings
                    }
                    else
                    {
                      // This is the part that starts in the U-subblock.
                      // It is actually already stable.
                      // So we should mark it as such and move it to the beginning of its list of BLC sets.
                      assert(!candidate->is_stable());
                      assert(!has_marked_transitions(*candidate));
//std::cerr << candidate->debug_id(*this) << " was (part of) the co-splitter but now starts in the U-subblock.\n";
                      candidate->make_stable();
                      linked_list<BLC_indicators>& U_btc=m_blocks[m_states[t.from()].block].block_to_constellation;
                      if (U_btc.begin()!=candidate)
                      {
                        assert(!(is_inert_during_init(t) && m_blocks[m_states[t.from()].block].constellation==old_constellation));
                        linked_list<BLC_indicators>::iterator move_candidate_to=U_btc.end();
                        if (m_branching)
                        {
                          const transition& perhaps_inert_t=m_aut.get_transitions()[*U_btc.begin()->start_same_BLC];
                          if (is_inert_during_init_if_branching(perhaps_inert_t) &&
                              m_blocks[m_states[perhaps_inert_t.from()].block].constellation ==
                              m_blocks[m_states[perhaps_inert_t.to()  ].block].constellation)
                          {
                            move_candidate_to=U_btc.begin();
                          }
                        }
                        U_btc.splice_to_after(move_candidate_to, U_btc, candidate);
                      }
                    }
                  }
                  while (co_splitter_begin<co_splitter_end);
                }
                if (null_block == Bpp)
                {
                  // all bottom states in the R-subblock are new bottom states.
                  // Then no co-split is needed to stabilize the old bottom states in the R-block!
                  continue;
                }
                assert(0 <= Bpp); assert(Bpp < m_blocks.size());
                // Algorithm 1, line 1.13.
                // Algorithm 1, line 1.14 is implicitly done in the call of splitB above.
              }
//else { std::cerr << "No main split is needed because all bottom states have a transition in the splitter.\n"; }
              assert(!m_blocks[Bpp].contains_new_bottom_states);
              // Algorithm 1, line 1.17 and line 1.18.
              if (m_blocks[Bpp].block_to_constellation.end()!=co_splitter)
              {
//std::cerr << "BLOCK THAT IS CO-SPLIT " << Bpp << "\n";
                assert(!co_splitter->is_stable());
                // Algorithm 1, line 1.19.
                if (1>=number_of_states_in_block(Bpp))
                {
//std::cerr << "No co-split is needed because the remaining block contains only 1 state.\n";
                  //co_splitter->make_stable();
                }
                else
                {
                  const BLC_list_iterator splitter_end_unmarked_BLC = co_splitter->start_marked_BLC;
                  first_unmarked_bottom_state = not_all_bottom_states_are_touched(co_splitter
                            #ifndef NDEBUG
                              , splitter_end_unmarked_BLC
                            #endif
                            );
                  assert(co_splitter->is_stable());
                  if (first_unmarked_bottom_state < m_blocks[Bpp].start_non_bottom_states)
                  {
//std::cerr << "PERFORM A CO-SPLIT\n";
                    splitB(co_splitter,
                        first_unmarked_bottom_state,
                        splitter_end_unmarked_BLC,
                        #ifdef CO_SPLITTER_IN_BLC_LIST
                          old_constellation
                        #else
                          [&block_label_to_cotransition, old_constellation, this]
                          (const block_index old_block,
                           const block_index new_block,
                           const transition_index moved_transition,
                           const transition_index alternative_transition)
                          {
                            maintain_block_label_to_cotransition(
                                    old_block,
                                    new_block,
                                    moved_transition,
                                    alternative_transition, block_label_to_cotransition,
                                    old_constellation);
                          }
                        #endif
                        );
                  }
//else { std::cerr << "No co-split is needed because all bottom states have a transition in the co-splitter.\n"; }
                }
                // Algorithm 1, line 1.20 and 1.21. P is updated implicitly when splitting Bpp.
              }
              else
              {
                  // check that there is really no co-splitter
                  #ifndef NDEBUG
                    if (m_branching && m_aut.is_tau(a) && m_blocks[Bpp].constellation == old_constellation)
                    {
//std::cerr << "No co-split is needed because the co-splitting transitions are constellation-inert.\n";
                    }
                    else
                    {
                      for (const BLC_indicators& ind : m_blocks[Bpp].block_to_constellation)
                      {
                        assert(ind.start_same_BLC<ind.end_same_BLC);
                        const transition& co_t = m_aut.get_transitions()[*ind.start_same_BLC];
                        assert(m_states[co_t.from()].block == Bpp);
                        assert(label_or_divergence(co_t) != a ||
                             m_blocks[m_states[co_t.to()].block].constellation != old_constellation);
                      }
//std::cerr << "No co-split is needed because there is no co-splitter.\n";
                    }
                  #endif
              }
            }
          }
          while (calM_elt.first < calM_elt.second);
        }

        //print_data_structures("Before stabilize");
        assert(check_data_structures("Before stabilize", false, false));
        assert(check_stability("Before stabilize"));
        stabilizeB();
      }
    }
};

/* ************************************************************************* */
/*                                                                           */
/*                             I N T E R F A C E                             */
/*                                                                           */
/* ************************************************************************* */


/// \brief nonmember functions serving as interface with the rest of mCRL2
/// \details These functions are copied, almost without changes, from
/// liblts_bisim_gw.h, which was written by Anton Wijs.

/// \brief Reduce transition system l with respect to strong or
/// (divergence-preserving) branching bisimulation.
/// \param[in,out] l                   The transition system that is reduced.
/// \param         branching           If true branching bisimulation is
///                                    applied, otherwise strong bisimulation.
/// \param         preserve_divergence Indicates whether loops of internal
///                                    actions on states must be preserved.  If
///                                    false these are removed.  If true these
///                                    are preserved.
template <class LTS_TYPE>
void bisimulation_reduce_gj(LTS_TYPE& l, const bool branching = false,
                                         const bool preserve_divergence = false)
{
    if (1 >= l.num_states())
    {
        mCRL2log(log::warning) << "There is only 1 state in the LTS. It is not "
                "guaranteed that branching bisimulation minimisation runs in "
                "time O(m log n).\n";
    }
    // Line 1.2: Find tau-SCCs and contract each of them to a single state
    mCRL2log(log::verbose) << "Start SCC\n";
    if (branching)
    {
        scc_reduce(l, preserve_divergence);
    }

    // Now apply the branching bisimulation reduction algorithm.  If there
    // are no taus, this will automatically yield strong bisimulation.
    mCRL2log(log::verbose) << "Start Partitioning\n";
    bisim_partitioner_gj<LTS_TYPE> bisim_part(l, branching, preserve_divergence);

    // Assign the reduced LTS
    mCRL2log(log::verbose) << "Start finalizing\n";
    bisim_part.finalize_minimized_LTS();
}


/// \brief Checks whether the two initial states of two LTSs are strong or
/// (divergence-preserving) branching bisimilar.
/// \details This routine uses the O(m log n) branching bisimulation algorithm
/// developed in 2018 by David N. Jansen.  It runs in O(m log n) time and uses
/// O(m) memory, where n is the number of states and m is the number of
/// transitions.
///
/// The LTSs l1 and l2 are not usable anymore after this call.
/// \param[in,out] l1                  A first transition system.
/// \param[in,out] l2                  A second transistion system.
/// \param         branching           If true branching bisimulation is used,
///                                    otherwise strong bisimulation is
///                                    applied.
/// \param         preserve_divergence If true and branching is true, preserve
///                                    tau loops on states.
/// \param         generate_counter_examples  (non-functional, only in the
///                                    interface for historical reasons)
/// \returns True iff the initial states of the transition systems l1 and l2
/// are ((divergence-preserving) branching) bisimilar.
template <class LTS_TYPE>
bool destructive_bisimulation_compare_gj(LTS_TYPE& l1, LTS_TYPE& l2,
        const bool branching = false, const bool preserve_divergence = false,
        const bool generate_counter_examples = false,
        const std::string& /*counter_example_file*/ = "", bool /*structured_output*/ = false)
{
    if (generate_counter_examples)
    {
        mCRL2log(log::warning) << "The GJ24 branching bisimulation "
                              "algorithm does not generate counterexamples.\n";
    }
    std::size_t init_l2(l2.initial_state() + l1.num_states());
    detail::merge(l1, std::move(l2));
    l2.clear(); // No use for l2 anymore.

    // Line 2.1: Find tau-SCCs and contract each of them to a single state
    if (branching)
    {
        detail::scc_partitioner<LTS_TYPE> scc_part(l1);
        scc_part.replace_transition_system(preserve_divergence);
        init_l2 = scc_part.get_eq_class(init_l2);
    }                                                                           else  assert(!preserve_divergence);
                                                                                assert(1 < l1.num_states());
    bisim_partitioner_gj<LTS_TYPE> bisim_part(l1, branching, preserve_divergence);

    return bisim_part.in_same_class(l1.initial_state(), init_l2);
}


/// \brief Checks whether the two initial states of two LTSs are strong or
/// (divergence-preserving) branching bisimilar.
/// \details The LTSs l1 and l2 are first duplicated and subsequently reduced
/// modulo bisimulation.  If memory is a concern, one could consider to use
/// destructive_bisimulation_compare().  This routine uses the O(m log n)
/// branching bisimulation algorithm developed in 2018 by David N. Jansen.  It
/// runs in O(m log n) time and uses O(m) memory, where n is the number of
/// states and m is the number of transitions.
/// \param l1                  A first transition system.
/// \param l2                  A second transistion system.
/// \param branching           If true branching bisimulation is used,
///                            otherwise strong bisimulation is applied.
/// \param preserve_divergence If true and branching is true, preserve tau
///                            loops on states.
/// \retval True iff the initial states of the transition systems l1 and l2
/// are ((divergence-preserving) branching) bisimilar.
template <class LTS_TYPE>
inline bool bisimulation_compare_gj(const LTS_TYPE& l1, const LTS_TYPE& l2,
          const bool branching = false, const bool preserve_divergence = false)
{
    LTS_TYPE l1_copy(l1);
    LTS_TYPE l2_copy(l2);
    return destructive_bisimulation_compare_gj(l1_copy, l2_copy, branching,
                                                          preserve_divergence);
}


} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef LIBLTS_BISIM_GJ_H
