/*{{{  credits */

/* Added by JFG */
/* This file is made by Jan Friso Groote, January 2000. It contains
   implementations of the ATtable and ATindexedSet, which are different
   implementations of hashtables. The implementations are 
   (intended to be) particularily efficient, both in space
   (appr 12 bytes are necessary for a set entry, and 16 for
   a table entry) and time. It is an improvement of the previously
   implemented ATtables which were a straightforward but not
   so efficient (both in time and space).
   Note that hashtables can only grow, until
   they are destroyed. Also resetting the hashtables does 
   not help reducing memory consumption */

/*}}}  */
/*{{{  includes */

#include "aterm2.h"
#include "_aterm.h"
#include "util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*}}}  */
/*{{{  defines */

/* ======================================================= */

#define STEP 1   /* The position on which the next hash entry
		    searched */
#define INITIAL_NR_OF_TABLES 8
#define TABLE_SHIFT 13
#define ELEMENTS_PER_TABLE (1<<TABLE_SHIFT)
#define modELEMENTS_PER_TABLE(n) ((n) & (ELEMENTS_PER_TABLE-1))
#define divELEMENTS_PER_TABLE(n) ((n) >> TABLE_SHIFT)

/*-----------------------------------------------------------*/

/* in the hashtable we use the following constants to
   indicate designated positions */
#define EMPTY -1
#define DELETED -2

#define a_prime_number 134217689

/* A very simple hashing function. */

#define hashcode(a,sizeMinus1) (((((long) a) >> 2) * a_prime_number ) & sizeMinus1)

/*}}}  */
/*{{{  types */

struct _ATermTable
{
  long sizeMinus1;
  long nr_entries; /* the number of occupied positions in the hashtable,
		      including the elements that are explicitly marked
		      as deleted */
  long nr_deletions;
  int max_load;
  long max_entries;
  long *hashtable; long nr_tables;
  struct _ATerm ***keys;
  long nr_free_tables;
  long first_free_position;
  long **free_table;
  struct _ATerm ***values;
};

/*}}}  */

/*{{{  static long approximatepowerof2(long n) */

/**
 * return smallest 2^m-1 larger or equal than n, where
 * returned size must at least be 127 
 */

static long approximatepowerof2(long n)
{
  int mask = n;

  while(mask >>= 1) {
    n |= mask;
  }

  if (n<127) n=127;
  return n;
}

/*}}}  */
/*{{{  static long calc_long_max() */
static long calc_long_max()
{
  long try_long_max;
  long long_max;
  long delta;

  try_long_max = 1;
  do {
    long_max = try_long_max;
    try_long_max = long_max * 2;
  } while (try_long_max > 0);

  delta = long_max;
  while (delta > 1) {
    while (long_max + delta < 0) {
      delta /= 2;
    }
    long_max += delta;
  }

  return long_max;

}
/*}}}  */
/*{{{  static long calculateNewSize(sizeMinus1, nrdel, nrentries) */

static long calculateNewSize
(long sizeMinus1, long nr_deletions, long nr_entries)
{ 

  /* Hack: LONG_MAX (limits.h) is often unreliable, we need to find
   * out the maximum possible value of a signed long dynamically.
   */
  static long st_long_max = 0;

  /* the resulting length has the form 2^k-1 */

  if (nr_deletions >= nr_entries/2) { 
    return sizeMinus1;
  }

  if (st_long_max == 0) {
    st_long_max = calc_long_max();
  }

  if (sizeMinus1 > st_long_max / 2) {
    return st_long_max-1;
  }

  return (2*sizeMinus1)+1;
}

/*}}}  */
/*{{{  static ATerm tableGet(ATerm **tableindex, long n) */

static ATerm tableGet(ATerm **tableindex, long n)
{ 
  assert(n>=0);
  return tableindex[divELEMENTS_PER_TABLE(n)][modELEMENTS_PER_TABLE(n)];
}

/*}}}  */
/*{{{  static void insertKeyvalue(set, long n, ATerm t, ATerm v) */

