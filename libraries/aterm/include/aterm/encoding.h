/**
  * encoding.h: Low level encoding of ATerm datatype.
  */

#ifndef ENCODING_H
#define ENCODING_H
#include "atypes.h"

namespace aterm
{

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

  /* Although atypes.h defines MachineWord, it wasn't used here:
     typedef unsigned long header_type; */
  typedef MachineWord header_type;

  template <typename T>
  inline
  header_type _HT(const T t)
  {
    return static_cast<MachineWord>(t);
  }

  struct __ATerm
  {
    header_type   header;
    union _ATerm* next;
  };

  inline
  size_t TERM_SIZE_APPL(const size_t arity)
  {
    return (sizeof(struct __ATerm)/sizeof(size_t))+arity;
  }

  typedef union _ATerm
  {
    header_type     header;
    struct __ATerm  aterm;
    union _ATerm*   subaterm[1];
    MachineWord     word[1];
  }* ATerm;

  //_I(x) ((size_t)(x))

  typedef void (*ATermProtFunc)();

    /**
   * These are the types of ATerms there are. \see ATgetType().
   */
  static const size_t AT_FREE = 0;
  static const size_t AT_APPL = 1;
  static const size_t AT_INT = 2;
  // static const size_t AT_REAL = 3;
  static const size_t AT_LIST = 4;
  // static const size_t AT_PLACEHOLDER = 5;
  // static const size_t AT_BLOB = 6;
  static const size_t AT_SYMBOL = 7;

  static const size_t HEADER_BITS = sizeof(size_t)*8;
//#define HEADER_BITS      (sizeof(size_t)*8)

#ifdef AT_64BIT
  static const header_type SHIFT_LENGTH = _HT(34);
  static const header_type ARITY_BITS = _HT(8);
//#define SHIFT_LENGTH     34
//#define ARITY_BITS       8
#else
  static const header_type SHIFT_LENGTH = _HT(10);
  static const header_type ARITY_BITS = _HT(3);
//#define SHIFT_LENGTH     10
//#define ARITY_BITS       3
#endif /* AT_64BIT */

  static const header_type TYPE_BITS = _HT(3);
  static const header_type LENGTH_BITS = HEADER_BITS - SHIFT_LENGTH;
//#define TYPE_BITS        3
//#define LENGTH_BITS      (HEADER_BITS - SHIFT_LENGTH)

  static const header_type SHIFT_TYPE = _HT(4);
  static const header_type SHIFT_ARITY = _HT(7);
  static const header_type SHIFT_AGE = _HT(0);
  static const header_type SHIFT_REMOVE_MARK_AGE = _HT(3);
//#define SHIFT_TYPE       4
//#define SHIFT_ARITY      7
//#define SHIFT_AGE        0
//#define SHIFT_REMOVE_MARK_AGE 3

  static const header_type MASK_AGE = (_HT(1)<<_HT(0)) | (_HT(1)<<_HT(1));
  static const header_type MASK_MARK = _HT(1)<<_HT(2);
  static const header_type MASK_QUOTED = _HT(1)<<_HT(3);
  static const header_type MASK_TYPE = ((_HT(1) << TYPE_BITS)-_HT(1)) << SHIFT_TYPE;
  static const header_type MASK_ARITY = ((_HT(1) << ARITY_BITS)-_HT(1)) << SHIFT_ARITY;
  static const header_type MASK_AGE_MARK = MASK_AGE|MASK_MARK;
//#define MASK_AGE         ((1<<0) | (1<<1))
//#define MASK_MARK        (1<<2)
//#define MASK_QUOTED      (1<<3)
//#define MASK_TYPE        (((1 << TYPE_BITS)-1) << SHIFT_TYPE)
//#define MASK_ARITY       (((1 << ARITY_BITS)-1) << SHIFT_ARITY)
//#define MASK_AGE_MARK    (MASK_AGE|MASK_MARK)

  static const size_t MAX_LENGTH = ((MachineWord)1) << LENGTH_BITS;
//#define MAX_LENGTH       (((MachineWord)1) << LENGTH_BITS)

  inline
  size_t GET_AGE(const header_type h)
  {
    return (h & MASK_AGE) >> SHIFT_AGE;
  }
//#define GET_AGE(h)       ((size_t)(((h) & MASK_AGE) >> SHIFT_AGE))

  inline
  void SET_AGE(header_type& h, const header_type a)
  {
    do { h = ((h) & ~MASK_AGE) | (((a) << SHIFT_AGE) & MASK_AGE); } while (0);
  }
//#define SET_AGE(h, a)    do { h = ((h) & ~MASK_AGE) | (((a) << SHIFT_AGE) & MASK_AGE); } while (0)

