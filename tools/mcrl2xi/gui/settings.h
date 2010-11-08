/*
 * settings.h
 *
 *  Created on: Nov 6, 2010
 *      Author: fstapper
 */

#ifndef MCRL2XI_SETTINGS_H_
#define MCRL2XI_SETTINGS_H_

#include "wx/wx.h"

wxString mcrl2_spec_keywords(wxT("sort cons map var eqn act proc init struct glob "));
wxString mcrl2_proc_keywords(wxT("delta tau sum block allow hide rename comm "));
wxString mcrl2_sort_keywords(wxT("Bool Pos Nat Int Real Set List Bag "));
wxString mcrl2_data_keywords(wxT("true false whr end lambda forall exists div mod in if "));
wxString mcrl2_operator_keywords(wxT("min max succ pred abs floor ceil round exp A2B head tail rhead rtail count Set2Bag Bag2Set "));

#endif /* SETTINGS_H_ */
