#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <feos.h>
#include <ftl.h>

#include <MyLibrary.h>

// Avoid unnecessary retyping of namespace names
using namespace FOM;
using namespace FTL;
using namespace MyLibrary;

class TestClassImpl : public ITest, public ObjectImpl
{
public:
	FTL_IMPLEMENT_IOBJECT(clsnameof<TestClass>()); // clsnameof() retrieves the name of a FOM class
	FTL_BEGIN_IFACE_MAP(TestClassImpl)
		FTL_IFACE_MAP_ENTRY(ITest)
	FTL_END_IFACE_MAP()

	// Method prototypes
	status_t Method();
};
