#include <xml2pp/text_reader.h>

#include <sip/visitors.h>

namespace sip {

  class restore_visitor_impl :
                        public utility::visitor < restore_visitor_impl, false >,
                        private boost::noncopyable {

    friend class visitors;
    friend class restore_visitor;

    private:

      /** \brief Alias for the first base class */
      typedef utility::visitor < restore_visitor_impl, false > base;

    private:

      /** \brief Points to interface object */
      sip::restore_visitor&     m_interface;

      /** \brief The source of input */
      xml2pp::text_reader&      in;

    protected:

      /** \brief Reads from string */
      restore_visitor_impl(restore_visitor&, xml2pp::text_reader&);

    public:

      template < typename T >
      void visit(T&);

      template < typename T, typename U >
      void visit(T&, U&);
  };
}
