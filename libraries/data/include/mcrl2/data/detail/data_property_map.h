// Author(s): Jeroen van der Wulp and Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_property_map.h
/// \brief A property map containing properties of an LPS specification.

#ifndef MCRL2_DATA_DETAIL_DATA_PROPERTY_MAP_H
#define MCRL2_DATA_DETAIL_DATA_PROPERTY_MAP_H

#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <type_traits>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <functional>

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief Base class for storing properties of mCRL2 types.
/// Properties are (key, value) pairs stored as strings in <tt>KEY = VALUE</tt>
/// format. The data_property_map has some predefined functions for
/// types in the Data Library.
/// The optional type argument is used by derived classes. The type
/// represents the name of a derived class as per CRTP.
template < typename Derived = void >
class data_property_map
{
  protected:

    /// \brief Add start/end separators for non-set container types
    template < typename Container >
    static std::string add_separators(std::string const& c, typename std::enable_if< atermpp::is_set< Container >::value >::type* = nullptr)
    {
      return "[" + c + "]";
    }

    /// \brief Add start/end separators for set container types
    template < typename Container >
    static std::string add_separators(std::string const& c, typename std::enable_if< !atermpp::is_set< Container >::value >::type* = 0)
    {
      return "{" + c + "}";
    }

    /// \brief Contains a normalized string representation of the properties.
    std::map<std::string, std::string> m_data;

    //--------------------------------------------//
    // print functions
    //--------------------------------------------//
    std::string print(size_t n) const
    {
      std::ostringstream out;
      out << n;
      return out.str();
    }

    std::string print(std::string s) const
    {
      return s;
    }

    std::string print(const core::identifier_string& s) const
    {
      return s;
    }

    std::string print(const data::variable& v) const
    {
      return data::pp(v) + ":" + data::pp(v.sort());
    }

    template < typename Container >
    std::string print(const Container& v, typename atermpp::enable_if_container< Container >::type* = nullptr) const
    {
      std::set<std::string> elements;

      for (auto i = v.begin(); i != v.end(); ++i)
      {
        elements.insert(static_cast< Derived const& >(*this).print(*i));
      }

      return utilities::string_join(elements, ", ");
    }

    template < typename Container >
    std::string print(const Container& v, bool print_separators, typename atermpp::enable_if_container< Container >::type* = nullptr) const
    {
      return (print_separators) ? add_separators< Container >(print(v)) : print(v);
    }

    //--------------------------------------------//
    // parse functions
    //--------------------------------------------//
    unsigned int parse_unsigned_int(std::string const& text) const
    {
      return boost::lexical_cast<unsigned int>(utilities::remove_whitespace(text));
    }

    std::set<std::string> parse_set_string(std::string const& text) const
    {
      std::vector<std::string> v = utilities::split(text, ",");
      std::for_each(v.begin(), v.end(), utilities::trim);
      return std::set<std::string>(v.begin(), v.end());
    }

    std::set<std::multiset<std::string> > parse_set_multiset_string(std::string const& text) const
    {
      std::set<std::multiset<std::string> > result;
      std::vector<std::string> multisets = utilities::split(text, ";");
      for (const std::string& ms: multisets)
      {
        std::string s = utilities::regex_replace("[{}]", "", ms);
        std::vector<std::string> v = utilities::split(s, ",");
        std::for_each(v.begin(), v.end(), utilities::trim);
        result.insert(std::multiset<std::string>(v.begin(), v.end()));
      }
      return result;
    }

    //--------------------------------------------//
    // compare functions
    //--------------------------------------------//
    /// \brief Compares two integers, and returns a message if they are different.
    /// If if they are equal the empty string is returned.
    std::string compare(const std::string& property, unsigned int x, unsigned int y) const
    {
      if (x != y)
      {
        std::ostringstream out;
        out << "Difference in property " << property << " detected: " << x << " versus " << y << "\n";
        return out.str();
      }
      return "";
    }

    /// \brief Compares two sets and returns a string with the differences encountered.
    /// Elements present in the first one but not in the second are printed with a '+'
    /// in front of it, elements present in the seconde but not in the first one with a '-'
    /// in front of it. A value x of the type T is printed using print(x), so this
    /// operation must be defined.
    /// If no differences are found the empty string is returned.
    template <typename T>
    std::string compare(const std::string& property, const std::set<T>& x, const std::set<T>& y) const
    {
      std::ostringstream out;

      // compute elements in x but not in y
      std::set<T> plus;
      std::set_difference(x.begin(), x.end(), y.begin(), y.end(), std::inserter(plus, plus.end()));

      // compute elements in y but not in x
      std::set<T> minus;
      std::set_difference(y.begin(), y.end(), x.begin(), x.end(), std::inserter(minus, minus.end()));

      if (!plus.empty() || !minus.empty())
      {
        out << "Difference in property " << property << " detected:";
        for (auto i = plus.begin(); i != plus.end(); ++i)
        {
          out << " +" << print(*i);
        }
        for (auto i = minus.begin(); i != minus.end(); ++i)
        {
          out << " -" << print(*i);
        }
        out << "\n";
        return out.str();
      }
      return "";
    }

