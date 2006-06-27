/*======================================================================
//
// Copyright (c) 2005 2006 TU/e
//
// author     : Jeroen van der Wulp <J.v.d.Wulp@tue.nl>
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// About :
// 
//  This is a C++ interface wrapper around the XMLTextReader functionality
//  provided by the gnome libxml2 library.
//
//=====================================================================*/

#ifndef XML2PP_TEXT_READER_H
#define XML2PP_TEXT_READER_H

#include <string>
#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <xml2pp/detail/exception.h>

namespace xml2pp {

  class reader_wrapper;

  /** \brief A C++ wrapper around xmlTextReader from libxml2 */
  class text_reader : public boost::noncopyable {

    private:

      /** \brief Wrapper around a libXML2 text reader (Pimpl idiom) */
      boost::shared_ptr < reader_wrapper > wrapped_reader;

    public:

      /** \brief Constructor for reading from a file */
      text_reader(boost::filesystem::path const&);

      /** \brief Constructor for reading an in memory document */
      text_reader(const char*);

      /** \brief Constructor for reading an in memory document */
      text_reader(std::string const&);

      /** \brief Constructor for reading part of an in memory document */
      text_reader(const char*, const size_t prefix_length);

      /** \brief Constructor for reading part of an in memory document */
      text_reader(std::string const&, const size_t prefix_length);

      /** \brief Set schema for validation purposes */
      void set_schema(boost::filesystem::path const&);

      /** \brief Advances to the next element in the XML document tree */
      void next_element(unsigned int = 0);

      /** \brief Returns the value of a named attribute attribute as a string */
      std::string get_attribute_as_string(const char*) const;

      /** \brief Returns whether the attribute is present or not (or true/false) */
      bool get_attribute(const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(std::string* string, const char* attribute_name);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(char*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(unsigned char*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(short int*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(short unsigned int*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(int*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(unsigned int*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(long int*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(long unsigned int*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(float*, const char*);

      /** \brief Get the value of an attribute as ... (no change if the attribute is not present) */
      bool get_attribute(double*, const char*);

      /** \brief Get the value of an element as a string */
      std::string get_value_as_string() const;

      /** \brief Get the value of an element as a string */
      void get_value(std::string*) const;

      /** \brief Get the value of an element as a single char */
      void get_value(char*) const;

      /** \brief Get the value of an element as a single unsigned char */
      void get_value(unsigned char*) const;

      /** \brief Get the value of an element as a single int */
      void get_value(short int*) const;

      /** \brief Get the value of an element as a single unsigned int */
      void get_value(short unsigned int*) const;

      /** \brief Get the value of an element as a single int */
      void get_value(int*) const;

      /** \brief Get the value of an element as a single unsigned int */
      void get_value(unsigned int*) const;

      /** \brief Get the value of an element as a single long int */
      void get_value(long int*) const;

      /** \brief Get the value of an element as a single unsigned long int */
      void get_value(long unsigned int*) const;

      /** \brief Get the value of an element as a single float */
      void get_value(float*) const;

      /** \brief Get the value of an element as a single double */
      void get_value(double*) const;

      /** \brief Returns the name of the current element */
      std::string element_name() const;

      /** \brief Whether the current element matches element_name */
      bool is_element(const char*) const;

      /** \brief Whether the current element is an end of element tag */
      bool is_end_element() const;

      /** \brief Whether the current element is an end of element tag */
      bool is_end_element(const char*) const;

      /** \brief Skips to the next position if the current element is an end of element tag */
      bool skip_end_element(const char*);

      /** \brief Whether the current element is empty */
      bool is_empty_element() const;

      /** \brief Whether the reader currently points to a valid element */
      bool valid() const;
  };
}

#endif
