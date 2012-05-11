/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/aterm/aterm.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace aterm_deprecated
{

char memory_id[] = "$Id$";

ATermAppl ATmakeAppl_varargs(const AFun &sym, ...)
{
  _ATerm* cur;
  size_t arity = ATgetArity(sym);
  bool found;
  // header_type header;
  va_list args;
  _ATerm* arg;
  // ATerm* buffer;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,_ATerm*,arity);

  HashNumber hnr = START(sym.number());
  va_start(args, sym);
  for (size_t i=0; i<arity; i++)
  {
    arg = va_arg(args, _ATerm *);
    CHECK_TERM(&*arg);
    hnr = COMBINE(hnr, HN(&*arg));
    buffer[i] = arg;
  }
  va_end(args);
  hnr = FINISH(hnr);

  cur = (_ATermAppl *)&*ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i]!=buffer[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = (_ATermAppl *)cur->next;
  }

  if (!cur)
  {
    cur = (_ATermAppl *) &*AT_allocate(TERM_SIZE_APPL(arity));
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    // AFun::increase_reference_count<true>(sym.number());
    for (size_t i=0; i<arity; i++)
    {
      buffer[i]->reference_count++;
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = buffer[i];
    }
    cur->next = &*ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */


ATermAppl ATmakeApplArray(const AFun &sym, const ATerm args[])
{
  _ATerm* cur;
  size_t arity = ATgetArity(sym);
  bool found;

  HashNumber hnr = START(sym.number());
  for (size_t i=0; i<arity; i++)
  {
    CHECK_TERM(&*args[i]);
    hnr = COMBINE(hnr, (HashNumber)&*args[i]);
  }
  hnr = FINISH(hnr);

  cur = ATerm::hashtable[hnr & table_mask];
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<_ATermAppl*>(cur)->arg[i] != &*args[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_APPL(arity));

    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = sym;
    // AFun::increase_reference_count<true>(sym.number());
    for (size_t i=0; i<arity; i++)
    {
      args[i]->reference_count++;
      reinterpret_cast<_ATermAppl*>(cur)->arg[i] = &*args[i];
    }
    cur->next = ATerm::hashtable[hnr];
    ATerm::hashtable[hnr] = cur;
  }

  return reinterpret_cast<_ATermAppl*>(cur);
}

} // namespace aterm_deprecated

