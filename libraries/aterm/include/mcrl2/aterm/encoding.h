// #define PRINT_GC_INFO
/**
  * encoding.h: Low level encoding of ATerm datatype.
  */

#ifndef ENCODING_H
#define ENCODING_H
#include <vector>
#include <assert.h>
#include "mcrl2/aterm/architecture.h"
#include "mcrl2/aterm/atypes.h"

namespace aterm
{

/*
 32-bit:

 |---------------------------------|
 | info|arity| type|quoted|mark|age|   NOTE: AGE SHOULD FROM NOW ON ALWAYS BE 00 (25/3/2012).
 |---------------------------------|
  31 10 9 8 7 6 5 4   3     2   1 0

 64-bit:

 |------------------------------------|
 | info|  |arity| type|quoted|mark|age|   NOTE: AGE SHOULD FROM NOW ON ALWAYS BE 00 (25/3/2012).
 |------------------------------------|
  63 34 15 14  7 6 5 4   3     2   1 0

*/

/* Although atypes.h defines MachineWord, it wasn't used here:
   typedef unsigned long header_type; */
typedef MachineWord header_type;

#define MCRL2_HT(t) (static_cast<MachineWord>(t))

class _ATerm
{
  public:
    header_type   header;
    size_t reference_count;
    _ATerm* next;
};

inline
size_t TERM_SIZE_APPL(const size_t arity)
{
  return (sizeof(_ATerm)/sizeof(size_t))+arity;
}

extern void at_free_term(_ATerm* t);

class ATerm
{
  public:
    static std::vector <_ATerm*> hashtable;

  protected:
    _ATerm *m_aterm;

    void decrease_reference_count()
    {
      if (m_aterm!=NULL)
      {
#ifdef PRINT_GC_INFO
fprintf(stderr,"decrease reference count %ld  %p\n",m_aterm->reference_count,m_aterm);
#endif
        assert(m_aterm->reference_count>0);
        if (0== --m_aterm->reference_count)
        {
          at_free_term(m_aterm);
          return;
        }
      }
    }

    template <bool CHECK>
    static void increase_reference_count(_ATerm* t)
    {
      if (t!=NULL)
      {
#ifdef PRINT_GC_INFO
fprintf(stderr,"increase reference count %ld  %p\n",t->reference_count,t);
#endif
        if (CHECK) assert(t->reference_count>0);
        t->reference_count++;
      }

    }

    void copy_term(_ATerm* t)
    {
      increase_reference_count<true>(t);
      decrease_reference_count();
      m_aterm=t;
    }

  public:

    ATerm ():m_aterm(NULL)
    {}

    ATerm (const ATerm &t):m_aterm(t.m_aterm)
    {
      increase_reference_count<true>(m_aterm);
    }

    ATerm (_ATerm *t):m_aterm(t)
    {
      // Note that reference_count can be 0, as this term can just be constructed,
      // and is now handed over to become a real ATerm.
      increase_reference_count<false>(m_aterm);
    }

    ATerm &operator=(const ATerm &t)
    {
      copy_term(t.m_aterm);
      return *this;
    }

    ~ATerm ()
    {
      decrease_reference_count();
    }

    _ATerm & operator *() const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      return *m_aterm;
    }

    _ATerm * operator ->() const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      return m_aterm;
    }

    bool operator ==(const ATerm &t) const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      assert(t.m_aterm==NULL || t.m_aterm->reference_count>0);
      return m_aterm==t.m_aterm;
    }

    bool operator !=(const ATerm &t) const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      assert(t.m_aterm==NULL || t.m_aterm->reference_count>0);
      return m_aterm!=t.m_aterm;
    }

    bool operator <(const ATerm &t) const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      assert(t.m_aterm==NULL || t.m_aterm->reference_count>0);
      return m_aterm<t.m_aterm;
    }

    bool operator >(const ATerm &t) const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      assert(t.m_aterm==NULL || t.m_aterm->reference_count>0);
      return m_aterm>t.m_aterm;
    }

    bool operator <=(const ATerm &t) const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      assert(t.m_aterm==NULL || t.m_aterm->reference_count>0);
      return m_aterm<=t.m_aterm;
    }

    bool operator >=(const ATerm &t) const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      assert(t.m_aterm==NULL || t.m_aterm->reference_count>0);
      return m_aterm>=t.m_aterm;
    }

    /// \brief Test on whether an the ATerm is not equal to NULL.
    bool is_defined() const
    {
      assert(m_aterm==NULL || m_aterm->reference_count>0);
      return m_aterm!=NULL;
    }
};



typedef void (*ATermProtFunc)();

/**
 * These are the types of ATerms there are. \see ATgetType().
 */
static const size_t AT_FREE = 0;
static const size_t AT_APPL = 1;
static const size_t AT_INT = 2;
static const size_t AT_LIST = 4;
static const size_t AT_SYMBOL = 7;

