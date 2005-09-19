#ifndef __PROJECT_H
#define __PROJECT_H

#include <list>
#include "general.h"
#include "object.h"
#include "analysis.h"

using namespace std;

class gsProject: public gsStorable
{
	public:
		gsProject();

		virtual void LoadFromFile(istream &i);
		virtual void SaveToFile(ostream &o);

		virtual void AddObject(gsObject *obj);
		virtual void RemoveObject(gsObject *obj);
		virtual void RemoveObject(const string &tag);
		virtual gsObject *GetObject(const string &tag);

		virtual void AddAnalysis(gsAnalysis *analysis);
		virtual void RemoveAnalysis(gsAnalysis *analysis);
		virtual void RemoveAnalysis(const string &tag);
		virtual gsAnalysis *GetAnalysis(const string &tag);

	private:
		list<gsObject *> objects;
		list<gsAnalysis *> analyses;

/*		virtual void AddFile(const string &tag, const string &filename, const string &description);
		virtual void AddAnalysis(const string &tag, const string &description, gsAnalysis *analysis);
		virtual void RemoveFile(const string &tag);
		virtual void RemoveAnalysis(const string &tag);

		virtual const string &GetFileName(const string &tag);
		virtual const string &GetFileDescription(const string &tag);
		virtual const string &GetAnalysisDescription(const string &tag);
		virtual gsAnalysis *GetAnalysisObject(const string &tag);

		virtual void RenameFileTag(const string &oldtag, const string &newtag);
		virtual void RenameAnalysisTag(const string &oldtag, const string &newtag);
		virtual void SetFileName(const string &tag, const string &filename);
		virtual void SetFileDescription(const string &tag, const string &description);
		virtual void SetAnalysisDescription(const string &tag, const string &description);
		virtual void SetAnalysisObject(const string &tag, gsAnalysis *analysis);*/
};

#endif
