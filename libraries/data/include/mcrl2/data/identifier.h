#ifndef MCRL2_DATA_IDENTIFIER_H
#define MCRL2_DATA_IDENTIFIER_H

#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

//--- start generated class ---//
/// \brief An identifier
class identifier_base: public data_expression
{
  public:
    /// \brief Constructor.
    /// \param term A term
    identifier_base(atermpp::aterm_appl term)
      : data_expression(term)
    {
      assert(core::detail::check_term_Id(m_term));
    }

    /// \brief Constructor.
    identifier_base(const core::identifier_string& name)
      : data_expression(core::detail::gsMakeId(name))
    {}

    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated class ---//

    } // namespace detail

/// \brief identifier
/// \details This class should only be used up to and including
///          the type checking phase, as it yields an untyped,
///          unstructured data expression!
class identifier : public detail::identifier_base
{
  public:

  /// \brief Default constructor for identifier. This does not yield
  ///        a valid expression.
  identifier()
    : detail::identifier_base(atermpp::aterm_appl(core::detail::constructId()))
  {}

  /// \overload
  identifier(atermpp::aterm_appl term)
    : identifier_base(term)
  {}

  /// \brief Constructor for an identifier with name s
  /// \param s A string
  identifier(const mcrl2::core::identifier_string& s)
    : detail::identifier_base(s)
  {}

  /// \brief Constructor for an identifier with name s
  /// \param s A string
  identifier(const std::string& s)
    : detail::identifier_base(mcrl2::core::identifier_string(s))
  {}

}; // class identifier

  } // namespace data
} // namespace mcrl2

#endif
