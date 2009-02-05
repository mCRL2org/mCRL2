// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/detail/utility/logger.hpp

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <memory>
#include <iostream>
#include <fstream>

#include <boost/format.hpp>
#include <boost/filesystem/path.hpp>

namespace tipi {
  namespace utility {

    /** \brief Minimal interface for a logger component */
    class logger {

      public:

        /** \brief Type for log level specification */
        typedef unsigned char log_level;

      protected:

        /** \brief Target for writing the log messages */
        std::ofstream                    m_sink;

        /** \brief The log level below which all messages are printed */
        log_level                        m_filter_level;

      protected:

        /** \brief The default log level */
        static log_level& default_filter_level() {
          static logger::log_level default_level = 1;

          return default_level;
        }

      public:

        /** \brief Constructor */
        logger() : m_filter_level(default_filter_level()) { }

        /** \brief Constructor */
        logger(const log_level l);

        /** \brief Adds a log message with a string and a log level */
        void log(log_level, std::string const&);

        /** \brief Adds a log message with a format object and a log level */
        void log(log_level, boost::format const&);

        /** \brief Redirects output to ostream */
        void redirect(std::ostream&);

        /** \brief Redirects output to file */
        void redirect(boost::filesystem::path const& p);

        /** \brief Sets default filter level */
        static void set_default_filter_level(log_level l);

        /** \brief Sets default filter level */
        static log_level get_default_filter_level();

        /** \brief Sets filter level below which messages are logged */
        void set_filter_level(log_level l);

        /** \brief Returns filter level below which messages are logged */
        log_level get_filter_level() const;
    };

    /**
     * \param[in] l log level
     **/
    inline logger::logger(const log_level l) : m_filter_level(l) {
      m_sink.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);

      redirect(std::clog);
    }

    /**
     * \param[in] s stream to which log messages are written
     **/
    inline void logger::redirect(std::ostream& s) {
      static_cast < std::ostream& > (m_sink).rdbuf(s.rdbuf());
    }

    /**
     * \param[in] p path to which log messages are written
     **/
    inline void logger::redirect(boost::filesystem::path const& p) {
      m_sink.open(p.string().c_str(), std::ios::out);
    }

    /**
     * \param[in] l log level
     **/
    inline void logger::set_default_filter_level(log_level l) {
      default_filter_level() = l;
    }

    inline logger::log_level logger::get_default_filter_level() {
      return default_filter_level();
    }

    /**
     * @param[in] l the log level of the message
     * @param[in] m the message content
     **/
    inline void logger::log(log_level l, std::string const& m) {
      if (l < m_filter_level) {
        m_sink << m;
      }
    }

    /**
     * @param[in] l the log level of the message
     * @param[in] m the message content
     **/
    inline void logger::log(log_level l, boost::format const& m) {
      if (l < m_filter_level) {
        m_sink << m;
      }
    }

    /**
     * \param[in] l log level
     **/
    inline void logger::set_filter_level(log_level l) {
      m_filter_level = l;
    }

    inline logger::log_level logger::get_filter_level() const {
      return m_filter_level;
    }
  }
}

#endif
