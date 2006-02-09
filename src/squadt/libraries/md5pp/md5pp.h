#ifndef MD5PP_H
#define MD5PP_H

#include "source/md5.h"

#include <cstdio>
#include <iostream>

namespace md5 {

  union compact_digest {
    uint8_t  bytes[16];
    uint32_t dwords[4];

    /* Read digest from a string and convert to compact format */
    void read(const char*);

    /** Whether digest is the zero checksum */
    inline bool is_zero() const;

    /** Fill with zeroes */
    inline void zero_out();

    /** Compare two compact digests for equality */
    inline bool operator== (const compact_digest& r) const;

    /** Assign to ... */
    inline void operator= (const compact_digest& r);
  };

  extern compact_digest zero_digest;

  /* Whether digest is the zero checksum */
  inline bool compact_digest::is_zero() const {
    return (dwords[0] == 0 && dwords[1] == 0 && dwords[2] == 0 && dwords[3] == 0);
  }

  /* Set to the zero checksum */
  inline void compact_digest::zero_out() {
    dwords[0] = 0;
    dwords[1] = 0;
    dwords[2] = 0;
    dwords[3] = 0;
  }

  /* Compare to MD5 checksums in compact format */
  inline bool compact_digest::operator== (const compact_digest& r) const {
    return (dwords[0] == r.dwords[0] && dwords[1] == r.dwords[1] && dwords[2] == r.dwords[2] && dwords[3] == r.dwords[3]);
  }

  /* Assign MD5 checksums in compact format */
  inline void compact_digest::operator= (const compact_digest& r) {
    dwords[0] = r.dwords[0];
    dwords[1] = r.dwords[1];
    dwords[2] = r.dwords[2];
    dwords[3] = r.dwords[3];
  }

  std::ostream& operator<< (std::ostream&, const compact_digest&);

  class MD5 {
    private:
      bool finalised;
 
      md5_context    context;
 
      compact_digest digest;
 
    public:
 
      /** Constructor */
      MD5();
 
      /** Creates a new compact digest using data from a string */
      static compact_digest MD5_Sum(std::string);

      /** Creates a new compact digest using data from a stream */
      static compact_digest MD5_Sum(std::istream&);
 
      /** Update checksum with a specified amount of data from string */
      void Update(unsigned char*, unsigned int);
 
      /** Update checksum with data from string */
      void Update(std::string);
 
      /** Update checksum with data from stream */
      void Update(std::istream&);
 
      /** Complete checksum (Update() is no longer possible) */
      void Finalise();
 
      /** Empty context */
      void Clear();
 
      /** Return the computed MD5 sum */
      const compact_digest & GetDigest() const;
 
      /** Print the current digest to a stream */
      friend std::ostream& operator<< (std::ostream&, const MD5&);
  };

  inline void MD5::Update(unsigned char* content, unsigned int length) {
    md5_update(&context, content, length);
  }

  inline void MD5::Update(std::string data) {
    Update((unsigned char*) data.c_str(), data.size());
  }

  inline void MD5::Update(std::istream& stream) {
    unsigned char buffer[1000];

    while (stream.good()) {
      stream.get(reinterpret_cast < char* > (buffer), 1000);

      Update(buffer, stream.gcount());
    }
  }

  inline void MD5::Finalise() {
    md5_finish(&context, digest.bytes);

    finalised = true;
  }

  inline std::ostream& operator<< (std::ostream& stream, const MD5& context) {
    if (context.finalised) {
      stream << context.digest;
    }

    return (stream);
  }

  inline void MD5::Clear() {
    finalised = false;

    md5_starts(&context);
  }

}

#endif
