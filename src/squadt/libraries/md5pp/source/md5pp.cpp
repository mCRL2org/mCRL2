#include <iomanip> 
#include <cstdio>

#include <md5pp/md5pp.h>

namespace md5 {

  compact_digest zero_digest = { 0, 0, 0, 0, 0, 0, 0, 0 };

  void compact_digest::read(const char* string) {
    unsigned char* pointer = (unsigned char*) string;
    unsigned char  buffer[3];

    buffer[2] = '\0';

    for (unsigned int i = 0; i < 16; ++i) {
      unsigned int value;

      buffer[0] = pointer[0];
      buffer[1] = pointer[1];

      sscanf((const char*) buffer, "%x", &value);

      bytes[i] = value;
 
      pointer += 2;
    }
  }

  MD5::MD5() {
    finalised = false;

    md5_starts(&context);
  }

  compact_digest MD5::MD5_Sum(std::string data) {
    MD5 sum;

    sum.Update(data);
    sum.Finalise();

    return (sum.digest);
  }

  compact_digest MD5::MD5_Sum(std::istream& stream) {
    MD5 sum;

    sum.Update(stream);
    sum.Finalise();

    return (sum.digest);
  }

  std::ostream& operator<< (std::ostream& stream, const compact_digest& digest) {
    for (unsigned int i = 0; i < 16; ++i) {
      stream << std::hex << std::setw(2) << std::setfill('0') << (int) digest.bytes[i];
    }

    return (stream);
  }

}

