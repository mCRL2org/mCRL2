#ifndef ltx_h
#define ltx_h

#include <string>
#include <vector>

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
	ExportToLatex(const char * _filename, vector<nodeLatex> _node, vector<edgeLatex> _edge);
	bool Generate();
	

private:

	string EscSpecChar(string);//To escape special characters
	string str_replace(string to_replace, string replace_by, string replace_in);

	const char * filename;
	string LatexCode;

	vector<nodeLatex> node;
	vector<edgeLatex> edge;

	int height;
	int width;

};



#endif //latex_h
