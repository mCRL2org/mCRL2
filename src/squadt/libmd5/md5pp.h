#ifndef MD5PP_H
#define MD5PP_H

#include "md5.h"

#include <cstdio>
#include <iostream>

namespace md5 {

  typedef union {
    uint8_t bytes[16];
    uint8_t dwords[4];
  } compact_digest;

  /* Set to the zero checksum */
  inline void zero_out(compact_digest& digest) {
    digest.dwords[0] = 0;
    digest.dwords[1] = 0;
    digest.dwords[2] = 0;
    digest.dwords[3] = 0;
  }

  /* Whether digest is the zero checksum */
  inline bool is_zero(const compact_digest& digest) {
    return (digest.dwords[0] == 0 && digest.dwords[1] == 0 && digest.dwords[2] == 0 && digest.dwords[3] == 0);
  }

  /* Compare to MD5 checksums in compact format */
  inline bool   operator== (const compact_digest& l, const compact_digest& r) {
    return (l.dwords[0] == r.dwords[0] && l.dwords[1] == r.dwords[1] && l.dwords[2] == r.dwords[2] && l.dwords[3] == r.dwords[3]);
  }

  /* Assign MD5 checksums in compact format */
  inline void assign(compact_digest& l, const compact_digest& r) {
    l.dwords[0] = r.dwords[0];
    l.dwords[1] = r.dwords[1];
    l.dwords[2] = r.dwords[2];
    l.dwords[3] = r.dwords[3];
  }

  /* Read digest from a string and convert to compact format */
  void convert(compact_digest& space, const char*);

  std::ostream& operator<< (std::ostream&, const compact_digest&);

  class MD5 {
    private:
      bool finalised;
 
      md5_context    context;
 
      compact_digest digest;
 
    public:
 
      MD5();
 
      static compact_digest MD5_Sum(std::string);
      static compact_digest MD5_Sum(std::istream&);
 
      void Update(unsigned char*, unsigned int);
 
      void Update(std::string);
 
      /* Import from opened istream */
      void Update(std::istream&);
 
      void Finalise();
 
      /* Empty context */
      void Clear();
 
      const compact_digest & GetDigest() const;
 
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