static void insertKeyValue(ATermIndexedSet s, 
			   long n, ATerm t, ATerm v)
{
  long x,y,i;
  ATerm *keytable, *valuetable;

  x = divELEMENTS_PER_TABLE(n);
  if (x>=s->nr_tables) { 
    s->keys = (ATerm **)realloc(s->keys,
				sizeof(ATerm *)*s->nr_tables*2);
    if (s->keys==NULL) { 
      ATerror("insertKeyValue: Cannot extend key table\n");
    }

    if (s->values!=NULL) { 
      s->values = (ATerm **)realloc(s->values,
				    sizeof(ATerm *)*2*s->nr_tables);
      if(s->values == NULL) { 
	ATerror("insertKeyValue: Cannot extend value table\n");
      }
    }

    for(i=s->nr_tables; i<2*s->nr_tables; i++) { 
      s->keys[i] = NULL; 
      if (s->values != NULL) {
	s->values[i] = NULL;
      }
    }
    s->nr_tables = s->nr_tables*2;
  }

  keytable = s->keys[x];
  if (keytable == NULL) { 
    /* create a new key table */
    keytable = calloc(sizeof(ATerm), ELEMENTS_PER_TABLE);
    s->keys[x] = keytable;
    if (keytable == NULL) { 
      ATerror("insertKeyValue: Cannot create new key table\n");
    }
    ATprotectArray(keytable, ELEMENTS_PER_TABLE);

    if (s->values != NULL) { 
      valuetable = calloc(sizeof(ATerm), ELEMENTS_PER_TABLE);
      s->values[x] = valuetable;
      if (valuetable == NULL) { 
	ATerror("insertKeyValue: Cannot create new value table\n");
      }
      ATprotectArray(valuetable, ELEMENTS_PER_TABLE);
    }
  }

  assert(keytable != NULL);

  y = modELEMENTS_PER_TABLE(n);
  keytable[y] = t;
  if(s->values != NULL) { 
    s->values[x][y] = v; 
  }
}

/*}}}  */
/*{{{  static long hashPut(ATermTable s, ATerm key, long n) */

static long hashPut(ATermTable s, ATerm key, long n)
{  
  long c,v;

  /* Find a place to insert key, 
     and find whether key already exists */

  assert(n>=0);

  c = hashcode(key, s->sizeMinus1);

  while (1) {
    v = s->hashtable[c];
    if (v == EMPTY || v == DELETED) {
      /* Found an empty spot, insert a new index belonging to key */
      s->hashtable[c] = n;
      return n;
    }

    if (ATisEqual(tableGet(s->keys, v), key)) {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + STEP) & s->sizeMinus1;
  } 
}

/*}}}  */
/*{{{  static void hashResizeSet(ATermIndexedSet s) */

static void hashResizeSet(ATermIndexedSet s)
{
  long i,oldsize;
  ATerm t;

  oldsize = s->sizeMinus1;
  s->sizeMinus1 = calculateNewSize(s->sizeMinus1,
				   s->nr_deletions, s->nr_entries);

  s->hashtable = (long *)realloc(s->hashtable, 
				 sizeof(long) * (1+s->sizeMinus1));
  s->max_entries = ((s->sizeMinus1/100)*s->max_load);
  if (s->hashtable==NULL) { 
#ifndef NDEBUG
    fprintf(stderr,"No memory to increase the size of the hash table\n");
#endif
    s->sizeMinus1 = oldsize;
    if(s->nr_entries-s->nr_deletions+2>=s->sizeMinus1) {
      ATerror("hashResizeSet: Hashtable is full\n");
    }

    /* I do not know whether the bound below is very optimal,
       but we are anyhow in a precarious situation now */
    s->max_entries = (s->nr_entries-s->nr_deletions+s->sizeMinus1)/2;
    s->hashtable = (long *)malloc(sizeof(long) * (1+oldsize));
    if (s->hashtable == NULL) {
      ATerror("hashResizeSet: No memory to re-claim hashtable\n");
    }
  }  else {
#ifndef NDEBUG
    if(!silent) { 
      fprintf(stderr,"Hashtable has been resized to %ld with %ld entries.\n",
	      s->sizeMinus1+1,s->nr_entries);
    }
#endif
  }

  /* reset the hashtable. */
  for(i=0; i<=s->sizeMinus1 ; i++ ) { 
    s->hashtable[i] = EMPTY; 
  }

  /* rebuild the hashtable again */
  for (i=0; i<s->nr_entries; i++) { 
    t = tableGet(s->keys, i);
    if (t != NULL) { 
      hashPut(s, t, i);
    }
  }  
  s->nr_deletions = 0;
}

