#include <ftl.h>

using namespace FOM;
using namespace FTL;

FEOS_EXPORT status_t FTL::InternalRegister(ClassEntry* pMap)
{
	for (; pMap->name; pMap ++)
	{
		auto pMap2 = (ClassEntry2*) pMap;
		REGCOOKIE cookie;
		status_t x = FOM_RegisterClass(pMap->name, pMap->pFactory, &cookie);
		if (FAILED(x))
		{
			pMap2->status = __ST_FAIL;
			continue;
		}
		pMap2->cookie = cookie;
		pMap2->status = __ST_OK;
	}
	((ClassEntry2*)pMap)->status = __ST_END;
	return S_OK;
}

FEOS_EXPORT status_t FTL::InternalUnregister(ClassEntry* pMap1)
{
	auto pMap = (ClassEntry2*) pMap1;
	for (; pMap->status != __ST_END; pMap ++)
		if (pMap->status == __ST_OK)
			FOM_UnregisterClass(pMap->cookie);
	return S_OK;
}
