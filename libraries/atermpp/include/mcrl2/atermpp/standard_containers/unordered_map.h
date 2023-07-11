// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/data/standard_containers/unordered_map.h
/// \brief This file contains a unordered_map class that behaves 
///        exactly as a standard unordered_map. It can only be used
///        to store class instances that derive from aterms.
///        The stored aterms are protected as a whole, i.e.,
///        time and memory is saved as individual protection
///        per element is unnecessary. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_MAP_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_MAP_H

#include <unordered_map>
#include "mcrl2/atermpp/detail/aterm_container.h"
#include "mcrl2/utilities/shared_mutex.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A unordered_map class in which aterms can be stored. 
template < class Key, 
           class T,
           class Hash = std::hash<detail::reference_aterm<Key> >,
           class Pred = std::equal_to<detail::reference_aterm<Key> >,
           class Alloc = std::allocator< std::pair<const detail::reference_aterm<Key>, detail::reference_aterm<T> > > >

class unordered_map : public std::unordered_map< detail::reference_aterm<Key>, detail::reference_aterm<T>, Hash, Pred, Alloc >,
                        protected detail::generic_aterm_container<
                                     std::unordered_map<detail::reference_aterm<Key>, 
                                     detail::reference_aterm<T>, Hash, Pred, Alloc> >
{
protected:
  typedef std::unordered_map< detail::reference_aterm<Key>, detail::reference_aterm<T>, Hash, Pred, Alloc > super;
  typedef detail::generic_aterm_container<std::unordered_map< detail::reference_aterm<Key>, detail::reference_aterm<T>, Hash, Pred, Alloc > > container_wrapper;

public:
  
  /// Standard typedefs.
  typedef typename super::allocator_type allocator_type;
  typedef typename super::value_type value_type;
  typedef typename super::size_type size_type;
  typedef typename super::node_type node_type;
  typedef typename super::reference reference;
  typedef typename super::iterator iterator;
  typedef typename super::const_iterator const_iterator;
  typedef typename super::insert_return_type insert_return_type;

  /// \brief Default constructor.
  unordered_map()
   : super(),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  explicit unordered_map (const allocator_type& alloc)
   : super::unordered_map(alloc),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  explicit unordered_map (size_type n, const allocator_type& alloc = allocator_type())
   : super::unordered_map(n, alloc),
     container_wrapper(*this, true)     
  {}

  unordered_map (size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
   : super::unordered_map(n, detail::reference_aterm(val), alloc),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  template <class InputIterator>
  unordered_map (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : super::unordered_map(first, last, alloc),
     container_wrapper(*this, true)     
  {}
    
  /// \brief Constructor.
  unordered_map (const unordered_map& x)
   : super::unordered_map(x),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  unordered_map (const unordered_map& x, const allocator_type& alloc)
   : super::unordered_map(x, alloc),
     container_wrapper(*this, true)     
  {}
  
  /// \brief Constructor.
  unordered_map (unordered_map&& x)
   : super::unordered_map(std::move(x)),
     container_wrapper(*this, true)     
  {}


  /// \brief Constructor.
  unordered_map (unordered_map&& x, const allocator_type& alloc)
   : super::unordered_map(std::move(x), alloc),
     container_wrapper(*this, true)
  {}

  /// \brief Constructor. To be done later....
  unordered_map (std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
    : super::unordered_map(il, alloc),
      container_wrapper(*this, true)      
  {}

  /// \brief Standard assignment.
  unordered_map& operator=(const unordered_map& other)=default;

  /// \brief Standard move assignment.
  unordered_map& operator=(unordered_map&& other)=default;

  /// \brief Standard destructor.
  ~unordered_map()=default;

  void clear() noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::clear();
  }

  /// \brief Inserts an element referring to a default value in the map. 
  std::pair<iterator,bool> insert( const value_type& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value);
  }

  template< class P >
  std::pair<iterator,bool> insert( P&& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value); 
  }

  iterator insert( const_iterator hint, const value_type& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }

  template< class P >
  iterator insert( const_iterator hint, P&& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }

  template< class InputIt >
  void insert( InputIt first, InputIt last )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(first, last);
  }

  void insert( std::initializer_list<value_type> ilist )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(ilist);
  }

  insert_return_type insert( node_type&& nh )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(nh);
  }

  iterator insert( const_iterator hint, node_type&& nh )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, nh);
  }

  template <class M>
  std::pair<iterator, bool> insert_or_assign( const Key& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(k, obj);
  }

  template <class M>
  std::pair<iterator, bool> insert_or_assign( Key&& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(k, obj);
  }

  template <class M>
  iterator insert_or_assign( const_iterator hint, const Key& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(hint, k, obj); 
  }

  template <class M>
  iterator insert_or_assign( const_iterator hint, Key&& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(hint, k, obj);   
  }

  template< class... Args >
  std::pair<iterator,bool> emplace( Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace(args...);
  }

  template <class... Args>
  iterator emplace_hint( const_iterator hint, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace_hint(hint, args...);
  }

  template< class... Args >
  std::pair<iterator, bool> try_emplace( const Key& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(k, args...);
  }

  template< class... Args >
  std::pair<iterator, bool> try_emplace( Key&& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(k, args...);
  }

  template< class... Args >
  iterator try_emplace( const_iterator hint, const Key& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(hint, k, args...);
  }

  template< class... Args >
  iterator try_emplace( const_iterator hint, Key&& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(hint, k, args...);
  }

  iterator erase( iterator pos )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);
  }

  iterator erase( const_iterator pos )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);   
  }	
  
  iterator erase( const_iterator first, const_iterator last )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(first, last);
  }

  size_type erase( const Key& key )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(key);
  }

  void swap( unordered_map& other )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::swap(other);
  }

  node_type extract( const_iterator position )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return extract(position);
  }
  
  node_type extract( const Key& k )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return extract(k);
  }

  template<class H2, class P2>
  void merge( std::unordered_map<Key, T, H2, P2, allocator_type>& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }

  template<class H2, class P2>
  void merge( std::unordered_map<Key, T, H2, P2, allocator_type>&& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }

  template<class H2, class P2>
  void merge( std::unordered_multimap<Key, T, H2, P2, allocator_type>& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }

  template<class H2, class P2>
  void merge( std::unordered_multimap<Key, T, H2, P2, allocator_type>&& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }

  std::size_t size() const override
  {
    return super::size();
  }
};