/*}}}  */
/*{{{  static ATermList tableContent(ATerm **tableidx, entries) */

static ATermList tableContent(ATerm **tableindex,long nr_entries)
{ 
  long i;
  ATerm t;
  ATermList result = ATempty;

  for (i=nr_entries-1; i>=0; i--) {
    t = tableGet(tableindex, i);
    if (t != NULL) { 
      result = ATinsert(result, t);
    }
  }
  return result;
}

/*}}}  */

/*{{{  ATermIndexedSet ATindexedSetCreate(init_size, max_load_pct) */

ATermIndexedSet ATindexedSetCreate(long initial_size, int max_load_pct)
{
  long i;
  ATermIndexedSet hashset;

  hashset = (ATermIndexedSet)malloc(sizeof(struct _ATermTable));
  if (hashset==NULL) { 
    ATerror("ATindexedSetCreate: cannot allocate new ATermIndexedSet n");
  }
  hashset->sizeMinus1 = approximatepowerof2(initial_size);
  hashset->nr_entries = 0;
  hashset->nr_deletions = 0;
  hashset->max_load = max_load_pct;
  hashset->max_entries = ((hashset->sizeMinus1/100)*hashset->max_load);
  hashset->hashtable=
    (long *)malloc(sizeof(long)*(1+hashset->sizeMinus1));
  if (hashset->hashtable==NULL) { 
    ATerror("ATindexedSetCreate: cannot allocate ATermIndexedSet "
	    "of %d entries\n", initial_size);
  }
  for(i=0 ; i<=hashset->sizeMinus1 ; i++ ) { 
    hashset->hashtable[i] = EMPTY; 
  }

  hashset->nr_tables = INITIAL_NR_OF_TABLES;
  hashset->keys = (ATerm **)calloc(hashset->nr_tables,
				   sizeof(ATerm *));
  if (hashset->keys == NULL) {
    ATerror("ATindexedSetCreate: cannot creat key index table\n");
  }

  hashset->nr_free_tables = INITIAL_NR_OF_TABLES;
  hashset->first_free_position = 0;
  hashset->free_table=calloc(sizeof(long *),
			     hashset->nr_free_tables);
  if (hashset->free_table == NULL) { 
    ATerror("ATindexedSetCreate: cannot allocate table to store deleted elements\n");
  }

  hashset->values = NULL;

  return hashset;
}

/*}}}  */
/*{{{  void ATindexedSetDestroy(ATermIndexedSet hashset) */

void ATindexedSetDestroy(ATermIndexedSet hashset)
{ 
  ATtableDestroy(hashset);
}

/*}}}  */
/*{{{  void ATindexedSetReset(ATermIndexedSet hashset) */

void ATindexedSetReset(ATermIndexedSet hashset)
{ 
  ATtableReset(hashset);
}

/*}}}  */
/*{{{  static long keyPut(ATermIndexedSet hashset, key, value, ATbool *isnew)*/

