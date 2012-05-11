#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "mcrl2/atermpp/detail/idmappings.h"

namespace atermpp
{

static const size_t DEFAULTTABLEBITSIZE = 8; /* Default table size is 8 bits (256 entries) */

static const size_t PREALLOCATEDENTRYBLOCKSINCREMENT = 16;
static const size_t PREALLOCATEDENTRYBLOCKSINCREMENTMASK = 0x0000000fU;
static const size_t PREALLOCATEDENTRYBLOCKSIZE = 256;

struct Entry
{
  void* key;
  size_t hash;

  size_t value;

  Entry* next;
};

/**
 * Creates a new entry cache.
 * This cache contains nodes that have been pre-allocated and released.
 */
static EntryCache createEntryCache()
{
  Entry* block;

  EntryCache entryCache = (EntryCache) malloc(sizeof(struct _EntryCache));
  if (entryCache == NULL)
  {
    printf("Failed to allocate memory for entry cache.\n");
    exit(1);
  }

  entryCache->blocks = (Entry**) malloc(PREALLOCATEDENTRYBLOCKSINCREMENT * sizeof(Entry*));
  if (entryCache->blocks == NULL)
  {
    printf("Failed to allocate array for storing references to pre-allocated entries.\n");
    exit(1);
  }
  entryCache->nrOfBlocks = 1;

  block = (Entry*) malloc(PREALLOCATEDENTRYBLOCKSIZE * sizeof(struct Entry));
  if (block == NULL)
  {
    printf("Failed to allocate block of memory for pre-allocated entries.\n");
    exit(1);
  }
  entryCache->nextEntry = block;
  entryCache->spaceLeft = PREALLOCATEDENTRYBLOCKSIZE;

  entryCache->blocks[0] = block;

  entryCache->freeList = NULL;

  return entryCache;
}

/**
 * Frees the memory associated with the given entry cache.
 * NOTE: Do NOT invoke this method with the entry cache of a idMapping that is still in use.
 */
static void destroyEntryCache(EntryCache entryCache)
{
  size_t i = entryCache->nrOfBlocks;
  do
  {
    free(entryCache->blocks[--i]);
  }
  while (i > 0);
  free(entryCache->blocks);

  free(entryCache);
}

/**
 * Adds an additional block of pre-allocated entries to the given entry cache.
 */
static void expandEntryCache(EntryCache entryCache)
{
  Entry* block = (Entry*) malloc(PREALLOCATEDENTRYBLOCKSIZE * sizeof(struct Entry));
  if (block == NULL)
  {
    printf("Failed to allocate block of memory for pre-allocated entries.\n");
    exit(1);
  }

  if ((entryCache->nrOfBlocks & PREALLOCATEDENTRYBLOCKSINCREMENTMASK) == 0)
  {
    entryCache->blocks = (Entry**) realloc(entryCache->blocks, (entryCache->nrOfBlocks + PREALLOCATEDENTRYBLOCKSINCREMENT) * sizeof(Entry*));
    if (entryCache->blocks == NULL)
    {
      printf("Failed to allocate array for storing references to pre-allocated entries.\n");
      exit(1);
    }
  }

  entryCache->blocks[entryCache->nrOfBlocks++] = block;

  entryCache->spaceLeft = PREALLOCATEDENTRYBLOCKSIZE;
  entryCache->nextEntry = block;
}

/**
 * Returns a entry from the given entry cache.
 * This entry needs to be properly initialized after it is returned.
 *
 * First this function will look in the free list if there are any released nodes that can be reused;
 * If the free list is empty a pre-allocated entry will be returned.
 */
inline static Entry* getEntry(EntryCache entryCache)
{
  Entry* entry = entryCache->freeList;

  if (entry != NULL)
  {
    entryCache->freeList = entry->next;
  }
  else
  {
    if (entryCache->spaceLeft == 0)
    {
      expandEntryCache(entryCache);
    }

    entryCache->spaceLeft--;
    entry = entryCache->nextEntry++;
  }

  return entry;
}

/**
 * Releases the given entry to the entry cache, so it can be reused.
 * The entry will be added to the free list.
 */
inline static void releaseEntry(EntryCache entryCache, Entry* entry)
{
  entry->next = entryCache->freeList;
  entryCache->freeList = entry;
}

/**
 * This function protects against weak hashes in which only the high-order bits are occupied for example
 * (we don't want too many stuff ending up in the same bucket). This is nessecary because we use a 'order of 2'
 * instead of a prime number for the table size.
 */
static size_t supplementalHash(size_t h)
{
  return ((h << 7) - h + (h >> 9) + (h >> 17));
}

/**
 * This function takes care of the resizing of the entry table (when necessary).
 * The capacity will be doubled when we run out of space.
 */
static void ensureTableCapacity(IDMappings idMappings)
{
  Entry** oldTable = idMappings->table;

  size_t currentTableSize = idMappings->tableSize;
  if (idMappings->load >= idMappings->threshold)
  {
    size_t hashMask;
    size_t i = currentTableSize - 1;

    size_t newTableSize = currentTableSize << 1;
    Entry** table = (Entry**) calloc(newTableSize, sizeof(Entry*));
    if (table == NULL)
    {
      printf("The idMapping was unable to allocate memory for extending the entry table.\n");
      exit(1);
    }
    idMappings->table = table;
    idMappings->tableSize = newTableSize;

    hashMask = newTableSize - 1;
    idMappings->hashMask = hashMask;
    idMappings->threshold <<= 1;

    do
    {
      Entry* e = oldTable[i];
      while (e != NULL)
      {
        size_t bucketPos = e->hash & hashMask;

        Entry* currentEntry = table[bucketPos];

        /* Find the next entry in the old table. */
        Entry* nextEntry = e->next;

        /* Add the entry in the new table. */
        e->next = currentEntry;
        table[bucketPos] = e;

        e = nextEntry;
      }
      i--;
    }
    while (i!=ATERM_NON_EXISTING_POSITION);

    free(oldTable);
  }
}

/**
 * Creates a new ID mappings table.
 */
IDMappings IMcreateIDMappings(unsigned int loadPercentage)
{
  size_t tableSize = 1 << DEFAULTTABLEBITSIZE;

  IDMappings idMappings = (IDMappings) malloc(sizeof(struct _IDMappings));
  assert(loadPercentage <= 100);
  if (idMappings == NULL)
  {
    printf("Unable to allocate memory for creating a idMapping.\n");
    exit(1);
  }

  idMappings->entryCache = createEntryCache();

  idMappings->table = (Entry**) calloc(tableSize, sizeof(Entry*));
  if (idMappings->table == NULL)
  {
    printf("The idMapping was unable to allocate memory for the entry table.\n");
    exit(1);
  }
  idMappings->tableSize = tableSize;

  assert(tableSize!=0);
  idMappings->hashMask = tableSize - 1;
  idMappings->threshold = (tableSize * loadPercentage)/100;

  idMappings->load = 0;

  return idMappings;
}

/**
 * Inserts an ID mapping in the table.
 * If an entry with the same key is already present in the table the ID of that entry will be replaced and returned by this function so the 'exception' can be handled by the caller;
 * otherwise -1 will be returned.
 */
size_t IMmakeIDMapping(IDMappings idMappings, void* key, size_t h, size_t value)
{
  size_t bucketPos;
  Entry** table;
  Entry* currentEntry, *entry;

  size_t hash = supplementalHash(h);

  ensureTableCapacity(idMappings);

  bucketPos = hash & idMappings->hashMask;
  table = idMappings->table;
  currentEntry = table[bucketPos];

  entry = currentEntry;
  while (entry != NULL)
  {
    /* If the key is already present just replace the ID. */
    if (entry->key == key)
    {
      size_t oldValue = entry->value;

      entry->value = value;

      return oldValue;
    }

    entry = entry->next;
  }

  /* Insert the new entry at the start of the bucket and link it with the colliding entries (if present). */
  entry = getEntry(idMappings->entryCache);
  entry->hash = hash;
  entry->key = key;
  entry->value = value;
  entry->next = currentEntry;

  table[bucketPos] = entry;
  idMappings->load++;

  return ATERM_NON_EXISTING_POSITION;
}

/**
 * Retrieves an ID from the table that matches the given key and hashcode.
 * If no matching entry can be found -1 will be returned.
 */
size_t IMgetID(IDMappings idMappings, void* key, size_t h)
{
  size_t hash = supplementalHash(h);

  size_t bucketPos = hash & idMappings->hashMask;
  Entry* entry = idMappings->table[bucketPos];
  while (entry != NULL && entry->key != key)
  {
    entry = entry->next;
  }

  if (entry == NULL)
  {
    return ATERM_NON_EXISTING_POSITION;
  }
  return entry->value;
}

/**
 * Removes the entry associated with the given key and hashcode from the table.
 */
void IMremoveIDMapping(IDMappings idMappings, void* key, size_t h)
{
  size_t hash = supplementalHash(h);

  size_t bucketPos = hash & idMappings->hashMask;
  Entry** table = idMappings->table;
  Entry* entry = table[bucketPos];

  Entry* previousEntry = NULL;
  while (entry != NULL)
  {
    if (entry->key == key)
    {
      Entry* nextEntry = entry->next;
      if (previousEntry == NULL)
      {
        table[bucketPos] = nextEntry;
      }
      else
      {
        previousEntry->next = nextEntry;
      }

      idMappings->load--;

      releaseEntry(idMappings->entryCache, entry);

      return;
    }
    previousEntry = entry;
    entry = entry->next;
  }
}

/**
 * Returns the number of entries in the table.
 */
unsigned int IMgetSize(IDMappings idMappings)
{
  return idMappings->load;
}

/**
 * Destroys the given ID mappings table.
 */
void IMdestroyIDMappings(IDMappings idMappings)
{
  Entry** table = idMappings->table;

  destroyEntryCache(idMappings->entryCache);

  free(table);

  free(idMappings);
}

} // namespace atermpp
