#ifndef __ANALYSIS_H
#define __ANALYSIS_H

#include <string>
#include <list>
#include <wx/treectrl.h>
#include "types.h"
#include "object.h"
#include "tool.h"

using namespace std;

class gsAnalysisItem: public gsToolMonitor
{
	public:
		gsAnalysisItem(gsObject *obj, gsAnalysisItem *parent = NULL);

		virtual gsObject *GetObject();
		virtual string GetTag();
		virtual gsAnalysisItem *GetParent();
		virtual list<gsAnalysisItem *> &GetChildren();
		virtual void AddChild(gsAnalysisItem *child);
		virtual gsTool *GetTool();
		virtual void SetTool(gsTool *tool);
		virtual void ParentModified();
		virtual bool IsUpToDate();
		virtual bool IsAvailable();

		virtual void ExecutionStarted();
		virtual void ExecutionEnded();
		virtual void AvailabilityChanged(bool available);
		virtual void Modified();
		virtual void DescriptionChanged();

	private:
		gsObject *object;
		gsAnalysisItem *parent;
		list<gsAnalysisItem *> children;
		gsTool *tool;
		bool uptodate;
		bool available;
};

class gsAnalysisTreeData: public wxTreeItemData
{
	public:
		gsAnalysisTreeData(gsAnalysisItem *item);

		gsAnalysisItem *GetAnalysisItem();
	private:
		gsAnalysisItem *item;
};

class gsAnalysis
{
	public:
		gsAnalysis(const string &tag, const string &description);

		virtual string &GetTag();
		virtual string &GetDescription();

		virtual void SetTag(const string &tag);
		virtual void SetDescription(const string &description);

		virtual void LinkTreeView(wxTreeCtrl *tree);
		virtual void UnlinkTreeView();

		virtual wxTreeCtrl *GetTreeView();

		virtual void AddRoot(gsObject *obj);
		virtual void AddTool(gsTool *tool);

		virtual void RunTool();

	private:
		string tag;
		string description;
		wxTreeCtrl *tree;
		list<gsAnalysisItem *> roots;
};

#endif
