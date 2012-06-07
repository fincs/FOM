//
// FeOS Standard Library
// feosobj.h
//     FeOS Object Model
//

#pragma once
#pragma interface
#include <feos.h>

#ifndef __cplusplus
#error This header file requires C++.
#endif

typedef struct
{
	word_t data1;
	hword_t data2, data3;
	byte_t data4[8];
} GUID;

typedef GUID IID;
typedef GUID CLSID;
typedef void* REGCOOKIE;
typedef int status_t;

typedef const GUID& REFGUID;
typedef const GUID& REFIID;
typedef const GUID& REFCLSID;

#define MAKE_STATUS(severity, facility, code) ( ((severity) ? -1 : 1) * ( (status_t(facility) << 16) | (status_t(code) & 0xFFFF) ) )
#define SEV_SUCCESS 0
#define SEV_FAILURE 1

enum { FAC_GENERAL = 0 };

#define SUCCEEDED(n) ((n)>=0)
#define FAILED(n) ((n)<0)

#define S_OK    MAKE_STATUS(0,0,0)
#define S_FALSE MAKE_STATUS(0,0,1)

#define E_FAIL         MAKE_STATUS(1,0,1)
#define E_NOINTERFACE  MAKE_STATUS(1,0,2)
#define E_POINTER      MAKE_STATUS(1,0,3)
//#define E_OUTOFMEMORY  MAKE_STATUS(1,0,4)   // Already defined in feoserror.h
#define E_HANDLE       MAKE_STATUS(1,0,5)
#define E_UNEXPECTED   MAKE_STATUS(1,0,6)
#define E_ABORT        MAKE_STATUS(1,0,7)
#define E_NOTIMPL      MAKE_STATUS(1,0,8)
#define E_ACCESSDENIED MAKE_STATUS(1,0,9)
//#define E_FILENOTFOUND MAKE_STATUS(1,0,10)  // Already defined in feoserror.h
//#define E_INVALIDARG   MAKE_STATUS(1,0,11)  // Already defined in feoserror.h
//#define E_APPKILLED    MAKE_STATUS(1,0,12)  // Already defined in feoserror.h

#define FOM_BIND_GUID(l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
	static inline REFGUID __guidof() \
	{ \
		static const GUID guid = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}; \
		return guid; \
	}

namespace FOM
{
	template <typename T>
	static inline REFGUID guidof(T* x = nullptr) { return T::__guidof(); }
	template <typename T>
	static inline const char* clsnameof() { return T::_GetName(); }
}

#define IID_PPV_ARGS(x) ::FOM::guidof(*x), (void**) x

#define _STMETHODS(xType) \
	template <class T> \
	static inline status_t New(T** ppObj) { return ::FOM::ClassNew<xType,T>(ppObj); } \
	template <class T> \
	static inline status_t GetFactory(T** ppObj) { return ::FOM::ClassGetFactory<xType,T>(ppObj); }

#define FOM_DECLARE_CLASS(className, classString) \
	class className \
	{ \
		className() { } \
	public: \
		static inline const char* _GetName() { return classString; } \
		_STMETHODS(className) \
	}

#define FOM_DECLARE_CLASS_WITH_STATICS(className, classString, staticType) \
	class className \
	{ \
		className() { } \
	public: \
		typedef const staticType* StaticMethodTableType; \
		static inline const char* _GetName() { return classString; } \
		_STMETHODS(className) \
		static inline StaticMethodTableType StaticMethods() { return ::FOM::ClassGetStatics<className>(); } \
	}

//-----------------------------------------------------------------------------
// IObject {57ADE01C-8181-448B-AE6C-4153E0D41ED0}
//-----------------------------------------------------------------------------

struct IObject
{
	FOM_BIND_GUID(0x57ADE01C, 0x8181, 0x448B, 0xAE, 0x6C, 0x41, 0x53, 0xE0, 0xD4, 0x1E, 0xD0);

