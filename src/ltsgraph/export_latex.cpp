#include "export_latex.h"

#include <fstream>
#include <iostream>

#include <boost/format.hpp>

ExportToLatex::ExportToLatex(const char * _filename, vector<nodeLatex> _node, vector<edgeLatex> _edge) : 
		filename(_filename) , node(_node), edge(_edge)
{}

bool ExportToLatex::Generate() {

	//Create the latex code
	LatexCode  = "\\documentclass{article} \n";
	LatexCode += "\\usepackage {pstricks,pst-node} \n";
	LatexCode += "\\begin{document}\n";
	LatexCode += "\\begin{pspicture}(0.0,12.0) \n";
	LatexCode += "\\psset{arrows=->,shortput=nab} \n";

	for (unsigned int i=0; i<node.size(); i++) {
		boost::format f("\\rput(%1%,%2%){\\circlenode{%3%}{%4%}} \n");
		double nodeX = (node[i].x)/50;
		double nodeY = (node[i].y)/50;
		f%nodeX%nodeY%node[i].num%node[i].num;
		LatexCode += boost::str(f);
	}

	for (unsigned int i=0; i<edge.size(); i++) {
		boost::format f("\\ncline{%1%}{%2%}^{%3%} \n");
		f%edge[i].numNode1%edge[i].numNode2%EscSpecChar(edge[i].lbl);
		LatexCode += boost::str(f);
	}

	LatexCode += "\\end{pspicture} \n";
	LatexCode += "\\end{document} \n";

	//Create a file
	ofstream file(filename);

	if (!file)
		return false;
 
	file << LatexCode;

	if (!file)
		return false;


  return true;

}

string ExportToLatex::EscSpecChar(string str) {

	str = str_replace("_","\\_",str);
	return str;
}

string ExportToLatex::str_replace(string to_replace, string replace_by, string replace_in) {
	int i_pos;
	string text_left;
	string text_right;
	string str_result="";

	i_pos = replace_in.find(to_replace);
	if(i_pos == -1 || i_pos == (int)string::npos)
	{
		str_result = replace_in;
	}
	else
	{
		while(i_pos != -1 && i_pos != (int)string::npos)
		{
			text_left = replace_in.substr(0, i_pos);
			text_right = replace_in.substr((i_pos + to_replace.size()), (replace_in.size() - (i_pos + to_replace.size())));
			replace_in = text_right;
			i_pos = replace_in.find(to_replace);
			str_result = str_result + text_left + replace_by;
		}
		str_result += text_right;
	}
	return str_result;
}

