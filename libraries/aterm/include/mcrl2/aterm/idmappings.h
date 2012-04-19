#ifndef IDMAPPINGS_H_
#define IDMAPPINGS_H_

#include "mcrl2/aterm/atypes.h"

namespace aterm
{

struct Entry;
typedef struct Entry Entry;

typedef struct _EntryCache
{
  Entry** blocks;
  size_t nrOfBlocks;

  Entry* nextEntry;
  size_t spaceLeft;

  Entry* freeList;
}* EntryCache;

typedef struct _IDMappings
{
  EntryCache entryCache;

  Entry** table;
  size_t tableSize;
  size_t hashMask;

  unsigned int load;
  size_t threshold;
} *IDMappings;

IDMappings IMcreateIDMappings(unsigned int loadPercentage);

size_t IMmakeIDMapping(IDMappings idMappings, void* key, size_t h, size_t value);

size_t IMgetID(IDMappings idMappings, void* key, size_t h);

void IMremoveIDMapping(IDMappings idMappings, void* key, size_t h);

unsigned int IMgetSize(IDMappings idMappings);

void IMdestroyIDMappings(IDMappings idMappings);

} // namespace aterm

#endif /* HASHTABLE_H_ */
