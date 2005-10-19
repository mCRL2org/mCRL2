// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm.h
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_H
#define ATERM_H

/** @file
  * This is a C++ wrapper around the ATerm library.
  */

#include <string>
#include <iostream>
#include <cassert>
#include <limits>
#include <boost/iterator/iterator_facade.hpp>
#include "aterm2.h"
#include "atermpp/aterm_conversion.h"

namespace atermpp
{
  // prototype
  class aterm_list;
  class aterm_appl;
  class aterm_blob;
  class aterm_int;
  class aterm_real;
  class aterm_place_holder;

  //---------------------------------------------------------//
  //                    function_symbol
  //---------------------------------------------------------//
  class function_symbol
  {
    protected:
      AFun m_function;
  
    public:
      function_symbol(const std::string& name, int arity, bool quoted = false)
        : m_function(ATmakeAFun(const_cast<char*>(name.c_str()), arity, quoted ? ATtrue : ATfalse))
      {}
      
      function_symbol(AFun function):
        m_function(function)
      {}
  
      /**
        * Protect the function symbol.
        * Just as aterms which are not on the stack or in registers must be protected through
        * a call to protect, so must function_symbols be protected by calling protect.
        **/
      void protect()
      {
        ATprotectAFun(m_function);
      }
  
      /**
        * Release an function_symbol's protection.
        **/
      void unprotect()
      {
        ATunprotectAFun(m_function);
      }
      
      /**
        * Return the name of the function_symbol.
        **/
      std::string name() const
      {
        return std::string(ATgetName(m_function));
      }
      
      /**
        * Return the arity (number of arguments) of the function symbol (function_symbol).
        **/
      unsigned int arity() const
      {
        return ATgetArity(m_function);
      }
      
      /**
        * Determine if the function symbol (function_symbol) is quoted or not.
        **/
      bool is_quoted() const
      {
        return ATisQuoted(m_function);
      }

      /**
        * Conversion to AFun.
        **/
      operator AFun() const
      { return m_function; }
      
      friend bool operator!=(const function_symbol& x, const function_symbol& y);
  };

  /**
    * Tests equality of function symbols f1 and f2.
    * Function symbols f1 and f2 are considered equal if they have the same name,
    * the same arity and the same value for the quoted attribute.
    **/
  inline
  bool operator==(const function_symbol& x, const function_symbol& y)
  { 
    // return x.name() == y.name() && x.arity() == y.arity() && x.is_quoted() == y.is_quoted();
    return AFun(x) == AFun(y);
  }
  
  /**
    * Returns !(x==y).
    **/
  inline
  bool operator!=(const function_symbol& x, const function_symbol& y)
  { return !(x == y); }
  
  //---------------------------------------------------------//
  //                    aterm
  //---------------------------------------------------------//
  class aterm
  {
    friend class aterm_appl;
  
    protected:
      void* m_term;
  
    public:
      aterm()
        : m_term(0)
      {}
      
      aterm(ATerm term)
        : m_term(term)
      {
      }
  
      aterm(ATermList term)
        : m_term(term)
      {}
  
      aterm(ATermInt term)
        : m_term(term)
      {
      }
  
      aterm(ATermReal term)
        : m_term(term)
      {
      }
  
      aterm(ATermBlob term)
        : m_term(term)
      {
      }
  
      aterm(ATermAppl term)
        : m_term(term)
      {
      }

      aterm(ATermPlaceholder term)
        : m_term(term)
      {
      }
  
      aterm(const std::string& s)
        : m_term(ATmake(const_cast<char*>(s.c_str())))
      {}

      ATerm to_ATerm() const
      { return reinterpret_cast<ATerm>(m_term); }

      const ATerm& term() const
      { return reinterpret_cast<const ATerm&>(m_term); }
  
      ATerm& term()
      { return reinterpret_cast<ATerm&>(m_term); }

      /**
        * Protect the aterm.
        * Protects the aterm from being freed at garbage collection.
        **/
      void protect()
      {
        ATprotect(&term());
      }

      /**
        * Unprotect the aterm.
        * Releases protection of the aterm which has previously been protected through a
        * call to protect.
        **/
      void unprotect()
      {
        ATunprotect(&term());
      }

