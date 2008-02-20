#ifndef WORKAROUNDS_H__
#define WORKAROUNDS_H__

//String manipulation
//-------------------
//
//Re-implementation of strdup (because it is not part of the C99 standard)
#if !(defined __sun__ || defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER || defined __MINGW32__ || defined __CYGWIN__ || defined __FreeBSD__)
inline char *strdup(const char *s) {
    char *p;

    if((p = (char *)malloc(strlen(s) + 1)) == NULL)
      return NULL;
    return strcpy(p, s);
} 
#endif

//Declare a local array NAME of type TYPE and SIZE elements (where SIZE
//is not a constant value)
#ifndef DECL_A
#define DECL_A(NAME,TYPE,SIZE)  TYPE NAME[SIZE]
#define FREE_A(NAME)            
#endif

//Make sure __func__ works (as well as possible)
#ifndef __func__
#ifdef __FUNCTION__
#define __func__ __FUNCTION__
#else
#define __func__ __FILE__
#endif
#endif

#endif
