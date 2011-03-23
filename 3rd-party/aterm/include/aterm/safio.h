#ifndef SAFIO_H
#define SAFIO_H

#include "aterm1.h"
#include "aterm2.h"
#include "idmappings.h"

#define SAF_IDENTIFICATION_TOKEN '?'

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  /* Stores */
  typedef struct _ProtectedMemoryStack
  {
    ATerm** blocks;
    size_t* freeBlockSpaces;
    size_t nrOfBlocks;

    size_t currentBlockNr;
    ATerm* currentIndex;
    size_t spaceLeft;
  }* ProtectedMemoryStack;

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
    ATermMapping* stack;
    size_t stackSize;
    size_t stackPosition;

    IDMappings sharedTerms;
    int currentSharedTermKey;

    IDMappings sharedAFuns;
    int currentSharedAFunKey;

    ATerm currentTerm;
    size_t indexInTerm;
    size_t totalBytesInTerm;
  }* BinaryWriter;

  BinaryWriter ATcreateBinaryWriter(ATerm term);

  void ATserialize(BinaryWriter binaryWriter, ByteBuffer buffer);

  int ATisFinishedWriting(BinaryWriter binaryWriter);

  void ATdestroyBinaryWriter(BinaryWriter binaryWriter);


  /* For reading */
  typedef struct _ATermConstruct
  {
    size_t type;

    ATerm tempTerm;
    size_t termKey;

    size_t nrOfSubTerms;
    size_t subTermIndex;
    ATerm* subTerms;

  } ATermConstruct;

  typedef struct _BinaryReader
  {
    ProtectedMemoryStack protectedMemoryStack;

    ATermConstruct* stack;
    size_t stackSize;
    size_t stackPosition;

    ATerm* sharedTerms;
    size_t sharedTermsSize;
    size_t sharedTermsIndex;

    SymEntry* sharedAFuns;
    size_t sharedAFunsSize;
    size_t sharedAFunsIndex;

    char* tempNamePage;

    size_t tempType;
    char* tempBytes;
    size_t tempBytesSize;
    size_t tempBytesIndex;
    size_t tempArity;
    ATbool tempIsQuoted;

    int isDone;
  }* BinaryReader;

  BinaryReader ATcreateBinaryReader();

  void ATdeserialize(BinaryReader binaryReader, ByteBuffer buffer);

  int ATisFinishedReading(BinaryReader binaryReader);

  ATerm ATgetRoot(BinaryReader binaryReader);

  void ATdestroyBinaryReader(BinaryReader binaryReader);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAFIO_H */