      /**
        * Return the type of term.
        * Result is one of AT_APPL, AT_INT,
        * AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
        **/
      int type() const
      { return ATgetType(reinterpret_cast<ATerm>(m_term)); }
      
      /**
        * Writes the term to a string.
        **/
      std::string to_string() const
      { return std::string(ATwriteToString(to_ATerm())); }

      /** Retrieve the annotation with the given label.
        *
        **/
      aterm annotation(aterm label) const
      {
        return ATgetAnnotation(to_ATerm(), label.to_ATerm());
      }

      aterm_blob to_aterm_blob() const;
      aterm_real to_aterm_real() const;
      aterm_int  to_aterm_int() const;
      aterm_list to_aterm_list() const;
      aterm_appl to_aterm_appl() const;
      aterm to_aterm() const { return *this; }

      // allow conversion to ATerm
      operator ATerm() const
      { return to_ATerm(); } 
  };
  
  inline
  bool operator!(const aterm& x)
  { return ATisEqual(x.to_ATerm(), ATfalse); }
  
  /**
    * Returns !(x==y).
    **/
  inline
  bool operator!=(const aterm& x, const aterm& y)
  { return !(x == y); }

  inline
  std::ostream& operator<<(std::ostream& out, const aterm& t)
  {
    return out << t.to_string();
  }

  //---------------------------------------------------------//
  //                    aterm_int
  //---------------------------------------------------------//
  class aterm_int: public aterm
  {
    public:
      aterm_int(ATermInt t)
        : aterm(t)
      {}
  
      aterm_int(int value)
        : aterm(ATmakeInt(value))
      {}
      
      /**
        * Get the integer value of the aterm_int.
        **/
      int value() const
      {
        return ATgetInt(reinterpret_cast<ATermInt>(m_term));
      }
  };

  //---------------------------------------------------------//
  //                    aterm_real
  //---------------------------------------------------------//
  class aterm_real: public aterm
  {
    public:
      aterm_real(double value)
        : aterm(ATmakeReal(value))
      {}
      
      aterm_real(ATermReal t)
        : aterm(t)
      {}
  
      /**
        * Get the real value of the aterm_real.
        **/
      double value() const
      {
        return ATgetReal(reinterpret_cast<ATermReal>(m_term));
      }
  };
  
  //---------------------------------------------------------//
  //                     aterm_list_iterator
  //---------------------------------------------------------//
  class aterm_list_iterator: public boost::iterator_facade<
          aterm_list_iterator,
          const aterm,
          boost::forward_traversal_tag,
          const aterm
      >
  {
   public:
      aterm_list_iterator()
        : m_list(ATempty)
      {}
  
      aterm_list_iterator(ATermList l)
        : m_list(l)
      {}

   private:
      friend class boost::iterator_core_access;
  
      const aterm dereference() const
      { return aterm(ATgetFirst(m_list)); }
  
      bool equal(aterm_list_iterator const& other) const
      { return this->m_list == other.m_list; }
  
      void increment()
      { m_list = ATgetNext(m_list); }
  
      ATermList m_list;
  };
  
  //---------------------------------------------------------//
  //                     aterm_list
  //---------------------------------------------------------//
  // defines a (constant) singly linked list interface on top of an ATermList
  
  class aterm_list: public aterm
  {
    public:
      /**
        * The type of object, aterm, stored in the aterm_list.
        **/
      typedef aterm value_type;

      /**
        * Pointer to aterm.
        **/
      typedef aterm* pointer;

      /**
        * Reference to T.
        **/
      typedef aterm& reference;

      /**
        * Const reference to T.
        **/
      typedef const aterm const_reference;

      /**
        * An unsigned integral type.                                      
        **/
      typedef size_t size_type;

      /**
        * A signed integral type.                                         
        **/
      typedef ptrdiff_t difference_type;

      /**
        * Iterator used to iterate through an aterm_list.                      
        **/
      typedef aterm_list_iterator iterator;

      /**
        * Const iterator used to iterate through an aterm_list.                
        **/
      typedef aterm_list_iterator const_iterator;

