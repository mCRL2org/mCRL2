#ifndef DETAIL_FUNCTION_SYMBOL_H
#define DETAIL_FUNCTION_SYMBOL_H

#include <string>
#include <assert.h>

namespace atermpp
{
namespace detail
{

/* The type _function_symbol is used to store information about function_symbols. */
struct _function_symbol
{
  public:
    size_t m_arity;
    size_t next;
    size_t reference_count;
    std::string name;

    _function_symbol(const std::string &s, const size_t arity):
        m_arity(arity),
        next(size_t(-1)),
        reference_count(0),
        name(s)
    {
    }

    size_t arity() const    
    {
      return m_arity;
    }
};

extern std::vector < detail::_function_symbol > at_lookup_table;

void at_free_afun(const size_t n);

inline bool AT_isValidAFun(const size_t sym);

template <bool CHECK>
void increase_reference_count(const size_t n);

inline
void decrease_reference_count(const size_t n);


} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

