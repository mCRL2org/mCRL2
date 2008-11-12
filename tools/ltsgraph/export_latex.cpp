// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./export_latex.cpp

#include "wx.hpp" // precompiled headers

#include "export_latex.h"

#include <wx/textfile.h>

#include <boost/format.hpp>

ExportToLatex::ExportToLatex(wxString _filename, std::vector<nodeLatex> _node, std::vector<edgeLatex> _edge, int _height) : 
		filename(_filename) , node(_node), edge(_edge), height(_height)
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
		double nodeY = (height - node[i].y)/50;
		f%nodeX%nodeY%node[i].num%node[i].num;
		LatexCode += boost::str(f);
	}

	for (unsigned int i=0; i<edge.size(); i++) {
          if (edge[i].numNode1 == edge[i].numNode2) {
            //Draw self-loop
            boost::format f("\\nccircle{%1%}{.5}^{%2%} \n");
            f%edge[i].numNode1%EscSpecChar(edge[i].lbl);
            LatexCode += boost::str(f);
          }
          else {
            boost::format f("\\ncline{%1%}{%2%}^{%3%} \n");
	    f%edge[i].numNode1%edge[i].numNode2%EscSpecChar(edge[i].lbl);
	    LatexCode += boost::str(f);
          }
	}

        // Redraw nodes, to overlap the edges.
	for (unsigned int i=0; i<node.size(); i++) {
		boost::format f("\\rput(%1%,%2%){\\circlenode{%3%}{%4%}} \n");
		double nodeX = (node[i].x)/50;
		double nodeY = (height - node[i].y)/50;
		f%nodeX%nodeY%node[i].num%node[i].num;
		LatexCode += boost::str(f);
	}

	LatexCode += "\\end{pspicture} \n";
	LatexCode += "\\end{document} \n";

	//Create a file
	wxTextFile latex_file(filename);

	if (latex_file.Exists()) {
          if (!latex_file.Open(filename)) {
            return false;
          }
          else {
            latex_file.Clear();
          }
        }
        else {
          if (!latex_file.Create(filename)) {
            return false;
          }
        }

        // Write the code to the file.
        wxString latex_code_wx(LatexCode.c_str(), wxConvLocal);
	latex_file.AddLine(latex_code_wx);
        latex_file.AddLine(wxEmptyString);

	if (!latex_file.Write()) {
          return false;
        }

        if (!latex_file.Close()) {
          return false;
        }
  return true;

}

std::string ExportToLatex::EscSpecChar(std::string str) {

	str = str_replace("_","\\_",str);
	return str;
}

std::string ExportToLatex::str_replace(std::string to_replace, std::string replace_by, std::string replace_in) {
	int i_pos;
	std::string text_left;
	std::string text_right;
	std::string str_result="";

	i_pos = replace_in.find(to_replace);
	if(i_pos == -1 || i_pos == (int)std::string::npos)
	{
		str_result = replace_in;
	}
	else
	{
		while(i_pos != -1 && i_pos != (int)std::string::npos)
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