namespace utilities
{
/// \brief A unordered_map class in which aterms can be stored. 
template < class Key,
  class T,
  class Hash = std::hash<detail::reference_aterm<Key> >,
  class Pred = std::equal_to<detail::reference_aterm<Key> >,
  class Alloc = std::allocator< std::pair<const detail::reference_aterm<Key>, detail::reference_aterm<T> > >,
  bool ThreadSafe = false >

class unordered_map : public mcrl2::utilities::unordered_map< detail::reference_aterm<Key>, 
                                                              detail::reference_aterm<T>, Hash, Pred, Alloc, ThreadSafe, false >,
                      protected detail::generic_aterm_container< mcrl2::utilities::unordered_map<detail::reference_aterm<Key>,
                                                                                                 detail::reference_aterm<T>, 
                                                                                                 Hash, Pred, Alloc, ThreadSafe, false > >
{
  protected:
    typedef mcrl2::utilities::unordered_map< detail::reference_aterm<Key>, detail::reference_aterm<T>, Hash, Pred, Alloc, ThreadSafe, false > super;
    typedef detail::generic_aterm_container<mcrl2::utilities::unordered_map< detail::reference_aterm<Key>, detail::reference_aterm<T>, Hash, Pred, Alloc, ThreadSafe, false > > container_wrapper;

  public:

    /// Standard typedefs.
    typedef typename super::allocator_type allocator_type;
    typedef typename super::value_type value_type;
    typedef typename super::size_type size_type;
    typedef typename super::reference reference;
    typedef typename super::iterator iterator;
    typedef typename super::const_iterator const_iterator;

    /// \brief Default constructor.
    unordered_map()
      : super(),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor.
    explicit unordered_map(const allocator_type& alloc)
      : super::unordered_map(alloc),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor.
    explicit unordered_map(size_type n, const allocator_type& alloc = allocator_type())
      : super::unordered_map(n, alloc),
      container_wrapper(*this, true)
    {}

    unordered_map(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
      : super::unordered_map(n, detail::reference_aterm(val), alloc),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor.
    template <class InputIterator>
    unordered_map(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
      : super::unordered_map(first, last, alloc),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor.
    unordered_map(const unordered_map& x)
      : super::unordered_map(x),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor.
    unordered_map(const unordered_map& x, const allocator_type& alloc)
      : super::unordered_map(x, alloc),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor.
    unordered_map(unordered_map&& x)
      : super::unordered_map(std::move(x)),
      container_wrapper(*this, true)
    {}


    /// \brief Constructor.
    unordered_map(unordered_map&& x, const allocator_type& alloc)
      : super::unordered_map(std::move(x), alloc),
      container_wrapper(*this, true)
    {}

    /// \brief Constructor. To be done later....
    unordered_map(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
      : super::unordered_map(il, alloc),
      container_wrapper(*this, true)
    {}

    /// \brief Standard assignment.
    unordered_map& operator=(const unordered_map& other) = default;

    /// \brief Standard move assignment.
    unordered_map& operator=(unordered_map&& other) = default;

    /// \brief Standard destructor.
    ~unordered_map() = default;

    void clear() noexcept
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      super::clear();
    }

    /// \brief Standard find function in a map.
    /// \returns Element with the specified key.
    template<typename ...Args>
    iterator find(const Args&... args)
    { 
      if constexpr (ThreadSafe)
      {
        mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
        return super::find(args...); 
      }
      return super::find(args...); 
    }

    /// \brief Standard find function in a map.
    /// \returns Element with the specified key.
    template<typename ...Args>
    const_iterator find(const Args&... args) const 
    { 
      if (ThreadSafe)
      {
        mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
        return super::find(args...); 
      }
      return super::find(args...); 
    }

    std::pair<iterator, bool> insert(const value_type& value)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert(value);
    }

    template< class P >
    std::pair<iterator, bool> insert(P&& value)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert(value);
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert(hint, value);
    }

    template< class P >
    iterator insert(const_iterator hint, P&& value)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert(hint, value);
    }

