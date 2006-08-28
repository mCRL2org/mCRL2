#ifndef ltsgraph_backup_h
#define ltsgraph_backup_h

#include <vector>

#include "node.h"
#include "edge.h"

#include <wx/gdicmn.h>


using namespace std;

class LtsgraphBackup
{
public:
	LtsgraphBackup(wxSize _sz);

	void SetLayout(vector<Node*> _BackupVectNode, vector<edge*> _BackupVectEdge);
	void SetInformation(wxString _InitialState, wxString _NumStates, wxString _NumTransitions, wxString _NumLabels);
	void SetAlgoSettings(double _EdgeStiffness, double _NodeStrength, double _NaturalLength);
	void SetOtherSettings(int _StateRadius = 10, bool _StateLabel = true, bool _TransitionLabel = true);

	bool Backup(string filename = "backup.ltsgraph");

	bool Restore(string filename = "backup.ltsgraph");

	vector<Node*> GetVectNode();
	vector<edge*> GetVectEdge();
		
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

	vector<Node*> BackupVectNode;
	vector<edge*> BackupVectEdge;

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
