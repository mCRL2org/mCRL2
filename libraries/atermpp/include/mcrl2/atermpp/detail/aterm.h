#ifndef DETAIL_ATERM_H
#define DETAIL_ATERM_H

#include <cstddef>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol_constants.h"
#include "mcrl2/atermpp/function_symbol.h"

namespace atermpp
{

static const size_t AT_APPL = 1;
static const size_t AT_INT = 2;
static const size_t AT_LIST = 3;

namespace detail
{

class _aterm
{
  protected:
    function_symbol m_function_symbol;
    mutable size_t  m_reference_count;
    mutable const _aterm* m_next;

  public:
    function_symbol &function() 
    {
      return m_function_symbol;
    }

    const function_symbol &function() const
    {
      return m_function_symbol;
    }

    size_t decrease_reference_count() const
    {
      return --m_reference_count;
    } 

    void increase_reference_count() const
    {
      ++m_reference_count;
    } 

    void reset_reference_count() const
    {
      m_reference_count=0;
    } 

    size_t reference_count() const 
    {
      return m_reference_count;
    }

    const _aterm* next() const
    {
      return m_next;
    }

    void set_next(const _aterm* n) const
    {
      m_next=n;
    }

    size_t type() const
    {
      const size_t number=m_function_symbol.number();
      if (number==detail::function_adm.AS_LIST.number() || number==detail::function_adm.AS_EMPTY_LIST.number())
      {
        return AT_LIST;
      }
      else if (number==detail::function_adm.AS_INT.number())
      {
        return AT_INT;
      }
      return AT_APPL;
    }

};

static const size_t TERM_SIZE=sizeof(_aterm)/sizeof(size_t);

const detail::_aterm* allocate_term(const size_t size);
const _aterm* aterm0(const function_symbol &sym);
void remove_from_hashtable(const _aterm *t);
void simple_free_term(const _aterm *t, const size_t arity);
void initialise_aterm_administration();
inline HashNumber hash_number(const _aterm *t);

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