      /**
        * Returns an iterator pointing to the end of the aterm_list.     
        **/
      const_iterator begin() const
      { return const_iterator(void2list(m_term)); } 

      /**
        * Returns a const_iterator pointing to the beginning of the aterm_list.
        **/
      const_iterator end() const
      { return const_iterator(ATmakeList0()); }
  
      /**
        * Returns the size of the aterm_list.
        **/
      size_type size()
      { return ATgetLength(void2list(m_term)); }     

      /**
        * Returns the largest possible size of the aterm_list.
        **/
      size_type max_size()
      { return GET_LENGTH((std::numeric_limits<unsigned long>::max)()); }

      /**
        * true if the list's size is 0.
        **/
      bool empty() const
      { return ATisEmpty(void2list(m_term)) == ATtrue; }

      /**
        * Creates an empty aterm_list.
        **/
      aterm_list()
        : aterm(ATmakeList0())
      {}

      /**
        * Creates an aterm_list with n elements, each of which is a copy of T().
        **/

      /**
        * Creates an aterm_list with n copies of t.
        **/

      /**
        * The copy constructor.
        **/
      aterm_list(ATermList l)
        : aterm(l)
      {}

      /**
        * Creates an aterm_list with a copy of a range.
        **/
      template <class Iter>
      aterm_list(Iter first, Iter last)
      {
        m_term = ATmakeList0();
        while (first != last)
          m_term = ATinsert(void2list(m_term), *(--last));
      }

      /**
        * Allow construction from an aterm.
        **/
      aterm_list(aterm t)
        : aterm(void2list(term2void(t.to_ATerm())))
      {}

      /**
        * The destructor.
        **/

      /**
        * The assignment operator                                                                                                                                                        
        **/

      /**
        * Swaps the contents of two aterm_lists.
        **/
      void swap(aterm_list& l)
      {
        std::swap(m_term, l.m_term);
      }

      /**
        * Returns the first element.
        **/
      aterm front() const
      { return aterm(ATgetFirst(void2list(m_term))); }

      /**
        * pos must be a valid iterator in *this. The return value is an iterator prev such that ++prev == pos. Complexity: linear in the number of iterators in the range [begin(), pos).
        **/
      const_iterator previous(const_iterator pos) const
      {
        const_iterator prev = end();
        for (const_iterator i = begin(); i != end(); ++i)
        {
          if (i == pos)
            return prev;
          prev = i;
        }
        return end();
      }

      /**
        * Erases all of the elements.
        **/
      void clear()
      { m_term = list2void(ATmakeList0()); }

      /**
        * Conversion to ATermList.
        **/
      operator ATermList() const
      { return void2list(m_term); }
  
      /**
        * Returns the ATermList that is contained by the aterm_list.
        **/
      ATermList to_ATermList() const
      { return void2list(m_term); }
  };

  /**
    * Lexicographical comparison. This is a global function, not a member function.
    **/

  /**
    * Inserts a new element at the beginning.
    **/
  aterm_list push_front(aterm_list l, aterm elem)
  { return aterm_list(ATinsert(l.to_ATermList(), elem.to_ATerm())); }

  /**
    * Removes the first element.
    **/
  aterm_list pop_front(aterm_list l)
  { return aterm_list(ATgetNext(l.to_ATermList())); }

  /**
    * Returns the next part (the tail) of list l.
    **/
  aterm_list get_next(aterm_list l)
  {
    return ATgetNext(l.to_ATermList());
  }

  /**
    * Return the sublist from start to the end of list l.
    **/
  inline aterm_list tail(aterm_list l, int start)
  { return ATgetTail(l.to_ATermList(), start); }
  
  /**
    * Replace the tail of list l from position start with new_tail.
    **/
  inline aterm_list replace_tail(aterm_list l, aterm_list new_tail, int start)
  { return ATreplaceTail(l.to_ATermList(), new_tail.to_ATermList(), start); }
  
  /**
    * Return all but the last element of list l.
    **/
  inline aterm_list prefix(aterm_list l)
  { return ATgetPrefix(l.to_ATermList()); }
  
  /**
    * Return the last element of list l.
    **/
  inline aterm get_last(aterm_list l)
  { return ATgetLast(l.to_ATermList()); }
  
