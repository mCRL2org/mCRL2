#include <cstddef>
#include "mcrl2/atermpp/detail/byteencoding.h"

namespace atermpp
{

/**
 * This file contains some routines for encoding and decoding integers and double in a portable way.
 */

static const unsigned int SEVENBITS = 0x0000007fU;
static const unsigned int SIGNBIT = 0x80U;

/**
 * Converts a signed integer (that uses the left most bit to store the sign) to a unsigned integer.
 */
inline static unsigned int signedToUnsignedInt(int signedInt)
{
  union _Integer
  {
    int s;
    unsigned int u;
  } Integer;
  Integer.s = signedInt;

  return Integer.u;
}

/**
 * Converts a unsigned integer to a signed integer (that uses the left most bit to store the sign).
 */
inline static int unsignedToSignedInt(unsigned int unsignedInt)
{
  union _Integer
  {
    int s;
    unsigned int u;
  } Integer;
  Integer.u = unsignedInt;

  return Integer.s;
}


/**
 * Writes a signed integer value to a byte sequence using 'multi-byte' integer encoding.
 * Every byte will hold 7 bits of data and uses the left most bit to indicate if there are more bytes coming.
 * Every consecutive byte will have a greater significance.
 * This will save space when encoding small values (which are most frequent);
 * the down side is that encoding large and negative values will require one extra byte.
 */
size_t BEserializeMultiByteInt(const int i, char* c)
{
  unsigned int ui = signedToUnsignedInt(i);

  if ((ui & 0xffffff80U) == 0)
  {
    c[0] = (char)(ui & SEVENBITS);
    return 1;
  }
  c[0] = (char)((ui & SEVENBITS) | SIGNBIT);

  if ((ui & 0xffffc000U) == 0)
  {
    c[1] = (char)((ui >> 7) & SEVENBITS);
    return 2;
  }
  c[1] = (char)(((ui >> 7) & SEVENBITS) | SIGNBIT);

  if ((ui & 0xffe00000U) == 0)
  {
    c[2] = (char)((ui >> 14) & SEVENBITS);
    return 3;
  }
  c[2] = (char)(((ui >> 14) & SEVENBITS) | SIGNBIT);

  if ((ui & 0xf0000000U) == 0)
  {
    c[3] = (char)((ui >> 21) & SEVENBITS);
    return 4;
  }
  c[3] = (char)(((ui >> 21) & SEVENBITS) | SIGNBIT);

  c[4] = (char)((ui >> 28) & SEVENBITS);
  return 5;
}

/**
 * Deserializes a 'multi-byte' encoded integer from a sequence of bytes.
 * When this function returns the parameter 'unsigned int *count' will hold how many bytes where read during the decoding process.
 */
int BEdeserializeMultiByteInt(const char* c, size_t* count)
{
  unsigned char part = (unsigned char) c[0];
  unsigned int result = (part & SEVENBITS);

  if ((part & SIGNBIT) == 0)
  {
    *count = 1;
    return unsignedToSignedInt(result);
  }

  part = (unsigned char) c[1];
  result |= ((part & SEVENBITS) << 7);
  if ((part & SIGNBIT) == 0)
  {
    *count = 2;
    return unsignedToSignedInt(result);
  }

  part = (unsigned char) c[2];
  result |= ((part & SEVENBITS) << 14);
  if ((part & SIGNBIT) == 0)
  {
    *count = 3;
    return unsignedToSignedInt(result);
  }

  part = (unsigned char) c[3];
  result |= ((part & SEVENBITS) << 21);
  if ((part & SIGNBIT) == 0)
  {
    *count = 4;
    return unsignedToSignedInt(result);
  }

  part = (unsigned char) c[4];
  result |= ((part & SEVENBITS) << 28);
  *count = 5;
  return unsignedToSignedInt(result);
}

} // namespace atermpp