static const size_t HEADER_BITS = sizeof(size_t)*8;

#ifdef AT_64BIT
static const header_type SHIFT_LENGTH = MCRL2_HT(34);
static const header_type ARITY_BITS = MCRL2_HT(8);
#endif /* AT_64BIT */
#ifdef AT_32BIT
static const header_type SHIFT_LENGTH = MCRL2_HT(10);
static const header_type ARITY_BITS = MCRL2_HT(3);
#endif /* AT_32BIT */

static const header_type TYPE_BITS = MCRL2_HT(3);
static const header_type LENGTH_BITS = HEADER_BITS - SHIFT_LENGTH;

static const header_type SHIFT_TYPE = MCRL2_HT(4);

static const header_type SHIFT_ARITY = MCRL2_HT(7);

static const header_type MASK_MARK = MCRL2_HT(1)<<MCRL2_HT(2);
static const header_type MASK_QUOTED = MCRL2_HT(1)<<MCRL2_HT(3);
static const header_type MASK_TYPE = ((MCRL2_HT(1) << TYPE_BITS)-MCRL2_HT(1)) << SHIFT_TYPE;
static const header_type MASK_ARITY = ((MCRL2_HT(1) << ARITY_BITS)-MCRL2_HT(1)) << SHIFT_ARITY;

static const size_t MAX_LENGTH = ((MachineWord)1) << LENGTH_BITS;

inline
bool EQUAL_HEADER(const header_type h1, const header_type h2)
{
  return h1==h2;
}

static const size_t SHIFT_SYMBOL = SHIFT_LENGTH;
static const size_t SHIFT_SYM_ARITY = SHIFT_LENGTH;

inline
bool IS_MARKED(const header_type h)
{
  return (h & MASK_MARK) != (header_type)(0);
}

inline
size_t GET_TYPE(const header_type h)
{
  return (h & MASK_TYPE) >> SHIFT_TYPE;
}

inline
size_t GET_ARITY(const header_type h)
{
  return (h & MASK_ARITY) >> SHIFT_ARITY;
}

inline size_t GET_SYMBOL(const header_type h)
{
  return h >> SHIFT_SYMBOL;
}

inline
size_t GET_LENGTH(const header_type h)
{
  return h >> SHIFT_LENGTH;
}

inline
bool IS_QUOTED(const header_type h)
{
  return (h & MASK_QUOTED) != (header_type)(0);
}

inline
void SET_MARK(header_type& h)
{
  do
  {
    (h) |= MASK_MARK;
  }
  while (0);
}

inline
void SET_QUOTED(header_type& h)
{
  do
  {
    (h) |= MASK_QUOTED;
  }
  while (0);
}

inline
void CLR_MARK(header_type& h)
{
  do
  {
    (h) &= ~MASK_MARK;
  }
  while (0);
}

inline
void CLR_QUOTED(header_type& h)
{
  do
  {
    (h) &= ~MASK_QUOTED;
  }
  while (0);
}

inline
header_type APPL_HEADER(const size_t ari, const size_t sym)
{
  return ((ari) << SHIFT_ARITY) |
         (AT_APPL << SHIFT_TYPE) |
         ((header_type)(sym) << SHIFT_SYMBOL);
}

static const header_type INT_HEADER = AT_INT << SHIFT_TYPE;
static const header_type EMPTY_HEADER = AT_LIST << SHIFT_TYPE;

inline
header_type LIST_HEADER(const size_t len)
{
  return (AT_LIST << SHIFT_TYPE) |
         ((MachineWord)(len) << SHIFT_LENGTH) | ((MachineWord)(2) << SHIFT_ARITY);
}

inline
header_type SYMBOL_HEADER(const size_t arity, const bool quoted)
{
  return ((header_type)(arity) << SHIFT_SYM_ARITY) |
         ((quoted) ? MASK_QUOTED : 0) |
         (AT_SYMBOL << SHIFT_TYPE);
}

static const size_t FREE_HEADER = AT_FREE << SHIFT_TYPE;

static const size_t ARG_OFFSET = TERM_SIZE_APPL(0);

static const size_t MAX_HEADER_BITS = 64;

inline
MachineWord AT_TABLE_SIZE(const size_t table_class)
{
  return (size_t)1<<(table_class);
}

inline
MachineWord AT_TABLE_MASK(const size_t table_class)
{
  return AT_TABLE_SIZE(table_class)-1;
}

/* Integers in BAF are always exactly 32 bits.  The size must be fixed so that
 * BAF terms can be exchanged between platforms. */
static const size_t INT_SIZE_IN_BAF = 32;

static const size_t MAX_ARITY = MAX_LENGTH;

/* The constants below are not static to prevent some compiler warnings */
const size_t MIN_TERM_SIZE = TERM_SIZE_APPL(0);
const size_t INITIAL_MAX_TERM_SIZE = 256;

} // namespace aterm

#endif
