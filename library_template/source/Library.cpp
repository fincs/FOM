// Add #includes for all your classes...
#include "TestClassImpl.h"

class TemplateLib : public LibraryBase
{
	FTL_BEGIN_CLASS_MAP(TemplateLib)
		FTL_CLASS_MAP_ENTRY(TestClassImpl)
	FTL_END_CLASS_MAP()
};

FTL_IMPLEMENT_LIB_INTERFACE(TemplateLib)