  static const size_t YOUNG_AGE = 0;
  static const size_t OLD_AGE = 3;
//#define YOUNG_AGE        0
//#define OLD_AGE          3

  inline
  bool IS_OLD(const header_type h)
  {
    return GET_AGE(h) == OLD_AGE;
  }
//#define IS_OLD(h)        (GET_AGE(h) == OLD_AGE)

  inline
  bool IS_YOUNG(const header_type h)
  {
    return !IS_OLD(h);
  }
//#define IS_YOUNG(h)      (!(IS_OLD(h)))

  /* TODO: Optimize */
  inline
  void INCREMENT_AGE(header_type& h)
  {
    do { size_t age = GET_AGE(h);
      if (age<OLD_AGE) {
        SET_AGE((h), age+1);
      }
    } while (0);
  }
/*#define INCREMENT_AGE(h) do { size_t age = GET_AGE(h); \
    if (age<OLD_AGE) { \
      SET_AGE((h), age+1); \
    } \
  } while (0)*/

  inline
  header_type HIDE_AGE_MARK(const header_type h)
  {
    return h & ~MASK_AGE_MARK;
  }
//#define HIDE_AGE_MARK(h)    ((h) & ~MASK_AGE_MARK)

  inline
  bool EQUAL_HEADER(const header_type h1, const header_type h2)
  {
    return HIDE_AGE_MARK(h1^h2) == 0;
  }
//#define EQUAL_HEADER(h1,h2) (HIDE_AGE_MARK(h1^h2) == 0)

  static const size_t SHIFT_SYMBOL = SHIFT_LENGTH;
  static const size_t SHIFT_SYM_ARITY = SHIFT_LENGTH;
//#define SHIFT_SYMBOL  SHIFT_LENGTH
//#define SHIFT_SYM_ARITY SHIFT_LENGTH

//#define TERM_SIZE_APPL(arity) ((sizeof(struct __ATerm)/sizeof(size_t))+arity)
//#define TERM_SIZE_INT         (sizeof(struct __ATermInt)/sizeof(size_t))
//#define TERM_SIZE_LIST        (sizeof(struct __ATermList)/sizeof(size_t))
//#define TERM_SIZE_SYMBOL      (sizeof(struct _SymEntry)/sizeof(size_t))

  inline
  bool IS_MARKED(const header_type h)
  {
    return h & MASK_MARK;
  }
//#define IS_MARKED(h)          ((h) & MASK_MARK)

  inline
  size_t GET_TYPE(const header_type h)
  {
    return (h & MASK_TYPE) >> SHIFT_TYPE;
  }
//#define GET_TYPE(h)           ((size_t)(((h) & MASK_TYPE) >> SHIFT_TYPE))

  inline
  size_t GET_ARITY(const header_type h)
  {
    return (h & MASK_ARITY) >> SHIFT_ARITY;
  }
//#define GET_ARITY(h)        ((size_t)(((h) & MASK_ARITY) >> SHIFT_ARITY))

  inline
  // XXX FIXME
  //AFun GET_SYMBOL(const header_type h)
  size_t GET_SYMBOL(const header_type h)
  {
    return h >> SHIFT_SYMBOL;
  }
//#define GET_SYMBOL(h)       ((AFun)((h) >> SHIFT_SYMBOL))

  inline
  size_t GET_LENGTH(const header_type h)
  {
    return h >> SHIFT_LENGTH;
  }
//#define GET_LENGTH(h)         ((size_t)((h) >> SHIFT_LENGTH))

  inline
  bool IS_QUOTED(const header_type h)
  {
    return h & MASK_QUOTED;
  }
//#define IS_QUOTED(h)          (((h) & MASK_QUOTED) ? ATtrue : ATfalse)

  inline
  void SET_MARK(header_type& h)
  {
    do { (h) |= MASK_MARK; } while (0);
  }
//#define SET_MARK(h)           do { (h) |= MASK_MARK; } while (0)

  /* UNUSED
  inline
  void SET_SYMBOL(header_type& h, const size_t sym)
  {
    do { (h) = ((h) & ~MASK_SYMBOL)
          | (((header_type)(sym)) << SHIFT_SYMBOL);
    } while (0);
  } */
/*
#define SET_SYMBOL(h, sym)    do { (h) = ((h) & ~MASK_SYMBOL) \
      | (((header_type)(sym)) << SHIFT_SYMBOL); \
} while (0)
*/

