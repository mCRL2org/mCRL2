#ifndef TOOL_MANAGER_H_
#define TOOL_MANAGER_H_

#include <algorithm>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>
#include <list>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include "tool.h"
#include "processor.h"
#include "executor.h"

namespace squadt {

  /**
   * \brief Basic component that provides information about a set of tools
   *
   * The tool manager is a special purpose database that is used to keep track
   * of a set of tools across system runs. To this end it can store and
   * retrieve tool information to/from persistent storage. Additionally the
   * tool manager provides functionality to help run, and once running, to
   * terminate tools.
   **/
  class tool_manager : public boost::noncopyable {
    friend class processor;
 
    public:
 
      /** \brief Convenience type alias for hiding shared pointer implementation */
      typedef boost::shared_ptr < tool_manager > ptr;

      /** \brief Convenience type alias the list of tools */
      typedef std::list < tool::ptr >            tool_list;
 
    private:
 
      /** \brief Type for output print format variants */
      enum output_variant {
        plain_text,          ///< Output is formatted plain text
        squadt_tool_catalog  ///< Output is squadt XML tool catalog format
      };

      /** \brief List of known tools */
      tool_list                tools;

    public:
 
      /** \brief Constructor */
      tool_manager();
 
      /** \brief Destructor */
      ~tool_manager();
 
      /** \brief Write configuration to stream */
      void write(std::ostream& = std::cout) const;
 
      /** \brief Read configuration from the default location */
      static tool_manager::ptr read();

      /** \brief Read configuration from file */
      static tool_manager::ptr read(const std::string&);

      /** \brief Read configuration with an XML text reader */
      static tool_manager::ptr read(xml2pp::text_reader&);
 
      /** \brief Establishes whether the named tool is among the known tools or not */
      inline bool exists(const std::string&) const;

      /** \brief Returns a tool by its name */
      inline tool& find(const std::string&) const;

      /** \brief Start a tool */
      void run(tool&, processor* = 0) const;

      /** \brief Add a new tool to the catalog */
      inline bool add(const std::string&, const std::string&);
 
      /** \brief Get the list of known tools */
      inline const tool_list& get_tools();
 
      /** \brief Get the number of known tools */
      inline const unsigned int number_of_tools() const;
 
      /** \brief Have the tool executor terminate all running tools */
      void terminate();
  };

  inline const std::list < tool::ptr >& tool_manager::get_tools() {
    return (tools);
  }

  inline const unsigned int tool_manager::number_of_tools() const {
    return (tools.size());
  }

  /**
   * @param n the name of the tool
   *
   * \pre a tool with this name must be among the known tools
   **/
  inline tool& tool_manager::find(const std::string& n) const {
    using namespace boost;

    return (*(*std::find_if(tools.begin(), tools.end(), 
               bind(std::equal_to< std::string >(), n, 
                       bind(&tool::get_name,
                               bind(&tool::ptr::get, _1))))).get());
  }

  /**
   * @param n the name of the tool
   **/
  inline bool tool_manager::exists(const std::string& n) const {
    using namespace boost;

    return (tools.end() != std::find_if(tools.begin(), tools.end(), 
               bind(std::equal_to< std::string >(), n, 
                       bind(&tool::get_name,
                               bind(&tool::ptr::get, _1)))));
  }

  /**
   * @param n the name of the tool
   *
   * \return whether the tool was added or not
   **/
  inline bool tool_manager::add(const std::string& n, const std::string& l) {
    bool b = exists(n);

    if (!b) {
      tools.push_back(tool::ptr(new tool(n, l)));
    }

    return (b);
  }
}

#endif
