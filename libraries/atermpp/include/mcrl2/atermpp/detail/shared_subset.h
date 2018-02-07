// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/shared_subset.h
/// \brief Shared subset of a give set.

#ifndef MCRL2_ATERMPP_SHARED_SUBSET_H
#define MCRL2_ATERMPP_SHARED_SUBSET_H

#include <algorithm>
#include <iterator>
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_string.h"

static inline int highest_bit(std::size_t x)
{
  int i = -1;
  while (x)
  {
    x = x >> 1;
    i++;
  }
  return i;
}

namespace atermpp
{
namespace detail
{

/// \brief Stores a subset of a given base set using maximum sharing.
///
/// Subsets are stored as a relation on the element index, in a BDD whose variables each represent one bit of the index.
template <typename T>
class shared_subset
{
  protected:
    static atermpp::aterm_string& get_true()
    {
      static atermpp::aterm_string true_ = atermpp::aterm_string("true");
      return true_;
    }

    static atermpp::aterm_string& get_false()
    {
      static atermpp::aterm_string false_ = atermpp::aterm_string("false");
      return false_;
    }

    static atermpp::function_symbol& get_node()
    {
      static atermpp::function_symbol node_ = atermpp::function_symbol("node", 3);
      return node_;
    }

    class bdd_node : public atermpp::aterm_appl
    {
      public:
        bdd_node()
        {}

        bdd_node(const atermpp::aterm_appl& t)
          : atermpp::aterm_appl(t)
        {}

        bdd_node(bool value)
          : atermpp::aterm_appl(value ? get_true() : get_false())
        {}

        bdd_node(std::size_t bit, const bdd_node& true_node, const bdd_node& false_node)
          : atermpp::aterm_appl(get_node(), atermpp::aterm_int(bit), true_node, false_node)
        {}

        bool is_true()
        {
          return *this == get_true();
        }

        bool is_false()
        {
          return *this == get_false();
        }

        bool is_node()
        {
          return function() == get_node();
        }

        std::size_t bit()
        {
          return atermpp::aterm_int((*this)[0]).value();
        }

        bdd_node true_node()
        {
          return bdd_node(atermpp::aterm_appl((*this)[1]));
        }

        bdd_node false_node()
        {
          return bdd_node(atermpp::aterm_appl((*this)[2]));
        }
    };

  protected:
    std::vector<T> *m_set;
    std::size_t m_bits;
    bdd_node m_bdd_root;

  public:
    // Iterates over elements in the subset *in order*. The predicate-constructor of shared_subset depends on this property.
    class iterator: public boost::iterator_facade<iterator, T, boost::forward_traversal_tag>
    {
      protected:
        const shared_subset<T> *m_subset;
        std::size_t m_index;

      public:

        iterator()
          : m_subset(nullptr),
            m_index(-1)
        {
        }

        iterator(const shared_subset& subset)
          : m_subset(&subset),
            m_index(0)
        {
          find_next_index();
        }

        std::size_t index() const
        {
          return m_index;
        }

        operator bool() const
        {
          return m_index != (std::size_t)(-1);
        }

      private:
        friend class boost::iterator_core_access;

        bool equal(iterator const& other) const
        {
          return m_index == other.m_index;
        }

        T& dereference() const
        {
          return (*m_subset->m_set)[m_index];
        }

        void increment()
        {
          m_index++;
          find_next_index();
        }

        void find_next_index()
        {
          std::vector<bdd_node> path_stack;
          bdd_node node = m_subset->m_bdd_root;

          while (true)
          {
            assert(m_subset->m_set != nullptr);
            if (m_index >= m_subset->m_set->size())
            {
              m_index = -1;
              return;
            }

            while (node.is_node())
            {
              path_stack.push_back(node);
              node = (m_index & ((std::size_t)1 << node.bit())) ? node.true_node() : node.false_node();
            }

            if (node.is_true())
            {
              return;
            }

            while (true)
            {
              bdd_node start;
              std::size_t bit;

              if (path_stack.empty())
              {
                start = m_subset->m_bdd_root;
                bit = m_subset->m_bits;
              }
              else
              {
                start = node;
                bit = path_stack.back().bit();
              }

              if (!start.is_false())
              {
                assert(start.is_node());
                bool found = false;
                for (std::size_t i = start.bit() + 1; i < bit; i++)
                {
                  if (!(m_index & ((std::size_t)1 << i)))
                  {
                    m_index |= ((std::size_t)1 << i);
                    m_index &= ~(((std::size_t)1 << i) - 1);
                    found = true;
                    break;
                  }
                }
                if (found)
                {
                  break;
                }
              }

              if (path_stack.empty())
              {
                m_index = -1;
                return;
              }
              else
              {
                node = path_stack.back();
                path_stack.pop_back();
                if (!(m_index & ((std::size_t)1 << bit)) && !node.true_node().is_false())
                {
                  m_index |= ((std::size_t)1 << bit);
                  m_index &= ~(((std::size_t)1 << bit) - 1);
                  break;
                }
              }
            }
          }
        }
    };

