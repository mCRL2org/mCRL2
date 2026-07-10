/*
 * MSVC workaround header for mCRL2.
 *
 * Windows/MSVC does not provide a POSIX <sys/time.h> and mCRL2 does not
 * implement a shim for it. This file intentionally provides no declarations: it
 * exists only so that including <sys/time.h> resolves to an actionable
 * diagnostic rather than silently succeeding with an empty header (which would
 * surface later as confusing errors on struct timeval / gettimeofday).
 *
 * Any use of these POSIX time facilities must be guarded with
 * #if !defined(WIN32) (as in 3rd-party/dparser/d.h) or replaced with <chrono>.
 */
#error "sys/time.h is not available on MSVC. Guard POSIX time usage with #if !defined(WIN32) or use <chrono> instead."
