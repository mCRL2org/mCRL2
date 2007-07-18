#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

struct Entry;
typedef struct Entry Entry;

typedef struct _EntryCache{
	Entry **blocks;
	unsigned int nrOfBlocks;
	
	Entry *nextEntry;
	unsigned int spaceLeft;
	
	Entry *freeList;
} *EntryCache;

typedef struct _HashTable{
	EntryCache entryCache;
	
	Entry **table;
	unsigned int tableSize;
	unsigned int hashMask;
	
	unsigned int load;
	unsigned int threshold;
	
	int (*eqCheckFunc)(void*, void*);
} *HashTable;

HashTable HTcreateHashTable(float loadPercentage, int (*eqCheckFunc)(void*, void*));

void* HTputElement(HashTable hashTable, void *key, unsigned int h, void *value);

void* HTgetElement(HashTable hashTable, void *key, unsigned int h);

void HTremoveElement(HashTable hashTable, void *key, unsigned int h, int delValue);

int HTgetSize(HashTable hashTable);

void HTdestroyHashTable(HashTable hashTable, int delValue);

int defaultEqCheck(void *a, void *b);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HASHTABLE_H_ */
