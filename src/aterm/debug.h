/**
  * Debugging macros
  */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#define EXEC(cmd) (cmd); fflush(stdout)

#ifdef DEBUG_ALL
#define DEBUG_MEM
#define DEBUG_ARITY
#endif

#ifdef DEBUG_MEM
#define DBG_MEM(cmd) EXEC(cmd)
#else
#define DBG_MEM(cmd)
#endif

#ifdef DEBUG_ARITY
#define DBG_ARITY(cmd) EXEC(cmd)
#else
#define DBG_ARITY(cmd)
#endif

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
