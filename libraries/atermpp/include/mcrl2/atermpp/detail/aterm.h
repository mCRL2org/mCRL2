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

struct _aterm
{
    function_symbol    m_function_symbol;
    size_t  reference_count;
    _aterm* next;

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

    const function_symbol function() const
    {
      return m_function_symbol;
    }
};

inline
size_t TERM_SIZE_APPL(const size_t arity)
{
  return (sizeof(_aterm)/sizeof(size_t))+arity;
}

static const size_t ARG_OFFSET = TERM_SIZE_APPL(0);

/* The constants below are not static to prevent some compiler warnings */
const size_t MIN_TERM_SIZE = TERM_SIZE_APPL(0);
const size_t INITIAL_MAX_TERM_SIZE = 256;

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
