#include <sip/visitors.h>

namespace sip {

  class store_visitor_impl : public utility::visitor < store_visitor_impl >,
                             private boost::noncopyable {

    friend class visitors;
    friend class store_visitor;

    private:

      /** \brief Alias for the first base class */
      typedef utility::visitor < store_visitor_impl > base;

    private:

      /** \brief Points to interface object */
      sip::store_visitor&  m_interface;

      /** \brief The destination of output */
      std::ostream&        out;

    protected:

      /** \brief Writes to stream */
      store_visitor_impl(store_visitor&, std::ostream&);

    public:

      template < typename T >
      void visit(T const&);

      template < typename T, typename U >
      void visit(T const&, U const&);
  };
}
