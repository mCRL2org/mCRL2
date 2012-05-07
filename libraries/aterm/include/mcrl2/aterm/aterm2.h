/**
  * aterm2.h: Definition of the level 2 interface
  * of the ATerm library.
  */

#ifndef ATERM2_H
#define ATERM2_H

/**
  * The level 2 interface is a strict superset
  * of the level 1 interface.
  */

#include <unistd.h>
#include <stack>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "mcrl2/aterm/aterm1.h"
#include "mcrl2/aterm/afun.h"
#include "mcrl2/aterm/aterm_list_iterator.h"
#include "mcrl2/aterm/aterm_appl_iterator.h"

/* The largest size_t is used as an indicator that an element does not exist.
   This is used as a replacement of a negative number as an indicator of non
   existence */


namespace aterm
{

class _ATermInt:public _ATerm
{
  public:
    union
    {
      int value;
      MachineWord reserved; /* Only use lower 32 bits as int. The value is used ambiguously
                               as integer and as MachineWord. For all cases using bitwise
                               operations, the MachineWord version must be used,
                               as failing to do so may lead to improper initialisation
                               of the last 32 bits during casting. */
    };
};

static const size_t TERM_SIZE_INT = sizeof(_ATermInt)/sizeof(size_t);

class ATermInt:public ATerm
{
  public:

    /// \brief Constructor.
    ATermInt()
    {}

    
    ATermInt(_ATermInt *t):ATerm(reinterpret_cast<_ATerm*>(t))
    {
    }

    explicit ATermInt(const ATerm &t):ATerm(t) 
    {
    }
    
    /// \brief Constructor.
    /// \param value An integer value.
    ATermInt(int value);

    /// \brief Assignment operator.
    /// \param t A term representing an integer.
    ATermInt &operator=(const ATermInt &t)
    {
      copy_term(t.m_term);
      return *this;
    }

    _ATermInt & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term==NULL || m_term->reference_count>0);
      return *reinterpret_cast<_ATermInt*>(m_term); 
    }

    _ATermInt *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count>0);
      return reinterpret_cast<_ATermInt*>(m_term);
    }

    /// \brief Get the integer value of the aterm_int.
    /// \return The value of the term.
    int value() const
    {
      return reinterpret_cast<_ATermInt*>(&*m_term)->value;
    }
};

/* template <class Term>
struct default_aterm_converter
{
  template <typename T>
  Term operator()(const T& x) const
  {
    return static_cast<Term>(x);
  }
}; */

template <class Term>
class term_appl:public ATerm
{
  protected:
    /// \brief Conversion operator.
    /// \return The wrapped ATerm.
    _ATermAppl* appl() const
    {
      return static_cast<_ATermAppl*>(m_term);
    }

  public:
    /// The type of object, T stored in the term_appl.
    typedef Term value_type;
    
    /// Pointer to T.
    typedef Term* pointer;
    
    /// Reference to T.
    typedef Term& reference;
    
    /// Const reference to T.
    typedef const Term const_reference;
    
    /// An unsigned integral type.
    typedef size_t size_type;
    
    /// A signed integral type.
    typedef ptrdiff_t difference_type;
    
    /// Iterator used to iterate through an term_appl.
    typedef term_appl_iterator<Term> iterator;
    
    /// Const iterator used to iterate through an term_appl.
    typedef term_appl_iterator<Term> const_iterator;
    
    /// \brief Default constructor.
    term_appl():ATerm()
    {}

    term_appl (_ATermAppl *t):ATerm(reinterpret_cast<_ATerm*>(t))
    {
    }

    /// \brief Explicit constructor from an ATerm.
    /// \param t The ATerm.
    explicit term_appl (const ATerm &t):ATerm(t)
    {
    }

    /// \brief Constructor.
    /// \details The iterator range is traversed only once, assuming Iter is a forward iterator.
    ///          The length of the iterator range should must match the arity of the function symbol.
    /// \param sym A function symbol.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_appl(const AFun &sym, Iter first, Iter last);
    
    /// \brief Constructor.
    /// \details The iterator range is traversed only once, assuming Iter is a forward iterator.
    ///          This means that the ATermConverter is applied exactly once to each element.
    ///          The length of the iterator range must be equal to the arity of the function symbol.
    /// \param sym A function symbol.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    /// \param convert_to_aterm. An optional translator that is applied to each element in the iterator range,
    //                              and which must translate these elements to type Term.
    template <class Iter, class ATermConverter>
    term_appl(const AFun &sym, Iter first, Iter last, ATermConverter convert_to_aterm);
    
