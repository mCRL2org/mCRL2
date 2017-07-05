// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABORTABLE_H_INCLUDED
#define ABORTABLE_H_INCLUDED

/*! Mix-in class for classes whose operations can be aborted asynchronously.

    Classes inheriting Abortable should periodically check whether they are
    aborted by calling aborted() in time-consuming procedures.
*/
class Abortable
{
public:
    //! Abort all abortable processes.
    static void abort_all() { global_abort_ = true; }

    //! Returns whether this instance has been aborted.
    bool aborted() { return global_abort_; }

private:
    //! Global variable indicating whether the process has been aborted.
    static volatile bool global_abort_;
};

#endif /* ndef ABORTABLE_H_INCLUDED */
