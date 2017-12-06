// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_PG_REFCOUNTED_H
#define MCRL2_PG_REFCOUNTED_H

#include <cassert>
#include <cstdio>

/*! A simple reference counting base class.

    Instances of this class start with an initial reference count of 1 (by
    default), which can be increased or decreased by calling the ref() and
    deref() methods.  When the reference count becomes zero, the object
    is deleted and should not be used anymore.

    It is allowed to delete an object directly (without calling deref())
    provided the caller has the only reference to the object.  In effect, this
    is the same as calling deref(), but supports use cases like putting
    instances into std::auto_ptr wrappers.
*/
class RefCounted
{
public:
    //! Construct and initialize the reference count to `init_refcount`.
    RefCounted(std::size_t init_refcount = 1)
        : refs_(init_refcount) { }

    //! Increment reference count.
    void ref() const { ++refs_; }

    //! Decrement reference count and delete the object if it becomes zero.
    void deref() const
    {
        assert(refs_ > 0);
        if (--refs_ == 0) delete this;
    }

    //! Return the current reference count. Mostly useful for debugging.
    std::size_t refcount() { return refs_; }

protected:
    virtual ~RefCounted() { assert(refs_ <= 1); }

protected:
    mutable std::size_t refs_;  //!< Number of references to this object
};

#endif /* ndef MCRL2_PG_REFCOUNTED_H */
