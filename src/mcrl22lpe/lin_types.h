#ifndef __LIN_TYPES_H
#define __LIN_TYPES_H

#include <stdbool.h>
#include <string>

using namespace std;

//Type definitions for all linearisers

//t_lin_method represents the available linearisation methods
typedef enum { lmStack, lmRegular, lmRegular2, lmAlternative } t_lin_method;

//t_phase represents the phases at which the program should be able to stop
typedef enum { phNone, phParse, phTypeCheck, phAlphaRed, phDataImpl } t_phase;

//t_lin_options represents the options of the lineariser
typedef struct {
  t_lin_method lin_method;
  bool no_intermediate_cluster;
  bool final_cluster;
  bool newstate;
  bool binary;
  bool statenames;
  bool norewrite;
  bool nofreevars;
  bool nosumelm;
  bool nodeltaelimination;
  bool opt_check_only;
  t_phase opt_end_phase;
  bool opt_noalpha;
  string infilename;
  string outfilename;
} t_lin_options;

#endif //__LIN_TYPES_H
