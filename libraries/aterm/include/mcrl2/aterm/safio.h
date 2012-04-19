#ifndef SAFIO_H
#define SAFIO_H

#include <stack>
#include "mcrl2/aterm/aterm1.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/idmappings.h"

namespace aterm
{

static const char SAF_IDENTIFICATION_TOKEN = '?';

/* Stores */
/* typedef struct _ProtectedMemoryStack
{
  ATerm** blocks;
  size_t* freeBlockSpaces;
  size_t nrOfBlocks;

  size_t currentBlockNr;
  ATerm* currentIndex;
  size_t spaceLeft;
}* ProtectedMemoryStack; */

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
  ATerm term;
  size_t subTermIndex;
  size_t nrOfSubTerms;
  ATermList nextPartOfList; /* This is for a ATermList 'nextTerm' optimalization only. */
} ATermMapping;

typedef struct _BinaryWriter
{
  std::stack <ATermMapping> stack;
  /* ATermMapping* stack;
  size_t stackSize;
  size_t stackPosition; */

  IDMappings sharedTerms;
  int currentSharedTermKey;

  IDMappings sharedAFuns;
  int currentSharedAFunKey;

  ATerm currentTerm;
  size_t indexInTerm;
  size_t totalBytesInTerm;
}* BinaryWriter;

BinaryWriter ATcreateBinaryWriter(const ATerm term);

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
  std::vector <ATerm> subTerms;

} ATermConstruct;

typedef struct _BinaryReader
{
//   ProtectedMemoryStack protectedMemoryStack;

  std::stack <ATermConstruct> stack;
/*   size_t stackSize;
  size_t stackPosition; */

  /* ATerm* sharedTerms;
  size_t sharedTermsSize;
  size_t sharedTermsIndex; */

  std::vector<ATerm> sharedTerms;
  std::set<AFun> protected_afuns;

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

ATerm ATgetRoot(BinaryReader binaryReader);

void ATdestroyBinaryReader(BinaryReader binaryReader);

} // namespace aterm

#endif /* SAFIO_H */
