#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#include <set>
#include <string.h>
#include <sstream>


#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace atermpp
{

/*{{{  globals */

namespace detail
{
  static _function_symbol *const END_OF_LIST(NULL);
  // The hashtables are not vectors to prevent them from being
  // destroyed prematurely.
  static size_t function_symbol_table_size=0;
  static size_t function_symbol_table_mask;
  static _function_symbol** function_symbol_hashtable; 

  static const size_t INITIAL_FUNCTION_HASH_TABLE_SIZE = 1<<FUNCTION_SYMBOL_BLOCK_CLASS; // Must be a multiple of 2.
  static const size_t INITIAL_FUNCTION_INDEX_TABLE_SIZE=128;


  // The function_lookup_table is not a vector to prevent it from being destroyed prematurely.
  static size_t function_symbol_index_table_size=0;
  size_t function_symbol_index_table_number_of_elements=0;
  static _function_symbol* function_symbol_free_list=END_OF_LIST;
  constant_function_symbols function_adm;
  
  detail::_function_symbol** function_symbol_index_table;
  
  static void create_new_function_symbol_block()
  {
    if (function_symbol_index_table_number_of_elements==function_symbol_index_table_size)
    {
      // There is no place in the function_symbol_index_table. Double it.
      function_symbol_index_table=reinterpret_cast<_function_symbol**>(realloc(function_symbol_index_table,2*function_symbol_index_table_size*sizeof(_function_symbol*)));
      if (function_symbol_index_table==NULL)
      {
        throw std::runtime_error("Out of memory. Cannot extend function symbol index table.");
      }
      function_symbol_index_table_size=function_symbol_index_table_size*2;
    }
  
    assert(function_symbol_index_table_number_of_elements<function_symbol_index_table_size);
    function_symbol_index_table[function_symbol_index_table_number_of_elements]=
             reinterpret_cast<_function_symbol*>(
                        malloc(FUNCTION_SYMBOL_BLOCK_SIZE*sizeof(_function_symbol)));
    if (function_symbol_index_table[function_symbol_index_table_number_of_elements]==NULL)
    {
      throw std::runtime_error("Out of memory. Fail to resize function_lookup_table.");
    }
    for(size_t i=FUNCTION_SYMBOL_BLOCK_SIZE; i>0; )
    {
      --i;
      function_symbol_index_table[function_symbol_index_table_number_of_elements][i].arity=0;
      new (&function_symbol_index_table[function_symbol_index_table_number_of_elements][i].name) std::string(); // Placement new
      function_symbol_index_table[function_symbol_index_table_number_of_elements][i].reference_count=0;
      function_symbol_index_table[function_symbol_index_table_number_of_elements][i].next=function_symbol_free_list;
      function_symbol_free_list=&function_symbol_index_table[function_symbol_index_table_number_of_elements][i];
      function_symbol_index_table[function_symbol_index_table_number_of_elements][i].number=i+FUNCTION_SYMBOL_BLOCK_SIZE*function_symbol_index_table_number_of_elements;

    }
    function_symbol_index_table_number_of_elements++;
    assert(function_symbol_free_list!=END_OF_LIST);
  }


  bool check_that_the_function_symbol_points_to_memory_containing_a_function(const detail::_function_symbol* f)
  {
    for (size_t i=0; i<function_symbol_index_table_number_of_elements; ++i)
    {
      if (function_symbol_index_table[i]<=f && f<(function_symbol_index_table[i])+FUNCTION_SYMBOL_BLOCK_SIZE)
      { 
        return true;
      }
    }
    return false;
  }

  void initialise_administration()
  {
    // Explict initialisation on first use. This first
    // use is when a function symbol is created for the first time,
    // which may be due to the initialisation of a global variable in
    // a .cpp file, or due to the initialisation of a pre-main initialisation
    // of a static variable, which some compilers do.

    if (function_symbol_table_size==0)
    { 
      function_symbol_table_size=INITIAL_FUNCTION_HASH_TABLE_SIZE;
      function_symbol_table_mask=function_symbol_table_size-1;
      
      function_symbol_hashtable=reinterpret_cast<_function_symbol**>(malloc(function_symbol_table_size*sizeof(_function_symbol*)));
      if (function_symbol_hashtable==NULL)
      {
        throw std::runtime_error("Out of memory. Cannot create function symbol hashtable.");
      }
      for(size_t i=0; i<function_symbol_table_size; ++i)
      { 
        function_symbol_hashtable[i]=END_OF_LIST;
      }

      function_symbol_index_table_size=INITIAL_FUNCTION_INDEX_TABLE_SIZE;
      function_symbol_index_table=reinterpret_cast<_function_symbol**>(calloc(function_symbol_index_table_size,sizeof(_function_symbol*)));
      if (function_symbol_index_table==NULL)
      {
        throw std::runtime_error("Out of memory. Cannot create function symbol index table.");
      }
      function_symbol_index_table_number_of_elements=0;
      create_new_function_symbol_block(); // This guarantees that function_symbol_index_table[0][0] exists;

      function_adm.initialise_function_symbols();

      initialise_aterm_administration();
    }
  }


  static HashNumber calculate_hash_of_function_symbol(const std::string &name, const size_t arity);
  
  static void resize_function_symbol_hashtable()
  {
    function_symbol_table_size  <<=1;  // Double the size.
  
    _function_symbol** const old_function_symbol_hashtable=function_symbol_hashtable;
    function_symbol_hashtable=reinterpret_cast<_function_symbol**>(realloc(function_symbol_hashtable,function_symbol_table_size*sizeof(_function_symbol*)));
    if (function_symbol_hashtable==NULL)
    {
      // resizing the hashtable failed; continue with the old hashtable.
      mCRL2log(mcrl2::log::warning) << "could not resize function symbol hashtable to class " << function_symbol_table_size << "."; 
      function_symbol_table_size  >>=1; // Restore the size by dividing it by 2.
      function_symbol_hashtable=old_function_symbol_hashtable;
      return;
    }
    function_symbol_table_mask  = function_symbol_table_size-1;
  
    for(size_t i=0; i<function_symbol_table_size; ++i)
    {
      function_symbol_hashtable[i]=END_OF_LIST;
    }
  
    for (size_t i=0; i<function_symbol_index_table_number_of_elements; i++)
    {
      for (size_t j=0; j<FUNCTION_SYMBOL_BLOCK_SIZE; j++)
      {
        _function_symbol* entry = &function_symbol_index_table[i][j];
        if (entry->reference_count>0) 
        {
          HashNumber hnr = calculate_hash_of_function_symbol(entry->name, entry->arity );
          hnr &= function_symbol_table_mask;
          entry->next = function_symbol_hashtable[hnr];
          function_symbol_hashtable[hnr] = entry;
        }
      }
    }
  }
  
  static const size_t MAGIC_PRIME = 7;
  
  static HashNumber calculate_hash_of_function_symbol(const std::string &name, const size_t arity)
  {
    HashNumber hnr = arity*3;
  
    for (std::string::const_iterator i=name.begin(); i!=name.end(); i++)
    {
      hnr = 251 * hnr + *i;
    }
  
    return hnr*MAGIC_PRIME;
  }
} // namespace detail

function_symbol::function_symbol()
{
  if (detail::function_symbol_table_size==0)
  {
    detail::initialise_administration();
  }
  m_function_symbol=&detail::function_symbol_index_table[0][0];
  increase_reference_count<false>();
}

function_symbol::function_symbol(const std::string &name, const size_t arity_)
{
  if (detail::function_symbol_table_size==0)
  {
    detail::initialise_administration();
  }
  if (detail::function_symbol_index_table_number_of_elements<<(FUNCTION_SYMBOL_BLOCK_CLASS+1) > detail::function_symbol_table_size)
  {
    detail::resize_function_symbol_hashtable();
  }

  const HashNumber hnr = detail::calculate_hash_of_function_symbol(name, arity_) & detail::function_symbol_table_mask;
  /* Find symbol in table */
  detail::_function_symbol* cur = detail::function_symbol_hashtable[hnr];
  while (cur!=detail::END_OF_LIST)
  { 
    if (cur->arity==arity_ && cur->name==name)
    {
      // The function_symbol was already present. Return it.
      m_function_symbol=cur;
      increase_reference_count<true>();
      return;
    }
    cur = cur->next;
  }

  // The function symbol does not exist. Make it.
  if (detail::function_symbol_free_list==detail::END_OF_LIST) // There is a free place in function_lookup_table() to store an function_symbol.
  {
    detail::create_new_function_symbol_block();
  }

  cur=detail::function_symbol_free_list;
  detail::function_symbol_free_list = cur->next;
  assert(cur->reference_count==0);
  cur->name=name;
  cur->arity=arity_;
  cur->next=detail::function_symbol_hashtable[hnr];
  
  detail::function_symbol_hashtable[hnr] = cur;
  m_function_symbol=cur;
  increase_reference_count<false>();
}


void function_symbol::free_function_symbol() const
{
  assert(m_function_symbol->reference_count==0);
  /* Calculate hashnumber */
  const HashNumber hnr = detail::calculate_hash_of_function_symbol(m_function_symbol->name, m_function_symbol->arity) & detail::function_symbol_table_mask;

  /* Update hashtable */
  if (detail::function_symbol_hashtable[hnr] == m_function_symbol)
  {
    detail::function_symbol_hashtable[hnr] = m_function_symbol->next;
  }
  else
  {
    detail::_function_symbol* cur;
    detail::_function_symbol* prev = detail::function_symbol_hashtable[hnr];
    for (cur = prev->next; cur != m_function_symbol; prev = cur, cur = cur->next)
    {
      assert(cur != detail::END_OF_LIST);
    }
    prev->next = cur->next;
  }

  const_cast<detail::_function_symbol*>(m_function_symbol)->next = detail::function_symbol_free_list;
  detail::function_symbol_free_list = const_cast<detail::_function_symbol*>(m_function_symbol);
}

} // namespace atermpp

