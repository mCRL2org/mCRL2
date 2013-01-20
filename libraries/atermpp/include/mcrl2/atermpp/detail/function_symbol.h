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
    size_t arity;
    size_t next;
    size_t reference_count;
    std::string name;

    _function_symbol(const std::string &s, const size_t a, const size_t n):
        arity(a),
        next(n),
        reference_count(0),
        name(s)
    {
    }
};

extern size_t function_lookup_table_size;
extern detail::_function_symbol* function_lookup_table;

inline
bool is_valid_function_symbol(const size_t sym)
{
  return (sym != size_t(-1) &&
          sym < detail::function_lookup_table_size &&
          detail::function_lookup_table[sym].reference_count>0);
}

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