    /// \brief Compares two values x and y of a given property. This function should
    /// be redefined in derived classes.
    /// \return An empty string if the two values are equal, otherwise a string indicating
    /// the differences between the two.
    std::string compare_property(const std::string& property, const std::string& /* x */, const std::string& /* y */) const
    {
      return "ERROR: unknown property " + property + " encountered!";
    }

    //--------------------------------------------//
    // miscellaneous functions
    //--------------------------------------------//
    /// \brief Returns the maximum length of the property names
    unsigned int max_key_length() const
    {
      unsigned int result = 0;
      for (auto i = m_data.begin(); i != m_data.end(); ++i)
      {
        result = (std::max)(static_cast< size_t >(result), i->first.size());
      }
      return result;
    }

    std::string align(const std::string& s, unsigned int n) const
    {
      if (s.size() >= n)
      {
        return s;
      }
      return s + std::string(n - s.size(), ' ');
    }

    /// \brief Collects the names of the elements of the container.
    /// The name of element x is retrieved by x.name().
    template <typename Container>
    std::set<core::identifier_string> names(const Container& v) const
    {
      std::set<core::identifier_string> result;
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        result.insert(i->name());
      }
      return result;
    }

    /// \brief Default constructor for derived types
    data_property_map()
    {
    }

    /// \brief Initializes the property map with text containing lines in
    /// <tt>KEY = VALUE</tt> format.
    void parse_text(const std::string& text)
    {
      for (const std::string& line: utilities::split(text, "\n"))
      {
        std::vector<std::string> words = utilities::split(line, "=");
        if (words.size() == 2)
        {
          utilities::trim(words[0]);
          utilities::trim(words[1]);
          m_data[words[0]] = words[1];
        }
      }
    }

  public:
    /// The strings may appear in a random order, and not all of them need to be present
    data_property_map(const std::string& text)
    {
      parse_text(text);
    }

    /// \brief Returns a string representation of the properties
    std::string to_string() const
    {
      unsigned int n = max_key_length();
      std::vector<std::string> lines;
      for (auto i = m_data.begin(); i != m_data.end(); ++i)
      {
        lines.push_back(align(i->first, n) + " = " + i->second);
      }
      return utilities::string_join(lines, "\n");
    }

    /// \brief Returns the stored properties
    const std::map<std::string, std::string>& data() const
    {
      return m_data;
    }

    /// \brief Returns the value corresponding to key.
    /// Throws an exception if the key is not found.
    std::string operator[](const std::string& key) const
    {
      auto i = m_data.find(key);
      if (i == m_data.end())
      {
        throw mcrl2::runtime_error("property_map: could not find key " + key);
      }
      return i->second;
    }

    /// \brief Compares this property map with another property map.
    /// The function compare_property must be defined properly for all
    /// available properties.
    /// \return A string describing the differences found.
    std::string compare(const data_property_map& other) const
    {
      std::ostringstream out;
      for (auto i = m_data.begin(); i != m_data.end(); ++i)
      {
        auto j = other.data().find(i->first);
        if (j != other.data().end())
        {
          out << static_cast< Derived const& >(*this).compare_property(i->first, i->second, j->second);
        }
      }
      return out.str();
    }
};

template <typename PropertyMap>
bool compare_property_maps(const std::string& message, const PropertyMap& map1, const std::string& expected_result)
{
  PropertyMap map2(expected_result);
  std::string result = map1.compare(map2);
  if (!result.empty())
  {
    std::cerr << "------------------------------" << std::endl;
    std::cerr << "          Failed test         " << std::endl;
    std::cerr << "------------------------------" << std::endl;
    std::cerr << message << std::endl;
    std::cerr << "--- expected result ---" << std::endl;
    std::cerr << expected_result << std::endl;
    std::cerr << "--- found result ---" << std::endl;
    std::cerr << map1.to_string() << std::endl;
    std::cerr << "--- differences ---" << std::endl;
    std::cerr << result << std::endl;
  }
  return result.empty();
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_PROPERTY_MAP_H
