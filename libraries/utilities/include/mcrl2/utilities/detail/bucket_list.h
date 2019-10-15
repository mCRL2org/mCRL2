// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_DETAIL_BUCKETLIST_H_
#define MCRL2_UTILITIES_DETAIL_BUCKETLIST_H_

#include <assert.h>
#include <atomic>
#include <cstddef>
#include <iterator>
#include <memory>

namespace mcrl2
{
namespace utilities
{
namespace detail
{

struct Sentinel{};

/// \brief A end of the iterator sentinel.
static constexpr Sentinel EndIterator = {};

/// \brief This essentially implements the std::forward_list, with the difference that
///        it does not own the nodes in the list. It just keeps track of the list
///        next pointers.
template<typename Key, typename Allocator>
class bucket_list
{
public:

  /// \brief The nodes of the bucket list without carrying any additional informations.
  ///        Used to make no different between the head and the tail of the list.
  class node_base
  {
  public:
    /// \returns The next bucket in the linked list.
    node_base* next() const noexcept { return m_next; }

    /// \returns True if and only there is a next bucket in the linked list.
    bool has_next() const noexcept { return m_next != nullptr; }

    /// \brief Set the next pointer to the given next pointer.
    void next(node_base* next) noexcept { m_next = next; }
  protected:

    /// \brief Pointer to the next node.
    node_base* m_next = nullptr;
  };

  /// \brief The nodes of the bucket list.
  class node : public node_base
  {
  public:

    /// \brief Constructs a key by using the given arguments.
    template<typename ...Args>
    explicit node(Args&&... args)
      : m_key(std::forward<Args>(args)...)
    {}

    /// \returns A reference to the key stored in this node.
    Key& key() noexcept { return m_key; }
    const Key& key() const noexcept { return m_key; }

    /// \returns An implicit conversion to the underlying key.
    operator Key&() { return m_key; }
  private:
    /// \brief Store the actual key.
    Key m_key;
  };

  /// \brief Iterator over all keys in a bucket list.
  template<bool Constant = true>
  class key_iterator
  {
    friend class bucket_list<Key, Allocator>;
    using bucket_type = typename std::conditional<Constant, const bucket_list<Key, Allocator>, bucket_list<Key, Allocator>>::type;

    using node_pointer = typename std::conditional<Constant, const node*, node*>::type;
    using node_base_pointer = typename std::conditional<Constant, const node_base*, node_base*>::type;

  public:
    using value_type = typename std::conditional<Constant, const Key, Key>::type;
    using reference = typename std::conditional<Constant, const Key&, Key&>::type;
    using pointer = typename std::conditional<Constant, const Key*, Key*>::type;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    explicit key_iterator(node_base_pointer node)
      : m_bucket_node(reinterpret_cast<node_pointer>(node))
    {}

    key_iterator()
      : m_bucket_node(nullptr)
    {}

    /// \brief Implicit conversion to the const_iterator variant.
    operator key_iterator<true>() const
    {
      return key_iterator<true>(m_bucket_node);
    }

    key_iterator& operator++()
    {
      m_bucket_node = reinterpret_cast<class node*>(m_bucket_node->next());
      return *this;
    }

    template<bool Constant_ = Constant>
    typename std::enable_if<!Constant_, reference>::type operator*()
    {
      return m_bucket_node->key();
    }

    template<bool Constant_ = Constant>
    typename std::enable_if<Constant_, reference>::type operator*() const
    {
      return m_bucket_node->key();
    }

    bool operator == (const key_iterator& it) const noexcept
    {
      !(*this != it);
    }

    bool operator != (const key_iterator& it) const noexcept
    {
      return m_bucket_node != it.m_bucket_node;
    }

    bool operator == (Sentinel) const noexcept
    {
      !(*this != Sentinel());
    }

    bool operator != (Sentinel) const noexcept
    {
      return m_bucket_node != nullptr;
    }

    node* get_node() const noexcept
    {
      return m_bucket_node;
    }

  private:
    node_pointer m_bucket_node;
  };

public:
  using Bucket = bucket_list<Key, Allocator>;
  using iterator = key_iterator<false>;
  using const_iterator = key_iterator<true>;

  /// Rebind the passed to allocator to a bucket list node allocator.
  using NodeAllocator = typename Allocator::template rebind<typename Bucket::node>::other;

  /// \returns An iterator over the keys of this bucket and successor buckets.
  iterator begin() { return iterator(m_head.next()); }
  iterator end() { return iterator(); }

  /// \return An iterator pointing to the before the head of the list.
  iterator before_begin() { return iterator(&m_head); }

  /// \returns A const iterator over the keys of this bucket and successor buckets.
  const_iterator begin() const { return const_iterator(m_head.next()); }
  const_iterator end() const { return const_iterator(); }

  const_iterator cbegin() const { return const_iterator(m_head.next()); }
  const_iterator cend() const { return const_iterator(); }

  /// \return A const iterator pointing to the before the head of the list.
  const_iterator before_begin() const { return const_iterator(&m_head); }

  /// \brief Puts the given node before the head of the list.
  void push_front(node* node)
  {
    assert(!contains(node));
    node->next(m_head.next());
    m_head.next(node);
  }

  /// \brief Removes the element after the given iterator from the list. The returned iterator
  iterator erase_after(iterator it, NodeAllocator& allocator)
  {
    node* current_node = it.get_node();
    assert(current_node->next() != nullptr); // Cannot erase after the last node.

    // Keep track of the node that we should remove.
    node* erased_node = reinterpret_cast<node*>(current_node->next());
    node* next_node = reinterpret_cast<node*>(erased_node->next());

    // Update the next pointer of the current node.
    current_node->next(next_node);

    // Clean up the old node.
    std::allocator_traits<NodeAllocator>::destroy(allocator, erased_node);
    std::allocator_traits<NodeAllocator>::deallocate(allocator, erased_node, 1);
    return iterator(next_node);
  }

  const node_base* front() const noexcept { return &m_head; }

  /// \brief Empties the bucket list.
  void clear(NodeAllocator& allocator)
  {
    while(!empty())
    {
      erase_after(before_begin(), allocator);
    }
  }

  /// \returns True iff this bucket has no elements.
  bool empty() const { return !m_head.has_next(); }

private:

  /// \returns True iff this bucket already contains the given node.
  bool contains(node* node)
  {
    node_base* element = &m_head;
    while(element->has_next())
    {
      if (node == element->next())
      {
        return true;
      }

      element = element->next();
    }

    return false;
  }

  /// \brief The first node in the bucket list.
  node_base m_head;
};

} // namespace detail
} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_BUCKETLIST_H_