  /**
    * Get a portion (slice) of list l.
    * Return the portion of list that lies between start and end.  Thus start is
    * included, end is not.
    **/
  inline aterm_list slice(aterm_list l, int start, int end)
  { return ATgetSlice(l.to_ATermList(), start, end); }
  
  /**
    * Return list l with el inserted.
    * The behaviour of insert is of constant complexity. That is, the behaviour of
    * insert does not degrade as the length of list increases.
    **/
  inline
  aterm_list insert(aterm_list l, aterm el)
  { return ATinsert(l.to_ATermList(), el.to_ATerm()); }
  
  /**
    * Return list l with el inserted at position index.
    **/
  inline
  aterm_list insert_at(aterm_list l, aterm el, int index)
  { return ATinsertAt(l.to_ATermList(), el.to_ATerm(), index); }
  
  /**
    * Return list l with el appended to it.
    * Note that append is implemented in terms of insert by making a new list
    * with el as the first element and then inserting all elements from list. As such, the complexity
    * of append is linear in the number of elements in list.
    *    When append is needed inside a loop that traverses a list behaviour
    * of the loop will demonstrate quadratic complexity.
    **/
  inline
  aterm_list append(aterm_list l, aterm el)
  { return ATappend(l.to_ATermList(), el.to_ATerm()); }
  
  /**
    * Return the concatenation of the list l and m.
    **/
  inline
  aterm_list concat(aterm_list l, aterm_list m)
  { return ATconcat(l.to_ATermList(), m.to_ATermList()); }
  
  /**
    * Return the index of an aterm in a list.
    * Return the index where el can be found in list. Start looking at position start.
    * Returns -1 if el is not in list.
    **/
  inline
  int index_of(aterm_list l, aterm el, int start)
  { return ATindexOf(l.to_ATermList(), el.to_ATerm(), start); }
  
  /**
    * Return the index of an aterm in a list (reverse).
    * Search backwards for el in list. Start searching at start. Return the index of
    * the first occurrence of l encountered, or -1 when el is not present before start.
    **/
  inline
  int last_index_of(aterm_list l, aterm elem, int start)
  { return ATlastIndexOf(l.to_ATermList(), elem.to_ATerm(), start); }
  
  /**
    * Return a specific element of a list.
    * Return the element at position index in list. Returns `aterm()` when index is not in
    * list.
    **/
  inline
  aterm element_at(aterm_list l, int index)
  { return ATelementAt(l.to_ATermList(), index); }
  
  /**
    * Return list with one occurrence of el removed.
    **/
  inline
  aterm_list remove_element(aterm_list l, aterm elem)
  { return ATremoveElement(l.to_ATermList(), elem.to_ATerm()); }
  
  /**
    * Return list l with all occurrences of el removed.
    **/
  inline
  aterm_list remove_all(aterm_list l, aterm el)
  {
    return ATremoveAll(l.to_ATermList(), el.to_ATerm());
  }

  /**
    * Return list l with the element at index removed.
    **/
  inline
  aterm_list remove_element_at(aterm_list l, int index)
  { return ATremoveElementAt(l.to_ATermList(), index); }
  
  /**
    * Return list l with the element at index replaced by el.
    **/
  inline
  aterm_list replace(aterm_list l, aterm elem, int index)
  { return ATreplace(l.to_ATermList(), elem.to_ATerm(), index); }
  
  /**
    * Return list l with its elements in reversed order.
    **/
  inline
  aterm_list reverse(aterm_list l)
  { return ATreverse(l.to_ATermList()); }


//  /**
//    * The filter predicate type.
//    **/
//  typedef ATbool *filter_predicate(ATerm);
//
//  /**
//    * Filter entries from a list using a predicate.
//    * This function can be used to filter entries that satisfy a given predicate from a
//    * list. Each item in list is judged through a call to predicate. If predicate returns true the
//    * entry is added to a list, otherwise it is skipped. The function returns the list containing exactly
//    * those items that satisfy predicate.
//    **/
//  aterm filter(aterm_list l, filter_predicate predicate)
//  {
//    return ATfilter(l.to_ATermList(), predicate);
//  }

