#include "project.h"

gsProject::gsProject()
{
}

void gsProject::LoadFromFile(istream &i)
{
}

void gsProject::SaveToFile(ostream &o)
{
}

void gsProject::AddObject(gsObject *obj)
{
	objects.push_back(obj);
}

void gsProject::RemoveObject(gsObject *obj)
{
	objects.remove(obj);
}

void gsProject::RemoveObject(const string &tag)
{
	list<gsObject *>::iterator i;
	for (i = objects.begin(); i != objects.end(); i++)
	{
		if ( (*i)->GetTag() == tag )
		{
			objects.erase(i);
			break;
		}
	}
}

gsObject *gsProject::GetObject(const string &tag)
{
	list<gsObject *>::iterator i;
	for (i = objects.begin(); i != objects.end(); i++)
	{
		if ( (*i)->GetTag() == tag )
		{
			return *i;
		}
	}

	return NULL;
}

void gsProject::AddAnalysis(gsAnalysis *analysis)
{
	analyses.push_back(analysis);
}

void gsProject::RemoveAnalysis(gsAnalysis *analysis)
{
	analyses.remove(analysis);
}

void gsProject::RemoveAnalysis(const string &tag)
{
	list<gsAnalysis *>::iterator i;
	for (i = analyses.begin(); i != analyses.end(); i++)
	{
		if ( (*i)->GetTag() == tag )
		{
			analyses.erase(i);
			break;
		}
	}
}

gsAnalysis *gsProject::GetAnalysis(const string &tag)
{
	list<gsAnalysis *>::iterator i;
	for (i = analyses.begin(); i != analyses.end(); i++)
	{
		if ( (*i)->GetTag() == tag )
		{
			return *i;
		}
	}

	return NULL;
}
