#ifndef ITERATORS_H
#define ITERATORS_H

#include <memory>

#include <boost/type_traits/is_const.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace iterator_wrapper {

  /**
   * \brief Interface class for allowing limited external iteration over
   * private members that support STL compatible iterators that hold smart pointers
   *
   * The value type of the container are assumed to be a pointer type, (or
   * otherwise be dereferencable). The dereference operator of the iterator
   * results in the target of the (smart) pointer than the smart pointer itself.
   *
   * The purpose of this wrapper is to provide access to sequences that are
   * declared private in a class but whose contents must be readable or
   * modifiable in the world outside the class. Modification of the sequence
   * itself is impossible through the wrapper.
   *
   * In multi-threaded programs locking can be added by deriving a new class
   * that keeps a reference to a lock. The lock is acquired in the constructor
   * and released when the iterator object goes out of scope (so in the
   * destructor).
   *
   * @param T the type of the container type which elements should be iterated
   * @param S the type of an element
   * @param I the iterator type
   *
   * \attention only under the assumption that the container is not
   * changed during iteration.
   **/
  template < typename T, typename S, typename I = typename T::const_iterator >
  class constant_indirect_iterator {

    private:

      /** \brief The actual iterator */
      I iterator;

      /** \brief The past-end-of-sequence value */
      I end;

    private:

      /** \brief Helper function for boost shared pointers */
      inline static S* get_pointer(const boost::shared_ptr < S >&);

      /** \brief Helper function for boost weak pointers */
      inline static S* get_pointer(const boost::weak_ptr < S >&);

      /** \brief Helper function for standard library auto pointers */
      inline static S* get_pointer(const std::auto_ptr < S >&);

      /** \brief Helper function for ordinary pointers */
      inline static S* get_pointer(const S*);

    public:

      /** \brief Constructor for STL containers */
      constant_indirect_iterator(const T&);

      /** \brief Constructor */
      constant_indirect_iterator(I& begin, I&end);

      /** \brief Whether the iterator has moved past the end of the sequence */
      inline bool valid() const;

      /** \brief Advances to the next element */
      inline void operator++();

      /** \brief Get the element that is currently referenced */
      inline S* operator*() const;
  };

  /**
   * \brief Interface class for allowing limited external iteration over
   * private members that support STL compatible iterators that hold smart pointers
   *
   * The value type of the container are assumed to be a pointer type, (or
   * otherwise be dereferencable). The dereference operator of the iterator
   * results in the target of the (smart) pointer than the smart pointer itself.
   *
   * The purpose of this wrapper is to provide access to sequences that are
   * declared private in a class but whose contents must be readable or
   * modifiable in the world outside the class. Modification of the sequence
   * itself is impossible through the wrapper.
   *
   * In multi-threaded programs locking can be added by deriving a new class
   * that keeps a reference to a lock. The lock is acquired in the constructor
   * and released when the iterator object goes out of scope (so in the
   * destructor).
   *
   * @param T the type of the container type which elements should be iterated
   * @param S the type of an element
   * @param I the iterator type
   *
   * \attention only under the assumption that the container is not
   * changed during iteration.
   **/
  template < typename T, typename S, typename I = typename T::iterator >
  class indirect_iterator {

    private:

      /** \brief The actual iterator */
      I iterator;

      /** \brief The past-end-of-sequence value */
      I end;

    private:

      /** \brief Helper function for boost shared pointers */
      inline static S* get_pointer(boost::shared_ptr < S >&);

      /** \brief Helper function for boost weak pointers */
      inline static S* get_pointer(boost::weak_ptr < S >&);

      /** \brief Helper function for standard library auto pointers */
      inline static S* get_pointer(std::auto_ptr < S >&);

      /** \brief Helper function for ordinary pointers */
      inline static S* get_pointer(S*);

    public:

      /** \brief Constructor for STL containers */
      indirect_iterator(T&);

      /** \brief Constructor */
      indirect_iterator(I& begin, I&end);

      /** \brief Whether the iterator has moved past the end of the sequence */
      inline bool valid() const;

      /** \brief Advances to the next element */
      inline void operator++();

      /** \brief Get the element that is currently referenced */
      inline S* operator*() const;
  };

  /**
   * @param c the container with the elements over which to iterate
   **/
  template < typename T, typename S, typename I >
  inline constant_indirect_iterator< T, S, I >::constant_indirect_iterator(const T& c) : iterator(c.begin()), end(c.end()) {
  }
 
  /**
   * @param c the container with the elements over which to iterate
   **/
  template < typename T, typename S, typename I >
  inline indirect_iterator< T, S, I >::indirect_iterator(T& c) : iterator(c.begin()), end(c.end()) {
  }
 
  /**
   * @param b the iterator from which to start
   * @param e the iterator with which to end
   **/
  template < typename T, typename S, typename I >
  inline constant_indirect_iterator< T, S, I >::constant_indirect_iterator(I& b, I& e) : iterator(b), end(e) {
  }
 
  /**
   * @param b the iterator from which to start
   * @param e the iterator with which to end
   **/
  template < typename T, typename S, typename I >
  inline indirect_iterator< T, S, I >::indirect_iterator(I& b, I& e) : iterator(b), end(e) {
  }
 
  template < typename T, typename S, typename I >
  inline bool constant_indirect_iterator< T, S, I >::valid() const {
    return (iterator != end);
  }
 
  template < typename T, typename S, typename I >
  inline bool indirect_iterator< T, S, I >::valid() const {
    return (iterator != end);
  }
 
  template < typename T, typename S, typename I >
  inline void constant_indirect_iterator< T, S, I >::operator++() {
    ++iterator;
  }
 
  template < typename T, typename S, typename I >
  inline void indirect_iterator< T, S, I >::operator++() {
    ++iterator;
  }
 
  template < typename T, typename S, typename I >
  inline S* constant_indirect_iterator< T, S, I >::operator*() const {
    return (get_pointer(*iterator));
  }

  template < typename T, typename S, typename I >
  inline S* indirect_iterator< T, S, I >::operator*() const {
    return (get_pointer(*iterator));
  }

  template < typename T, typename S, typename I >
  inline S* constant_indirect_iterator< T, S, I >::get_pointer(const S* i) {
    return (i);
  }

  template < typename T, typename S, typename I >
  inline S* indirect_iterator< T, S, I >::get_pointer(S* i) {
    return (i);
  }

  template < typename T, typename S, typename I >
  inline S* constant_indirect_iterator< T, S, I >::get_pointer(const boost::shared_ptr < S >& i) {
    return (i.get());
  }

  template < typename T, typename S, typename I >
  inline S* indirect_iterator< T, S, I >::get_pointer(boost::shared_ptr < S >& i) {
    return (i.get());
  }

  template < typename T, typename S, typename I >
  inline S* constant_indirect_iterator< T, S, I >::get_pointer(const boost::weak_ptr < S >& i) {
    return (i.lock().get());
  }

  template < typename T, typename S, typename I >
  inline S* indirect_iterator< T, S, I >::get_pointer(boost::weak_ptr < S >& i) {
    return (i.lock().get());
  }

  template < typename T, typename S, typename I >
  inline S* constant_indirect_iterator< T, S, I >::get_pointer(const std::auto_ptr < S >& i) {
    return (i.get());
  }

  template < typename T, typename S, typename I >
  inline S* indirect_iterator< T, S, I >::get_pointer(std::auto_ptr < S >& i) {
    return (i.get());
  }
}

#endif

