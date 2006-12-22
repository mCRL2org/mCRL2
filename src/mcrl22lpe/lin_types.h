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
struct t_lin_options {
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
  bool add_delta;
  bool opt_check_only;
  t_phase opt_end_phase;
  bool opt_noalpha;
  string infilename;
  string outfilename;
  
  t_lin_options()
    : lin_method(lmRegular),
      no_intermediate_cluster(false),
      final_cluster(false),
      newstate(false),
      binary(false),
      statenames(false),
      norewrite(false),
      nofreevars(false),
      nosumelm(false),
      nodeltaelimination(false),
      add_delta(false),
      opt_check_only(false),
      opt_end_phase(phNone),
      opt_noalpha(false)
      
  {}
};

#endif //__LIN_TYPES_H
