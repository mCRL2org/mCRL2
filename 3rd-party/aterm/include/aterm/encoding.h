/**
  * encoding.h: Low level encoding of ATerm datatype.
  */

#ifndef ENCODING_H
#define ENCODING_H
#include "atypes.h"

#ifndef AT_64BIT
/* Covers gcc, icc, msvc and Solaris cc */
# if defined(__LP64__) || defined(_LP64) || defined(__lp64) || \
     defined(_ADDR64) || defined(__arch64__) || defined(_M_X64)
#  define AT_64BIT
# endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#ifndef SIZEOF_LONG
#define SIZEOF_LONG (sizeof(long))
#endif

/*
 32-bit:
  
 |---------------------------------|
 | info|arity| type|quoted|mark|age|
 |---------------------------------|
  31 10 9 8 7 6 5 4   3     2   1 0
  
 64-bit:

 |------------------------------------|
 | info|  |arity| type|quoted|mark|age|
 |------------------------------------|
  63 34 15 14  7 6 5 4   3     2   1 0
  
*/

typedef unsigned long header_type;

#define HEADER_BITS      (SIZEOF_LONG*8)

#ifdef AT_64BIT
#define SHIFT_LENGTH     34
#define ARITY_BITS       8
#else
#define SHIFT_LENGTH     10
#define ARITY_BITS       3
#endif /* AT_64BIT */

#define TYPE_BITS        3
#define LENGTH_BITS      (HEADER_BITS - SHIFT_LENGTH)

#define SHIFT_TYPE       4
#define SHIFT_ARITY      7

#define SHIFT_AGE        0
#define SHIFT_REMOVE_MARK_AGE 3

#define MASK_AGE         ((1<<0) | (1<<1))
#define MASK_MARK        (1<<2)
#define	MASK_QUOTED      (1<<3)
#define	MASK_ANNO        MASK_QUOTED
#define MASK_TYPE        (((1 << TYPE_BITS)-1) << SHIFT_TYPE)
#define MASK_ARITY       (((1 << ARITY_BITS)-1) << SHIFT_ARITY)

#define MASK_AGE_MARK    (MASK_AGE|MASK_MARK)

#define MAX_LENGTH       (1 << LENGTH_BITS)

#define GET_AGE(h)       ((unsigned int)(((h) & MASK_AGE) >> SHIFT_AGE))
#define SET_AGE(h, a)    do { h = ((h) & ~MASK_AGE) | (((a) << SHIFT_AGE) & MASK_AGE); } while (0)

#define YOUNG_AGE        0
#define OLD_AGE          3

#define IS_OLD(h)        (GET_AGE(h) == OLD_AGE)
#define IS_YOUNG(h)      (!(IS_OLD(h)))

/* TODO: Optimize */
#define INCREMENT_AGE(h) do { int age = GET_AGE(h); \
	                      if (age<OLD_AGE) { \
	                        SET_AGE((h), age+1); \
                              } \
                         } while (0)

#define HIDE_AGE_MARK(h)    ((h) & ~MASK_AGE_MARK)
/* #define EQUAL_HEADER(h1,h2) (HIDE_AGE_MARK(h1)==HIDE_AGE_MARK(h2)) */
#define EQUAL_HEADER(h1,h2) (HIDE_AGE_MARK(h1^h2) == 0)

#define SHIFT_SYMBOL  SHIFT_LENGTH
#define SHIFT_SYM_ARITY SHIFT_LENGTH

#define TERM_SIZE_APPL(arity) ((sizeof(struct __ATerm)/SIZEOF_LONG)+arity)
#define TERM_SIZE_INT         (sizeof(struct __ATermInt)/SIZEOF_LONG)
#define TERM_SIZE_REAL        (sizeof(struct __ATermReal)/SIZEOF_LONG)
#define TERM_SIZE_BLOB        (sizeof(struct __ATermBlob)/SIZEOF_LONG)
#define TERM_SIZE_LIST        (sizeof(struct __ATermList)/SIZEOF_LONG)
#define TERM_SIZE_PLACEHOLDER (sizeof(struct __ATermPlaceholder)/SIZEOF_LONG)
#define TERM_SIZE_SYMBOL      (sizeof(struct _SymEntry)/SIZEOF_LONG)