    /// \brief Constructor.
    /// \param sym A function symbol.
    term_appl(const AFun &sym);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    term_appl(const AFun &sym, const Term &t1);

    /// \brief Constructor for a binary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    term_appl(const AFun &sym, const Term &t1, const Term &t2);

    /// \brief Constructor for a ternary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    term_appl(const AFun &sym, const Term &t1, const Term &t2, const Term &t3);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    term_appl(const AFun &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    /// \param t5 The fifth argument.
    term_appl(const AFun &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    /// \param t5 The fifth argument.
    /// \param t6 The sixth argument.
    term_appl(const AFun &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5, const Term &t6);

    /// \brief assignment operator
    /// \param t The assigned term
    term_appl &operator=(const term_appl &t)
    {
      copy_term(t.m_term);
      return *this;
    }

    _ATermAppl & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term==NULL || m_term->reference_count>0);
      return *reinterpret_cast<_ATermAppl*>(m_term); 
    }

    _ATermAppl *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count>0);
      return reinterpret_cast<_ATermAppl*>(m_term);
    }

    /// \brief Returns the size of the list.
    /// \return The size of the list.
    size_type size() const
    {
      return m_term->m_function_symbol.arity();
    }

    /// \brief Returns true if the term has no arguments.
    /// \return True if this term has no arguments.
    bool empty() const
    {
      return m_term->m_function_symbol.arity()==0;
    }

    /// \brief Returns an iterator pointing to the beginning of the list.
    /// \return An iterator pointing to the beginning of the list.
    const_iterator begin() const
    {
      return const_iterator(&(static_cast<_ATermAppl*>(m_term)->arg[0]));
    }

    /// \brief Returns a const_iterator pointing to the beginning of the list.
    /// \return A const_iterator pointing to the beginning of the list.
    const_iterator end() const
    {
      return const_iterator(&static_cast<_ATermAppl*>(m_term)->arg[size()]);
      // return const_iterator(((ATerm*)(&*m_term) + ARG_OFFSET + size()));
    }

    /// \brief Returns the largest possible size of the list.
    /// \return The largest possible size of the list.
    size_type max_size() const
    {
      return (std::numeric_limits<unsigned long>::max)();
    }

    /// \brief Returns a copy of the term with the i-th child replaced by t.
    /// \deprecated
    /// \param arg The new i-th argument
    /// \param i A positive integer
    /// \return The term with one of its arguments replaced.
    term_appl<Term> set_argument(const Term &arg, const size_type i) const;

    /// \brief Get the function symbol (function_symbol) of the application.
    /// \return The function symbol of the function application.
    /* AFun function() const
    {
      return m_term->function();
    } */

    /// \brief Returns the i-th argument.
    /// \param i A positive integer
    /// \return The argument with the given index.
    const Term operator()(size_type i) const
    {
      assert(i<m_term->function().arity());
      return static_cast<const Term>(static_cast<ATerm>(static_cast<_ATermAppl*>(m_term)->arg[i]));
    }

};

typedef term_appl<ATerm> ATermAppl;
// typedef term_appl<ATerm> aterm_appl;


extern _ATermList* ATempty_address;

template <typename Term>
class term_list:public ATerm
{
  public:

    /// The type of object, T stored in the term_list.
    typedef Term value_type;
    
    /// Pointer to T.
    typedef Term* pointer;
    
    /// Reference to T.
    typedef Term& reference;
    
    /// Const reference to T.
    typedef const Term const_reference;
    
    /// An unsigned integral type.
    typedef size_t size_type;
    
    /// A signed integral type.
    typedef ptrdiff_t difference_type;
    
    /// Iterator used to iterate through an term_list.
    typedef term_list_iterator<Term> iterator;
    
    /// Const iterator used to iterate through an term_list.
    typedef term_list_iterator<Term> const_iterator;
    
    /// Default constructor. Creates an empty list.
    term_list ():ATerm(ATempty_address)
    {
    }

    /// Construction from ATermList.
    /// \param l A list.
    term_list(const term_list<Term> &t):ATerm(reinterpret_cast<_ATerm *>(&*t))
    {
      assert(m_term==NULL || type() == AT_LIST); // term list can be NULL.
    }

    /// \brief Construction from ATermList.
    /// \param l A list.
    term_list(_ATermList *t):ATerm(reinterpret_cast<_ATerm *>(t))
    {
      assert(t==NULL || type() == AT_LIST); // term list can be NULL. This is generally used to indicate a faulty
                                            // situation. This used should be discouraged.
    }

    /// Construction from aterm_list.
    /// \param t A term containing a list.
    //  \deprecated This conversion should be removed. Conversions
    //  between lists must be explicit.
    //  \return The same list of a different type.
    template <typename SpecificTerm>
    term_list<Term>(const term_list<SpecificTerm> &t): ATerm(t)
    {} 
    

