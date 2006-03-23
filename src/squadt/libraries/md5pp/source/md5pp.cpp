#include <iomanip> 
#include <cstdio>
#include <fstream>

#include <md5pp/md5pp.h>

namespace md5pp {

  compact_digest zero_digest = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

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

  /**
   * @param[in] s a string that contains the data
   **/
  compact_digest MD5::MD5_sum(const std::string s) {
    MD5 sum;

    sum.update(s);
    sum.finalise();

    return (sum.digest);
  }

  /**
   * @param[in] s a stream that contains the data
   **/
  compact_digest MD5::MD5_sum(std::istream& s) {
    MD5 sum;

    sum.update(s);
    sum.finalise();

    return (sum.digest);
  }

  /**
   * @param[in] p a path to the file that contains the data
   **/
  compact_digest MD5::MD5_sum(const boost::filesystem::path& p) {
    MD5 sum;

    std::ifstream s;
            
    s.open(p.string().c_str());

    assert (s.good());

    sum.update(s);
    sum.finalise();

    s.close();

    return (sum.digest);
  }

  std::ostream& operator<< (std::ostream& stream, const compact_digest& digest) {
    for (unsigned int i = 0; i < 16; ++i) {
      stream << std::hex << std::setw(2) << std::setfill('0') << (int) digest.bytes[i];
    }

    return (stream);
  }
}

