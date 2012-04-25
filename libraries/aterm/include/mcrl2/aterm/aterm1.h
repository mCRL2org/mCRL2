/**
 * \mainpage
 *
 * The ATerm library is an open-source library for term manipulation.
 * This C version of the library implements the four standard serialization
 * interfaces, features a generational garbage collector and maximal subterm
 * sharing.
 *
 * It is important to realize that the ATerm interface is fully 'functional',
 * which means that all variables of type ATerms should be regarded as
 * VALUES by the programmer, not pointers. ATerms are never updated
 * destructively. The only side-effects of the ATerm API are the
 * administration of maximal subterm sharing and garbage collection, both
 * are hidden from the programmer.
 *
 * The major blessing AND pitfall of ATerm programming is automatic garbage
 * collection. The garbage
 * collector automatically finds all living terms that are rooted somewhere
 * on the
 * C call stack (which means they are local variables in one of your
 * functions). Global variables, or ATerm members of your own structures
 * that are allocated on the heap need to be protected from garbage collection
 * by ATprotect(), and unprotected again when their context is freed by
 * ATunprotect().
 *
 * \section docs Other sources of documentation
 *
 * Please visit <a href="http://www.meta-environment.org">meta-environment.org</a>
 * for documentation and papers on the ATerm library.
 *
 * \section files Important files
 *    - \ref aterm1.h The simple make/match API for ATerms
 *    - \ref aterm2.h The efficient second level API for ATerms
 *
 * \section functions Important functions
 *    - ATinit() should be called always in the main of an application
 *    - ATmake() for term construction
 *    - ATmatch() for term deconstruction
 *    - ATprotect() for protecting terms that are not pointed to from
 *    a stack frame from the garbage collector.
 *
 */

/**
 * \file
 * Definition of the level 1 interface of the ATerm library. This interface
 * basically implements the make & match paradigm for constructing and
 * deconstructing ATerms. It is an easy interface but not optimized for
 * run-time efficiency.
 *
 * \section index Start with the following functions
 *    - ATinit() should be called from your main() function
 *    - ATmake() to construct terms.  Now deprecated and removed.
 *    - ATmatch() to deconstruct terms. Deprecated and removed.
  *    - ATprotect() to protect global variable from the garbage collector
 *    - ATreadFromNamedFile to read terms from disk
 *
 */

#ifndef ATERM1_H
#define ATERM1_H

#include <cstdio>
#include <cstdarg>
#include "mcrl2/aterm/encoding.h"
#include "mcrl2/aterm/atypes.h"
#include "mcrl2/aterm/afun.h"

