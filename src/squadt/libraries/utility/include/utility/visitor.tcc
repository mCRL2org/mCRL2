#include "visitor.h"

namespace utility {

  /** \brief Visit constant interface function */
  template < typename S >
  template < class T >
  inline void visitor< S, true >::visit(T const& t) {
    dynamic_cast < S& > (*this).visit(t);
  }

  /** \brief Visit interface function */
  template < typename S >
  template < class T >
  inline void visitor< S, false >::visit(T& t) {
    dynamic_cast < S& > (*this).visit(t);
  }

  /** \brief Visit constant interface function, with constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, true >::visit(T const& t, U const& u) {
    dynamic_cast < S& > (*this).visit(t, u);
  }

  /** \brief Visit constant interface function, with non-constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, true >::visit(T const& t, U& u) {
    dynamic_cast < S& > (*this)->visit(t, u);
  }

  /** \brief Visit interface function, with constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, false >::visit(T& t, U const& u) {
    dynamic_cast < S& > (*this).visit(t, u);
  }

  /** \brief Visit interface function, with non-constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, false >::visit(T& t, U& u) {
    dynamic_cast < S& > (*this).visit(t, u);
  }

  /** \brief Visit constant interface function */
  template < typename S >
  template < class T >
  inline void visitor< S, true >::do_accept(T const& t) {
    t.accept(static_cast < S& > (*this));
  }

  /** \brief Visit interface function */
  template < typename S >
  template < class T >
  inline void visitor< S, false >::do_accept(T& t) {
    t.accept(static_cast < S& > (*this));
  }

  /** \brief Visit constant interface function, with constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, true >::do_accept(T const& t, U const& u) {
    t.accept(static_cast < S& > (*this), u);
  }

  /** \brief Visit constant interface function, with non-constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, true >::do_accept(T const& t, U& u) {
    t.accept(static_cast < S& > (*this), u);
  }

  /** \brief Visit interface function, with constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, false >::do_accept(T& t, U const& u) {
    t.accept(static_cast < S& > (*this), u);
  }

  /** \brief Visit interface function, with non-constant argument */
  template < typename S >
  template < class T, typename U >
  inline void visitor< S, false >::do_accept(T& t, U& u) {
    t.accept(static_cast < S& > (*this), u);
  }
}
