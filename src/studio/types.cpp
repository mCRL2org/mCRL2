#include "types.h"

#include "unknown.xpm"
#include "spec.xpm"

wxIcon icon_unknown;

wxIcon &sType::GetIcon(int index)
{
	if ( !icon_unknown.Ok() )
	{
		icon_unknown.LoadFile("unknown",wxBITMAP_TYPE_XPM);
	}

	return icon_unknown;
}

sSpecType::sSpecType(bool is_lpe)
{
}

bool sSpecType::IsText()
{
	return true;
}

bool sSpecType::IsLPE()
{
	return false;
}

int sStateSpaceType::GetFormat()
{
	return false;
}