    /// Explicit construction from ATerm. 
    ///  \param t An aterm.
    explicit term_list(const ATerm &t):ATerm(t)
    {
      assert(m_term==NULL || t.type()==AT_LIST); // Term list can be NULL; Generally, this is used to indicate a faulty situation.
                                                 // This use should be discouraged.
    }

    /// \brief Construction of a list from a list and an element
    /// \detail This is the standard cons operator on lists.
    /// \param l A list
    /// \param t An element
    term_list(const term_list<Term> &l, const Term &t);
    

    /// Creates an term_list with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_list(Iter first, Iter last, typename boost::enable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::random_access_traversal_tag >::type >::type* = 0)
    {
      term_list<Term> result(ATempty_address);
      while (first != last)
      {
        const Term t=*(--last);
        result = term_list<Term>(result, t);
      }
      m_term=&*result;
      increase_reference_count<false>(m_term);
    }
    
    /// Creates an term_list with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
             term_list(Iter first, Iter last, typename boost::disable_if<
             typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
             boost::random_access_traversal_tag >::type >::type* = 0)
    {
      std::vector<Term> temporary_store;
      while (first != last)
      {
        const Term t= *first;
        temporary_store.push_back(t);
        first++;
      }
      term_list<Term> result(ATempty_address);
      for(typename std::vector<Term>::reverse_iterator i=temporary_store.rbegin(); 
              i!=temporary_store.rend(); ++i)
      { 
        result=term_list(result, *i); 
      }
      m_term=&*result;
      increase_reference_count<false>(m_term);
    }

    /// Assigment operator.
    /// \param l A list.
    term_list<Term> &operator=(const term_list &l)
    {
      copy_term(l.m_term);
      return *this;
    }

    _ATermList & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term==NULL || m_term->reference_count>0);
      return *reinterpret_cast<_ATermList*>(m_term); 
    }

    _ATermList *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count>0);
      return reinterpret_cast<_ATermList*>(m_term);
    }

    /// \brief Conversion to ATermList.
    /// \return The wrapped ATermList pointer.
    operator term_list<ATerm>() const
    {
      return static_cast<_ATermList*>(m_term);
    }

    /// \brief Returns the tail of the list.
    /// \return The tail of the list.
    const term_list<Term> tail() const
    {
      return (static_cast<_ATermList*>(m_term))->tail;
    }

    /// \brief Returns the head of the list.
    /// \return The term at the head of the list.
    const Term head()
    {
      return (static_cast<_ATermList*>(m_term))->head;
    }

    /// \brief Returns the size of the term_list.
    /// \detail The complexity of this function is linear in the size of the list.
    /// \return The size of the list.
    size_type size() const
    {
      size_t size=0;
      for(_ATermList* m_list=static_cast<_ATermList*>(m_term);
                 m_list!=ATempty_address; m_list=m_list->tail)
      {
        size++;
      }
      return size;
    }

    /// \brief Returns true if the list's size is 0.
    /// \return True if the list is empty.
    bool empty() const
    {
      return &*m_term==ATempty_address;
    }

    /// \brief Returns the first element.
    /// \return The first element of the list.
    Term front() const
    {
      return Term(static_cast<ATerm>(static_cast<_ATermList*>(m_term)->head));
    }

    /// \brief Returns a const_iterator pointing to the beginning of the term_list.
    /// \return The beginning of the list.
    const_iterator begin() const
    {
      return const_iterator(m_term);
    }

    /// \brief Returns a const_iterator pointing to the end of the term_list.
    /// \return The end of the list.
    const_iterator end() const
    {
      return const_iterator(ATempty_address);
    }

    /// \brief Returns the largest possible size of the term_list.
    /// \return The largest possible size of the list.
    size_type max_size() const
    {
      return (std::numeric_limits<size_t>::max)();
    }
};

/// \brief Returns the first element of the list l.
/// \param l A list
/// \return The first element of the list.
template <typename Term>
inline
const Term front(const term_list<Term> &l)
{
  return *l.begin();
}

/// \brief Returns the list obtained by inserting a new element at the beginning.
/// \param l A list.
/// \param elem A term
/// \return The list with an element inserted in front of it.
template <typename Term>
inline
term_list<Term> push_front(const term_list<Term> &l, const Term &elem)
{
  return term_list<Term>(l, elem);
}

