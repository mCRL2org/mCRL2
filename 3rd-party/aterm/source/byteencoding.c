#include "byteencoding.h"

union DoubleEncoding{
	double d;

	struct{
	#ifdef WORDS_BIGENDIAN
		unsigned int negative:1;
		unsigned int exponent:11;
		unsigned int mantissa0:20;
		unsigned int mantissa1:32;
	#else
	# ifdef FLOAT_WORDS_BIGENDIAN
		unsigned int mantissa0:20;
		unsigned int exponent:11;
		unsigned int negative:1;
		unsigned int mantissa1:32;
	# else
		unsigned int mantissa1:32;
		unsigned int mantissa0:20;
		unsigned int exponent:11;
		unsigned int negative:1;
	# endif
	#endif
	} EncodedDouble;
};

#if __STDC_VERSION__ >= 199901L
  /* "inline" is a keyword */
#else
# ifndef inline
#  define inline /* nothing */
# endif
#endif

/**
 * This file contains some routines for encoding and decoding integers and double in a portable way.
 */

#define SEVENBITS 0x0000007fU
#define SIGNBIT 0x80U

/**
 * Converts a signed integer (that uses the left most bit to store the sign) to a unsigned integer.
 */
inline static unsigned int signedToUnsignedInt(int signedInt){
	union _Integer{
		int s;
		unsigned int u;
	} Integer;
	Integer.s = signedInt;
	
	return Integer.u;
}

/**
 * Converts a unsigned integer to a signed integer (that uses the left most bit to store the sign).
 */
inline static int unsignedToSignedInt(unsigned int unsignedInt){
	union _Integer{
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
int BEserializeMultiByteInt(int i, char *c){
	unsigned int ui = signedToUnsignedInt(i);
	
	if((ui & 0xffffff80U) == 0){
		c[0] = (char) (ui & SEVENBITS);
		return 1;
	}
	c[0] = (char) ((ui & SEVENBITS) | SIGNBIT);
	
	if((ui & 0xffffc000U) == 0){
		c[1] = (char) ((ui >> 7) & SEVENBITS);
		return 2;
	}
	c[1] = (char) (((ui >> 7) & SEVENBITS) | SIGNBIT);
	
	if((ui & 0xffe00000U) == 0){
		c[2] = (char) ((ui >> 14) & SEVENBITS);
		return 3;
	}
	c[2] = (char) (((ui >> 14) & SEVENBITS) | SIGNBIT);
	
	if((ui & 0xf0000000U) == 0){
		c[3] = (char) ((ui >> 21) & SEVENBITS);
		return 4;
	}
	c[3] = (char) (((ui >> 21) & SEVENBITS) | SIGNBIT);
	
	c[4] = (char) ((ui >> 28) & SEVENBITS);
	return 5;
}

/**
 * Serializes a double value using IEEE 754 encoding.
 */
void BEserializeDouble(double d, char *c){
	unsigned int negative, exponent, mantissa0, mantissa1;
	union DoubleEncoding de;
	
	de.d = d;
	
	negative = de.EncodedDouble.negative;
	exponent = de.EncodedDouble.exponent;
	mantissa0 = de.EncodedDouble.mantissa0;
	mantissa1 = de.EncodedDouble.mantissa1;
	
	c[0] = mantissa1 & 0x000000ffU;
	c[1] = (mantissa1 & 0x0000ff00U) >> 8;
	c[2] = (mantissa1 & 0x00ff0000U) >> 16;
	c[3] = (mantissa1 & 0xff000000U) >> 24;
	
	c[4] = mantissa0 & 0x000000ffU;
	c[5] = (mantissa0 & 0x0000ff00U) >> 8;
	c[6] = ((mantissa0 & 0x000f0000U) >> 16) | ((exponent & 0x0000000fU) << 4);
	c[7] = ((exponent & 0x00007f0U) >> 4) | (negative & 0x00000001U) << 7;
}


/**
 * Deserializes a 'multi-byte' encoded integer from a sequence of bytes.
 * When this function returns the parameter 'unsigned int *count' will hold how many bytes where read during the decoding process.
 */
int BEdeserializeMultiByteInt(char *c, unsigned int *count){
	unsigned char part = (unsigned char) c[0];
	unsigned int result = (part & SEVENBITS);
	
	if((part & SIGNBIT) == 0){
		*count = 1;
		return unsignedToSignedInt(result);
	}
		
	part = (unsigned char) c[1];
	result |= ((part & SEVENBITS) << 7);
	if((part & SIGNBIT) == 0){
		*count = 2;
		return unsignedToSignedInt(result);
	}
		
	part = (unsigned char) c[2];
	result |= ((part & SEVENBITS) << 14);
	if((part & SIGNBIT) == 0){
		*count = 3;
		return unsignedToSignedInt(result);
	}
		
	part = (unsigned char) c[3];
	result |= ((part & SEVENBITS) << 21);
	if((part & SIGNBIT) == 0){
		*count = 4;
		return unsignedToSignedInt(result);
	}
		
	part = (unsigned char) c[4];
	result |= ((part & SEVENBITS) << 28);
	*count = 5;
	return unsignedToSignedInt(result);
}

/**
 * Deserializes a double in IEEE 754 encoding from the given sequence of bytes.
 */
double BEdeserializeDouble(char *c){
	union DoubleEncoding de;
	
	unsigned int mantissa1 = 	(c[0] & 0x000000ffU) |
								(c[1] & 0x000000ffU) << 8 |
								(c[2] & 0x000000ffU) << 16 |
								(c[3] & 0x000000ffU) << 24;
	
	unsigned int mantissa0 = 	(c[4] & 0x000000ffU) |
								(c[5] & 0x000000ffU) << 8 |
								(c[6] & 0x0000000fU) << 16;
	
	unsigned int exponent = 	(c[6] & 0x000000f0U) >> 4 |
								(c[7] & 0x0000007fU) << 4;
	
	unsigned int negative = (c[7] & 0x00000080U) >> 7;
	
	de.EncodedDouble.negative = negative;
	de.EncodedDouble.exponent = exponent;
	de.EncodedDouble.mantissa0 = mantissa0;
	de.EncodedDouble.mantissa1 = mantissa1;
	
	return de.d;
}
