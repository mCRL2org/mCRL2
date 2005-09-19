#include "object.h"

gsObject::gsObject(const string &tag, const string &filename, const string &description)
{
	SetTag(tag);
	SetFilename(filename);
	SetDescription(description);
}

void gsObject::LoadFromFile(istream &i)
{
}

void gsObject::SaveToFile(ostream &o)
{
}

string &gsObject::GetTag()
{
	return tag;
}

string &gsObject::GetDescription()
{
	return description;
}

string &gsObject::GetFilename()
{
	return filename;
}

sType &gsObject::GetType()
{
	return type;
}

void gsObject::SetTag(const string &s)
{
	tag = s;
}

void gsObject::SetDescription(const string &s)
{
	description = s;
}

void gsObject::SetFilename(const string &s)
{
	filename = s;

	FILE *f;
	if ( (f = fopen(filename.c_str(),"r")) == NULL )
	{
		perror("fopen");
		fprintf(stderr,"error opening file '%s'\n",filename.c_str());
		return;
	}

	char buf[100];
	int r = fread(buf,100,1,f);

	if ( (r >= 6) && !strncmp(buf,"SpecV1",6) )
	{
		type = sSpecType(true);
	}
	if ( (r >= 4) && !strncmp(buf,"sort",6) )
	{
		type = sSpecType(false);
	}
	if ( (r >= 3) && !strncmp(buf,"des",6) )
	{
		type = sStateSpaceType();
	}

	fclose(f);
}