  //---------------------------------------------------------//
  //                     aterm_appl
  //---------------------------------------------------------//
  class aterm_appl: public aterm
  {
    public:
      aterm_appl()
      {}
  
      aterm_appl(ATermAppl a)
        : aterm(a)
      {}

      aterm_appl(function_symbol sym, aterm_list args)
        : aterm(ATmakeApplList(sym, args))
      {}

      /**
        * Allow construction from an aterm.
        **/
      aterm_appl(aterm t)
        : aterm(void2appl(term2void(t.to_ATerm())))
      {}

#include "atermpp/aterm_appl_constructor.h" // additional constructors generated by preprocessor

      /**
        * Get the function symbol (function_symbol) of the application.
        **/
      function_symbol function() const
      {
        return function_symbol(ATgetAFun(void2appl(m_term)));
      }
  
      bool is_quoted() const
      {
        return function().is_quoted();
      }
  
      /**
        * Get the i-th argument of the application.
        **/
      aterm argument(unsigned int i) const
      {
        return aterm(ATgetArgument(void2appl(m_term), i));
      }
  
      /**
        * Get the list of arguments of the application.
        **/
      aterm_list argument_list() const
      {
        return aterm_list(ATgetArguments(void2appl(m_term)));
      } 

      /**
        * Returns the ATermAppl that is contained by the aterm_appl.
        **/
      ATermAppl to_ATermAppl() const
      { return void2appl(m_term); }
  };
  
  //---------------------------------------------------------//
  //                     aterm_place_holder
  //---------------------------------------------------------//
  class aterm_place_holder: public aterm
  {
   public:
      aterm_place_holder(ATermPlaceholder t)
        : aterm(t)
      {}
  
      /**
        * Build an aterm_place_holder of a specific type. The type is taken from the type
        * parameter.
        **/
      aterm_place_holder(aterm type)
        : aterm(ATmakePlaceholder(type.to_ATerm()))
      {}
      
      /**
        * Get the type of the aterm_place_holder.
        **/
      aterm type()
      {
        return aterm(ATgetPlaceholder(void2place_holder(m_term)));
      }
  };

  //---------------------------------------------------------//
  //                     aterm_blob
  //---------------------------------------------------------//
  class aterm_blob: public aterm
  {
   public:
      aterm_blob(ATermBlob t)
        : aterm(t)
      {}

      /**
        * Build a Binary Large OBject given size (in bytes) and data.
        * This function can be used to create an aterm of type blob, holding the data
        * pointed to by data. No copy of this data area is made, so the user should allocate this himself.
        *    Note:  due to the internal representation of a blob, size cannot exceed 224 in the current
        * implementation. This limits the size of the data area to 16 Mb.
        **/
      aterm_blob(unsigned int size, void* data)
        : aterm(ATmakeBlob(size, data))
      {}

      /**
        * Get the data section of the blob.
        **/
      void* data()
      {
        return ATgetBlobData(void2blob(m_term));
      }

      /**
        * Get the size (in bytes) of the blob.
        **/
      unsigned int size() const
      {
        return ATgetBlobSize(void2blob(m_term));
      }
  };
  
  //---------------------------------------------------------//
  //                     dictionary
  //---------------------------------------------------------//
  class dictionary: public aterm
  {
   public:
      /**
        * Create a new dictionary.
        **/
      dictionary()
        : aterm(ATdictCreate())
      {}

      /**
        * Get the value belonging to a given key in the dictionary.
        **/
      aterm get(aterm key)
      {
        return ATdictGet(to_ATerm(), key.to_ATerm());
      }
      
      /**
        * Add / update a (key, value)-pair in a dictionary.
        * If key does not already exist in the dictionary, this function adds the (key,
        * value)-pair to the dictionary. Otherwise, it updates the value to value.
        **/
      void put(aterm key, aterm value)
      {
        m_term = ATdictPut(to_ATerm(), key.to_ATerm(), value.to_ATerm());
      }
      
      /**
        * Remove the (key, value)-pair from the dictionary.
        * This function can be used to remove an entry from the dictionary.
        **/
      void dict_remove(aterm key)
      {
        m_term = ATdictRemove(to_ATerm(), key.to_ATerm());
      }
  };

