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

typedef struct _IDMappings{
	EntryCache entryCache;
	
	Entry **table;
	unsigned int tableSize;
	unsigned int hashMask;
	
	unsigned int load;
	unsigned int threshold;
} *IDMappings;

IDMappings IMcreateIDMappings(float loadPercentage);

int IMmakeIDMapping(IDMappings idMappings, void *key, unsigned int h, int value);

int IMgetID(IDMappings idMappings, void *key, unsigned int h);

void IMremoveIDMapping(IDMappings idMappings, void *key, unsigned int h);

unsigned int IMgetSize(IDMappings idMappings);

void IMdestroyIDMappings(IDMappings idMappings);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HASHTABLE_H_ */