static long keyPut(ATermIndexedSet hashset, ATerm key, 
		   ATerm value, ATbool *isnew)
{ 
  long n,m;

  if(hashset->first_free_position == 0) { 
    m = hashset->nr_entries; 
    n = hashPut(hashset,key,m);
    if (n != m)  { 
      if (isnew != NULL) {
	*isnew = ATfalse;
      }
      if(value != NULL) { 
	assert(hashset->values!=NULL);
	hashset->values[ divELEMENTS_PER_TABLE(n)]
	  [ modELEMENTS_PER_TABLE(n)] = value;
      }
      return n;
    }
    hashset->nr_entries++;
  } else { 
    m = hashset->free_table
      [divELEMENTS_PER_TABLE(hashset->first_free_position-1)]
      [modELEMENTS_PER_TABLE(hashset->first_free_position-1)]; 
    n = hashPut(hashset, key, m);
    if (n != m) { 
      if (isnew != NULL) {
	*isnew = ATfalse;
      }
      if(value != NULL) { 
	assert(hashset->values != NULL);
	hashset->values[ divELEMENTS_PER_TABLE(n)]
	  [ modELEMENTS_PER_TABLE(n)] = value;
      }
      return n;
    }
    hashset->first_free_position--;
  }

  if (isnew != NULL) {
    *isnew = ATtrue;
  }
  insertKeyValue(hashset, n, key, value);
  if(hashset->nr_entries >= hashset->max_entries) {
    hashResizeSet(hashset); /* repaired by Jan Friso Groote, 25/7/00 */
  }

  return n;
}

/*}}}  */
/*{{{  long ATindexedSetPut(ATermIndexedSet hashset, elem, *isnew) */

/**
 * insert entry elem into the hashtable, and deliver
 * an index. If elem is already in the set, deliver 0 
 */

long ATindexedSetPut(ATermIndexedSet hashset, ATerm elem, ATbool *isnew)
{
  return keyPut(hashset, elem, NULL, isnew);
}

/*}}}  */
/*{{{  long ATindexedSetGetIndex(ATermIndexedSet hashset, ATerm key) */

long ATindexedSetGetIndex(ATermIndexedSet hashset, ATerm elem)
{ 
  long c,start,v;

  start = hashcode(elem, hashset->sizeMinus1);
  c = start;
  do {
    v=hashset->hashtable[c];
    if(v == EMPTY) {
      return -1;
    }

    if(v != DELETED && ATisEqual(elem,tableGet(hashset->keys, v))) {
      return v;
    }

    c = (c+STEP) & hashset->sizeMinus1;
  } while (c != start);

  return -1;
}

/*}}}  */
/*{{{  void ATindexedSetRemove(ATermIndexedSet hashset, ATerm elem) */

void ATindexedSetRemove(ATermIndexedSet hashset, ATerm elem)
{ 
  ATtableRemove(hashset, elem);
}

/*}}}  */
/*{{{  ATermList ATindexedSetElements(ATermIndexedSet hashset) */

ATermList ATindexedSetElements(ATermIndexedSet hashset)
{ 
  return tableContent(hashset->keys, hashset->nr_entries);
}

/*}}}  */
/*{{{  ATerm ATindexedSetGetElem(ATermIndexedSet hashset, long index) */

ATerm ATindexedSetGetElem(ATermIndexedSet hashset, long index)
{ 
  assert((index>=0) && (hashset->nr_entries>index));
  return tableGet(hashset->keys, index);
}

/*}}}  */

/*{{{  ATermTable ATtableCreate(long initial_size, int max_load_pct) */

ATermTable ATtableCreate(long initial_size, int max_load_pct)
{ 
  ATermTable hashtable;

  hashtable = (ATermTable)ATindexedSetCreate(initial_size,
					     max_load_pct);

  hashtable->values = (ATerm **)calloc(hashtable->nr_tables,
				       sizeof(ATerm *));

  if (hashtable->values == NULL) { 
    ATerror("ATtableCreate: cannot creat value index table\n");
  }

  return hashtable;
}

/*}}}  */
/*{{{  void ATtableDestroy(ATermTable table) */

void ATtableDestroy(ATermTable table)
{ 
  long i;

  free(table->hashtable);
  for(i=0; ((i<table->nr_tables) && (table->keys[i]!=NULL)) ; i++) { 
    ATunprotectArray(table->keys[i]);
    free(table->keys[i]);
  }

  free(table->keys);

  if(table->values != NULL) { 
    for(i=0; ((i<table->nr_tables) && 
	      (table->values[i]!=NULL)); i++) { 
      ATunprotectArray(table->values[i]);
      free(table->values[i]);
    }

    free(table->values);
  }

  for(i=0; ((i<table->nr_free_tables) &&
	    (table->free_table[i]!=NULL)) ; i++) { 
    free(table->free_table[i]);
  }

  free(table->free_table);

  free(table);
}

