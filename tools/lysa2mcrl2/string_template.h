// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
// A class that manages a simple form to string templating. StringTemplateFile encapsulates a
// file with <key> = <template code> lines, each of which corresponds to a StringTemplate class.
// variables inside a template string are specified as %varname%. StringTemplate overloads the
// [] operator, such that keys map to %varname% instances in the template string. The [] operator
// returns a stringstream whose content replaces every %varname% in the template string when get()
// is called.

#ifndef MCRL2_FORMATTER_H
#define MCRL2_FORMATTER_H

#include <sstream>
#include <map>
#include <fstream>
#include <string>
#include <boost/smart_ptr.hpp>


using namespace std;
using boost::shared_ptr;

class StringTemplate
{
protected:
	string subject;
	string current_key;
  map<string, shared_ptr<ostringstream> > streams;
public:
	StringTemplate(string format_string) : subject(format_string) {};

	void replace(string key, string value);
	bool has(string key);
	bool replace_by(string value);

  ostringstream& operator[] (string key);
  void finalise();
	string get() { finalise(); return subject; }
};


class StringTemplateFile
{
protected:
	map<string, string> format_strings;
public:
	StringTemplateFile(string filecontent);
	StringTemplate get(string id);
  string fmt_string(string id);
	string all(string id, map<string, string> substitutions);
};

#endif
