#pragma once
#pragma interface

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
