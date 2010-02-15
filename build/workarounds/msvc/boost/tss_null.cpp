// (C) Copyright Michael Glassford 2004.
// (C) Copyright 2007 Anthony Williams
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    //Adapted implementation for MSVC build with mCRL2 tool-set
  
    /*
    This file is a "null" implementation of tss cleanup; it's
    purpose is to to eliminate link errors in cases
    where it is known that tss cleanup is not needed.
    */

    extern "C" void tss_cleanup_implemented(void)
    {
    }