/// \brief Returns the list obtained by inserting a new element at the end. Note
/// that the complexity of this function is O(n), with n the number of
/// elements in the list!!!
/// \param l A list.
/// \param elem A term
/// \return The list with an element appended to it.
template <typename Term>
inline
term_list<Term> push_back(const term_list<Term> &l, const Term &elem)
{
  return term_list<Term>(ATappend(l, (ATerm)elem));
}

/// \brief Returns the list obtained by removing the first element.
///// \param l A list.
///// \return The list with the first element removed.
template <typename Term>
inline
const term_list<Term> pop_front(const term_list<Term> &l)
{
  return l.tail();
}

/// \brief Returns the list with the elements in reversed order.
/// \param l A list.
/// \return The reversed list.
template <typename Term>
inline
term_list<Term> reverse(term_list<Term> l)
{
  return term_list<Term>(ATreverse(l));
}

/// \brief Returns the concatenation of two lists.
/// \param l A list.
/// \param m A list.
/// \return The concatenation of the lists.
template <typename Term>
inline
term_list<Term> operator+(term_list<Term> l, term_list<Term> m)
{
  return term_list<Term>(ATconcat(l, m));
}

/// \brief Applies a function to all elements of the list and returns the result.
/// \param l The list that is transformed.
/// \param f The function that is applied to the elements of the list.
/// \return The transformed list.
template <typename Term, typename Function>
inline
term_list<ATerm> apply(term_list<Term> l, const Function f)
{
  term_list<ATerm> result;
  for (typename term_list<Term>::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, f(*i));
  }
  return reverse(result);
}

/// \brief Returns the list l with one occurrence of the element x removed, or l if x is not present.
/// \param l A list.
/// \param x A list element.
template <typename Term>
inline
term_list<Term> remove_one_element(const term_list<Term>& l, const Term& x)
{
  return term_list<Term>(ATremoveElement(l, static_cast<ATerm>(x)));
}

/// \brief A term_list with elements of type aterm.
typedef term_list<ATerm> ATermList;

struct _ATermTable;

typedef _ATermTable *ATermIndexedSet;

typedef _ATermTable *ATermTable;


/** The following functions implement the operations of
  * the 'standard' ATerm interface, and should appear
  * in some form in every implementation of the ATerm
  * datatype.
  */

/* The ATermInt type */
inline
ATermInt ATmakeInt(const int value)
{
  return ATermInt(value);
}

inline
int ATgetInt(const ATermInt &t)
{
  return t.value();
  // return t->value;
}

/* The ATermAppl type */
ATermAppl ATmakeAppl_varargs(const AFun &sym, ...); // Only used in the jittyc compiler.

/* The implementation of the function below can be found in memory.h */
template <class TERM_ITERATOR>
ATermAppl ATmakeAppl_iterator(const AFun &sym, const TERM_ITERATOR begin, const TERM_ITERATOR end)
{
  return term_appl<ATerm>(sym,begin,end);
}

inline
ATermAppl ATmakeAppl0(const AFun &sym)
{ 
  return term_appl<ATerm>(sym);
}

inline
ATermAppl ATmakeAppl1(const AFun &sym, const ATerm &arg0)
{ 
  return term_appl<ATerm>(sym,arg0);
}

inline
ATermAppl ATmakeAppl2(const AFun &sym, const ATerm &arg0, const ATerm &arg1)
{ 
  return term_appl<ATerm>(sym,arg0,arg1);
}

inline
ATermAppl ATmakeAppl3(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2)
{ 
  return term_appl<ATerm>(sym,arg0,arg1,arg2);
}

inline
ATermAppl ATmakeAppl4(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3)
{ 
  return term_appl<ATerm>(sym,arg0,arg1,arg2,arg3);
}

inline
ATermAppl ATmakeAppl5(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3, const ATerm &arg4)
{ 
  return term_appl<ATerm>(sym,arg0,arg1,arg2,arg3,arg4);
}

inline
ATermAppl ATmakeAppl6(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3, const ATerm &arg4, const ATerm &arg5)
{ 
  return term_appl<ATerm>(sym,arg0,arg1,arg2,arg3,arg4,arg5);
}


inline
size_t ATgetAFun(const _ATermAppl* appl)
{
  return appl->m_function_symbol.number();
}

inline
size_t ATgetAFun(const ATermAppl &appl)
{
  return appl.function().number();
}

inline
const ATerm ATgetArgument(const ATermAppl &appl, const size_t idx)
{
  return appl(idx);
}

inline
ATermAppl ATsetArgument(const ATermAppl &appl, const ATerm &arg, const size_t n)
{
  return appl.set_argument(arg,n);
}

