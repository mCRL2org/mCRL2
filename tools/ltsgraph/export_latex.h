// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./export_latex.h

#ifndef ltx_h
#define ltx_h

#include <wx/string.h>
#include <vector>
#include <string>

typedef struct {
	unsigned int num;
	double x, y;
	std::string lbl;
} nodeLatex;

typedef struct {
	unsigned int numNode1, numNode2;
	std::string lbl;
} edgeLatex ;



class ExportToLatex
{
public:
	ExportToLatex(wxString _filename, std::vector<nodeLatex> _node, std::vector<edgeLatex> _edge, int _height);
	bool Generate();
	

private:

	std::string EscSpecChar(std::string);//To escape special characters
	std::string str_replace(std::string to_replace, std::string replace_by, std::string replace_in);

	wxString filename;
	std::string LatexCode;

	std::vector<nodeLatex> node;
	std::vector<edgeLatex> edge;

	int height; //height of the drawing area


};



#endif //latex_h