/*}}}  */
/*{{{  void ATtableReset(ATermTable table) */

void ATtableReset(ATermTable table)
{ 
  long i;

  table->nr_entries = 0;
  table->nr_deletions = 0;

  for(i=0; i<=table->sizeMinus1 ; i++) { 
    table->hashtable[i] = EMPTY;
  }

  for(i=0; ((i<table->nr_tables) && 
	    (table->keys[i]!=NULL)); i++) { 
    memset(table->keys[i], 0, sizeof(ATerm)*ELEMENTS_PER_TABLE);
    if (table->values!=NULL) {
      memset(table->values[i], 0, sizeof(ATerm)*ELEMENTS_PER_TABLE);
    }
  }

  for(i=0; ((i<table->nr_free_tables) &&
	    (table->free_table[i]!=NULL)); i++) { 
    memset(table->free_table[i], 0, ELEMENTS_PER_TABLE);
  }
  table->first_free_position = 0;
}

/*}}}  */
/*{{{  void ATtablePut(ATermTable table, ATerm key, ATerm value) */

void ATtablePut(ATermTable table, ATerm key, ATerm value)
{ 
  /* insert entry key into the hashtable, and deliver
     an index. If key is already in the set, deliver 0 */

  ATbool isnew;

  keyPut(table, key, value, &isnew);
}

/*}}}  */
/*{{{  ATerm ATtableGet(ATermTable table, ATerm key) */

ATerm ATtableGet(ATermTable table, ATerm key)
{ 
  long v;

  v = ATindexedSetGetIndex(table, key);
  if(v<0) {
    return NULL;
  }
  return tableGet(table->values, v);
}

/*}}}  */
/*{{{  void ATtableRemove(ATermTable table, ATerm key) */

void ATtableRemove(ATermTable table, ATerm key)
{ 
  long start,c,v,x,y,i;
  long *ltable;

  start = hashcode(key,table->sizeMinus1);
  c = start;
  while(1) {
    v = table->hashtable[c];
    if(v == EMPTY) {
      return;
    }
    if(v != DELETED && ATisEqual(key, tableGet(table->keys, v))) {
      break;
    }

    c = (c + STEP) & table->sizeMinus1;
    if (c == start) {
      return;
    }
  }

  table->hashtable[c] = DELETED;

  insertKeyValue(table, v, NULL, NULL);

  x=divELEMENTS_PER_TABLE(table->first_free_position);
  if (x>=table->nr_free_tables) { 
    table->free_table = (long **)realloc(table->free_table,
					 sizeof(long)*table->nr_free_tables*2);
    if (table->free_table==NULL) {
      ATerror("ATtableRemove: Cannot allocate memory for free table index\n");
    }
    for(i=table->nr_free_tables; i<2*table->nr_free_tables; i++) {
      table->free_table[i] = NULL;
    }

    table->nr_free_tables = table->nr_free_tables*2;
  }

  ltable = table->free_table[x];
  if (ltable == NULL) {
    /* create a new key table */
    ltable = calloc(sizeof(long),ELEMENTS_PER_TABLE);
    table->free_table[x] = ltable;
    if (ltable == NULL) { 
      ATerror("ATtableRemove: Cannot create new free table\n");
    }
  }

  y = modELEMENTS_PER_TABLE(table->first_free_position);
  ltable[y] = v;
  table->first_free_position++;
  table->nr_deletions++; 
}

/*}}}  */
/*{{{  ATermList ATtableKeys(ATermTable table) */

ATermList ATtableKeys(ATermTable table)
{ 
  return tableContent(table->keys, table->nr_entries);
}

/*}}}  */
/*{{{  ATermList  ATtableValues(ATermTable table) */

ATermList  ATtableValues(ATermTable table)
{ 
  return tableContent(table->values, table->nr_entries);
}

/*}}}  */
