#ifndef SAFIO_H
#define SAFIO_H

#include <stack>
#include <set>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/idmappings.h"

namespace atermpp
{

static const char SAF_IDENTIFICATION_TOKEN = '?';

/* Buffer */
typedef struct _ByteBuffer
{
  char* buffer;
  size_t capacity;

  char* currentPos;
  size_t limit;
}* ByteBuffer;

ByteBuffer ATcreateByteBuffer(size_t size);

ByteBuffer ATwrapBuffer(char* buffer, size_t capacity);

size_t ATgetRemainingBufferSpace(ByteBuffer byteBuffer);

void ATflipByteBuffer(ByteBuffer byteBuffer);

void ATresetByteBuffer(ByteBuffer byteBuffer);

void ATdestroyByteBuffer(ByteBuffer byteBuffer);


/* For writing */
typedef struct _ATermMapping
{
  aterm term;
  size_t subTermIndex;
  size_t nrOfSubTerms;
  aterm_list nextPartOfList; /* This is for a ATermList 'nextTerm' optimalization only. */
} ATermMapping;

typedef struct _BinaryWriter
{
  std::stack <ATermMapping> stack;

  IDMappings sharedTerms;
  int currentSharedTermKey;

  IDMappings sharedAFuns;
  int currentSharedAFunKey;

  aterm currentTerm;
  size_t indexInTerm;
  size_t totalBytesInTerm;
}* BinaryWriter;

BinaryWriter ATcreateBinaryWriter(const aterm term);

void ATserialize(BinaryWriter binaryWriter, ByteBuffer buffer);

int ATisFinishedWriting(BinaryWriter binaryWriter);

void ATdestroyBinaryWriter(BinaryWriter binaryWriter);


/* For reading */
typedef struct _ATermConstruct
{
  size_t type;

  _SymEntry* tempTerm;
  size_t termKey;

  size_t nrOfSubTerms;
  size_t subTermIndex;
  std::vector <aterm> subTerms;

} ATermConstruct;

typedef struct _BinaryReader
{
//   ProtectedMemoryStack protectedMemoryStack;

  std::stack <ATermConstruct> stack;

  std::vector<aterm> sharedTerms;
  std::set<function_symbol> protected_afuns;

  _SymEntry** sharedAFuns;
  size_t sharedAFunsSize;
  size_t sharedAFunsIndex;

  char* tempNamePage;

  size_t tempType;
  char* tempBytes;
  size_t tempBytesSize;
  size_t tempBytesIndex;
  size_t tempArity;
  bool tempIsQuoted;

  int isDone;
}* BinaryReader;

BinaryReader ATcreateBinaryReader();

void ATdeserialize(BinaryReader binaryReader, ByteBuffer buffer);

int ATisFinishedReading(BinaryReader binaryReader);

aterm ATgetRoot(BinaryReader binaryReader);

void ATdestroyBinaryReader(BinaryReader binaryReader);

bool ATwriteToSAFFile(const aterm &aTerm, FILE* file);

aterm ATreadFromSAFFile(FILE* file);

bool ATwriteToNamedSAFFile(const aterm& aterm, const char* filename);

aterm ATreadFromNamedSAFFile(const char* filename);

char* ATwriteToSAFString(const aterm &aTerm, size_t* length);

aterm ATreadFromSAFString(char* data, size_t length);


} // namespace aterm

#endif /* SAFIO_H */
