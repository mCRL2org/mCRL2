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
 *    - ATwarning() to print terms to stderr (for debugging)
 *    - ATprotect() to protect global variable from the garbage collector
 *    - ATreadFromNamedFile to read terms from disk
 *    
 */

#ifndef ATERM1_H
#define ATERM1_H

#include <stdio.h>
#include <stdarg.h>
#include "encoding.h"
#include "abool.h"
#include "atypes.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

/** Returns the type of an ATerm, \see AT_APPL */
#define ATgetType(t) GET_TYPE((t)->header)

/**
 * These are the types of ATerms there are. \see ATgetType().
 */
#define	AT_FREE         0L /**< internally used type */
#define AT_APPL         1L /**< type of applications of function names to terms */
#define AT_INT          2L /**< type of integer numbers */
/* #define AT_REAL         3L / **< type of real numbers */
#define AT_LIST         4L /**< type of lists of terms */
/* #define AT_PLACEHOLDER  5L / **< type of a placeholder for a term */
/* #define AT_BLOB         6L / **< type of a binary large object */
#define AT_SYMBOL       7L /**< internally used type*/


#define ATisEqual(t1,t2) ((ATbool)((ATerm)(t1) == (ATerm)(t2)))
#define ATisEqualAFun(f1, f2) ((f1) == (f2))

/**
 * Serialize a term to file, in readable ATerm format. No sharing is applied
 * in the serialized format.
 */
ATbool ATwriteToTextFile(ATerm t, FILE *file);

ATbool ATwriteToBinaryFile(ATerm t, FILE *file);

/**
 * Call ATwriteToTextFile() after opening a file.
 * \arg t term to write
 * \arg name name of the file. If the name equals "-", stdout is used.
 */
ATbool ATwriteToNamedTextFile(ATerm t, const char *name);

/**
 * Call ATwriteToBinaryFile() after opening a file.
 * \arg t term to write
 * \arg name name of the file. If the name equals "-", stdout is used.
 */
ATbool ATwriteToNamedBinaryFile(ATerm t, const char *name);

/**
 * Serialize an ATerm to a static buffer. Note that the buffer is
 * shared between calls to ATwriteToString, and should not be freed.
 * \arg t term to write
 */
char  *ATwriteToString(ATerm t);

/**
 * Serialize an ATerm to a static buffer in binary format. Note that 
 * the buffer is
 * shared between calls to ATwriteToString, and should not be freed.
 * \arg t term to write
 * \arg len result variable that will hold the length of the string
 */
unsigned char *ATwriteToBinaryString(ATerm t, int *len);

/**
 * Read a textual ATerm from a file.
 * \arg file file to read from
 */
ATerm ATreadFromTextFile(FILE *file);

/**
 * Read a binary ATerm (BAF) from a file.
 * \arg file file to read from
 */
ATerm ATreadFromBinaryFile(FILE *file);

/**
 * Read an ATerm in any format from a file.
 * \arg file file to read from.
 */
ATerm ATreadFromFile(FILE *file);

/** 
 * Read an ATerm in any format from a file, using a file name.
 * \arg name name of the file, if the name equals "-" than the ATerm is read
 *           from stdin
 */
ATerm ATreadFromNamedFile(const char *name);

/**
 * Parse an ATerm from a string. Similar to ATmake() with only a pattern,
 * but slightly faster.
 * \arg string string containing a readable ATerm
 */
ATerm ATreadFromString(const char *string);

/**
 * Parse a binary ATerm from an array of bytes.
 * \arg s array containing a serialized ATerm in binary format
 * \arg size length of the array
 */
ATerm ATreadFromBinaryString(const unsigned char *s, int size);


/* SAF I/O stuff */
ATbool ATwriteToSAFFile(ATerm aTerm, FILE *file);

ATerm ATreadFromSAFFile(FILE *file);

ATbool ATwriteToNamedSAFFile(ATerm aTerm, const char *filename);

ATerm ATreadFromNamedSAFFile(const char *filename);

char* ATwriteToSAFString(ATerm aTerm, int *length);

ATerm ATreadFromSAFString(char *data, int length);


/** A short hand for ATreadFromString() */
#define ATparse(s)	ATreadFromString((s))

/**
 * Protect an ATerm from being garbage collected in the future.
 * \arg atp memory address of an ATerm
 */
void ATprotect(ATerm *atp);

/**
 * Remove the protection from the garbage collector for a certain term.
 * \arg atp the address of a previously protected ATerm, see ATprotect()
 */
