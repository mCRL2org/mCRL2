#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>

#if __STDC_VERSION__ >= 199901L
  /* "inline" is a keyword */
#else
# ifndef inline
#  define inline /* nothing */
# endif
#endif

#define DEFAULTTABLEBITSIZE 8 /* Default table size is 8 bits (256 entries) */

#define PREALLOCATEDENTRYBLOCKSINCREMENT 16
#define PREALLOCATEDENTRYBLOCKSINCREMENTMASK 0x0000000fU
#define PREALLOCATEDENTRYBLOCKSIZE 256

struct Entry{
	void *key;
	unsigned int hash;
	
	void *value;
	
	Entry *next;
};

/**
 * Creates a new entry cache.
 * This cache contains nodes that have been pre-allocated and released.
 */
static EntryCache createEntryCache(){
	Entry *block;
	
	EntryCache entryCache = (EntryCache) malloc(sizeof(struct _EntryCache));
	if(entryCache == NULL){
		printf("Failed to allocate memory for entry cache.\n");
		exit(1);
	}
	
	entryCache->blocks = (Entry**) malloc(PREALLOCATEDENTRYBLOCKSINCREMENT * sizeof(Entry*));
	if(entryCache->blocks == NULL){
		printf("Failed to allocate array for storing references to pre-allocated entries.\n");
		exit(1);
	}
	entryCache->nrOfBlocks = 1;
	
	block = (Entry*) malloc(PREALLOCATEDENTRYBLOCKSIZE * sizeof(struct Entry));
	if(block == NULL){
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
 * NOTE: Do NOT invoke this method with the entry cache of a hashtable that is still in use.
 */
static void destroyEntryCache(EntryCache entryCache){
	int i = entryCache->nrOfBlocks;
	do{
		free(entryCache->blocks[--i]);
	}while(i > 0);
	free(entryCache->blocks);
	
	free(entryCache);
}

/**
 * Adds an additional block of pre-allocated entries to the given entry cache.
 */
static void expandEntryCache(EntryCache entryCache){
	Entry *block = (Entry*) malloc(PREALLOCATEDENTRYBLOCKSIZE * sizeof(struct Entry));
	if(block == NULL){
		printf("Failed to allocate block of memory for pre-allocated entries.\n");
		exit(1);
	}
	
	if((entryCache->nrOfBlocks & PREALLOCATEDENTRYBLOCKSINCREMENTMASK) == 0){
		entryCache->blocks = (Entry**) realloc(entryCache->blocks, (entryCache->nrOfBlocks + PREALLOCATEDENTRYBLOCKSINCREMENT) * sizeof(Entry*));
		if(entryCache->blocks == NULL){
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
inline static Entry* getEntry(EntryCache entryCache){
	Entry *entry = entryCache->freeList;
	
	if(entry != NULL){
		entryCache->freeList = entry->next;
	}else{
		if(entryCache->spaceLeft == 0) expandEntryCache(entryCache);
		
		entryCache->spaceLeft--;
		entry = entryCache->nextEntry++;
	}
	
	return entry;
}

/**
 * Releases the given entry to the entry cache, so it can be reused.
 * The entry will be added to the free list.
 */
inline static void releaseEntry(EntryCache entryCache, Entry *entry){
	entry->next = entryCache->freeList;
	entryCache->freeList = entry;
}

/** 
 * Checks if two keys match. The hash is passed as well, because if that doesn't match neither do the keys 
 * (int == int is cheaper then comparing the content of two keys). 
 */
static unsigned int matches(HashTable hashTable, unsigned int hash, void* key, Entry *entry){
	return (key == entry->key || (hash == entry->hash && (hashTable->eqCheckFunc)(key, entry->key)));
}

/**
 * This function protects against weak hashes in which only the high-order bits are occupied for example 
 * (we don't want too many stuff ending up in the same bucket). This is nessecary because we use a 'order of 2' 
 * instead of a prime number for the table size.
 */
static unsigned int supplementalHash(unsigned int h){
	return ((h << 7) - h + (h >> 9) + (h >> 17));
}

/**
 * This function takes care of the resizing of the entry table (when nessecary).
 * The capacity will be doubled when we run out of space.
 */
static void ensureTableCapacity(HashTable hashTable){
	Entry **oldTable = hashTable->table;
	
	unsigned int currentTableSize = hashTable->tableSize;
	if(hashTable->load >= hashTable->threshold){
		unsigned int hashMask;
		int i = currentTableSize - 1;
		
		unsigned int newTableSize = currentTableSize << 1;
		Entry **table = (Entry**) calloc(newTableSize, sizeof(Entry*));
		if(table == NULL){
			printf("The hashtable was unable to allocate memory for extending the entry table.\n");
			exit(1);
		}
		hashTable->table = table;
		hashTable->tableSize = newTableSize;
		
		hashMask = newTableSize - 1;
		hashTable->hashMask = hashMask;
		hashTable->threshold <<= 1;
		
		do{
			Entry *e = oldTable[i];
			while(e != NULL){
				unsigned int bucketPos = e->hash & hashMask;
				
				Entry *currentEntry = table[bucketPos];
				
				/* Find the next entry in the old table. */
				Entry *nextEntry = e->next;
				
				/* Add the entry in the new table. */
				e->next = currentEntry;
				table[bucketPos] = e;
				
				e = nextEntry;
			}
			i--;
		}while(i >= 0);
		
		free(oldTable);
	}
}

/**
 * Creates a new HashTable.
 */
HashTable HTcreateHashTable(float loadPercentage, int (*eqCheckFunc)(void*, void*)){
	unsigned int tableSize = 1 << DEFAULTTABLEBITSIZE;
	
	HashTable hashTable = (HashTable) malloc(sizeof(struct _HashTable));
	if(hashTable == NULL){
		printf("Unable to allocate memory for creating a hashtable.\n");
		exit(1);
	}
	
	hashTable->entryCache = createEntryCache();
	
	hashTable->table = (Entry**) calloc(tableSize, sizeof(Entry*));
	if(hashTable->table == NULL){
		printf("The hashtable was unable to allocate memory for the entry table.\n");
		exit(1);
	}
	hashTable->tableSize = tableSize;
	
	hashTable->hashMask = tableSize - 1;
	hashTable->threshold = tableSize * loadPercentage;
	
	hashTable->load = 0;
	
	hashTable->eqCheckFunc = eqCheckFunc;
	
	return hashTable;
}

/**
 * Inserts an element in the table.
 * If an element with the same key is already present in the table the value of that entry will be replaced and returned by this function so the 'exception' can be handled by the caller;
 * otherwise NULL will be returned.
 */
void* HTputElement(HashTable hashTable, void *key, unsigned int h, void *value){
	unsigned int bucketPos;
	Entry **table;
	Entry *currentEntry, *entry;
	
	unsigned int hash = supplementalHash(h);
	
	ensureTableCapacity(hashTable);
	
	bucketPos = hash & hashTable->hashMask;
	table = hashTable->table;
	currentEntry = table[bucketPos];
	
	entry = currentEntry;
	while(entry != NULL){
		/* If the key is already present just replace the value */
		if(matches(hashTable, hash, key, entry)){
			void *oldValue = entry->value;
			if(oldValue != NULL && oldValue != key) free(oldValue); /* Free the old value if needed, but only if it isn't pointing to the same thing as the key */
			
			entry->value = value;
			
			return oldValue;
		}
		
		entry = entry->next;
	}
	
	/* Insert the new entry at the start of the bucket and link it with the colliding entries (if present). */
	entry = getEntry(hashTable->entryCache);
	entry->hash = hash;
	entry->key = key;
	entry->value = value;
	entry->next = currentEntry;
	
	table[bucketPos] = entry;
	hashTable->load++;
	
	return NULL;
}

/**
 * Retrieves an element from the table that matches the given key and hashcode.
 * If no matching entry can be found NULL will be returned.
 */
void* HTgetElement(HashTable hashTable, void *key, unsigned int h){
	unsigned int hash = supplementalHash(h);
	
	unsigned int bucketPos = hash & hashTable->hashMask;
	Entry *entry = hashTable->table[bucketPos];
	while(entry != NULL && !matches(hashTable, hash, key, entry)){
		entry = entry->next;
	}
	
	if(entry == NULL) return NULL;
	return entry->value;
}

/**
 * Removes the element associated with the given key and hashcode from the table.
 */
void HTremoveElement(HashTable hashTable, void *key, unsigned int h, int delValue){
	unsigned int hash = supplementalHash(h);
	
	unsigned int bucketPos = hash & hashTable->hashMask;
	Entry **table = hashTable->table;
	Entry *entry = table[bucketPos];
	
	Entry *previousEntry = NULL;
	while(entry != NULL){
		if(matches(hashTable, hash, key, entry)){
			Entry *nextEntry = entry->next;
			if(previousEntry == NULL) table[bucketPos] = nextEntry;
			else previousEntry->next = nextEntry;
			
			hashTable->load--;
			
			if(delValue == 1) free(entry->value);
			
			releaseEntry(hashTable->entryCache, entry);
			
			return;
		}
		previousEntry = entry;
		entry = entry->next;
	}
}

/**
 * Returns the number of elements in the table.
 */
int HTgetSize(HashTable hashTable){
	return hashTable->load;
}

/**
 * Destroys the given hashtable.
 * NOTE: with the 'delValue' flag you can indicate if you want the values to be freed or not (1 = free, 0 = don't free).
 * (Keep in mind that freeing the values can potentially be a slow process).
 * The keys will naturally not be freed, since they are still reachable elsewhere.
 */
void HTdestroyHashTable(HashTable hashTable, int delValue){
	Entry **table = hashTable->table;
	
	if(delValue == 1){
		int i = hashTable->tableSize;
		do{
			Entry *entry = table[--i];
			Entry *nextEntry = NULL;
			while(entry != NULL){
				nextEntry = entry->next;
				
				free(entry->value);
				
				entry = nextEntry;
			}
		}while(i > 0);
	}
	
	destroyEntryCache(hashTable->entryCache);
	
	free(table);
	
	free(hashTable);
}

/**
 * A standard equality check just compares pointers (match = 1, 0 otherwise).
 * (Note that the match function of the hashtable check on pointer equality before invoking this function, which just returns false).
 * If a more detailed check is required it should be implemented by the user and passed to the HTcreateHashTable function.
 */
int defaultEqCheck(void *a, void *b){
	return 0;
}
