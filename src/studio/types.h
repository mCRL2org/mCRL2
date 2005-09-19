#ifndef __TYPES_H
#define __TYPES_H

#include <wx/icon.h>

class sType
{
	public:
		wxIcon &GetIcon(int index);
};

class sSpecType: public sType
{
	public:
		sSpecType(bool is_lpe = false);

		virtual bool IsText();
		virtual bool IsLPE();
};

class sStateSpaceType: public sType
{
	public:
		virtual int GetFormat();
};

class sFormulaType: public sType
{
};

#endif