void ATunprotect(ATerm *atp);

/**
 * Efficiently ATprotect() an array of terms from the garbage collector.
 * \arg start array of terms
 * \arg size  length of |start|
 */
void ATprotectArray(ATerm *start, size_t size);

/**
 * Efficiently ATunprotect() an array of terms.
 * \arg start previously protected array of terms
 */
void ATunprotectArray(ATerm *start);

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
void ATaddProtectFunction(ATermProtFunc f);

/**
 * unregister a previously registered marking function. See ATaddProtectFunction().
 */ 
void ATremoveProtectFunction(ATermProtFunc f);

/**
 * Mark a term to prevent it from being garbage collected. CAVEAT EMPTOR. 
 * This function should only be called from a function previously registered
 * using ATaddProtectFunction(). Otherwise, you WILL get bus errors and
 * core dumps.
 */
void ATmarkTerm(ATerm t);


/**
 * Mark an array to prevent it from being garbage collected. CAVEAT EMPTOR. 
 * This function should only be called from a function previously registered
 * using ATaddProtectFunction(). Otherwise, you WILL get bus errors and
 * core dumps.
 */
void ATmarkArray(ATerm *start, int size);

/* Convenience macro's to circumvent gcc's (correct) warning:
 *   "dereferencing type-punned pointer will break strict-aliasing rules"
 * example usage: ATprotectList(&ATempty);
 */
#define ATprotectTerm(p) ATprotect((ATerm *)(void *)(p))
#define ATprotectList(p) ATprotect((ATerm *)(void *)(p))
#define ATprotectAppl(p) ATprotect((ATerm *)(void *)(p))
#define ATprotectInt(p) ATprotect((ATerm *)(void *)(p))

#define ATunprotectTerm(p) ATunprotect((ATerm *)(void *)(p))
#define ATunprotectList(p) ATunprotect((ATerm *)(void *)(p))
#define ATunprotectAppl(p) ATunprotect((ATerm *)(void *)(p))
#define ATunprotectInt(p) ATunprotect((ATerm *)(void *)(p))

/** 
 * Initialize the ATerm library. It is essential to call this function in the 
 * main frame of your application. It is needed to boot the administration
 * for maximal sharing and garbage collecting terms.
 *
 * \arg argc argc argument of your main function
 * \arg argv argv argument of your main function
 * \arg bottomOfStack a reference to a variable of type ATerm that is in the 
 *                    main stack frame. This should be the FIRST ATerm variable
 *                    declared.
 */
void ATinit(int argc, char *argv[], ATerm *bottomOfStack);

/**
 * \todo I don't know this function.
 */
/* void ATinitialize(int argc, char *argv[]);
*/

/**
 * Check whether the ATerm library has been initialized.
 * \returns ATtrue if the library was initialized, and ATfalse otherwise
 */
ATbool ATisInitialized();

/**
 * Set another handler for warning messages. Basically, this replaces
 * the default implementation of the ATwarning() function.
 */
void ATsetWarningHandler(void (*handler)(const char *format, va_list args));

/**
 * Set another handler for error messages. Basically, this replaces
 * the default implementation of the ATerror() function.
 */
void ATsetErrorHandler(void (*handler)(const char *format, va_list args));

/**
 * Set another handler for fatal error messages. Basically, this replaces
 * the default implementation of the ATabort() function.
 */
void ATsetAbortHandler(void (*handler)(const char *format, va_list args));

/** 
 * Prints a message to standard error, using ATfprintf(). If another
 * handler was set using ATsetWarningHandler(), that function is called 
 * instead.
 * 
 */
void ATwarning(const char *format, ...);

/** 
 * Prints a message to standard error, using ATfprintf(), and then exits
 * the program with return code 1. 
 * If another handler was set using ATsetErrorHandler(), 
 * that function is called instead.
 * 
 */
void ATerror(const char *format, ...);

/** 
 * Prints a message to standard error, using ATfprintf(), and then exits
 * the program using the abort() system call which will dump a core file.
 * If another handler was set using ATsetAbortHandler(), 
 * that function is called instead.
 * 
 */
void ATabort(const char *format, ...);

/**
 * Prints a formatted message to stdout using ATvprintf
 * \returns always 0
 */
int  ATprintf(const char *format, ...);

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
int  ATfprintf(FILE *stream, const char *format, ...);

/**
 * \see fprintf(), but with a va_list instead of a variable argument list.
 */
int  ATvfprintf(FILE *stream, const char *format, va_list args);

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
