// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef DENSE_MAP_H_INCLUDED
#define DENSE_MAP_H_INCLUDED

#include <memory>
#include <utility>

/*! \ingroup Containers

    A map-like data structure with keys from a dense integer range.

    Each map instance has a fixed range of possible keys.  Memory used and time
    required to iterate over the set is proportional to the size of the range
    (*not* the  size of the set, i.e. the number of elements).

    Internally, the DenseMap uses an array of key/value-pairs to store its
    contents.  A special `Unused` value marks the absence of stored elements.
    Note that this is quite different from DenseSet!

    Note that the `Unused` template parameter must never be used as a value
    stored in the map.  `Used` on the other hand, may be any valid value.  It
    will be used internally to mark the end of the stored data.

    \see DenseSet
*/

// N.B. this class is far from finished!

template<class Key, class Val, Val Used = Val(0), Val Unused = Val(-1),
    class Alloc = std::allocator<std::pair<Key, Val> > >
class DenseMap
{
public:
    typedef std::pair<Key, Val> value_type;

    class Iterator
    {
        value_type *pos_;

    public:
        Iterator(value_type *pos) : pos_(pos) { };
        Iterator(const Iterator &it) : pos_(it.pos_) { };
        Iterator& operator=(const Iterator &it) { pos_ = it.pos_; }

        bool operator== (const Iterator &other) { return pos_ == other.pos_; }
        bool operator!= (const Iterator &other) { return pos_ != other.pos_; }
        bool operator<  (const Iterator &other) { return pos_ <  other.pos_; }
        bool operator>  (const Iterator &other) { return pos_ >  other.pos_; }
        bool operator<= (const Iterator &other) { return pos_ <= other.pos_; }
        bool operator>= (const Iterator &other) { return pos_ >= other.pos_; }

        value_type operator*() { return *pos_; };
        value_type *operator->() { return pos_; };
    };

    typedef Iterator iterator;
    typedef Iterator const_iterator;

    DenseMap(Key begin, Key end, const Alloc &alloc = Alloc())
        : range_begin(begin), range_end(end < begin ? begin : end),
          range_size_(range_end - range_begin),
          values_(alloc), used_(0)
    {
        values_.reserve(range_size_ + 1);
        for (Key k = range_begin; k != range_end; ++k)
        {
            values_.push_back(value_type(k, Unused));
        }
        values_.push_back(value_type(range_end, Used));  // end of data marker
    }

    size_t size() const { return used_; }
    bool empty() const { return used_ == 0; }

    void clear()
    {
        if (used_ > 0)
        {
            for ( typename std::vector<value_type>::iterator
                    it = values_.begin(); it != values_.end(); ++it )
            {
                *it->second = Unused;
            }
            used_ = 0;
        }
    }

    iterator begin()
    {
        Val *v = &values_[0];
        while (v->second == Unused) ++v;
        return iterator(v);
    }

    iterator end()
    {
        return iterator(&values_[range_size_]);
    }

    const_iterator begin() const
    {
        return iterator(const_cast<DenseMap*>(this)->begin());  // HACK
    }

    const_iterator end() const
    {
        return iterator(const_cast<DenseMap*>(this)->end());  // HACK
    }

    iterator find(Key k)
    {
        if ( /* k >= range_begin && k < range_end && */
            values_[k - range_begin].second != Unused)
        {
            return iterator(&values_[k - range_begin]);
        }
        else
        {
            return end();
        }
    }

    const_iterator find(Key k) const
    {
        return const_iterator(const_cast<DenseMap*>(this)->find(k));  // HACK
    }

    Val &operator[](Key k)
    {
        value_type &kv = values_[k - range_begin];
        if (kv.second == Unused)
        {
            kv.second = Val();
            ++used_;
        }
        return kv.second;
    }

    std::pair<iterator, bool> insert(const value_type &new_kv)
    {
        value_type &kv = values_[new_kv.first - range_begin];
        bool inserted = (kv.second == Unused);
        kv.second = new_kv.second;
        return std::pair<iterator, bool>(iterator(&kv), inserted);
    }

public:
    const Key range_begin, range_end;

private:
    const size_t range_size_;
    std::vector<value_type> values_;
    size_t used_;

private:
    DenseMap(const DenseMap &);
    DenseMap &operator=(const DenseMap &);
};

#endif /* ndef DENSE_MAP_H_INCLUDED */