  //---------------------------------------------------------//
  //                     table
  //---------------------------------------------------------//
  class table
  {
   protected:
      ATermTable m_table;

   public:
      /**
        * Create an table.
        * This function creates an table given an initial size and a maximum load
        * percentage. Whenever this percentage is exceeded (which is detected when a new entry is added
        * using table_put), the table is automatically expanded and all existing entries are rehashed into
        * the new table. If you know in advance approximately how many items will be in the table, you
        * may set it up in such a way that no resizing (and thus no rehashing) is necessary. For example,
        * if you expect about 1000 items in the table, you can create it with its initial size set to 1333 and
        * a maximum load percentage of 75%. You are not required to do this, it merely saves a runtime
        * expansion and rehashing of the table which increases efficiency.
        **/
      table(unsigned int initial_size, unsigned int max_load_pct)
        : m_table(ATtableCreate(initial_size, max_load_pct))
      {}
      
      /**
        * Destroy an table.
        * Contrary to aterm_dictionaries, aterm_tables are themselves not aterms. This
        * means they are not freed by the garbage collector when they are no longer referred to. Therefore,
        * when the table is no longer needed, the user should release the resources allocated by the table
        * by calling table_destroy. All references the table has to aterms will then also be removed, so
        * that those may be freed by the garbage collector (if no other references to them exist of course).
        **/
      ~table()
      {
        ATtableDestroy(m_table);
      }
      
      /**
        * Reset an table.
        * This function resets an ermtable, without freeing the memory it occupies. Its
        * effect is the same as the subsequent execution of a destroy and a create of a table, but as no
        * memory is released and obtained from the C memeory management system this function is gen-
        * erally cheaper. but if subsequent tables differ very much in size, the use of table_destroy and
        * table_create may be prefered, because in such a way the sizes of the table adapt automatically
        * to the requirements of the application.
        **/
      void reset()
      {
        ATtableReset(m_table);
      }
      
      /**
        * Add / update a (key, value)-pair in a table.
        * If key does not already exist in the table, this function adds the (key, value)-pair
        * to the table. Otherwise, it updates the value to value.
        **/
      void put(aterm key, aterm value)
      {
        ATtablePut(m_table, key.to_ATerm(), value.to_ATerm());
      }
      
      /**
        * Get the value belonging to a given key in a table.
        **/
      aterm get(aterm key)
      {
        return ATtableGet(m_table, key.to_ATerm());
      }
      
      /**
        * Remove the (key, value)-pair from table.
        **/
      void remove(aterm key)
      {
        ATtableRemove(m_table, key.to_ATerm());
      }
      
      /**
        * Get an aterm_list of all the keys in a table.
        * This function can be useful if you need to iterate over all elements in a table. It
        * returns an aterm_list containing all the keys in the table. The corresponding values of each key
        * you are interested in can then be retrieved through respective calls to table_get.
        **/
      aterm_list table_keys()
      {
        return aterm_list(ATtableKeys(m_table));
      }
  };

  //---------------------------------------------------------//
  //                     indexed_set
  //---------------------------------------------------------//
  class indexed_set
  {
   protected:
      ATermIndexedSet m_set;

   public:
      /**
        * Create a new indexed_set.
        **/
      indexed_set(unsigned int initial_size, unsigned int max_load_pct)
        : m_set(ATindexedSetCreate(initial_size, max_load_pct))
      {}
      
      /**
        * This function releases all memory occupied by the indexed_set..
        **/
      ~indexed_set()
      {
        ATindexedSetDestroy(m_set);
      }
      
      /**
        * Clear the hash table in the set.
        * This function clears the hash table in the set, but does not release the memory.
        * Using indexed_set_reset instead of indexed_set_destroy is preferable when indexed sets of
        * approximately the same size are being used.
        **/
      void reset()
      {
        ATindexedSetReset(m_set);
      }
      
