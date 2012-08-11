#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdexcept>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/detail/util.h"
#include "mcrl2/atermpp/detail/safio.h"
#include "mcrl2/atermpp/detail/byteencoding.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_int.h"

namespace atermpp
{

static const size_t ISSHAREDFLAG = 0x00000080U;
static const size_t TYPEMASK = 0x0000000fU;

static const size_t FUNSHARED = 0x00000040U;
static const size_t APPLQUOTED = 0x00000020U;

static const size_t DEFAULTSTACKSIZE = 256;
static const size_t STACKSIZEINCREMENT = 512;

static const size_t MINIMUMFREEBUFFERSPACE = 10;

static const size_t TEMPNAMEPAGESIZE = 4096;

static const size_t DEFAULTSHAREDTERMARRAYSIZE = 1024;
static const size_t SHAREDTERMARRAYINCREMENT = 2048;
static const size_t DEFAULTSHAREDSYMBOLARRAYSIZE = 1024;
static const size_t SHAREDSYMBOLARRAYINCREMENT = 2048;

static const size_t INTEGERSTOREBLOCKSINCREMENT = 16;
static const size_t INTEGERSTOREBLOCKSINCREMENTMASK = 0x0000000fU;
static const size_t INTEGERSTOREBLOCKSIZE = 1024;
static const size_t INTEGERSTOREBLOCKSIZEMASK = 0x000003ffU;
static const size_t INTEGERSTOREBLOCKSIZEBITS = 10;

static const size_t PROTECTEDMEMORYSTACKBLOCKSINCREMENT = 16;
static const size_t PROTECTEDMEMORYSTACKBLOCKSINCREMENTMASK = 0x0000000fU;
static const size_t PROTECTEDMEMORYSTACKBLOCKSIZE = 1024;


static std::vector<aterm> getProtectedMemoryBlock(/* ProtectedMemoryStack protectedMemoryStack,*/ size_t size)
{
  return std::vector<aterm>(size);
}

/* BYTE BUFFER */

/**
 * Creates a byte buffer, with the given capacity.
 */
ByteBuffer ATcreateByteBuffer(size_t capacity)
{
  char* buffer;

  ByteBuffer byteBuffer = (ByteBuffer) malloc(sizeof(struct _ByteBuffer));
  if (byteBuffer == NULL)
  {
    std::runtime_error("Failed to allocate byte buffer.");
  }

  buffer = (char*) malloc(capacity * sizeof(char));
  if (buffer == NULL)
  {
    std::runtime_error("Failed to allocate buffer string for the byte buffer.");
  }
  byteBuffer->buffer = buffer;
  byteBuffer->currentPos = buffer;

  byteBuffer->capacity = capacity;
  byteBuffer->limit = capacity;

  return byteBuffer;
}

/**
 * Encapsulates the given string in a byte buffer.
 * The position will be set at the start and the limit to the capacity.
 * NOTE: The given string MUST have been allocated using malloc, so it can be passed to free() when destroying this bytebuffer.
 * Alternatively you could set the buffer field to NULL, before passing this buffer to the ATdestroyByteBuffer function and handle the freeing of the buffer manually.
 */
ByteBuffer ATwrapBuffer(char* buffer, size_t capacity)
{
  ByteBuffer byteBuffer = (ByteBuffer) malloc(sizeof(struct _ByteBuffer));
  if (byteBuffer == NULL)
  {
    std::runtime_error("Failed to allocate byte buffer.");
  }

  byteBuffer->buffer = buffer;
  byteBuffer->currentPos = buffer;

  byteBuffer->capacity = capacity;
  byteBuffer->limit = capacity;

  return byteBuffer;
}

/**
 * Returns the amount of space left in the given byte buffer.
 */
inline size_t ATgetRemainingBufferSpace(ByteBuffer byteBuffer)
{
  return (size_t)(byteBuffer->limit - (byteBuffer->currentPos - byteBuffer->buffer));
}

/**
 * Flips the byte buffer.
 * Setting the limit to the amount of bytes that are currently present in the buffer and setting the current position to the start of the buffer.
 */
inline void ATflipByteBuffer(ByteBuffer byteBuffer)
{
  byteBuffer->limit = (size_t)(byteBuffer->currentPos - byteBuffer->buffer);
  byteBuffer->currentPos = byteBuffer->buffer;
}

/**
 * Resets the given byte buffer.
 * The current position will be reset to the start of the buffer and the limit will be set to the capacity.
 * NOTE: Keep in mind that the data in the buffer will not be erased (not set to 0).
 */
inline void ATresetByteBuffer(ByteBuffer byteBuffer)
{
  byteBuffer->currentPos = byteBuffer->buffer;
  byteBuffer->limit = byteBuffer->capacity;
}

/**
 * Frees the memory associated with the given byte buffer.
 */
void ATdestroyByteBuffer(ByteBuffer byteBuffer)
{
  free(byteBuffer->buffer);

  free(byteBuffer);
}


/* WRITING */

/**
 * Writes the given integer to the byte buffer.
 * The encoding will be done in 'byteenconding.c'.
 */
inline static void writeInt(int value, ByteBuffer byteBuffer)
{
  byteBuffer->currentPos += BEserializeMultiByteInt(value, byteBuffer->currentPos);
}

/**
 * Writes the given double to the byte buffer.
 * The encoding will be done in 'byteenconding.c'.
 */
/* inline static void writeDouble(double value, ByteBuffer byteBuffer){
  BEserializeDouble(value, byteBuffer->currentPos);
  byteBuffer->currentPos += 8;
} */

/**
 * Returns the number of subTerms the given term has.
 */
inline static size_t getNrOfSubTerms(aterm term)
{
  size_t type = term.type();
  if (type == AT_APPL)
  {
    return aterm_appl(term).size();
  }
  else if (type == AT_LIST)
  {
    return aterm_list(term).size();
    /* }else if(type == AT_PLACEHOLDER){
      return 1; */
  }
  else
  {
    return 0;
  }
}

/**
 * Ensures that there is enough space left on the stack of the binary writer after the invocation of this function.
 */
/* static void ensureWriteStackCapacity(BinaryWriter binaryWriter)
{
  if ((binaryWriter->stackPosition + 1) >= binaryWriter->stackSize)
  {
    binaryWriter->stack = (ATermMapping*) AT_realloc(binaryWriter->stack, (binaryWriter->stackSize += STACKSIZEINCREMENT) * sizeof(struct _ATermMapping));
    if (binaryWriter->stack == NULL)
    {
      std::runtime_error("The binary writer was unable to enlarge the stack.");
    }
  }
} */

/**
 * Returns a reference to the next aterm that needs to be serialized to the stream the binary writer is working on.
 */
static aterm getNextTerm(BinaryWriter binaryWriter)
{
  aterm next;

  /* Make sure the stack remains large enough */
  // ensureWriteStackCapacity(binaryWriter);

  /* if(binaryWriter->stackPosition >= 0) */
  {
    size_t type;
    aterm term;
    ATermMapping* child;

    // ATermMapping* current = &(binaryWriter->stack[binaryWriter->stackPosition]);
    ATermMapping* current = &(binaryWriter->stack.top());
    while (current->subTermIndex == current->nrOfSubTerms)
    {
      binaryWriter->stack.pop();
      /* if (binaryWriter->stackPosition-- == 0)
      {
        return aterm();
      } */
      if (binaryWriter->stack.empty())
      {
        return aterm();
      }
      // current = &(binaryWriter->stack[binaryWriter->stackPosition]);
      current = &(binaryWriter->stack.top());
    }

    term = current->term;
    type = term.type();

    // child = &(binaryWriter->stack[++(binaryWriter->stackPosition)]);
    binaryWriter->stack.push(ATermMapping());
    child = &(binaryWriter->stack.top());

    if (type == AT_APPL)
    {
      next = aterm_appl(term)(current->subTermIndex++);
    }
    else if (type == AT_LIST)
    {
      aterm_list nextList = current->nextPartOfList;
      next = nextList.front();
      current->nextPartOfList = nextList.tail();

      current->subTermIndex++;
    }
    else
    {
      std::runtime_error("Could not find next term. Someone broke the above code.");
    }

    child->term = next;
    child->nrOfSubTerms = getNrOfSubTerms(next);
    child->subTermIndex = 0; /* Default value */
  }

  return next;
}

/**
 * Constructs the header for the given aterm.
 */
inline static size_t getHeader(aterm aTerm)
{
  size_t header = aTerm.type();

  return header;
}

/**
 * Serializes the given aterm_appl.
 */
static void visitAppl(BinaryWriter binaryWriter, aterm_appl arg, ByteBuffer byteBuffer)
{
  function_symbol fun = arg.function();

  if (binaryWriter->indexInTerm == 0)
  {
    // detail::_function_symbol* symEntry = detail::at_lookup_table[fun.number()];
    size_t symEntry = fun.number();
    size_t funHash = (size_t)((unsigned long) symEntry);

    IDMappings sharedAFuns = binaryWriter->sharedAFuns;
    size_t id = IMgetID(sharedAFuns, (void*)symEntry, funHash);

    size_t header = getHeader(arg);

    if (id != ATERM_NON_EXISTING_POSITION)
    {
      header |= FUNSHARED;
      *(byteBuffer->currentPos) = (char) header;
      byteBuffer->currentPos++;

      assert(id< (((size_t)1)<<(8*sizeof(int)-1))); /* id must fit in an int */
      writeInt((int)id, byteBuffer);
    }
    else
    {
      size_t remaining;

      std::string name = fun.name(); 
      size_t arity = fun.arity();
      size_t nameLength = name.size();

      size_t bytesToWrite = nameLength;

      header |= APPLQUOTED;
      *(byteBuffer->currentPos) = (char) header;
      byteBuffer->currentPos++;
      assert(arity< (((size_t)1)<<(8*sizeof(int)-1))); /* arity must fit in an int */
      writeInt((int)arity, byteBuffer);

      assert(nameLength< (((size_t)1)<<(8*sizeof(int)-1))); /* nameLength must fit in an int */
      writeInt((int)nameLength, byteBuffer);

      remaining = ATgetRemainingBufferSpace(byteBuffer);
      if (remaining < bytesToWrite)
      {
        bytesToWrite = remaining;
        binaryWriter->indexInTerm = bytesToWrite;
        binaryWriter->totalBytesInTerm = nameLength;
      }

      memcpy(byteBuffer->currentPos, name.c_str(), bytesToWrite);
      byteBuffer->currentPos += bytesToWrite;

      id = binaryWriter->currentSharedAFunKey++;

      IMmakeIDMapping(sharedAFuns, (void*)symEntry, funHash, id);
    }
  }
  else
  {
    std::string name = fun.name();
    size_t length = binaryWriter->totalBytesInTerm;

    size_t bytesToWrite = length - binaryWriter->indexInTerm;
    size_t remaining = ATgetRemainingBufferSpace(byteBuffer);
    if (remaining < bytesToWrite)
    {
      bytesToWrite = remaining;
    }

    memcpy(byteBuffer->currentPos, (name.c_str() + binaryWriter->indexInTerm), bytesToWrite);
    byteBuffer->currentPos += bytesToWrite;
    binaryWriter->indexInTerm += bytesToWrite;

    if (binaryWriter->indexInTerm == length)
    {
      binaryWriter->indexInTerm = 0;
    }
  }
}

/**
 * Serializes the given aterm_int
 */
static void visitInt(aterm_int arg, ByteBuffer byteBuffer)
{
  *(byteBuffer->currentPos) = (char) getHeader(arg);
  byteBuffer->currentPos++;

  writeInt(arg.value(), byteBuffer);
}

/**
 * Serializes the given aterm_list.
 */
static void visitList(aterm_list arg, ByteBuffer byteBuffer)
{
  const size_t n=arg.size();
  *(byteBuffer->currentPos) = (char) getHeader(arg);
  byteBuffer->currentPos++;

  assert(n< (((size_t)1)<<(8*sizeof(int)-1))); /* n must fit in an int */
  writeInt((int)n, byteBuffer);
}

/**
 * Constructs a binary writer that is responsible for serializing the given aterm.
 */
BinaryWriter ATcreateBinaryWriter(const aterm term)
{
  // ATermMapping* stack;
  ATermMapping* tm;

  BinaryWriter binaryWriter = new _BinaryWriter; // (BinaryWriter) malloc(sizeof(struct _BinaryWriter));
  /* if (binaryWriter == NULL)
  {
    std::runtime_error("Unable to allocate memory for the binary writer.");
  } */

  // stack = new _ATermMapping; // (ATermMapping*) malloc(DEFAULTSTACKSIZE * sizeof(struct _ATermMapping));
  /* if (stack == NULL)
  {
    std::runtime_error("Unable to allocate memory for the binaryWriter's stack.");
  } */
  /* binaryWriter->stack = stack;
  binaryWriter->stackSize = DEFAULTSTACKSIZE;
  binaryWriter->stackPosition = 0; */

  binaryWriter->sharedTerms = IMcreateIDMappings(75);
  binaryWriter->currentSharedTermKey = 0;

  binaryWriter->sharedAFuns = IMcreateIDMappings(75);
  binaryWriter->currentSharedAFunKey = 0;

  binaryWriter->currentTerm = term;
  binaryWriter->indexInTerm = 0;

  binaryWriter->stack.push(ATermMapping());
  tm = &(binaryWriter->stack.top());
  tm->term = term;
  tm->nrOfSubTerms = getNrOfSubTerms(term);
  tm->subTermIndex = 0; /* Default value */

  return binaryWriter;
}

/**
 * Checks if the given binary writer is finished with the serialization process.
 * Returns > 0 is true, 0 otherwise.
 */
int ATisFinishedWriting(BinaryWriter binaryWriter)
{
  int finished = 0;
  if (binaryWriter->currentTerm == aterm())
  {
    finished = 1;
  }

  return finished;
}

/**
 * Frees the memory associated with the given binary writer.
 */
void ATdestroyBinaryWriter(BinaryWriter binaryWriter)
{
  // free(binaryWriter->stack);
  // delete binaryWriter->stack;

  IMdestroyIDMappings(binaryWriter->sharedTerms);

  IMdestroyIDMappings(binaryWriter->sharedAFuns);

  delete binaryWriter;
}

/**
 * Serializes the next part of the aterm tree associated with the given binary writers to the byte buffer.
 * This function returns when the byte buffer is full; the buffer will be flipped before returning.
 */
void ATserialize(BinaryWriter binaryWriter, ByteBuffer byteBuffer)
{
  aterm currentTerm = binaryWriter->currentTerm;

  while (currentTerm != aterm() && ATgetRemainingBufferSpace(byteBuffer) >= MINIMUMFREEBUFFERSPACE)
  {
    size_t termHash = (size_t)(&*currentTerm);
    size_t id = IMgetID(binaryWriter->sharedTerms, &*currentTerm, termHash);
    if (id != ATERM_NON_EXISTING_POSITION)
    {
      *(byteBuffer->currentPos) = (char) ISSHAREDFLAG;
      byteBuffer->currentPos++;
      assert(id< (((size_t)1)<<(8*sizeof(int)-1))); /* id must fit in an int */
      writeInt((int)id, byteBuffer);

      // binaryWriter->stackPosition--; /* Pop the term from the stack, since it's subtree is shared. */
      binaryWriter->stack.pop();
    }
    else
    {
      size_t type = currentTerm.type();
      switch (type)
      {
        case AT_APPL:
          visitAppl(binaryWriter, aterm_appl(currentTerm), byteBuffer);
          break;
        case AT_INT:
          visitInt(aterm_int(currentTerm), byteBuffer);
          break;
        case AT_LIST:
          visitList(aterm_list(currentTerm), byteBuffer);
          binaryWriter->stack.top().nextPartOfList = aterm_list(currentTerm); /* <- for aterm_list->next optimizaton. */
          break;
        default:
          std::runtime_error(to_string(type) + " is not a valid term type.");
      }

      /* Don't add the term to the shared list until we are completely done with it. */
      if (binaryWriter->indexInTerm == 0)
      {
        id = binaryWriter->currentSharedTermKey++;
        IMmakeIDMapping(binaryWriter->sharedTerms, &*currentTerm, termHash, id);
      }
      else
      {
        break;
      }
    }

    currentTerm = getNextTerm(binaryWriter);
  }

  binaryWriter->currentTerm = currentTerm;

  ATflipByteBuffer(byteBuffer);
}


/* READING */

/**
 * Reads an integer from the given byte buffer.
 * The decoding will be done in 'byteencoding.c'.
 */
inline static int readInt(ByteBuffer byteBuffer)
{
  size_t count;
  int result = BEdeserializeMultiByteInt(byteBuffer->currentPos, &count);
  byteBuffer->currentPos += count;

  return result;
}

/**
 * Reads a double from the given byte buffer.
 * The decoding will be done in 'byteencoding.c'.
 */
/* inline static double readDouble(ByteBuffer byteBuffer){
  double result = BEdeserializeDouble(byteBuffer->currentPos);
  byteBuffer->currentPos += 8;

  return result;
} */

/**
 * Ensures that there is enough space left on the stack of the binary reader after the invocation of this function.
 */
/* static void ensureReadStackCapacity(BinaryReader binaryReader)
{
  if ((binaryReader->stackPosition + 1) >= binaryReader->stackSize)
  {
    binaryReader->stack = (ATermConstruct*) AT_realloc(binaryReader->stack, (binaryReader->stackSize += STACKSIZEINCREMENT) * sizeof(struct _ATermConstruct));
    if (binaryReader->stack == NULL)
    {
      std::runtime_error("Unable to allocate memory for expanding the binaryReader's stack.");
    }
  }
} */

/**
 * Ensures that there is enough space left in the shared terms array of the binary reader after the invocation of this function.
 */
/*static void ensureReadSharedTermCapacity(BinaryReader binaryReader)
{
  if ((binaryReader->sharedTermsIndex + 1) >= binaryReader->sharedTermsSize)
  {
    binaryReader->sharedTerms = (aterm*) AT_realloc(binaryReader->sharedTerms, (binaryReader->sharedTermsSize += SHAREDTERMARRAYINCREMENT) * sizeof(aterm));
    if (binaryReader->sharedTerms == NULL)
    {
      std::runtime_error("Unable to allocate memory for expanding the binaryReader's shared terms array.");
    }
  }
} */

/**
 * Ensures that there is enough space left in the shared signatures array of the binary reader after teh invocation of this function.
 */
static void ensureReadSharedAFunCapacity(BinaryReader binaryReader)
{
  if ((binaryReader->sharedAFunsIndex + 1) >= binaryReader->sharedAFunsSize)
  {
    binaryReader->sharedAFuns = (size_t*) realloc(binaryReader->sharedAFuns, (binaryReader->sharedAFunsSize += SHAREDSYMBOLARRAYINCREMENT) * sizeof(size_t));
    if (binaryReader->sharedAFuns == NULL)
    {
      std::runtime_error("Unable to allocate memory for expanding the binaryReader's shared signatures array.");
    }
  }
}

/**
 * Resets the temporary reader data of the given binary reader.
 * This temporary data is used when reading 'chunkified types' (name of a function symbol or BLOB);
 * These types can be deserialized in pieces.
 */
inline static void resetTempReaderData(BinaryReader binaryReader)
{
  binaryReader->tempType = 0;

  /* It doesn't matter if tempBytes is NULL, since free(NULL) does nothing */
  free(binaryReader->tempBytes);
  binaryReader->tempBytes = NULL;

  binaryReader->tempBytesSize = 0;
  binaryReader->tempBytesIndex = 0;

  binaryReader->tempArity = 0;
  binaryReader->tempIsQuoted = 0;
}

/**
 * Adds the given term to the shared terms list.
 */
inline static void shareTerm(BinaryReader binaryReader, ATermConstruct* ac, aterm term)
{
  binaryReader->sharedTerms[ac->termKey] = term;
}

/**
 * Constructs the term associated with the given aterm construct.
 */
static aterm buildTerm(BinaryReader /* binaryReader*/, ATermConstruct* parent)
{
  aterm constructedTerm;
  size_t type = parent->type;

  if (type == AT_APPL)
  {
    size_t nrOfSubTerms = parent->nrOfSubTerms;
    std::vector<aterm> subTerms = parent->subTerms;

    // detail::_function_symbol* symEntry = parent->tempTerm;
    // function_symbol fun = symEntry->id;
    function_symbol fun = parent->tempTerm;

    /* Use the appropriate way of constructing the appl, depending on if it has arguments or not. */
    if (nrOfSubTerms > 0)
    {
      constructedTerm = aterm_appl(fun, subTerms.begin(),subTerms.end());

    }
    else
    {
      constructedTerm = aterm_appl(fun);
    }

  }
  else  if (type == AT_LIST)
  {
    size_t nrOfSubTerms = parent->nrOfSubTerms;
    std::vector<aterm> subTerms = parent->subTerms;

    aterm_list list;

    if (nrOfSubTerms > 0)
    {
      size_t i = nrOfSubTerms;
      do
      {
        list = push_front(list, subTerms[--i]);
      }
      while (i > 0);

    }

    constructedTerm = list;

  }
  else
  {
    constructedTerm = aterm(); /* This line is purely for shutting up the compiler. */
    std::runtime_error("Unable to construct term.");
  }

  return constructedTerm;
}

/**
 * Links the given (deserialized) term with it's parent.
 */
static void linkTerm(BinaryReader binaryReader, aterm aTerm)
{
  aterm term = aTerm;

  // while (binaryReader->stackPosition != 0)
  while (binaryReader->stack.size()>1)
  {
    // ATermConstruct* parent = &(binaryReader->stack[--(binaryReader->stackPosition)]);
    binaryReader->stack.pop(); 
    ATermConstruct *parent = &(binaryReader->stack.top());

    size_t nrOfSubTerms = parent->nrOfSubTerms;
    if (nrOfSubTerms > parent->subTermIndex)
    {
      parent->subTerms[parent->subTermIndex++] = term;

      if (nrOfSubTerms != parent->subTermIndex)
      {
        return;  /* This is the 'normal' return point of this function. */
      }
    }
    else
    {
      std::runtime_error("Encountered a term that didn't fit anywhere. Type: " + to_string(term.type()));
    }
    term = buildTerm(binaryReader, parent);

    shareTerm(binaryReader, parent, term);
  }

  if (binaryReader->stack.size()==1)
  {
    binaryReader->isDone = 1;
  } 
}

/**
 * Reads bytes from the byte buffer until we either run out of data or the name of the function symbol or BLOB we are reconstructing is complete.
 */
static void readData(BinaryReader binaryReader, ByteBuffer byteBuffer)
{
  size_t length = binaryReader->tempBytesSize;
  size_t bytesToRead = (length - binaryReader->tempBytesIndex);
  size_t remaining = ATgetRemainingBufferSpace(byteBuffer);
  if (remaining < bytesToRead)
  {
    bytesToRead = remaining;
  }

  memcpy(binaryReader->tempBytes + binaryReader->tempBytesIndex, byteBuffer->currentPos, bytesToRead);
  byteBuffer->currentPos += bytesToRead;
  binaryReader->tempBytesIndex += bytesToRead;

  if (binaryReader->tempBytesIndex == length)
  {
    if (binaryReader->tempType == AT_APPL)
    {
      // ATermConstruct* ac = &(binaryReader->stack[binaryReader->stackPosition]);
      ATermConstruct* ac = &(binaryReader->stack.top());

      size_t arity = binaryReader->tempArity;
      // bool isQuoted = binaryReader->tempIsQuoted;
      char* name = binaryReader->tempBytes;

      // assert(isQuoted);
      function_symbol fun(name, arity);
      // detail::_function_symbol* symEntry = detail::at_lookup_table[fun.number()];
      size_t symEntry = fun.number();
      binaryReader->protected_afuns.insert(fun);

      ensureReadSharedAFunCapacity(binaryReader); /* Make sure we have enough space in the array */
      binaryReader->sharedAFuns[binaryReader->sharedAFunsIndex++] = symEntry;

      if (arity > 0)
      {
        ac->tempTerm = symEntry;

        ac->subTerms = getProtectedMemoryBlock(/* binaryReader->protectedMemoryStack, */ arity);
      }
      else
      {
        aterm term = aterm_appl(fun);

        shareTerm(binaryReader, ac, term);

        linkTerm(binaryReader, term);
      }

      if (length < TEMPNAMEPAGESIZE)
      {
        binaryReader->tempBytes = NULL;  /* Set to NULL, so we don't free the tempNamePage. */
      }
    }
    else
    {
      std::runtime_error("Unsupported chunkified type: " +to_string(binaryReader->tempType));
    }

    resetTempReaderData(binaryReader);
  }
}

/**
 * Starts the deserialization of an aterm_appl
 */
static void touchAppl(BinaryReader binaryReader, ByteBuffer byteBuffer, size_t header)
{
  if ((header & FUNSHARED) == FUNSHARED)
  {
    size_t key = readInt(byteBuffer);

    size_t symEntry = binaryReader->sharedAFuns[key];
    // ATermConstruct* ac = &(binaryReader->stack[binaryReader->stackPosition]);
    ATermConstruct* ac = &(binaryReader->stack.top());

    function_symbol fun = symEntry;
    size_t arity = fun.arity();

    if (arity > 0)
    {
      ac->tempTerm = symEntry;

      ac->nrOfSubTerms = arity;
      ac->subTerms = getProtectedMemoryBlock(/* binaryReader->protectedMemoryStack, */ arity);
    }
    else
    {
      aterm term = aterm_appl(fun);

      shareTerm(binaryReader, ac, term);

      linkTerm(binaryReader, term);
    }
  }
  else
  {
    /* Read arity */
    size_t arity = readInt(byteBuffer);

    /* Read name length */
    size_t nameLength = readInt(byteBuffer);

    // ATermConstruct* ac = &(binaryReader->stack[binaryReader->stackPosition]);
    ATermConstruct* ac = &(binaryReader->stack.top());
    ac->nrOfSubTerms = arity;

    binaryReader->tempArity = arity;
    binaryReader->tempIsQuoted = ((header & APPLQUOTED) == APPLQUOTED);

    binaryReader->tempBytesSize = nameLength;
    /* Only allocate a new block of memory if we're dealing with a very large name. */
    if (nameLength < TEMPNAMEPAGESIZE)
    {
      binaryReader->tempBytes = binaryReader->tempNamePage;
    }
    else
    {
      binaryReader->tempBytes = (char*) malloc((nameLength + 1) * sizeof(char));
      if (binaryReader->tempBytes == NULL)
      {
        std::runtime_error("The binary reader was unable to allocate memory for temporary function symbol data.");
      }
    }
    binaryReader->tempBytes[nameLength] = '\0'; /* CStrings are \0 terminated. */
    binaryReader->tempBytesIndex = 0;
    binaryReader->tempType = AT_APPL;

    /* Read name */
    readData(binaryReader, byteBuffer);
  }
}

/**
 * Starts the deserialization of a aterm_list.
 */
static void touchList(BinaryReader binaryReader, ByteBuffer byteBuffer)
{
  size_t size = readInt(byteBuffer);

  // ATermConstruct* ac = &(binaryReader->stack[binaryReader->stackPosition]);
  ATermConstruct* ac = &(binaryReader->stack.top());

  if (size > 0)
  {
    ac->nrOfSubTerms = size;
    ac->subTerms = getProtectedMemoryBlock(/* binaryReader->protectedMemoryStack,*/ size);
  }
  else
  {
    aterm term = aterm_list();

    shareTerm(binaryReader, ac, term);

    linkTerm(binaryReader, term);
  }
}

/**
 * Starts the deserialization of an aterm_int.
 */
static void touchInt(BinaryReader binaryReader, ByteBuffer byteBuffer)
{
  int value = readInt(byteBuffer);
  aterm term = aterm_int(value);

  // ATermConstruct* ac = &(binaryReader->stack[binaryReader->stackPosition]);
  ATermConstruct* ac = &(binaryReader->stack.top());

  shareTerm(binaryReader, ac, term);

  linkTerm(binaryReader, term);
}

/**
 * Continues the deserialization process, who's state is described in the binary reader with the data in the byte buffer.
 */
void ATdeserialize(BinaryReader binaryReader, ByteBuffer byteBuffer)
{
  char* endOfBuffer;

  if (binaryReader->tempType != 0)
  {
    readData(binaryReader, byteBuffer);
  }

  endOfBuffer = byteBuffer->buffer + byteBuffer->limit; /* Cache the end of buffer pointer, so we don't have to recalculate it every iteration. */
  while (byteBuffer->currentPos < endOfBuffer)
  {
    size_t header = (unsigned char) *(byteBuffer->currentPos);
    byteBuffer->currentPos++;

    if ((header & ISSHAREDFLAG) == ISSHAREDFLAG)
    {
      size_t termKey = readInt(byteBuffer);

      aterm term = binaryReader->sharedTerms[termKey];

      // binaryReader->stackPosition++;
      binaryReader->stack.push(ATermConstruct());

      linkTerm(binaryReader, term);
    }
    else
    {
      size_t type = (header & TYPEMASK);

      // ATermConstruct* ac = &(binaryReader->stack[++(binaryReader->stackPosition)]);
      binaryReader->stack.push(ATermConstruct());
      ATermConstruct* ac = &(binaryReader->stack.top());

      // ensureReadSharedTermCapacity(binaryReader); /* Make sure the shared terms array remains large enough. */

      // ac->termKey = binaryReader->sharedTermsIndex++;
      ac->termKey=binaryReader->sharedTerms.size();
      binaryReader->sharedTerms.push_back(aterm());

      ac->type = type;

      ac->nrOfSubTerms = 0; /* Default value */
      ac->subTermIndex = 0; /* Default value */

      switch (type)
      {
        case AT_APPL:
          touchAppl(binaryReader, byteBuffer, header);
          break;
        case AT_LIST:
          touchList(binaryReader, byteBuffer);
          break;
        case AT_INT:
          touchInt(binaryReader, byteBuffer);
          break;
        default:
          std::runtime_error("Unknown type id: " + to_string(type) + ". Current buffer position: " + to_string(byteBuffer->currentPos - byteBuffer->buffer));
      }
    }

    /* Make sure the stack remains large enough. */
    /* ensureReadStackCapacity(binaryReader); */
  }
}

/**
 * Constructs a binary reader that can interpret a SAF stream.
 */
BinaryReader ATcreateBinaryReader()
{
  // ATermConstruct* stack;
  // aterm* sharedTerms;
  size_t* sharedAFuns;

  BinaryReader binaryReader = new _BinaryReader; // (BinaryReader) malloc(sizeof(struct _BinaryReader));
  /* if (binaryReader == NULL)
  {
    std::runtime_error("Unable to allocate memory for the binary reader.");
  } */

  // binaryReader->protectedMemoryStack = createProtectedMemoryStack();

  // stack = (ATermConstruct*) malloc(DEFAULTSTACKSIZE * sizeof(struct _ATermConstruct));
  /* if (stack == NULL)
  {
    std::runtime_error("Unable to allocate memory for the binaryReader's stack.");
  } */
  /* binaryReader->stack = stack;
  binaryReader->stackSize = DEFAULTSTACKSIZE;
  binaryReader->stackPosition = (size_t)-1; // Initialise
  */

  /* sharedTerms = (aterm*) malloc(DEFAULTSHAREDTERMARRAYSIZE * sizeof(aterm));
  if (sharedTerms == NULL)
  {
    std::runtime_error("Unable to allocate memory for the binaryReader's shared terms array.");
  }
  binaryReader->sharedTerms = sharedTerms;
  binaryReader->sharedTermsSize = DEFAULTSHAREDTERMARRAYSIZE;
  binaryReader->sharedTermsIndex = 0; */

  sharedAFuns = (size_t*) malloc(DEFAULTSHAREDSYMBOLARRAYSIZE * sizeof(size_t));
  if (sharedAFuns == NULL)
  {
    std::runtime_error("Unable to allocate memory for the binaryReader's shared symbols array.");
  }
  binaryReader->sharedAFuns = sharedAFuns;
  binaryReader->sharedAFunsSize = DEFAULTSHAREDSYMBOLARRAYSIZE;
  binaryReader->sharedAFunsIndex = 0;

  binaryReader->tempNamePage = (char*) malloc(TEMPNAMEPAGESIZE * sizeof(char));
  if (binaryReader->tempNamePage == NULL)
  {
    std::runtime_error("Unable to allocate temporary name page.");
  }

  binaryReader->tempType = 0;
  binaryReader->tempBytes = NULL;
  binaryReader->tempBytesSize = 0;
  binaryReader->tempBytesIndex = 0;
  binaryReader->tempArity = 0;
  binaryReader->tempIsQuoted = false;

  binaryReader->isDone = 0;

  return binaryReader;
}

/**
 * Checks if the binary reader is done with the deserialization process.
 * Returns > 0 if true, 0 otherwise.
 */
int ATisFinishedReading(BinaryReader binaryReader)
{
  return binaryReader->isDone;
}

/**
 * Returns the root of the deserialized tree, present in the given binary writer.
 * NOTE: If the deserialization process is incomplete a warning will be issued and NULL returned.
 */
aterm ATgetRoot(BinaryReader binaryReader)
{
  if (binaryReader->isDone <= 0)
  {
    mCRL2log(mcrl2::log::info) << "Can't retrieve the root of the tree while it's still being constructed. Returning NULL." << std::endl;
    return aterm();
  }

  return binaryReader->sharedTerms[0]; /* Return the value of the first element (the root of the tree). */
}

/**
 * Frees the memory associated with the given binary reader.
 * NOTE: Calling this function with a binary reader that has started, but not completed, a deserialization process as argument has undefined behavior (protected zones will not be unprotected or freed).
 */
void ATdestroyBinaryReader(BinaryReader binaryReader)
{
  // detail::_function_symbol** sharedAFuns = binaryReader->sharedAFuns;
  // ptrdiff_t sharedAFunsIndex = binaryReader->sharedAFunsIndex;

  // destroyProtectedMemoryStack(binaryReader->protectedMemoryStack);

  /* We can just free the shared terms, shared signatures and the stack, since they're all present in the memory block store. */
  // free(binaryReader->sharedTerms);

  // free(binaryReader->stack);

  /* while (--sharedAFunsIndex >= 0)
  {
   ATunprotectAFun(sharedAFuns[sharedAFunsIndex]->id);
  } */
  free(binaryReader->sharedAFuns);

  free(binaryReader->tempNamePage);

  resetTempReaderData(binaryReader);

  delete binaryReader;
}


/* FILE I/O */

/**
 * Writes the given aterm in SAF format to the given file.
 * NOTE: The given file must be opened in binary mode (at least on Win32 this is required).
 */
bool ATwriteToSAFFile(const aterm &aTerm, FILE* file)
{
  BinaryWriter binaryWriter;
  ByteBuffer byteBuffer;

  size_t bytesWritten = fwrite("?", sizeof(char), 1, file);
  if (bytesWritten != 1)
  {
    mCRL2log(mcrl2::log::error) << "Unable to write SAF identifier token to file." << std::endl;
    return false;
  }

  binaryWriter = ATcreateBinaryWriter(aTerm);
  byteBuffer = ATcreateByteBuffer(65536);

  do
  {
    size_t blockSize;
    char sizeBytes[2];

    ATresetByteBuffer(byteBuffer);
    ATserialize(binaryWriter, byteBuffer);

    blockSize = byteBuffer->limit;
    sizeBytes[0] = blockSize & 0x000000ffU;
    sizeBytes[1] = (blockSize >> 8) & 0x000000ffU;

    bytesWritten = fwrite(sizeBytes, sizeof(char), 2, file);
    if (bytesWritten != 2)
    {
      mCRL2log(mcrl2::log::error) << "Unable to write block size bytes to file." << std::endl;
      ATdestroyByteBuffer(byteBuffer);
      ATdestroyBinaryWriter(binaryWriter);
      return false;
    }

    bytesWritten = fwrite(byteBuffer->buffer, sizeof(char), byteBuffer->limit, file);
    if (bytesWritten != byteBuffer->limit)
    {
      mCRL2log(mcrl2::log::error) << "Unable to write bytes to file." << std::endl;
      ATdestroyByteBuffer(byteBuffer);
      ATdestroyBinaryWriter(binaryWriter);
      return false;
    }
  }
  while (!ATisFinishedWriting(binaryWriter));

  ATdestroyByteBuffer(byteBuffer);
  ATdestroyBinaryWriter(binaryWriter);

  if (fflush(file) != 0)
  {
    mCRL2log(mcrl2::log::error) << "Unable to flush file stream." << std::endl;
    return false;
  }

  return true;
}

/**
 * Writes the given aterm in SAF format to the file with the given name.
 */
bool ATwriteToNamedSAFFile(const aterm& aTerm, const char* filename)
{
  bool result;
  FILE* file;

  if (strcmp(filename, "-") == 0)
  {
    return ATwriteToSAFFile(aTerm, stdout);
  }

  file = fopen(filename, "wb");
  if (file == NULL)
  {
    mCRL2log(mcrl2::log::error) << "Unable to open file for writing: " << filename << std::endl;
    return false;
  }

  result = ATwriteToSAFFile(aTerm, file);

  if (fclose(file) != 0)
  {
    return false;
  }

  return result;
}

/**
 * Interprets the content of the given SAF file and returns the constructed aterm.
 * NOTE: The given file must be opened in binary mode (at least on Win32 this is required)
 */
aterm ATreadFromSAFFile(FILE* file)
{
  aterm term;
  BinaryReader binaryReader;
  ByteBuffer byteBuffer;

  char buffer[1];
  size_t bytesRead = fread(buffer, sizeof(char), 1, file); /* Consume the first character in the stream. */
  if (bytesRead <= 0)
  {
    mCRL2log(mcrl2::log::error) << "Unable to read SAF id token from file." << std::endl;
    return aterm();
  }

  if (buffer[0] != SAF_IDENTIFICATION_TOKEN)
  {
    std::runtime_error("Not a SAF file.");
  }

  binaryReader = ATcreateBinaryReader();
  byteBuffer = ATcreateByteBuffer(65536);

  do
  {
    size_t blockSize;
    char sizeBytes[2];

    bytesRead = fread(sizeBytes, sizeof(char), 2, file);
    if (bytesRead <= 0)
    {
      break;
    }
    else if (bytesRead != 2)
    {
      mCRL2log(mcrl2::log::error) << "Unable to read block size bytes from file: " << bytesRead << std::endl;
      ATdestroyByteBuffer(byteBuffer);
      ATdestroyBinaryReader(binaryReader);
      return aterm();
    }
    blockSize = ((unsigned char) sizeBytes[0]) + (((unsigned char) sizeBytes[1]) << 8);
    if (blockSize == 0)
    {
      blockSize = 65536;
    }

    ATresetByteBuffer(byteBuffer);
    byteBuffer->limit = blockSize;
    bytesRead = fread(byteBuffer->buffer, sizeof(char), blockSize, file);
    if (bytesRead != blockSize)
    {
      mCRL2log(mcrl2::log::error) << "Unable to read bytes from file." << std::endl;
      ATdestroyByteBuffer(byteBuffer);
      ATdestroyBinaryReader(binaryReader);
      return aterm();
    }

    ATdeserialize(binaryReader, byteBuffer);
  }
  while (bytesRead > 0);

  ATdestroyByteBuffer(byteBuffer);

  if (!ATisFinishedReading(binaryReader))
  {
    mCRL2log(mcrl2::log::error) << "Term incomplete, missing data." << std::endl;
    term = aterm();
  }
  else
  {
    term = ATgetRoot(binaryReader);
  }

  ATdestroyBinaryReader(binaryReader);

  return term;
}

/**
 * Interprets the content of the SAF file with the given name and returns the constructed aterm.
 */
aterm ATreadFromNamedSAFFile(const char* filename)
{
  aterm result;
  FILE* file;

  if (strcmp(filename, "-") == 0)
  {
    return ATreadFromSAFFile(stdin);
  }

  file = fopen(filename, "rb");
  if (file == NULL)
  {
    mCRL2log(mcrl2::log::error) << "Unable to open file for reading: " << filename << std::endl;
    return aterm();
  }

  result = ATreadFromSAFFile(file);

  if (fclose(file) != 0)
  {
    return aterm();
  }

  return result;
}

/**
 * This is a node we need to build a linked list of ByteBuffers while writing SAF to a string.
 * By using this strategy, we avoid the unnecessary reallocation of a temporary buffer.
 */
typedef struct _BufferNode
{
  ByteBuffer byteBuffer;
  struct _BufferNode* next;
} BufferNode;

/**
 * Writes the given aterm to a string in SAF format.
 * Since the string will contain \0 bytes, the value the length parameter has after this function returns will specify the number of bytes that were written.
 * Note that the resulting string has been malloced and will need to be freed by the user.
 */
char* ATwriteToSAFString(const aterm& aTerm, size_t* length)
{
  char* result;
  size_t totalBytesWritten = 0;
  size_t position = 0;

  BinaryWriter binaryWriter = ATcreateBinaryWriter(aTerm);

  BufferNode* last;
  BufferNode* currentBufferNode;
  BufferNode* root = (BufferNode*) malloc(sizeof(struct _BufferNode));
  if (root == NULL)
  {
    std::runtime_error("Unable to allocate space for BufferNode.");
  }
  last = root;

  do
  {
    BufferNode* current;
    ByteBuffer byteBuffer = ATcreateByteBuffer(65536);

    ATresetByteBuffer(byteBuffer);
    ATserialize(binaryWriter, byteBuffer);

    current = (BufferNode*) malloc(sizeof(struct _BufferNode));
    current->byteBuffer = byteBuffer;
    current->next = NULL;
    last->next = current;
    last = current;

    totalBytesWritten += byteBuffer->limit + 2; /* Bytes written per block = buffer size + 2 bytes block size spec. */
  }
  while (!ATisFinishedWriting(binaryWriter));

  ATdestroyBinaryWriter(binaryWriter);

  result = (char*) malloc(totalBytesWritten * sizeof(char));
  if (result == NULL)
  {
    std::runtime_error("Unable to allocate space for result string.");
  }

  currentBufferNode = root->next;
  free(root);
  do
  {
    BufferNode* nextBufferNode;
    ByteBuffer currentByteBuffer = currentBufferNode->byteBuffer;
    size_t blockSize = currentByteBuffer->limit;

    result[position++] = blockSize & 0x000000ffU;
    result[position++] = (blockSize >> 8) & 0x000000ffU;
    memcpy(result + position, currentByteBuffer->buffer, blockSize * sizeof(char));
    position += blockSize;

    ATdestroyByteBuffer(currentByteBuffer);

    nextBufferNode = currentBufferNode->next;
    free(currentBufferNode);
    currentBufferNode = nextBufferNode;
  }
  while (currentBufferNode != NULL);

  *length = totalBytesWritten;
  return result;
}

/**
 * Interprets the given string in SAF format and returns the constructed aterm.
 */
aterm ATreadFromSAFString(char* data, size_t length)
{
  aterm term;

  size_t position = 0;

  BinaryReader binaryReader = ATcreateBinaryReader();

  do
  {
    ByteBuffer byteBuffer;
    int blockSize = (unsigned char) data[position++];
    blockSize += ((unsigned char) data[position++]) << 8;
    if (blockSize == 0)
    {
      blockSize = 65536;
    }
    byteBuffer = ATwrapBuffer(data + position, blockSize); /* Move the window to the next block. */

    ATdeserialize(binaryReader, byteBuffer);

    byteBuffer->buffer = NULL; /* Prevent the data string from being freed. */
    ATdestroyByteBuffer(byteBuffer);

    position += blockSize;
  }
  while (position < length);

  if (!ATisFinishedReading(binaryReader))
  {
    mCRL2log(mcrl2::log::error) << "Term incomplete, missing data." << std::endl;
    term = aterm();
  }
  else
  {
    term = ATgetRoot(binaryReader);
  }

  ATdestroyBinaryReader(binaryReader);

  return term;
}

} // namespace atermpp
