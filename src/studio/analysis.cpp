#include <wx/filename.h>
#include "analysis.h"
#include "common.h"

gsAnalysisItem::gsAnalysisItem(gsObject *obj, gsAnalysisItem *parent)
{
	object = obj;
	this->parent = parent;
	tool = NULL;
	uptodate = false;
	available = false;

	if ( this->parent != NULL )
	{
		this->parent->AddChild(this);
	}
}

gsObject *gsAnalysisItem::GetObject()
{
	return object;
}

string gsAnalysisItem::GetTag()
{
	if ( tool == NULL )
	{
		return object->GetTag();
	} else {
		return tool->GetDescription(object);
	}
}

gsAnalysisItem *gsAnalysisItem::GetParent()
{
	return parent;
}

list<gsAnalysisItem *> &gsAnalysisItem::GetChildren()
{
	return children;
}

void gsAnalysisItem::AddChild(gsAnalysisItem *child)
{
	children.push_back(child);
}

gsTool *gsAnalysisItem::GetTool()
{
	return tool;
}

void gsAnalysisItem::SetTool(gsTool *tool)
{
	this->tool = tool;
}

void gsAnalysisItem::ParentModified()
{
	uptodate = false;
}

bool gsAnalysisItem::IsUpToDate()
{
	return uptodate;
}

bool gsAnalysisItem::IsAvailable()
{
	return available;
}

void gsAnalysisItem::ExecutionStarted()
{
}

void gsAnalysisItem::ExecutionEnded()
{
}

void gsAnalysisItem::AvailabilityChanged(bool available)
{
	this->available = available;
}

void gsAnalysisItem::Modified()
{
	list<gsAnalysisItem *>::iterator i;
	for (i=children.begin(); i!=children.end(); i++)
	{
		(*i)->ParentModified();
	}
}

void gsAnalysisItem::DescriptionChanged()
{
}


gsAnalysisTreeData::gsAnalysisTreeData(gsAnalysisItem *item)
{
	this->item = item;
}

gsAnalysisItem *gsAnalysisTreeData::GetAnalysisItem()
{
	return item;
}


gsAnalysis::gsAnalysis(const string &tag, const string &description)
{
	this->tag = tag;
	this->description = description;
	tree = NULL;
}

string &gsAnalysis::GetTag()
{
	return tag;
}

string &gsAnalysis::GetDescription()
{
	return description;
}

void gsAnalysis::SetTag(const string &tag)
{
	this->tag = tag;
}

void gsAnalysis::SetDescription(const string &description)
{
	this->description = description;
}

static void build_tree(wxTreeCtrl *tree, wxTreeItemId root, list<gsAnalysisItem *> &children)
{
	list<gsAnalysisItem *>::iterator i;
	for (i=children.begin(); i!=children.end(); i++)
	{
		gsAnalysisTreeData *data = new gsAnalysisTreeData(*i);
		wxTreeItemId id = tree->AppendItem(root,(*i)->GetTag(),-1,-1,data);
		build_tree(tree,id,(*i)->GetChildren());
		tree->Expand(id);
	}
}

void gsAnalysis::LinkTreeView(wxTreeCtrl *tree)
{
	this->tree = tree;
	this->tree->DeleteAllItems();
	this->tree->AddRoot(wxT("")); // XXX must be before HIDE_ROOT
	this->tree->SetWindowStyle(wxTR_SINGLE|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT); // XXX does this work on windows?

	build_tree(this->tree,this->tree->GetRootItem(),roots);
/*	list<gsAnalysisItem *>::iterator i;
	for (i=roots.begin(); i!=roots.end(); i++)
	{
		gsAnalysisTreeData *data = new gsAnalysisTreeData(*i);
		this->tree->AppendItem(tree->GetRootItem(),(*i)->GetTag(),-1,-1,data);
	}*/
}

void gsAnalysis::UnlinkTreeView()
{
	tree = NULL;
}

wxTreeCtrl *gsAnalysis::GetTreeView()
{
	return tree;
}

void gsAnalysis::AddRoot(gsObject *obj)
{
	if ( tree != NULL )
	{
		gsAnalysisItem *item = new gsAnalysisItem(obj);
		gsAnalysisTreeData *data = new gsAnalysisTreeData(item);
		roots.push_back(item);
		tree->AppendItem(tree->GetRootItem(),item->GetTag(),-1,-1,data);
	}
}

void gsAnalysis::AddTool(gsTool *tool)
{
	if ( (tree != NULL) && (tree->GetSelection() != tree->GetRootItem()) )
	{
		wxString fn = wxFileName::CreateTempFileName(wxT(""));
		gsObject *obj = new gsObject(wxT(""),toSTL(fn),wxT(""));
		gsAnalysisItem *item = new gsAnalysisItem(obj,((gsAnalysisTreeData *) tree->GetItemData(tree->GetSelection()))->GetAnalysisItem());
		item->SetTool(tool);
		gsAnalysisTreeData *data = new gsAnalysisTreeData(item);
		tree->AppendItem(tree->GetSelection(),item->GetTag(),-1,-1,data);
		tree->Expand(tree->GetSelection());
	}
}

void gsAnalysis::RunTool()
{
	if ( (tree != NULL) && (tree->GetSelection() != tree->GetRootItem()) )
	{
		gsAnalysisItem *item = ((gsAnalysisTreeData *) tree->GetItemData(tree->GetSelection()))->GetAnalysisItem();
		item->GetTool()->Execute(item->GetParent()->GetObject(),item->GetObject());
	}
}