  /* UNUSED
  inline
  void SET_LENGTH(header_type& h, const size_t len)
  {
    do { (h) = ((h) & ~MASK_LENGTH) |
          | (((header_type)(len)) << SHIFT_LENGTH);
    } while (0);
  } */
/*
#define SET_LENGTH(h, len)    do { (h) = ((h) & ~MASK_LENGTH) | \
      | (((header_type)(len)) << SHIFT_LENGTH); \
} while (0)
*/

  inline
  void SET_QUOTED(header_type& h)
  {
    do { (h) |= MASK_QUOTED; } while (0);
  }
//#define SET_QUOTED(h)         do { (h) |= MASK_QUOTED; } while (0)

  inline
  void CLR_MARK(header_type& h)
  {
    do { (h) &= ~MASK_MARK; } while (0);
  }
//#define CLR_MARK(h)           do { (h) &= ~MASK_MARK; } while (0)

  inline
  void CLR_QUOTED(header_type& h)
  {
    do { (h) &= ~MASK_QUOTED; } while (0);
  }
//#define CLR_QUOTED(h)         do { (h) &= ~MASK_QUOTED; } while (0)

  inline
  header_type APPL_HEADER(const size_t ari, const size_t sym)
  {
    return ((ari) << SHIFT_ARITY) |
        (AT_APPL << SHIFT_TYPE) |
        ((header_type)(sym) << SHIFT_SYMBOL);
  }
/*
#define APPL_HEADER(ari,sym) (((ari) << SHIFT_ARITY) | \
                              (AT_APPL << SHIFT_TYPE) | \
                              ((header_type)(sym) << SHIFT_SYMBOL))
*/

  static const header_type INT_HEADER = AT_INT << SHIFT_TYPE;
//#define INT_HEADER           (AT_INT << SHIFT_TYPE)
  static const header_type EMPTY_HEADER = AT_LIST << SHIFT_TYPE;
//#define EMPTY_HEADER         (AT_LIST << SHIFT_TYPE)

  inline
  header_type LIST_HEADER(const size_t len)
  {
    return (AT_LIST << SHIFT_TYPE) |
        ((MachineWord)(len) << SHIFT_LENGTH) | (2 << SHIFT_ARITY);
  }
/*
#define LIST_HEADER(len)     ((AT_LIST << SHIFT_TYPE) | \
                              ((MachineWord)(len) << SHIFT_LENGTH) | (2 << SHIFT_ARITY))
*/

  inline
  header_type SYMBOL_HEADER(const size_t arity, const bool quoted)
  {
    return ((header_type)(arity) << SHIFT_SYM_ARITY) |
       ((quoted) ? MASK_QUOTED : 0) |
       (AT_SYMBOL << SHIFT_TYPE);
  }
/*
#define SYMBOL_HEADER(arity,quoted) \
  (((header_type)(arity) << SHIFT_SYM_ARITY) | \
   ((quoted) ? MASK_QUOTED : 0) | \
   (AT_SYMBOL << SHIFT_TYPE))
*/

static const size_t FREE_HEADER = AT_FREE << SHIFT_TYPE;
//#define FREE_HEADER               (AT_FREE << SHIFT_TYPE)

static const size_t ARG_OFFSET = TERM_SIZE_APPL(0);
//#define ARG_OFFSET                TERM_SIZE_APPL(0)

static const size_t MAX_HEADER_BITS = 64;
//#define MAX_HEADER_BITS 64

inline
MachineWord AT_TABLE_SIZE(const size_t table_class)
{
  return (size_t)1<<(table_class);
}
//#define AT_TABLE_SIZE(table_class)  ((size_t)1<<(table_class))

inline
MachineWord AT_TABLE_MASK(const size_t table_class)
{
  return AT_TABLE_SIZE(table_class)-1;
}
//#define AT_TABLE_MASK(table_class)  (AT_TABLE_SIZE(table_class)-1)

  /* Integers in BAF are always exactly 32 bits.  The size must be fixed so that
   * BAF terms can be exchanged between platforms. */
static const size_t INT_SIZE_IN_BAF = 32;
//#define INT_SIZE_IN_BAF 32

static const size_t MAX_ARITY = MAX_LENGTH;
//#define MAX_ARITY             MAX_LENGTH

static const size_t MIN_TERM_SIZE = TERM_SIZE_APPL(0);
//#define MIN_TERM_SIZE         TERM_SIZE_APPL(0)
static const size_t INITIAL_MAX_TERM_SIZE = 256;
//#define INITIAL_MAX_TERM_SIZE 256

} // namespace aterm

#endif