namespace aterm
{

/** Returns the type of an ATerm, \see AT_APPL */
//#define ATgetType(t) GET_TYPE((t)->header)
template <typename TermType>
inline
size_t ATgetType(const TermType &t)
{
  return GET_TYPE(t->header);
}

inline
bool ATisEqual(const ATerm &t1, const ATerm &t2)
{
  return t1 == t2;
}

inline
bool ATisEqualAFun(const AFun &f1, const AFun &f2)
{
  return f1 == f2;
}

/**
 * Serialize a term to file, in readable ATerm format. No sharing is applied
 * in the serialized format.
 */
bool ATwriteToTextFile(const ATerm &t, FILE* file);

bool ATwriteToBinaryFile(const ATerm &t, FILE* file);

/**
 * Call ATwriteToTextFile() after opening a file.
 * \arg t term to write
 * \arg name name of the file. If the name equals "-", stdout is used.
 */
bool ATwriteToNamedTextFile(const ATerm &t, const char* name);

/**
 * Call ATwriteToBinaryFile() after opening a file.
 * \arg t term to write
 * \arg name name of the file. If the name equals "-", stdout is used.
 */
bool ATwriteToNamedBinaryFile(const ATerm &t, const char* name);

/**
 * Serialize an ATerm to a string.
 * \arg t term to write
 */
std::string ATwriteToString(const ATerm &t);

/**
 * Serialize an ATerm to a static buffer in binary format. Note that
 * the buffer is
 * shared between calls to ATwriteToString, and should not be freed.
 * \arg t term to write
 * \arg len result variable that will hold the length of the string
 */
unsigned char* ATwriteToBinaryString(const ATerm &t, size_t* len);

/**
 * Read a textual ATerm from a file.
 * \arg file file to read from
 */
ATerm ATreadFromTextFile(FILE* file);

/**
 * Read a binary ATerm (BAF) from a file.
 * \arg file file to read from
 */
ATerm ATreadFromBinaryFile(FILE* file);

/**
 * Read an ATerm in any format from a file.
 * \arg file file to read from.
 */
ATerm ATreadFromFile(FILE* file);

/**
 * Read an ATerm in any format from a file, using a file name.
 * \arg name name of the file, if the name equals "-" than the ATerm is read
 *           from stdin
 */
ATerm ATreadFromNamedFile(const char* name);

/**
 * Parse an ATerm from a string. Similar to ATmake() with only a pattern,
 * but slightly faster.
 * \arg string string containing a readable ATerm
 */
ATerm ATreadFromString(const char* string);

/**
 * Parse a binary ATerm from an array of bytes.
 * \arg s array containing a serialized ATerm in binary format
 * \arg size length of the array
 */
ATerm ATreadFromBinaryString(const unsigned char* s, size_t size);


/* SAF I/O stuff */
bool ATwriteToSAFFile(const ATerm &aTerm, FILE* file);

ATerm ATreadFromSAFFile(FILE* file);

bool ATwriteToNamedSAFFile(const ATerm &aTerm, const char* filename);

ATerm ATreadFromNamedSAFFile(const char* filename);

char* ATwriteToSAFString(const ATerm &aTerm, size_t* length);

ATerm ATreadFromSAFString(char* data, size_t length);


/** A short hand for ATreadFromString() */
inline
ATerm ATparse(const char* s)
{
  return ATreadFromString(s);
}

/**
 * Protect an ATerm from being garbage collected in the future.
 * \arg atp memory address of an ATerm
 */
// void ATprotect(const ATerm* atp);

/**
 * Remove the protection from the garbage collector for a certain term.
 * \arg atp the address of a previously protected ATerm, see ATprotect()
 */
// void ATunprotect(const ATerm* atp);

/**
 * Efficiently ATprotect() an array of terms from the garbage collector.
 * \arg start array of terms
 * \arg size  length of |start|
 */
void ATprotectArray(const ATerm* start, const size_t size);

/**
 * Efficiently ATunprotect() an array of terms.
 * \arg start previously protected array of terms
 */
void ATunprotectArray(const ATerm* start);

/**
 * Register a function to be called before every garbage collection
 * starts. This can be used to enhance the garbage collector with a
 * user defined scheme for protecting terms. The function registered
 * should use ATmakeTerm() or ATmarkArray() to protect terms from
 * garbage collection. This scheme is a more complex alternative to
 * using the ATprotect() and ATunprotect() functions.
 * \arg f function to register
 *
 * \todo Should this functionality be in the level one interface?
 */
void ATaddProtectFunction(const ATermProtFunc f);

/**
 * unregister a previously registered marking function. See ATaddProtectFunction().
 */
/* void ATremoveProtectFunction(ATermProtFunc f);
*/

/**
 * Initialize the ATerm library. It is essential to call this function in the
 * main frame of your application. It is needed to boot the administration
 * for maximal sharing and garbage collecting terms.
 *
 * \arg bottomOfStack a reference to a variable of type ATerm that is in the
 *                    main stack frame. This should be the FIRST ATerm variable
 *                    declared. Not needed anymore in reference_count garbage 
 *                    collection.
 */
void ATinit();

/**
 * Check whether the ATerm library has been initialized.
 * \returns ATtrue if the library was initialized, and ATfalse otherwise
 */
bool ATisInitialized();

/**
 * Prints a formatted message to stdout using ATvprintf
 * \returns always 0
 */
int  ATprintf(const char* format, ...);

/**
 * Prints a formatted message to a stream. The function behaves like
 * fprintf(), but there are some more substition codes:
 *    - \%t : serializes the corresponding argument to a readable unshared ATerm
 *    - \%l : serializes the elements of a list, seperated by comma's
 *    - \%y : prints a function name
 *    - \%a : prints the outermost function name of an ATerm
 *    - \%h : prints the MD5 checksum of an ATerm
 *    - \%n : prints debugging information on an ATerm
 * \returns always 0
 */
int  ATfprintf(FILE* stream, const char* format, ...);

/**
 * \see fprintf(), but with a va_list instead of a variable argument list.
 */
int  ATvfprintf(FILE* stream, const char* format, va_list args);

} // namespace aterm

#endif
