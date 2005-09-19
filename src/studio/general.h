#ifndef __GENERAL_H
#define __GENERAL_H

#include <string>

using namespace std;

class gsStorable
{
	public:
		gsStorable();

		virtual void LoadFromFile(string &s);
		virtual void SaveToFile(string &s);

	public:
		virtual void LoadFromFile(istream &i) = 0;
		virtual void SaveToFile(ostream &i) = 0;
};

#endif
