#ifndef __OBJECT_H
#define __OBJECT_H

#include <wx/window.h>
#include "general.h"
#include "types.h"

class gsObject: public gsStorable
{
	public:
		gsObject(const string &tag, const string &filename, const string &description);

		virtual void LoadFromFile(istream &i);
		virtual void SaveToFile(ostream &o);

		virtual string &GetTag();
		virtual string &GetDescription();
		virtual string &GetFilename();
		virtual sType &GetType();

		virtual void SetTag(const string &s);
		virtual void SetDescription(const string &s);
		virtual void SetFilename(const string &s);

	private:
		string tag;
		string filename;
		string description;
		sType type;
};

class gsObjectCollection
{
	public:
		virtual gsObject *GetObject(wxWindow *parent) = 0;
		virtual bool AddObject(wxWindow *parent, gsObject *obj) = 0;
};

#endif