/* Portability */
ATermList ATgetArguments(const ATermAppl &appl);
ATermAppl ATmakeApplList(const AFun &sym, const ATermList &args);
ATermAppl ATmakeApplArray(const AFun &sym, const ATerm args[]);

size_t ATgetLength(const ATermList &list);

inline
ATerm ATgetFirst(const ATermList &l)
{
  return static_cast<ATerm>(l->head);
}

inline
ATermList ATgetNext(const ATermList &l)
{
  return static_cast<ATermList>(l->tail);
}

/* The ATermList type */
extern ATermList ATempty;

inline
bool ATisEmpty(const ATermList &l)
{
  return l == ATempty;

  // return l->head == NULL && l->tail == NULL;
}

ATermList ATgetTail(const ATermList &list, const int &start);
ATermList ATgetSlice(const ATermList &list, const size_t start, const size_t end);

inline
ATermList ATinsert(const ATermList &list, const ATerm &el)
{
  return term_list<ATerm>(list,el);
}

ATermList ATappend(const ATermList &list, const ATerm &el);
ATermList ATconcat(const ATermList &list1, const ATermList &list2);
size_t    ATindexOf(const ATermList &list, const ATerm &el, const int start);
const ATerm ATelementAt(const ATermList &list, size_t index);
ATermList ATremoveElement(const ATermList &list, const ATerm &el);
ATermList ATremoveElementAt(const ATermList &list, const size_t idx);
ATermList ATreplace(const ATermList &list, const ATerm &el, const size_t idx);
ATermList ATreverse(const ATermList &list);
ATermList ATsort(const ATermList &list, int (*compare)(const ATerm &t1, const ATerm &t2));

/* ATermList ATmakeList0(); */
inline
ATermList ATmakeList0()
{
  return static_cast<ATermList>(term_list<ATerm>());
  // return ATempty;
}

ATermList ATmakeList1(const ATerm &el0);

inline
ATermList ATmakeList2(const ATerm &el0, const ATerm &el1)
{
  return ATinsert(ATmakeList1(el1), el0);
}

inline
ATermList ATmakeList3(const ATerm &el0, const ATerm &el1, const ATerm &el2)
{
  return ATinsert(ATmakeList2(el1, el2), el0);
}

inline
ATermList ATmakeList4(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3)
{
  return ATinsert(ATmakeList3(el1, el2, el3), el0);
}

inline
ATermList ATmakeList5(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3, const ATerm &el4)
{
  return ATinsert(ATmakeList4(el1, el2, el3, el4), el0);
}

inline
ATermList ATmakeList6(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3, const ATerm &el4, const ATerm &el5)
{
  return ATinsert(ATmakeList5(el1, el2, el3, el4, el5), el0);
}

ATermTable ATtableCreate(const size_t initial_size, const unsigned int max_load_pct);
void       ATtableDestroy(ATermTable table);
void       ATtableReset(ATermTable table);
void       ATtablePut(ATermTable table, const ATerm &key, const ATerm &value);
ATerm      ATtableGet(ATermTable table, const ATerm &key);
bool     ATtableRemove(ATermTable table, const ATerm &key); /* Returns true if removal was successful. */
ATermList  ATtableKeys(ATermTable table);
ATermList  ATtableValues(ATermTable table);

ATermIndexedSet
ATindexedSetCreate(size_t initial_size, unsigned int max_load_pct);
void       ATindexedSetDestroy(ATermIndexedSet set);
void       ATindexedSetReset(ATermIndexedSet set);
size_t     ATindexedSetPut(ATermIndexedSet set, const ATerm &elem, bool* isnew);
ssize_t    ATindexedSetGetIndex(ATermIndexedSet set, const ATerm &elem); /* A negative value represents non existence. */
bool     ATindexedSetRemove(ATermIndexedSet set, const ATerm &elem);   /* Returns true if removal was successful. */
ATermList  ATindexedSetElements(ATermIndexedSet set);
ATerm      ATindexedSetGetElem(ATermIndexedSet set, size_t index);

// AFun  ATmakeAFun(const char* name, const size_t arity, const bool quoted);

inline
char* ATgetName(const AFun &sym)
{
  return AFun::at_lookup_table[sym.number()]->name;
}

/* inline
size_t ATgetArity(const size_t n)
{
  return GET_LENGTH(AFun::at_lookup_table[n]->header >> 1);
} */

inline
size_t ATgetArity(const AFun &sym)
{
  return sym.arity();
} 


inline
bool ATisQuoted(const AFun &sym)
{
  return sym.is_quoted();
}

size_t  AT_calcUniqueAFuns(const ATerm &t);
void AT_cleanup(void);


} // namespace aterm

#include "mcrl2/aterm/memory.h"

#endif