    template< class InputIt >
    void insert(InputIt first, InputIt last)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      super::insert(first, last);
    }

    void insert(std::initializer_list<value_type> ilist)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      super::insert(ilist);
    }

    /*insert_return_type insert(node_type&& nh)
    {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert(nh);
    }

    iterator insert(const_iterator hint, node_type&& nh)
    {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert(hint, nh);
    }*/
    
    template <class M>
    std::pair<iterator, bool> insert_or_assign(const Key& k, M&& obj)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert_or_assign(k, obj);
    }

    template <class M>
    std::pair<iterator, bool> insert_or_assign(Key&& k, M&& obj)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert_or_assign(k, obj);
    }

    template <class M>
    iterator insert_or_assign(const_iterator hint, const Key& k, M&& obj)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert_or_assign(hint, k, obj);
    }

    template <class M>
    iterator insert_or_assign(const_iterator hint, Key&& k, M&& obj)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::insert_or_assign(hint, k, obj);
    }

    template< class... Args >
    std::pair<iterator, bool> emplace(Args&&... args)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::emplace(args...);
    }

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::emplace(hint, args...);
    }

    template< class... Args >
    std::pair<iterator, bool> try_emplace(const Key& k, Args&&... args)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::try_emplace(k, args...);
    }

    template< class... Args >
    std::pair<iterator, bool> try_emplace(Key&& k, Args&&... args)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::try_emplace(k, args...);
    }

    template< class... Args >
    iterator try_emplace(const_iterator hint, const Key& k, Args&&... args)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::try_emplace(hint, k, args...);
    }

    template< class... Args >
    iterator try_emplace(const_iterator hint, Key&& k, Args&&... args)
    {
      rehash_if_needed();
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::try_emplace(hint, k, args...);
    }

    iterator erase(iterator pos)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::erase(pos);
    }

    iterator erase(const_iterator pos)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::erase(first, last);
    }

    size_type erase(const Key& key)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::erase(key);
    }

    void swap(unordered_map& other)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      super::swap(other);
    }

    /*node_type extract(const_iterator position)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return extract(position);
    }

    node_type extract(const Key& k)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return extract(k);
    }*/

    /*
    template<class H2, class P2>
    void merge(std::unordered_map<Key, T, H2, P2, allocator_type>& source)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return merge(source);
    }

    template<class H2, class P2>
    void merge(std::unordered_map<Key, T, H2, P2, allocator_type>&& source)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return merge(source);
    }

    template<class H2, class P2>
    void merge(std::unordered_multimap<Key, T, H2, P2, allocator_type>& source)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return merge(source);
    }

    template<class H2, class P2>
    void merge(std::unordered_multimap<Key, T, H2, P2, allocator_type>&& source)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return merge(source);
    }
    */

    std::size_t size() const override
    {
      return super::size();
    }

  protected:

    /// Function below is implemented in a .cpp file. 
    void rehash(std::size_t /* new_size */);
  
    void rehash_if_needed()
    {
      if (super::load_factor() >= super::max_load_factor())
      {
        rehash(super::bucket_count() * 2);
      }
    }

};
}

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_MAP_H


