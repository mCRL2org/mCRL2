// Author(s): Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lin_types.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_LIN_TYPES_H
#define MCRL2_LPS_LIN_TYPES_H

#include <stdbool.h>
#include <string>

using namespace std;

//Type definitions for all linearisers

//t_lin_method represents the available linearisation methods
typedef enum { lmStack, lmRegular, lmRegular2 } t_lin_method;

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
  bool check_only;
  t_phase end_phase;
  bool noalpha;
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
      check_only(false),
      end_phase(phNone),
      noalpha(false)
  {}
};

#endif //MCRL2_LPS_LIN_TYPES_H