	virtual status_t QueryInterface(REFIID riid, void** ppObject) = 0;
	virtual word_t AddRef() = 0;
	virtual word_t Release() = 0;
	virtual const char* GetClassName() = 0;

	template <class T>
	status_t QueryInterface(T** ppObject) { return QueryInterface(::FOM::guidof<T>(), (void**)ppObject); }
};

//-----------------------------------------------------------------------------
// IObjFactory {EC0D846D-8E70-41F8-A8E3-8507C9878641}
//-----------------------------------------------------------------------------

struct IObjFactory : public IObject
{
	FOM_BIND_GUID(0xEC0D846D, 0x8E70, 0x41F8, 0xA8, 0xE3, 0x85, 0x7, 0xC9, 0x87, 0x86, 0x41);

	virtual status_t CreateInstance(REFIID riid, void** ppObj) = 0;
	virtual const void* GetStaticMethods() = 0;
	virtual const char* GetName() = 0;
	//virtual status_t GetCLSID(CLSID* pclsid) = 0;

	template <class T>
	status_t CreateInstance(T** ppObj) { return CreateInstance(::FOM::guidof<T>(), (void**)ppObj); }

	template <class T>
	const T* GetStaticMethods() { return (const T*) GetStaticMethods(); }
};

namespace FOM
{
	static inline bool InlineIsEqualGUID(REFGUID _a, REFGUID _b)
	{
		const word_t* a = (word_t*) &_a;
		const word_t* b = (word_t*) &_b;
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
	}

	static inline bool IsEqualGUID(REFGUID a, REFGUID b)
	{
		return __builtin_memcmp(&a, &b, sizeof(GUID)) == 0;
	}

	extern "C"
	{
		status_t FOM_Initialize();
		status_t FOM_Uninitialize();
		status_t FOM_ClassGetFactory(const char* className, REFIID riid, void** ppObject);
		status_t FOM_ClassNew(const char* className, REFIID riid, void** ppObject);
		status_t FOM_ClassGetStatics(const char* className, const void** ppFuncTable);
		status_t FOM_ModuleGC();

		status_t FOM_RegisterClass(const char* className, IObject* pFactory, REGCOOKIE* pCookie);
		status_t FOM_UnregisterClass(REGCOOKIE cookie);
		status_t FOM_LinkModule(const char* moduleName);
	}

	typedef status_t (*RegisterModuleProc)();
	typedef status_t (*UnregisterModuleProc)();
	typedef bool (*CanUnloadProc)();

	struct InitToken
	{
		status_t rc;
		inline InitToken() : rc(FOM_Initialize()) { }
		inline ~InitToken() { if (SUCCEEDED(rc)) FOM_Uninitialize(); }

		inline bool Succeeded() { return SUCCEEDED(rc); }
		inline status_t ModuleGC() { return FOM_ModuleGC(); }
		inline status_t RegisterClass(const char* className, IObject* pFactory, REGCOOKIE* pCookie) { return FOM_RegisterClass(className, pFactory, pCookie); }
		inline status_t UnregisterClass(REGCOOKIE cookie) { return FOM_UnregisterClass(cookie); }
		inline status_t LinkModule(const char* moduleName) { return FOM_LinkModule(moduleName); }
	};

	template <class Cls, class Iface>
	static inline status_t ClassNew(Iface** ppObj)
	{
		return FOM_ClassNew(clsnameof<Cls>(), guidof<Iface>(), (void**)ppObj);
	}

	template <class Cls>
	static inline typename Cls::StaticMethodTableType ClassGetStatics()
	{
		typename Cls::StaticMethodTableType pTable = nullptr;
		status_t st = FOM_ClassGetStatics(clsnameof<Cls>(), (const void**) &pTable);
		return SUCCEEDED(st) ? pTable : nullptr;
	}

	template <class Cls, class Iface>
	static inline IObjFactory* ClassGetFactory(Iface** ppObj)
	{
		return FOM_ClassGetFactory(clsnameof<Cls>(), guidof<Iface>(), (void*)ppObj);
	}
}
