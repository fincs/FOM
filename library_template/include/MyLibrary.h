#pragma once
#pragma interface
#include <fom.h>

// It is recommended to have a namespace.
namespace MyLibrary
{
	// PLEASE GENERATE A NEW GUID

	// {01234567-89AB-CDEF-0123-456789ABCDEF}
	struct ITest : public IObject
	{
		FOM_BIND_GUID(0x01234567, 0x89AB, 0xCDEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF);

		virtual status_t Method() = 0;
	};

	FOM_DECLARE_CLASS(TestClass, "MyLibrary.TestClass");
}
