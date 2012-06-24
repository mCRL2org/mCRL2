#ifndef DETAIL_FUNCTION_SYMBOL_H
#define DETAIL_FUNCTION_SYMBOL_H

#include <string>
#include <assert.h>

namespace atermpp
{
namespace detail
{

/* The type _SymEntry is used to store information about function_symbols. */
struct _function_symbol
{
  public:
    size_t header;
/*  Onderstaande is een suggestie van Sjoerd, om van expliciete shifts af te komen,
 *      en de code helderder te krijgen.
 *          size_t arity: 63; XXXX
 *              size_t quoted: 1; */

    size_t next;
    size_t reference_count;
    size_t id;
    std::string name;
    size_t count;  /* used in bafio.c */
    size_t index;  /* used in bafio.c */

    _function_symbol(const size_t arity, bool quoted, const size_t i,size_t c,size_t in):
        header(make_header(arity,quoted)),
        next(size_t(-1)),
        reference_count(0),
        id(i),
        count(c),
        index(in)
    {
    }

    static size_t make_header(const size_t arity, bool quoted)
    {
      assert ((quoted & 1) == quoted);
      return arity << 1 | quoted;
    }

    size_t arity() const    {
      return header>>1;
    }

    bool is_quoted() const
    {
      return header & 1;
    }
};

void at_free_afun(const size_t n);
inline bool AT_isValidAFun(const size_t sym);


} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

