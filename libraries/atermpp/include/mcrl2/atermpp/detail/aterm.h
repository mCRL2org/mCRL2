#ifndef DETAIL_ATERM_H
#define DETAIL_ATERM_H

#include <cstddef>
#include "mcrl2/atermpp/detail/atypes.h"
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
    size_t  m_reference_count;
    _aterm* m_next;

  public:
    function_symbol &function() 
    {
      return m_function_symbol;
    }

    const function_symbol &function() const
    {
      return m_function_symbol;
    }

    size_t &reference_count()
    {
      return m_reference_count;
    }

    const size_t &reference_count() const 
    {
      return m_reference_count;
    }

    _aterm* &next()
    {
      return m_next;
    }

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


extern std::vector <detail::_aterm*>& hashtable();
detail::_aterm* allocate_term(const size_t size);
// void free_term(detail::_aterm *t);

inline size_t term_size(const detail::_aterm *t);
inline HashNumber hash_number(const detail::_aterm *t, const size_t size);

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
