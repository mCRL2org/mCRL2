#ifndef DETAIL_FUNCTION_SYMBOL_H
#define DETAIL_FUNCTION_SYMBOL_H

#include <string>

const size_t FUNCTION_SYMBOL_BLOCK_CLASS=14;
const size_t FUNCTION_SYMBOL_BLOCK_SIZE=1<<FUNCTION_SYMBOL_BLOCK_CLASS;
const size_t FUNCTION_SYMBOL_BLOCK_MASK=FUNCTION_SYMBOL_BLOCK_SIZE-1;


namespace atermpp
{
class function_symbol;

namespace detail
{
size_t addressf(const function_symbol &t);

/* The type _function_symbol is used to store information about function_symbols. */
struct _function_symbol
{
    size_t arity;
    _function_symbol* next;
    mutable size_t reference_count;
    std::string name;
    size_t number;
}; 

extern detail::_function_symbol** function_symbol_index_table;
extern size_t function_symbol_index_table_size;

inline
bool is_valid_function_symbol(const detail::_function_symbol* f)
{
  return f->reference_count>0;
}

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

