#include <ftl.h>

using namespace FOM;
using namespace FTL;

FEOS_EXPORT status_t FTL::InternalQI(void* pThis, const QIMapEntry* pMap, REFIID riid, void** ppObj)
{
	if (ppObj == nullptr) return E_POINTER;
	*ppObj = nullptr;

	// Shortcut for IObject - *always* use the first interface in the map
	if (InlineIsEqualGUID(riid, guidof<IObject>()))
	{
		IObject* pObj = (IObject*)((char*)pThis + (int)pMap->param);
		pObj->AddRef();
		*ppObj = pObj;
		return S_OK;
	}

	for (; pMap->func != nullptr; pMap ++)
	{
		bool bIsBlind = pMap->piid == nullptr;
		if (pMap->piid == FTL_QIMAP_DUMMY_IID) continue;
		if (bIsBlind || InlineIsEqualGUID(riid, *pMap->piid))
		{
			if (pMap->func == FTL_QIMAPENTRY_SIMPLE)
			{
				IObject* pObj = (IObject*)((char*)pThis + (int)pMap->param);
				pObj->AddRef();
				*ppObj = pObj;
				return S_OK;
			}

			status_t rc = pMap->func(pThis, riid, ppObj, pMap->param);
			if (rc == S_OK || (!bIsBlind && FAILED(rc)))
				return rc;
		}
	}

	return E_NOINTERFACE;
}
