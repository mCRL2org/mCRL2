// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// LICENSE_1_0.txt)
//
/// \file ./export_latex.h

#ifndef ltx_h
#define ltx_h

#include <wx/string.h>
#include <vector>
#include <string>

using namespace std;

typedef struct {
	unsigned int num;
	double x, y;
	string lbl;
} nodeLatex;

typedef struct {
	unsigned int numNode1, numNode2;
	string lbl;
} edgeLatex ;



class ExportToLatex
{
public:
	ExportToLatex(wxString _filename, vector<nodeLatex> _node, vector<edgeLatex> _edge, int _height);
	bool Generate();
	

private:

	string EscSpecChar(string);//To escape special characters
	string str_replace(string to_replace, string replace_by, string replace_in);

	wxString filename;
	string LatexCode;

	vector<nodeLatex> node;
	vector<edgeLatex> edge;

	int height; //height of the drawing area


};



#endif //latex_h
