#ifndef DETAIL_ATERM_H
#define DETAIL_ATERM_H

#include <cstddef>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol_constants.h"
#include "mcrl2/atermpp/function_symbol.h"

namespace atermpp
{

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

    void decrease_reference_count() const
    {
      assert(!reference_count_indicates_is_in_freelist());
      assert(!reference_count_is_zero());
      --m_reference_count;
    } 

    void increase_reference_count() const
    {
      assert(!reference_count_indicates_is_in_freelist());
      ++m_reference_count;
    } 

    void reset_reference_count(const bool check=true) const
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

    void set_reference_count_indicates_in_freelist(const bool check=true) const
    {
      if (check) assert(!reference_count_indicates_is_in_freelist());
      m_reference_count=size_t(-1);
    }

    bool reference_count_indicates_is_in_freelist() const
    {
      return m_reference_count==size_t(-1);
    }

    const _aterm* next() const
    {
      return m_next;
    }

    void set_next(const _aterm* n) const
    {
      m_next=n;
    }
};

static const size_t TERM_SIZE=sizeof(_aterm)/sizeof(size_t);

const detail::_aterm* allocate_term(const size_t size);
const _aterm* aterm0(const function_symbol &sym);
void remove_from_hashtable(const _aterm *t);
void simple_free_term(const _aterm *t, const size_t arity);
void free_term(const detail::_aterm* t);
void initialise_aterm_administration();
inline HashNumber hash_number(const _aterm *t);

template <class Term, class Iter, class ATermConverter>
const _aterm *make_list_backward(Iter first, Iter last, const ATermConverter &convert_to_aterm);

} // namespace detail
} // namespace atermpp

#endif /* DETAIL_ATERM_H */
