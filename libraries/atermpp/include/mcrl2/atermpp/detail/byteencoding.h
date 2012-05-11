#ifndef BYTEENCODING_H_
#define BYTEENCODING_H_

namespace atermpp
{

size_t BEserializeMultiByteInt(const int i, char* c);

int BEdeserializeMultiByteInt(const char* c, size_t* i);

} // namespace atermpp

#endif /*BYTEENCODING_H_*/
