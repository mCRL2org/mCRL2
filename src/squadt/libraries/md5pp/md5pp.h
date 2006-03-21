#ifndef MD5PP_H
#define MD5PP_H

#include "source/md5.h"

#include <cstdio>
#include <iostream>

namespace md5 {

  union compact_digest {
    uint8_t  bytes[16]; ///< Subsequent bytes of a digest
    uint32_t dwords[4]; ///< Subsequent quad-bytes of a digest

    /** \brief Read digest from a string and convert to compact format */
    void read(const char*);

    /** \brief Whether digest is the zero checksum */
    inline bool is_zero() const;

    /** \brief Fill with zeroes */
    inline void zero_out();

    /** \brief Compare two compact digests for equality */
    inline bool operator== (const compact_digest& r) const;

    /** \brief Assign to ... */
    inline void operator= (const compact_digest& r);
  };

  /** \brief The digest containing only zeroes, used for initialisation */
  extern compact_digest zero_digest;

  /** \brief Whether digest is the zero checksum */
  inline bool compact_digest::is_zero() const {
    return (dwords[0] == 0 && dwords[1] == 0 && dwords[2] == 0 && dwords[3] == 0);
  }

  /** \brief Set to the zero checksum */
  inline void compact_digest::zero_out() {
    dwords[0] = 0;
    dwords[1] = 0;
    dwords[2] = 0;
    dwords[3] = 0;
  }

  /** \brief Compare to MD5 checksums in compact format */
  inline bool compact_digest::operator== (const compact_digest& r) const {
    return (dwords[0] == r.dwords[0] && dwords[1] == r.dwords[1] && dwords[2] == r.dwords[2] && dwords[3] == r.dwords[3]);
  }

  /* \brief Assign MD5 checksums in compact format */
  inline void compact_digest::operator= (const compact_digest& r) {
    dwords[0] = r.dwords[0];
    dwords[1] = r.dwords[1];
    dwords[2] = r.dwords[2];
    dwords[3] = r.dwords[3];
  }

  /** \brief Print the current digest to a stream */
  std::ostream& operator<< (std::ostream&, const compact_digest&);

  /**
   * Basic C++ wrapper around an MD5 context
   **/
  class MD5 {
    private:
      /** \brief Whether the digest was finalised */
      bool           finalised;
 
      /** \brief The MD5 context, until finalised */
      md5_context    context;
 
      /** \brief Holds a digest after finalisation */
      compact_digest digest;
 
    public:
 
      /** \brief Constructor */
      MD5();
 
      /** \brief Creates a new compact digest using data from a string */
      static compact_digest MD5_Sum(std::string);

      /** \brief Creates a new compact digest using data from a stream */
      static compact_digest MD5_Sum(std::istream&);
 
      /** \brief Update checksum with a specified amount of data from string */
      void update(unsigned char*, unsigned int);
 
      /** \brief Update checksum with data from string */
      void update(const std::string&);
 
      /** \brief Update checksum with data from stream */
      void update(std::istream&);
 
      /** \brief Complete checksum (update() is no longer possible) */
      void finalise();
 
      /** \brief Empty context */
      void clear();
 
      /** \brief Return the computed MD5 sum */
      const compact_digest & get_digest() const;
 
      /** \brief Print the current digest to a stream */
      friend std::ostream& operator<< (std::ostream&, const MD5&);
  };

  /**
   * @param[in] d a string with data
   * @param[in] l length of the previx of d that will be read as data
   **/
  inline void MD5::update(unsigned char* d, unsigned int l) {
    md5_update(&context, d, l);
  }

  /**
   * @param[in] d a string with data
   **/
  inline void MD5::update(const std::string& d) {
    update((unsigned char*) d.c_str(), d.size());
  }

  /**
   * @param[in] s a stream from which to read data
   **/
  inline void MD5::update(std::istream& s) {
    unsigned char buffer[1000];

    while (s.good()) {
      s.get(reinterpret_cast < char* > (buffer), 1000);

      update(buffer, s.gcount());
    }
  }

  inline void MD5::finalise() {
    md5_finish(&context, digest.bytes);

    finalised = true;
  }

  /**
   * @param[in] s a stream from which to read data
   * @param[in] c an MD5 context
   **/
  inline std::ostream& operator<< (std::ostream& s, const MD5& c) {
    if (c.finalised) {
      s << c.digest;
    }

    return (s);
  }

  inline void MD5::clear() {
    finalised = false;

    md5_starts(&context);
  }
}

#endif
