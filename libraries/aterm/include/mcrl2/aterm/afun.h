// #define PRINT_GC_INFO
#ifndef AFUN_H
#define AFUN_H

#include <assert.h>
#include <string>
#include <cstdio>
#include <vector>
#include "mcrl2/aterm/atypes.h"
// #include "mcrl2/aterm/encoding.h"


namespace aterm
{

/* The type _SymEntry is used to store information about AFun's. */
struct _SymEntry
{
  public:
    size_t header;
    size_t next;
    size_t reference_count;
    size_t id;
    char*  name;
    size_t count;  /* used in bafio.c */
    size_t index;  /* used in bafio.c */

    _SymEntry(const size_t arity, bool quoted, const size_t i,size_t c,size_t in):
        header(make_header(arity,quoted)),
        next(size_t(-1)),
        reference_count(0),
        id(i),
        name(NULL),
        count(c),
        index(in)
    { 
    }

    static size_t make_header(const size_t arity, bool quoted)
    {
      assert ((quoted & 1) == quoted);
      return arity << 1 | quoted;
    }

    size_t arity() const
    {
      return header>>1;
    }

    bool is_quoted() const
    {
      return header & 1;
    }
};

void at_free_afun(const size_t n);

inline
bool AT_isValidAFun(const size_t sym);


class AFun
{
  protected:
    size_t m_number;

  public:
    static size_t first_free;
    static std::vector < _SymEntry* > at_lookup_table; // As safio uses stable pointers to _SymEntries,
                                                       // we cannot use a vector of _SymEntry, as these
                                                       // are relocated. 

  protected:
    template <bool CHECK>
    static void increase_reference_count(const size_t n)
    {
      if (n!=size_t(-1))
      {
#ifdef PRINT_GC_INFO
fprintf(stderr,"increase afun reference count %ld (%ld, %s)\n",n,at_lookup_table[n]->reference_count,at_lookup_table[n]->name);
#endif
        assert(n<at_lookup_table.size());
        if (CHECK) assert(at_lookup_table[n]->reference_count>0);
        at_lookup_table[n]->reference_count++;
      }
    }

    static void decrease_reference_count(const size_t n)
    { 
      if (n!=size_t(-1))
      {
#ifdef PRINT_GC_INFO
fprintf(stderr,"decrease afun reference count %ld (%ld, %s)\n",n,at_lookup_table[n]->reference_count,at_lookup_table[n]->name);
#endif
        assert(n<at_lookup_table.size());
        assert(at_lookup_table[n]->reference_count>0);

        if (--at_lookup_table[n]->reference_count==0)
        {
          at_free_afun(n);
        }
      }
    }

  public:
    /// \brief default constructor
    AFun():m_number(size_t(-1))
    {
    }

    /// \brief Constructor.
    /// \param name A string
    /// \param arity The arity of the function.
    /// \param quoted True if the function symbol is a quoted string.
    AFun(const char* name, const size_t arity, const bool quoted = false);
//                      : m_number(ATmakeAFun(const_cast<char*>(name.c_str()), arity, quoted ))
//    {}
    
    /// \brief default constructor
    /// \param n The number of an AFun
    AFun(const size_t n):m_number(n)
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      increase_reference_count<false>(m_number);
    }

    /// \brief Copy constructor
    AFun(const AFun &f):m_number(f.m_number)
    {
      increase_reference_count<true>(m_number);
    }

    /// \brief Assignment operator.
    AFun &operator=(const AFun &f)
    {
      increase_reference_count<true>(f.m_number);
      decrease_reference_count(m_number); // Decrease after increasing the number, as otherwise this goes wrong when 
                                          // carrying out x=x;
      m_number=f.m_number;
      return *this;
    }

    /// \brief Destructor
    ~AFun()
    {
      decrease_reference_count(m_number);
    }

    /// \brief Return the name of the function_symbol.
    /// \return The name of the function symbol.
    std::string name() const
    {
      assert(AT_isValidAFun(m_number));
      return std::string(at_lookup_table[m_number]->name);
    }
    
    /// \brief Return the number of the function_symbol.
    /// \return The number of the function symbol.
    size_t number() const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      return m_number;
    }

    /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
    /// \return The arity of the function symbol.
    size_t arity() const
    {
      assert(AT_isValidAFun(m_number));
      return at_lookup_table[m_number]->arity();
    }

    /// \brief Determine if the function symbol (function_symbol) is quoted or not.
    /// \return True if the function symbol is quoted.
    bool is_quoted() const
    {
      assert(AT_isValidAFun(m_number));
      return at_lookup_table[m_number]->is_quoted();
    }
    
    bool operator ==(const AFun &f) const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || AT_isValidAFun(f.m_number));
      return m_number==f.m_number;
    }

    bool operator !=(const AFun &f) const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || AT_isValidAFun(f.m_number));
      return m_number!=f.m_number;
    }

    bool operator <(const AFun &f) const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || AT_isValidAFun(f.m_number));
      return m_number<f.m_number;
    }

    bool operator >(const AFun &f) const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || AT_isValidAFun(f.m_number));
      return m_number>f.m_number;
    }

    bool operator <=(const AFun &f) const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || AT_isValidAFun(f.m_number));
      return m_number<=f.m_number;
    }

    bool operator >=(const AFun &f) const
    {
      assert(m_number==size_t(-1) || AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || AT_isValidAFun(f.m_number));
      return m_number>=f.m_number;
    }

    /// Functions below must be removed.
    void protect() const // to be removed.
    {}

    void unprotect() const // to be removed
    {}
};


AFun ATmakeAFun(const char* name, const size_t arity, const bool quoted);

// The following afuns are used in bafio.


extern const AFun AS_INT;
extern const AFun AS_LIST;
extern const AFun AS_EMPTY_LIST;


inline
bool AT_isValidAFun(const size_t sym)
{
  return (sym != size_t(-1) && 
          sym < AFun::at_lookup_table.size() && 
          AFun::at_lookup_table[sym]->reference_count>0);
}


size_t AT_printAFun(const size_t sym, FILE* f);

std::string ATwriteAFunToString(const AFun &t);

}

#endif