      /**
        * Enter elem into the set.
        * This functions enters elem into the set. If elem was already in the set the previously
        * assigned index of elem is returned, and new is set to false. If elem did not yet occur in set a
        * new number is assigned, and new is set to true.  This number can either be the number of an
        * element that has been removed, or, if such a number is not available, the lowest non used number
        * is assigned to elem and returned. The lowest number that is used is 0.
        **/
      std::pair<long, bool> put(aterm elem)
      {
        ATbool b;
        long l = ATindexedSetPut(m_set, elem.to_ATerm(), &b);
        return std::make_pair(l, b == ATtrue);
      }
      
      /**
        * Find the index of elem in set.
        * The index assigned to elem is returned, except when elem is not in the set, in
        * which case the return value is a negative number.
        **/
      long index(aterm elem)
      {
        return ATindexedSetGetIndex(m_set, elem.to_ATerm());
      }
      
      /**
        * Retrieve the element at index in set.
        * This function must be invoked with a valid index and it returns the elem assigned
        * to this index. If it is invoked with an invalid index, effects are not predictable.
        **/
      aterm get(long index)
      {
        return ATindexedSetGetElem(m_set, index);
      }
      
      /**
        * Remove elem from set.
        * The elem is removed from the indexed set, and if a number was assigned to elem,
        * it is freed to be reassigned to an element, that may be put into the set at some later instance.
        **/
      void remove(aterm elem)
      {
        ATindexedSetRemove(m_set, elem.to_ATerm());
      }
      
      /**
        * Retrieve all elements in set.
        * A list with all valid elements stored in the indexed set is returned.  The list is
        * ordered from element with index 0 onwards.
        **/
      aterm_list elements()
      {
        return aterm_list(ATindexedSetElements(m_set));
      }
  };

  //---------------------------------------------------------//
  //                     implementations
  //---------------------------------------------------------//
  inline
  aterm_list aterm::to_aterm_list() const
  {
    assert(type() == AT_LIST);
    return aterm_list(void2list(m_term));
  }
  
  inline
  aterm_appl aterm::to_aterm_appl() const
  {
    assert(type() == AT_APPL);
    return aterm_appl(void2appl(m_term));
  }
  
  inline
  aterm_int aterm::to_aterm_int() const
  {
    assert(type() == AT_INT);
    return aterm_int(void2int(m_term));
  }
  
  inline
  aterm_real aterm::to_aterm_real() const
  {
    assert(type() == AT_REAL);
    return aterm_real(void2real(m_term));
  }
  
  inline
  aterm_blob aterm::to_aterm_blob() const
  {
    assert(type() == AT_BLOB);
    return aterm_blob(void2blob(m_term));
  }
  
  /**
    * Read an aterm from string.
    * This function parses a character string into an aterm.
    **/
  inline
  aterm read_from_string(const std::string& s)
  {
    return ATreadFromString(s.c_str());
  }
  
  /**
    * Read a aterm from a string in baf format.
    * This function decodes a baf character string into an aterm.
    **/
  inline
  aterm read_from_binary_string(const std::string& s, unsigned int size)
  {
    return ATreadFromBinaryString(const_cast<char*>(s.c_str()), size);
  }
  
  /**
    * Read a aterm from a string in taf format.
    * This function decodes a taf character string into an aterm.
    **/
  inline
  aterm read_from_shared_string(const std::string& s, unsigned int size)
  {
    return ATreadFromSharedString(const_cast<char*>(s.c_str()), size);
  }
  
  /**
    * Read an aterm from named binary or text file.
    * This function reads an aterm file filename. A test is performed to see if the file
    * is in baf, taf, or plain text. "-" is standard input's filename.
    **/
  inline
  aterm read_from_named_file(const std::string& name)
  {
    return ATreadFromNamedFile(name.c_str());
  }

  /**
    * Writes term t to file named filename in textual format.
    * This function writes aterm t in textual representation to file filename. "-" is
    * standard output's filename.
    **/
  inline
  bool write_to_named_text_file(aterm t, const std::string& filename)
  {
    return ATwriteToNamedTextFile(t.to_ATerm(), filename.c_str()) == ATtrue;
  }

