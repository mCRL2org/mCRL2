// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_IO_H
#define MCRL2_ATERMPP_ATERM_IO_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"

namespace atermpp
{

/// \brief A function that is applied to all terms. The resulting term should only use
///        a subset of the original arguments (i.e. not introduce new terms).
/// \details Typical usage is removing the index traits from function symbols that represent operators.
using aterm_transformer = aterm_appl(const aterm_appl&);

/// \brief The default transformer that maps each term to itself.
inline aterm_appl identity(const aterm_appl& x) { return x; }

/// \brief The interface for a class that writes aterm to a stream.
///        Every written term is retrieved by the corresponding aterm_istream::get() call.
class aterm_ostream
{
public:
  virtual ~aterm_ostream();

  /// \brief Sets the given transformer to be applied to following writes.
  void set_transformer(aterm_transformer transformer) { m_transformer = transformer; }

  /// \returns The currently assigned transformer function.
  aterm_transformer* get_transformer() const { return m_transformer; }

  /// \brief Write the given term to the stream.
  virtual void put(const aterm& term) = 0;

protected:
  aterm_transformer* m_transformer = identity;
};

/// \brief The interface for a class that reads aterm from a stream.
///        The default constructed term aterm() indicates the end of the stream.
class aterm_istream
{
public:
  virtual ~aterm_istream();

  /// \brief Sets the given transformer to be applied to following reads.
  void set_transformer(aterm_transformer transformer) { m_transformer = transformer; }

  /// \returns The currently assigned transformer function.
  aterm_transformer* get_transformer() const { return m_transformer; }

  /// \brief Reads an object of type T from this stream, using the object specific >> operator.
  template<typename T>
  T get();

  /// \brief Reads a single term from this stream.
  virtual aterm get() = 0;

protected:
  aterm_transformer* m_transformer = identity;
};

// These free functions provide input/output operators for these streams.

/// \brief Sets the given transformer to be applied to following reads.
inline aterm_ostream& operator<<(aterm_ostream& stream, aterm_transformer transformer) { stream.set_transformer(transformer); return stream; }

/// \brief Write the given term to the stream.
inline aterm_ostream& operator<<(aterm_ostream& stream, const aterm& term) { stream.put(term); return stream; }

/// \brief Sets the given transformer to be applied to following reads.
inline aterm_istream& operator>>(aterm_istream& stream, aterm_transformer transformer) { stream.set_transformer(transformer); return stream; }

/// \brief Reads a single term from this stream.
inline aterm_istream& operator>>(aterm_istream& stream, aterm& term) { term = stream.get(); return stream; }

// Utility functions

namespace detail
{
  inline aterm end_of_container()
  {
    static aterm_appl t(function_symbol("end_of_container", 0));
    return t;
  }
}

/// \brief Write any container (that is not an aterm) to the stream.
/// \brief Write any container (that is not an aterm itself) to the stream.
template<typename T,
  typename std::enable_if_t<is_container<T, aterm>::value, int> = 0,
  typename std::enable_if_t<!std::is_base_of<aterm, T>::value, int> = 0>
inline aterm_ostream& operator<<(aterm_ostream& stream, const T& container)
{
  // Write the number of elements, followed by each element in the container.
  for (const auto& element : container)
  {
    stream << element;
  }

  stream << detail::end_of_container();

  return stream;
}

/// \brief Read any container (that is not an aterm itself) from the stream.
template<typename T,
  typename std::enable_if_t<is_container<T, aterm>::value, int> = 0,
  typename std::enable_if_t<!std::is_base_of<aterm, T>::value, int> = 0>
inline aterm_istream& operator>>(aterm_istream& stream, T& container)
{
  // Insert each term into the container until the end_of_container mark is found.
  aterm t;
  auto it = std::inserter(container, container.end());
  do
  {
    t = stream.get();
    it = static_cast<const typename T::value_type&>(t);
  }
  while (t != detail::end_of_container());

  return stream;
}

template<typename T>
inline aterm_ostream& operator<<(aterm_ostream&& stream, const T& t) { stream << t; return stream; }

template<typename T>
inline aterm_istream& operator>>(aterm_istream&& stream, T& t) { stream >> t; return stream; }

template<typename T>
inline T aterm_istream::get() { T t; *this >> t; return t; }

/// \brief Send the term in textual form to the ostream.
std::ostream& operator<<(std::ostream& out, const aterm& t);

/// \param t The input aterm.
/// \return A string representation of the given term derived from an aterm.
inline std::string pp(const aterm& t)
{
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

/// \brief Sends the name of a function symbol to an ostream.
/// \param out The out stream.
/// \param f The function symbol to be output.
/// \return The stream.
inline
std::ostream& operator<<(std::ostream& out, const function_symbol& f)
{
  return out << f.name();
}

/// \brief Prints the name of a function symbol as a string.
/// \param f The function symbol.
/// \return The string representation of r.
inline const std::string& pp(const function_symbol& f)
{
  return f.name();
}

/// \brief Writes term t to a stream in binary aterm format.
void write_term_to_binary_stream(const aterm& t, std::ostream& os);

/// \brief Reads a term from a stream in binary aterm format.
aterm read_term_from_binary_stream(std::istream& is);

/// \brief Writes term t to a stream in textual format.
void write_term_to_text_stream(const aterm& t, std::ostream& os);

/// \brief Reads a term from a stream which contains the term in textual format.
aterm read_term_from_text_stream(std::istream& is);

/// \brief Reads an aterm from a string. The string can be in either binary or text format.
aterm read_term_from_string(const std::string& s);

/// \brief Reads an aterm_list from a string. The string can be in either binary or text format.
/// \details If the input is not a string, an aterm is returned of the wrong type.
/// \return The term corresponding to the string.
inline aterm_list read_list_from_string(const std::string& s)
{
  const aterm_list l = down_cast<aterm_list>(read_term_from_string(s));
  assert(l.type_is_list());
  return l;
}

/// \brief Reads an aterm_int from a string. The string can be in either binary or text format.
/// \details If the input is not an int, an aterm is returned of the wrong type.
/// \return The aterm_int corresponding to the string.
inline aterm_int read_int_from_string(const std::string& s)
{
  const aterm_int n = down_cast<aterm_int>(read_term_from_string(s));
  assert(n.type_is_int());
  return n;
}

/// \brief Reads an aterm_appl from a string. The string can be in either binary or text format.
/// \details If the input is not an aterm_appl, an aterm is returned of the wrong type.
/// \return The term corresponding to the string.
inline aterm_appl read_appl_from_string(const std::string& s)
{
  const aterm_appl a = down_cast<aterm_appl>(read_term_from_string(s));
  assert(a.type_is_appl());
  return a;
}


} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_H
