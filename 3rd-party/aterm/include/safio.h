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
typedef struct _ProtectedMemoryStack{
	ATerm **blocks;
	unsigned int *freeBlockSpaces;
	unsigned int nrOfBlocks;
	
	unsigned int currentBlockNr;
	ATerm *currentIndex;
	unsigned int spaceLeft;
} *ProtectedMemoryStack;

/* Buffer */
typedef struct _ByteBuffer{
	char *buffer;
	unsigned int capacity;
	
	char *currentPos;
	unsigned int limit;
} *ByteBuffer;

ByteBuffer ATcreateByteBuffer(unsigned int size);

ByteBuffer ATwrapBuffer(char *buffer, unsigned int capacity);

unsigned int ATgetRemainingBufferSpace(ByteBuffer byteBuffer);

void ATflipByteBuffer(ByteBuffer byteBuffer);

void ATresetByteBuffer(ByteBuffer byteBuffer);

void ATdestroyByteBuffer(ByteBuffer byteBuffer);


/* For writing */
typedef struct _ATermMapping{
	ATerm term;
	unsigned int subTermIndex;
	unsigned int nrOfSubTerms;
	unsigned int annosDone;
	
	ATermList nextPartOfList; /* This is for a ATermList 'nextTerm' optimalization only. */
} ATermMapping;

typedef struct _BinaryWriter{
	ATermMapping *stack;
	int stackSize;
	int stackPosition;
	
	IDMappings sharedTerms;
	int currentSharedTermKey;
	
	IDMappings sharedSymbols;
	int currentSharedSymbolKey;
	
	ATerm currentTerm;
	unsigned int indexInTerm;
	unsigned int totalBytesInTerm;
} *BinaryWriter;

BinaryWriter ATcreateBinaryWriter(ATerm term);

void ATserialize(BinaryWriter binaryWriter, ByteBuffer buffer);

int ATisFinishedWriting(BinaryWriter binaryWriter);

void ATdestroyBinaryWriter(BinaryWriter binaryWriter);


/* For reading */
typedef struct _ATermConstruct{
	unsigned int type;
	
	ATerm tempTerm;
	unsigned int termKey;
	
	unsigned int nrOfSubTerms;
	unsigned int subTermIndex;
	ATerm *subTerms;
	
	unsigned int hasAnnos;
	ATerm annos;
} ATermConstruct;

typedef struct _BinaryReader{
	ProtectedMemoryStack protectedMemoryStack;
	
	ATermConstruct *stack;
	unsigned int stackSize;
	int stackPosition;
	
	ATerm *sharedTerms;
	unsigned int sharedTermsSize;
	unsigned int sharedTermsIndex;
	
	SymEntry *sharedSymbols;
	unsigned int sharedSymbolsSize;
	unsigned int sharedSymbolsIndex;
	
	char *tempNamePage;
	
	unsigned int tempType;
	char *tempBytes;
	unsigned int tempBytesSize;
	unsigned int tempBytesIndex;
	unsigned int tempArity;
	ATbool tempIsQuoted;
	
	unsigned int isDone;
} *BinaryReader;

BinaryReader ATcreateBinaryReader();

void ATdeserialize(BinaryReader binaryReader, ByteBuffer buffer);

int ATisFinishedReading(BinaryReader binaryReader);

ATerm ATgetRoot(BinaryReader binaryReader);

void ATdestroyBinaryReader(BinaryReader binaryReader);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAFIO_H */
