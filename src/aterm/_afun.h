#ifndef _AFUN_H

#define _AFUN_H

#include "afun.h"
#include "byteio.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

int AT_writeAFun(AFun fun, byte_writer *writer);

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif /* _AFUN_H */
