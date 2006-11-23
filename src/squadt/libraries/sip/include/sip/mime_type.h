#ifndef MIME_TYPE_H__
#define MIME_TYPE_H__

#include <string>

namespace sip {

  /**
   * \brief Class that provides a partial mime-type implementation
   **/
  class mime_type {

    public:

      /** \brief Recognised main types */
      enum main_type {
        application,
        audio,
        image,
        message,
        multipart,
        text,
        video,
        unknown
      };

    private:

      /** \brief Strings for conversion of main types */
      static char* const main_type_as_string[];

    private:

      /** \brief The main type */
      main_type   m_main;

      /** \brief The sub type */
      std::string m_sub;

    public:

      /** \brief Constructor */
      mime_type(std::string const&);

      /** \brief Constructor */
      mime_type(std::string const&, main_type s);

      /** \brief Gets the main type */
      std::string get_main_type() const;

      /** \brief Gets the sub type */
      std::string get_sub_type() const;

      /** \brief Converts to string */
      std::string as_string() const;

      /** \brief Converts to string */
      std::string to_string() const;

      /** \brief Compare for equality */
      bool operator== (mime_type const&) const;

      /** \brief Compare for inequality */
      bool operator!= (mime_type const&) const;

      /** \brief Compare for inequality */
      bool operator< (mime_type const&) const;
  };

  /** \brief Output to stream as string */
  inline std::ostream& operator<<(std::ostream& o, mime_type const& t) {
    o << t.as_string();

    return (o);
  }

  inline std::string mime_type::get_main_type() const {
    return (main_type_as_string[m_main]);
  }

  inline std::string mime_type::get_sub_type() const {
    return (main_type_as_string[m_main]);
  }

  inline std::string mime_type::as_string() const {
    return (std::string(main_type_as_string[m_main]) + "/" + m_sub);
  }

  inline std::string mime_type::to_string() const {
    return (std::string(main_type_as_string[m_main]) + "/" + m_sub);
  }

  inline bool mime_type::operator==(mime_type const& r) const {
    return (m_main == r.m_main && m_sub == r.m_sub);
  }

  inline bool mime_type::operator!=(mime_type const& r) const {
    return (m_main != r.m_main && m_sub != r.m_sub);
  }

  inline bool mime_type::operator<(mime_type const& r) const {
    return (m_main < r.m_main || (m_main == r.m_main && m_sub < r.m_sub));
  }
}

#endif
