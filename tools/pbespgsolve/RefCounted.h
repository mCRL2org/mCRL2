// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef REFCOUNTED
#define REFCOUNTED

#include <assert.h>
#include <stdio.h>

/*! A simple reference counting base class. */
class RefCounted
{
public:
    /*! Construct and initialize the reference count to `init_refcount'. */
    RefCounted(size_t init_refcount = 1)
        : refs_(init_refcount) { }

    /*! Increment reference count. */
    void ref() const { ++refs_; }

    /*! Decrement reference count and delete the object if it becomes zero. */
    void deref() const
    {
        assert(refs_ > 0);
        if (--refs_ == 0) delete this;
    }

    /*! Return the current reference count. Mostly useful for debugging. */
    size_t refcount() { return refs_; }

protected:
    virtual ~RefCounted() { assert(refs_ <= 1); }

protected:
    mutable size_t refs_;  //!< Number of references to this object
};

#endif /* ndef REFCOUNTED */
