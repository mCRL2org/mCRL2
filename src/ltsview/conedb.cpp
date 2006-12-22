#include "conedb.h"
#include <stdlib.h>

#define hash(k,tb,m) ((k*11408669 + tb*97416181) & m)

ConeDB::ConeDB(int hashclass,int blocksize) {
  hashmask = 1<<hashclass - 1;
  hashtable = (int*)malloc((hashmask+1)*sizeof(int));
  for (int i=0; i<=hashmask; ++i) {
    hashtable[i] = -1;
  }
  bucket_block = blocksize;
  bucket_size = blocksize;
  bucket_next = 0;
  buckets = (cone_bucket*)malloc(bucket_size*sizeof(cone_bucket));
}

ConeDB::~ConeDB() {
  if (hashtable!=NULL) {
    free(hashtable);
    hashtable = NULL;
  }
  if (buckets!=NULL) {
    free(buckets);
    buckets = NULL;
  }
}

void ConeDB::addCone(int k,unsigned char tb,int c) {
  int i = find_bucket(k,tb);
  if (i!=-1) {
    // c will be the new cone belonging to k and tb
    buckets[i].cone = c;
  }
  else {
    check_buckets();
    i = hash(k,tb,hashmask);
    buckets[bucket_next].key = k;
    buckets[bucket_next].top_bot = tb;
    buckets[bucket_next].cone = c;
    buckets[bucket_next].next = hashtable[i];
    hashtable[i] = bucket_next;
    ++bucket_next;
  }
}

int ConeDB::findCone(int k,unsigned char tb) {
  int i = find_bucket(k,tb);
  if (i==-1) {
    return -1;
  }
  else {
    return buckets[i].cone;
  }
}

void ConeDB::check_buckets() {
  if (bucket_next >= bucket_size) {
    bucket_size += bucket_block;
    buckets = (cone_bucket*)realloc(buckets,bucket_size*sizeof(cone_bucket));
  }
  if (4*bucket_next >= 3*hashmask) {
    hashmask = hashmask + hashmask + 1;
    hashtable = (int*)realloc(hashtable,(hashmask+1)*sizeof(int));
    int i,h;
    for (i=0; i<=hashmask; ++i) {
      hashtable[i] = -1;
    }
    for (i=0; i<bucket_next; ++i) {
      h = hash(buckets[i].key,buckets[i].top_bot,hashmask);
      buckets[i].next = hashtable[h];
      hashtable[h] = i;
    }
  }
}

int ConeDB::find_bucket(int k,unsigned char tb) {
  int h = hash(k,tb,hashmask);
  for (h=hashtable[h]; h!=-1; h=buckets[h].next) {
    if (buckets[h].key==k && buckets[h].top_bot==tb) {
      return h;
    }
  }
  return -1;
}
