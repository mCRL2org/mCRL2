#ifndef DETAIL_ATERM_H
#define DETAIL_ATERM_H

#include <cstddef>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol_constants.h"
#include "mcrl2/atermpp/function_symbol.h"

namespace atermpp
{

class aterm;

namespace detail
{

static const size_t IN_FREE_LIST(-1);

class _aterm
{
  protected:
    function_symbol m_function_symbol;
    size_t  m_reference_count;
    _aterm* m_next;

  public:
    function_symbol& function() 
    {
      return m_function_symbol;
    }

    const function_symbol& function() const
    {
      return m_function_symbol;
    }

    void decrease_reference_count()
    {
      assert(!reference_count_indicates_is_in_freelist());
      assert(!reference_count_is_zero());
      --m_reference_count;
    } 

    void increase_reference_count()
    {
      assert(!reference_count_indicates_is_in_freelist());
      ++m_reference_count;
    } 

    void reset_reference_count(const bool check=true)
    {
      if (check) assert(reference_count_indicates_is_in_freelist());
      m_reference_count=0;
    } 

    bool reference_count_is_zero() const
    {
      return m_reference_count==0;
    } 

    size_t reference_count() const 
    {
      return m_reference_count;
    }

    void set_reference_count_indicates_in_freelist(const bool check=true)
    {
      if (check) assert(!reference_count_indicates_is_in_freelist());
      m_reference_count=IN_FREE_LIST;
    }

    bool reference_count_indicates_is_in_freelist() const
    {
      return m_reference_count==IN_FREE_LIST;
    }

    _aterm* next() const
    {
      return m_next;
    }

    void set_next(_aterm* n)
    {
      m_next=n;
    }
};

static const size_t TERM_SIZE=sizeof(_aterm)/sizeof(size_t);

detail::_aterm* allocate_term(const size_t size);
void remove_from_hashtable(_aterm *t);
void free_term(detail::_aterm* t);
void free_term_aux(detail::_aterm* t, detail::_aterm*& terms_to_be_removed);
void initialise_aterm_administration();
inline HashNumber hash_number(_aterm *t);

template <class Term, class Iter, class ATermConverter>
_aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm);
template <class Term, class Iter, class ATermConverter, class ATermFilter>
_aterm *make_list_backward(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter);
template <class Term, class Iter, class ATermConverter>
_aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm);
template <class Term, class Iter, class ATermConverter, class ATermFilter>
_aterm *make_list_forward(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter);

// Provides the address where the data belonging to this aterm is stored.
inline _aterm* address(const aterm& t); 

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
