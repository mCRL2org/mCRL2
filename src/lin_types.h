#ifndef __LIN_TYPES_H
#define __LIN_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

//Type definitions for all linearisers

typedef enum { lmStack, lmRegular, lmRegular2, lmAlternative } t_lin_method;
//t_lin_method represents the available linearisation methods

typedef struct {
  t_lin_method lin_method;
  bool no_intermediate_cluster;
  bool final_cluster;
  bool newstate;
  bool binary;
  bool statenames;
  bool norewrite;
  bool nofreevars;
} t_lin_options;
//t_lin_options represents the options of the lineariser

#ifdef __cplusplus
}
#endif

#endif //__LIN_TYPES_H