    shared_subset()
      : m_set(nullptr),
        m_bits(0),
        m_bdd_root(false)
    {}

    /// \brief Constructor.
    shared_subset(std::vector<T>& set)
      : m_set(&set),
        m_bdd_root(true)
    {
      m_bits = 0;
      while (m_set->size() > ((std::size_t)1 << m_bits))
      {
        m_bits++;
      }

#ifndef NDEBUG
      std::size_t index = 0;
      for (iterator i = begin(); i != end(); i++)
      {
        assert(i.index() == index++);
      }
      assert(index == m_set->size());
#endif
    }

    template <class Predicate>
    shared_subset(const shared_subset<T>& set, Predicate p)
      : m_set(set.m_set),
        m_bits(set.m_bits)
    {
      std::vector<bdd_node> trees;
      std::fill_n(std::back_inserter(trees), m_bits + 1, bdd_node());
      std::size_t completed = 0;
      for (iterator i = set.begin(); i != set.end(); i++)
      {
        if (p(*i))
        {
          std::size_t target = i.index();

          for (int bit = highest_bit(target); bit >= 0; bit--)
          {
            if ((target & ((std::size_t)1 << bit)) && !(completed & ((std::size_t)1 << bit)))
            {
              bdd_node tree(false);
              for (int j = 0; j < bit; j++)
              {
                bdd_node true_node;
                bdd_node false_node;
                if (completed & ((std::size_t)1 << j))
                {
                  true_node = tree;
                  false_node = trees[j];
                }
                else
                {
                  true_node = false;
                  false_node = tree;
                }
                if (true_node == false_node)
                {
                  tree = true_node;
                }
                else
                {
                  tree = bdd_node(j, true_node, false_node);
                }
              }
              trees[bit] = tree;
              completed |= ((std::size_t)1 << bit);
              completed &= ~(((std::size_t)1 << bit) - 1);
            }
          }

          bdd_node tree(true);
          std::size_t bit;
          for (bit = 0; target & ((std::size_t)1 << bit); bit++)
          {
            if (tree != trees[bit])
            {
              tree = bdd_node(bit, tree, trees[bit]);
            }
          }
          trees[bit] = tree;
          completed = target + 1;
        }
      }

      if (completed != ((std::size_t)1 << m_bits))
      {
        bdd_node tree(false);
        for (std::size_t j = 0; j < m_bits; j++)
        {
          bdd_node true_node;
          bdd_node false_node;
          if (completed & ((std::size_t)1 << j))
          {
            true_node = tree;
            false_node = trees[j];
          }
          else
          {
            true_node = false;
            false_node = tree;
          }
          if (true_node == false_node)
          {
            tree = true_node;
          }
          else
          {
            tree = bdd_node(j, true_node, false_node);
          }
        }
        trees[m_bits] = tree;
      }

      m_bdd_root = trees[m_bits];

#ifndef NDEBUG
      iterator i = set.begin();
      iterator j = begin();
      while (i != set.end() && !p(*i))
      {
        i++;
      }
      while (i != set.end() && j != end())
      {
        assert (&*i == &*j);
        i++;
        j++;
        while (i != set.end() && !p(*i))
        {
          i++;
        }
      }
      assert (i == set.end() && j == end());
#endif
    }

    iterator begin() const
    {
      return iterator(*this);
    }

    iterator end() const
    {
      return iterator();
    }
};

} // namespace detail
} // namespace atermpp

#endif