#define IS_MARKED(h)          ((h) & MASK_MARK)
#define GET_TYPE(h)           ((unsigned int)(((h) & MASK_TYPE) >> SHIFT_TYPE))
#define HAS_ANNO(h)           (((h) & MASK_ANNO) ? ATtrue : ATfalse)
#define GET_ARITY(h)	      ((unsigned int)(((h) & MASK_ARITY) >> SHIFT_ARITY))
#define GET_SYMBOL(h)	      ((Symbol)((h) >> SHIFT_SYMBOL))
#define GET_LENGTH(h)         ((unsigned long)((h) >> SHIFT_LENGTH))
#define IS_QUOTED(h)          (((h) & MASK_QUOTED) ? ATtrue : ATfalse)

#define SET_MARK(h)           do { (h) |= MASK_MARK; } while (0)
#define SET_ANNO(h)           do { (h) |= MASK_ANNO; } while (0)
/* #define SET_ARITY(h, ar) ((h) = (((h) & ~MASK_ARITY) | \
									((ar) << SHIFT_ARITY)))
*/
#define SET_SYMBOL(h, sym)    do { (h) = ((h) & ~MASK_SYMBOL) \
                                     | (((header_type)(sym)) << SHIFT_SYMBOL); \
                              } while (0)
#define SET_LENGTH(h, len)    do { (h) = ((h) & ~MASK_LENGTH) | \
                                     | (((header_type)(len)) << SHIFT_LENGTH); \
                              } while (0) 
#define SET_QUOTED(h)         do { (h) |= MASK_QUOTED; } while (0)

#define CLR_MARK(h)           do { (h) &= ~MASK_MARK; } while (0)
#define CLR_ANNO(h)           do { (h) &= ~MASK_ANNO; } while (0)
#define CLR_QUOTED(h)         do { (h) &= ~MASK_QUOTED; } while (0)

#define APPL_HEADER(anno,ari,sym) ((anno) | ((ari) << SHIFT_ARITY) | \
				   (AT_APPL << SHIFT_TYPE) | \
				   ((header_type)(sym) << SHIFT_SYMBOL))
#define INT_HEADER(anno)          ((anno) | AT_INT << SHIFT_TYPE)
#define REAL_HEADER(anno)         ((anno) | AT_REAL << SHIFT_TYPE)
#define EMPTY_HEADER(anno)        ((anno) | AT_LIST << SHIFT_TYPE)

#define LIST_HEADER(anno,len)     ((anno) | (AT_LIST << SHIFT_TYPE) | \
				   ((MachineWord)(len) << SHIFT_LENGTH) | (2 << SHIFT_ARITY))

#define PLACEHOLDER_HEADER(anno)  ((anno) | (AT_PLACEHOLDER << SHIFT_TYPE) | \
           1 << SHIFT_ARITY)

/*
#define BLOB_HEADER(anno,len)     ((anno) | (AT_BLOB << SHIFT_TYPE) | \
				   ((MachineWord)(len) << SHIFT_LENGTH))
				   */
#define BLOB_HEADER(anno)	  ((anno) | (AT_BLOB << SHIFT_TYPE) | \
				   (2 << SHIFT_ARITY))

#define SYMBOL_HEADER(arity,quoted) \
	(((header_type)(arity) << SHIFT_SYM_ARITY) | \
	 ((quoted) ? MASK_QUOTED : 0) | \
	 (AT_SYMBOL << SHIFT_TYPE))

#define FREE_HEADER               (AT_FREE << SHIFT_TYPE)

#define ARG_OFFSET                TERM_SIZE_APPL(0)



#define MAX_HEADER_BITS 64

#define AT_TABLE_SIZE(table_class)  (1<<(table_class))
#define AT_TABLE_MASK(table_class)  (AT_TABLE_SIZE(table_class)-1)

/* Integers in BAF are always exactly 32 bits.  The size must be fixed so that
 * BAF terms can be exchanged between platforms. */
#define INT_SIZE_IN_BAF 32

#define MAX_ARITY             MAX_LENGTH

#define MIN_TERM_SIZE         TERM_SIZE_APPL(0)
#define INITIAL_MAX_TERM_SIZE 256


struct __ATerm
{
  header_type   header;
  union _ATerm *next;
};

typedef union _ATerm
{
  header_type     header;
  struct __ATerm  aterm;
  union _ATerm*   subaterm[1];
  MachineWord     word[1];
} *ATerm;

typedef void (*ATermProtFunc)();

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
