#include "KittyImpl.h"

class KittyLib : public LibraryBase
{
	FTL_BEGIN_CLASS_MAP(KittyLib)
		FTL_CLASS_MAP_ENTRY(CatImpl)
		FTL_CLASS_MAP_ENTRY(MysteryCatImpl)
	FTL_END_CLASS_MAP()
};

FTL_IMPLEMENT_LIB_INTERFACE(KittyLib)
