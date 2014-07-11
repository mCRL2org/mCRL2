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
extern size_t function_symbol_index_table_number_of_elements;

bool check_that_the_function_symbol_points_to_memory_containing_a_function(const detail::_function_symbol* f);

inline
bool is_valid_function_symbol(const detail::_function_symbol* f)
{
  return (f->reference_count>0) && check_that_the_function_symbol_points_to_memory_containing_a_function(f);
}

// set index such that no function symbol exists with the name 'prefix + std::to_string(n)'
// for all values n >= index
extern size_t get_sufficiently_large_postfix_index(const std::string& prefix_);

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

