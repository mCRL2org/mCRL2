// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./ltsgraph_backup.h

#ifndef ltsgraph_backup_h
#define ltsgraph_backup_h

#include <vector>

#include "node.h"
#include "edge.h"

#include <wx/gdicmn.h>


class LtsgraphBackup
{
public:
	LtsgraphBackup(wxSize _sz);

	void SetLayout(std::vector<Node*> _BackupVectNode, std::vector<edge*> _BackupVectEdge);
	void SetInformation(wxString _InitialState, wxString _NumStates, wxString _NumTransitions, wxString _NumLabels);
	void SetAlgoSettings(double _EdgeStiffness, double _NodeStrength, double _NaturalLength);
	void SetOtherSettings(int _StateRadius = 10, bool _StateLabel = true, bool _TransitionLabel = true);

	bool Backup(wxString filename = wxT("backup.ltsgraph"));

	bool Restore(wxString filename = wxT("backup.ltsgraph"));

	std::vector<Node*> GetVectNode();
	std::vector<edge*> GetVectEdge();
		
	wxString GetInitialState();
	wxString GetNumStates();
	wxString GetNumTransitions();
	wxString GetNumLabels();

	double GetEdgeStiffness();
  double GetNodeStrength();
  double GetNaturalLength();

	int  GetStateRadius();
	bool GetStateLabel();
	bool GetTransitionLabel();

private:

	wxSize sz;

	std::vector<Node*> BackupVectNode;
	std::vector<edge*> BackupVectEdge;

	wxString InitialState;
	wxString NumStates;
	wxString NumTransitions;
	wxString NumLabels;

	double NodeStrength;
  double EdgeStiffness;
  double NaturalLength;

	int  StateRadius;
	bool StateLabel;
	bool TransitionLabel;
	
};



#endif //ltsgraph_backup_h
