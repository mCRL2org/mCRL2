#include "string_template.h"
#include <boost/xpressive/xpressive.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>

using namespace boost;
using namespace boost::xpressive;

void StringTemplate::replace(string key, string value)
{
	//subject = regex_replace(subject, as_xpr("%" + key + "%"), value );
	//replace_all(subject, "%" + key + "%", value);
	subject = replace_all_copy(subject, "%" + key + "%", value);
}
/**
 * always returns false. this way, multiple has, replace_by pairs can be 
 * combined in && and || clauses using short circuiting
 */
bool StringTemplate::replace_by(string value)
{
	replace(current_key, value);
	return false;
}
bool StringTemplate::has(string key)
{
	if(subject.find("%"+key+"%") != string::npos)
	{
		current_key = key;
		return true;
	}
	else
	{
		return false;
	}
}

ostringstream& StringTemplate::operator[] (string key)
{ 
  if(!streams.count(key)) 
  { 
    shared_ptr<ostringstream> os(new ostringstream());
    streams.insert(make_pair(key, os));
  }
  return *(streams[key]); 
}

void StringTemplate::finalise()
{
  pair<string, shared_ptr<ostringstream> > v;
  BOOST_FOREACH(v, streams)
  {
    this->replace(v.first, v.second->str());
  }
  //removes dangling commas, right next to a bracket.
  sregex rex = ((s1=as_xpr("[")|"(") >> *_s >> "," >> *_s) | (*_s >> "," >> *_s >> (s2=as_xpr(")")|"]"|":"));
  subject = regex_replace(subject, rex, std::string("$1$2"));
}

StringTemplateFile::StringTemplateFile(string filename)
{
	ifstream f(filename.c_str());
	string fmtline;
	while(getline(f, fmtline))
	{
		//matches <key> = <value> where <key> must be a word and <value> is everything after the '='.
		//supports comments starting with f.ex. #
		sregex rex = bos >> *_s >> (s1= +_w) >> *_s >> '=' >> !_s >> (s2= *_) >> !(as_xpr('\r')) >> eos;
		smatch matches;

		if(regex_match(fmtline, matches, rex))
		{
			format_strings[matches[1]] = matches[2];
		}		
	}
}

string StringTemplateFile::fmt_string(string id)
{
	if(!format_strings.count(id))
	{
		throw "Cannot find key '" + id + "' in formatting file";
	}
	return format_strings.find(id)->second;
}
StringTemplate StringTemplateFile::get(string id)
{
	return StringTemplate(fmt_string(id));
}

string StringTemplateFile::all(string id, map<string, string> substitutions)
{
	StringTemplate sf(get(id));

	pair<string, string> subst;
	BOOST_FOREACH(subst, substitutions)
	{
		sf.replace(subst.first, subst.second);
	}
	return sf.get();
}

