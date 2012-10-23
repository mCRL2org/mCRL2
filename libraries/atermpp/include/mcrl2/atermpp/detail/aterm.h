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

detail::_aterm* allocate_term(const size_t size);
void simple_free_term(_aterm *t, const size_t arity);

inline size_t term_size(const detail::_aterm *t);
inline HashNumber hash_number(const detail::_aterm *t, const size_t size);

// extern _aterm *empty_aterm_list();

template <class Term, class ForwardIterator>
detail::_aterm* local_term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end);

template <class Term, class InputIterator, class ATermConverter>
_aterm* local_term_appl_with_converter(const function_symbol &sym, const InputIterator begin, const InputIterator end, const ATermConverter &convert_to_aterm);

template <class Term>
detail::_aterm* term_appl1(const function_symbol &sym, const Term &t1);

template <class Term>
detail::_aterm* term_appl2(const function_symbol &sym, const Term &t1, const Term &t2);

template <class Term>
detail::_aterm* term_appl3(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3);

template <class Term>
detail::_aterm* term_appl4(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4);

template <class Term>
detail::_aterm* term_appl5(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5);

template <class Term>
detail::_aterm* term_appl6(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5, const Term &t6);

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
