/**
  * encoding.h: Low level encoding of ATerm datatype.
  */

#ifndef ENCODING_H
#define ENCODING_H
#include <vector>
#include <assert.h>
#include <iostream>
#include "mcrl2/aterm/architecture.h"
#include "mcrl2/aterm/atypes.h"
#include "mcrl2/aterm/afun.h"

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

typedef void (*ATermProtFunc)();

/**
 * These are the types of ATerms there are. \see ATgetType().
 */

static const size_t AT_APPL = 1;
static const size_t AT_INT = 2;
static const size_t AT_LIST = 3;

struct _ATerm
{
    AFun    m_function_symbol;
    size_t  reference_count;
    _ATerm* next;

    size_t type() const
    {
      if (m_function_symbol.number()==AS_LIST.number() || m_function_symbol.number()==AS_EMPTY_LIST.number())
      { 
        return AT_LIST;
      }
      else if (m_function_symbol.number()==AS_INT.number())
      {
        return AT_INT;
      }
      return AT_APPL;
    }
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

    /// \brief Returns the function symbol belonging to a term.
    /// \return The function symbol of this term.
    const AFun &function_symbol() const
    {
      return m_aterm->m_function_symbol;
    }

    /// \brief Returns the type of this term.
    /// Result is one of AT_APPL, AT_INT,
    /// or AT_LIST.
    /// \detail Often it is more efficient to use function_symbol(),
    /// and check whether the function symbol matches AS_INT for an 
    /// AT_INT, AS_LIST or AS_EMPTY_LIST for AT_LIST, or something else
    /// for AT_APPL.
    /// \return The type of the term.
    size_t type() const
    {
      return m_aterm->type(); 
    }

    /// \brief Writes the term to a string.
    /// \return A string representation of the term.
    std::string to_string() const;
    /* {
      return ATwriteToString(m_aterm);
    } */

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

inline
std::ostream& operator<<(std::ostream& out, const ATerm& t)
{
  return out << t.to_string();
}

// static const size_t FREE_HEADER = AT_FREE << SHIFT_TYPE;

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

// static const size_t MAX_ARITY = MAX_LENGTH;

/* The constants below are not static to prevent some compiler warnings */
const size_t MIN_TERM_SIZE = TERM_SIZE_APPL(0);
const size_t INITIAL_MAX_TERM_SIZE = 256;

} // namespace aterm

#endif