  /**
    * Writes term t to file named filename in Binary aterm Format (baf).
    **/
  inline
  bool write_to_named_binary_file(aterm t, const std::string& filename)
  {
    return ATwriteToNamedBinaryFile(t.to_ATerm(), filename.c_str()) == ATtrue;
  }

  /**
    * Annotate a term with a labeled annotation.
    * Creates a version of t that is annotated with annotation and labeled by
    * label.
    **/
  inline
  aterm set_annotation(aterm t, aterm label, aterm annotation)
  {
    return ATsetAnnotation(t.to_ATerm(), label.to_ATerm(), annotation.to_ATerm());
  }

  /**
    * Retrieves annotation of t with label label.
    * This function can be used to retrieve a specific annotation of a term. If t has
    * no annotations, or no annotation labeled with label exists, `aterm()` is returned. Otherwise the
    * annotation is returned.
    **/
  inline
  aterm get_annotation(aterm t, aterm label)
  {
    return ATgetAnnotation(t.to_ATerm(), label.to_ATerm());
  }

  /**
    * Remove a specific annotation from a term.
    * This function returns a version of t which has its annotation with label label
    * removed. If t has no annotations, or no annotation labeled with label exists, t itself is returned.
    **/
  inline
  aterm remove_annotation(aterm t, aterm label)
  {
    return ATremoveAnnotation(t.to_ATerm(), label.to_ATerm());
  }

  /**
    * Initialise the ATerm++ Library. This call has only effect if the symbol 
    * ATERM_USER_INITIALIZATION is defined. See the section about initialization.
    **/
  void init(int argc, char* argv[], aterm& bottom_of_stack)
  {
#ifdef ATERM_USER_INITIALIZATION
    ATinit(argc, argv, &bottom_of_stack.to_ATerm());
#endif // ATERM_USER_INITIALIZATION
  }

  /**
    * Set the i-th argument of an application to term.
    * This function returns a copy of appl with argument i replaced by term.
    **/
  aterm_appl set_appl_argument(aterm_appl appl, unsigned int i, aterm term)
  {
    return ATsetArgument(appl.to_ATermAppl(), term.to_ATerm(), i);
  }   

  /**
    * Returns a quoted string.
    **/
  inline
  aterm_appl quoted_string(std::string s)
  {
    // TODO: this should be done more efficiently!
    return aterm_appl("\"" + s + "\"");
  }
  
  /**
    * Tests equality of aterms t1 and t2.
    * As aterms are created using maximal sharing (see Section 2.1), testing equality
    * is performed in constant time by comparing the addresses of t1 and t2.  Note however that
    * operator== only returns true when t1 and t2 are completely equal, inclusive any annotations
    * they might have!
    **/
  inline
  bool operator==(const aterm& x, const aterm& y)
  {
/*if ( (x.to_string() == y.to_string()) != (ATisEqual(x.to_ATerm(), y.to_ATerm()) == ATtrue) )
{
  std::cerr << "Error in bool operator==(const aterm& x, const aterm& y)\n"
            << "x == " << x.to_string() << "\n"
            << "y == " << y.to_string() << "\n"
            << "&x == " << x.to_ATerm() << "\n"
            << "&y == " << y.to_ATerm() << "\n"           
            << "x.f() == y.f() == " << (x.to_aterm_appl().function() == y.to_aterm_appl().function()) << "\n"
            << "x.q() == y.q() == " << (x.to_aterm_appl().is_quoted() == y.to_aterm_appl().is_quoted()) << "\n"
            ;
            // << "x.a() == y.a() == " << (x.to_aterm_appl().argument_list() == y.to_aterm_appl().argument_list()) << "\n"
            aterm_list xx = x.to_aterm_appl().argument_list();
            aterm_list yy = y.to_aterm_appl().argument_list();
            for (aterm_list::iterator i = xx.begin(), j = yy.begin(); i != xx.end(), j != yy.end(); ++i, ++j)
            {
              std::cerr << "comparing arguments..." << i->to_string() << " " << j->to_string() << "\n";
              std::cerr << (*i == *j ? "true" : "false") << "\n";
              std::cerr << "done" << "\n";
            }
}*/
    return ATisEqual(x.to_ATerm(), y.to_ATerm()) == ATtrue;
  }

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // ATERM_H
