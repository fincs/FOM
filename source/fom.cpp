#include <fom.h>
#include <ustl.h>
#include "registry.h"

using namespace ustl;
using namespace FOM;

static word_t g_refCount = 0;

vector<instance_t> instList;
map<string, IObject*> clsMap;

#define FOMAPI extern "C" FEOS_EXPORT

FOMAPI status_t FOM_Initialize()
{
	if (!g_refCount++)
	{
		// First time
	}
	return S_OK;
}

FOMAPI status_t FOM_Uninitialize()
{
	if (!--g_refCount)
	{
		// Deinit
	}
	return FOM_ModuleGC();
}

status_t FOM_ClassGetFactory2(const char* className, REFIID riid, void** ppObject, bool bAllowRegSearch)
{
	if (!className || !ppObject)
		return E_POINTER;

	auto x = clsMap.find(className);
	if (x == clsMap.end())
	{
		if (!bAllowRegSearch) return E_FAIL;

		status_t st = Reg_LoadModuleForClass(className);
		return FAILED(st) ? st : FOM_ClassGetFactory2(className, riid, ppObject, false);
	}

	return x->second->QueryInterface(riid, ppObject);
}

FOMAPI status_t FOM_ClassGetFactory(const char* className, REFIID riid, void** ppObject)
{
	return FOM_ClassGetFactory2(className, riid, ppObject, true);
}

FOMAPI status_t FOM_ClassNew(const char* className, REFIID riid, void** ppObject)
{
	IObjFactory* pFactory = nullptr;
	status_t rc = FOM_ClassGetFactory(className, IID_PPV_ARGS(&pFactory));
	if (FAILED(rc))
		return rc;

	rc = pFactory->CreateInstance(riid, ppObject);
	pFactory->Release();

	return rc;
}

FOMAPI status_t FOM_ClassGetStatics(const char* className, const void** ppFuncTable)
{
	if (!ppFuncTable)
		return E_POINTER;

	IObjFactory* pFactory;
	status_t rc = FOM_ClassGetFactory(className, IID_PPV_ARGS(&pFactory));
	if (FAILED(rc))
		return rc;

	*ppFuncTable = pFactory->GetStaticMethods();
	pFactory->Release();

	return S_OK;
}

FOMAPI status_t FOM_ModuleGC()
{
	for(auto it = instList.begin(); it != instList.end();)
	{
		instance_t hInst = *it;
		auto CanUnload = (CanUnloadProc) FeOS_FindSymbol(hInst, "FOMLIB_CanUnload");
		if (!CanUnload || !CanUnload())
			it ++;
		else
		{
			auto UnregisterModule = (UnregisterModuleProc) FeOS_FindSymbol(hInst, "FOMLIB_UnregisterModule");
			if (UnregisterModule) UnregisterModule();
			FeOS_FreeModule(hInst);
			it = instList.erase(it);
		}
	}
	return S_OK;
}

FOMAPI status_t FOM_RegisterClass(const char* className, IObject* pFactory, REGCOOKIE* pCookie)
{
	if (!className || !pFactory || !pCookie)
		return E_POINTER;

	pFactory->AddRef();
	*pCookie = clsMap.insert( pair<string,IObject*>(string(className), pFactory) ).first;
	return S_OK;
}

FOMAPI status_t FOM_UnregisterClass(REGCOOKIE cookie)
{
	if (!cookie)
		return E_POINTER;

	auto it = (decltype(clsMap)::iterator) cookie;
	it->second->Release();
	clsMap.erase(it);
	return S_OK;
}

FOMAPI status_t FOM_LinkModule(const char* moduleName)
{
	instance_t hInst = FeOS_LoadModule(moduleName);
	if (!hInst)
		return E_FAIL;

	auto RegisterModule = (RegisterModuleProc) FeOS_FindSymbol(hInst, "FOMLIB_RegisterModule");
	if (!RegisterModule)
	{
		FeOS_FreeModule(hInst);
		return E_FAIL;
	}

	status_t rc = RegisterModule();
	if (FAILED(rc))
	{
		FeOS_FreeModule(hInst);
		return E_FAIL;
	}

	instList.push_back(hInst);
	return S_OK;
}
