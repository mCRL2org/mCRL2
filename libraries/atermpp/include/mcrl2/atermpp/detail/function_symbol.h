#ifndef DETAIL_FUNCTION_SYMBOL_H
#define DETAIL_FUNCTION_SYMBOL_H

#include <string>

// If FUNCTION_SYMBOL_AS_POINTER is defined, function symbols are pointers
// pointing into blocks of memory that is not relocated, and which is accesible via
// a function_symbol_index_table. If FUNCTION_SYMBOL_AS_POINTER is not defined,
// function_symbols are consecutive numbers that index into a vector like piece of memory
// which is doubled and relocated when the number of function symbols in use grow.

// #define FUNCTION_SYMBOL_AS_POINTER

#ifdef FUNCTION_SYMBOL_AS_POINTER
const size_t FUNCTION_SYMBOL_BLOCK_CLASS=14;
const size_t FUNCTION_SYMBOL_BLOCK_SIZE=1<<FUNCTION_SYMBOL_BLOCK_CLASS;
const size_t FUNCTION_SYMBOL_BLOCK_MASK=FUNCTION_SYMBOL_BLOCK_SIZE-1;
#endif


namespace atermpp
{
class function_symbol;

namespace detail
{
size_t address(const function_symbol &t);

/* The type _function_symbol is used to store information about function_symbols. */
struct _function_symbol
{
    size_t arity;
#ifdef FUNCTION_SYMBOL_AS_POINTER 
    _function_symbol* next;
#else
    mutable size_t next;
#endif
    mutable size_t reference_count;
    std::string name;
#ifdef FUNCTION_SYMBOL_AS_POINTER 
    size_t number;
#endif

#ifdef FUNCTION_SYMBOL_AS_POINTER 
#else
    _function_symbol(const std::string &s, const size_t a, const size_t n):
        arity(a),
        next(n),
        reference_count(0),
        name(s)
    {
    }
#endif

}; 

#ifdef FUNCTION_SYMBOL_AS_POINTER
extern detail::_function_symbol** function_symbol_index_table;
extern size_t function_symbol_index_table_size;
#else
extern detail::_function_symbol* function_lookup_table;
extern size_t function_lookup_table_size;
#endif

inline
#ifdef FUNCTION_SYMBOL_AS_POINTER
bool is_valid_function_symbol(const detail::_function_symbol* f)
{
  // TODO: extend this check
  return f->reference_count>0;
}
#else
bool is_valid_function_symbol(const size_t sym)
{
  return (sym < detail::function_lookup_table_size &&
          detail::function_lookup_table[sym].reference_count>0);
}
#endif

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

