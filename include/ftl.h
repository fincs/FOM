//
// FeOS Standard Library
// ftl.h
//     FOM Template Library
//

#pragma once
#include <fom.h>
#include <assert.h>
#ifdef FTL_USE_EXCEPTIONS
#include <exception>
#include <new>
#else
#include <stdlib.h>
#endif

namespace FTL
{
#ifdef FTL_USE_EXCEPTIONS
	class fom_exception : public std::exception
	{
		status_t res;
	public:
		inline fom_exception(status_t _res) : res(_res) { }
		inline status_t errcode() { return res; }
		virtual const char* what() const throw()
		{
			return "FTL::fom_exception";
		}
	};

	static inline void throw_fom_excpt(status_t st)
	{
		throw fom_exception(st);
	}
#endif

	class LibraryBase
	{
		word_t lockCount;
	public:
		inline LibraryBase() : lockCount(0) { }
		inline bool CanUnload() { return lockCount == 0; }

		inline int Lock() { return ++lockCount; }
		inline int Unlock() { return --lockCount; }
	};

#ifdef FTL_BUILD_LIBRARY
	extern LibraryBase* thisLibrary;
	#define FTL_LIBRARY_LOCK()   thisLibrary->Lock()
	#define FTL_LIBRARY_UNLOCK() thisLibrary->Unlock()
#else
	#define FTL_LIBRARY_LOCK()   1
	#define FTL_LIBRARY_UNLOCK() 1
#endif

	typedef status_t (* QIHelperFunc)(void* pThis, REFIID riid, void** ppObj, word_t param);
	#define FTL_QIMAPENTRY_SIMPLE ((QIHelperFunc) 1)
	#define FTL_QIMAP_DUMMY_IID ((const IID*) 1)

	struct QIMapEntry
	{
		QIHelperFunc func;
		const IID* piid;
		word_t param;
	};

	status_t InternalQI(void* pThis, const QIMapEntry* pMap, REFIID riid, void** ppObj);

	template <class Derived, class Base, int Packing = 8>
	static inline int offsetofclass()
	{
		return (int)(static_cast<Base*>((Derived*)Packing)) - Packing;
	}

	template <class Derived, class Base, class Branch, int Packing = 8>
	static inline int offsetofclassbranch()
	{
		return (int)(static_cast<Base*>(static_cast<Branch*>((Derived*)Packing))) - Packing;
	}

	struct QIChainData
	{
		int offset;
		const QIMapEntry* pMap;
	};

	template <class Derived, class Base>
	struct QIHoldChainData
	{
		static QIChainData data;
	};

	template <class Derived, class Base>
	QIChainData QIHoldChainData<Derived,Base>::data = { offsetofclass<Derived, Base>(), Base::_GetEntries() };

	#define FTL_BEGIN_IFACE_MAP(Class) public: \
		typedef Class __thistype; \
		inline IObject* GetRawObject() \
		{ \
			return (IObject*)((char*)this + (int)_GetEntries()->param); \
		} \
		static inline const QIMapEntry* _GetEntries() \
		{ \
			static const QIMapEntry x[] = \
			{

	#define FTL_IFACE_MAP_ENTRY(Interface) \
				{ FTL_QIMAPENTRY_SIMPLE, &::FOM::guidof<Interface>(), (word_t)::FTL::offsetofclass<__thistype, Interface>() },

	#define FTL_IFACE_MAP_ENTRY_DUMMY() \
				{ FTL_QIMAPENTRY_SIMPLE, FTL_QIMAP_DUMMY_IID, 0 },

	#define FTL_IFACE_MAP_ENTRY_BRANCH(Interface, Branch) \
				{ FTL_QIMAPENTRY_SIMPLE, &::FOM::guidof<Interface>(), (word_t)::FTL::offsetofclassbranch<__thistype, Interface, Branch>() },

	#define FTL_IFACE_MAP_CHAIN(Class) \
				{ _ChainHlp, NULL, (word_t)&::FTL::QIHoldChainData<__thistype, Class>::data },

	#define FTL_END_IFACE_MAP() \
				{ nullptr, nullptr, 0 } \
			}; \
			return x; \
		}

	class RefCountImpl
	{
		word_t refCount;
	public:
		inline RefCountImpl() : refCount(1) { }
		inline word_t InternalAddRef()  { return ++refCount; }
		inline word_t InternalRelease() { return --refCount; }
	};

	class NoRefCountImpl
	{
	public:
		inline word_t InternalAddRef()  { return 1; }
		inline word_t InternalRelease() { return 1; }
	};

	template <class Base>
	class ObjectRoot : public Base
	{
	public:
		static inline status_t InternalQueryInterface(void* pThis, const QIMapEntry* pMap, REFIID riid, void** ppObj)
		{
			assert(pMap[0].func == FTL_QIMAPENTRY_SIMPLE);
			return InternalQI(pThis, pMap, riid, ppObj);
		}

		static inline status_t _ChainHlp(void* pThis, REFIID riid, void** ppObj, word_t param)
		{
			auto pData = (QIChainData*)param;
			return InternalQI((char*)pThis + pData->offset, pData->pMap, riid, ppObj);
		}

		inline status_t PostConstruct() { return S_OK; }
		inline void PreDestruct() { }

		static inline const void* _GetStaticMethods() { return NULL; }

#ifndef FTL_USE_EXCEPTIONS
		inline void* operator new (size_t sz) { return malloc(sz); }
		inline void operator delete (void* data) { free(data); }
#endif
	};

	typedef ObjectRoot<RefCountImpl> ObjectImpl;
	typedef ObjectRoot<NoRefCountImpl> StaticObjectImpl;

	#define FTL_IMPLEMENT_IOBJECT(className) public: \
		virtual status_t QueryInterface(REFIID riid, void** ppObject) \
		{ \
			return InternalQueryInterface(this, _GetEntries(), riid, ppObject); \
		} \
		virtual word_t AddRef() { return InternalAddRef(); } \
		virtual word_t Release() \
		{ \
			word_t r = InternalRelease(); \
			if (r == 0) \
			{ \
				PreDestruct(); \
				FTL_LIBRARY_UNLOCK(); \
				delete this; \
			} \
			return r; \
		} \
		static const char* _GetClassName() { return className; } \
		virtual const char* GetClassName() { return _GetClassName(); }

	#define FTL_STATIC_TABLE_BEGIN(typeName) \
		static inline const void* _GetStaticMethods() \
		{ \
			static const typeName x = {

	#define FTL_STATIC_TABLE_ENTRY(methodName) methodName,

	#define FTL_STATIC_TABLE_END() \
			}; \
			return &x; \
		}

	template <typename T>
	class ObjFactory: public IObjFactory, public StaticObjectImpl
	{
	public:
		FTL_IMPLEMENT_IOBJECT(NULL);
		FTL_BEGIN_IFACE_MAP(ObjFactory)
			FTL_IFACE_MAP_ENTRY(IObjFactory)
		FTL_END_IFACE_MAP()

		status_t CreateInstance(REFIID riid, void** ppObj)
		{
			T* pObj = nullptr;
			*ppObj = nullptr;
			
#ifdef FTL_USE_EXCEPTIONS
			try { pObj = new T; }
			catch (fom_exception& e) { return e.errcode(); }
			catch (std::bad_alloc& e) { return E_OUTOFMEMORY; }
			catch (...) { return E_FAIL; }
#else
			pObj = new T;
			if (!pObj) return E_OUTOFMEMORY;
#endif

			status_t st = pObj->PostConstruct();
			if (SUCCEEDED(st))
				st = pObj->QueryInterface(riid, ppObj);

			pObj->Release();

			if (SUCCEEDED(st))
				FTL_LIBRARY_LOCK();

			return st;
		}
		
		const void* GetStaticMethods() { return T::_GetStaticMethods(); }
		const char* GetName() { return T::_GetClassName(); }
	};

	template <class T>
	struct ObjFactoryFactory
	{
		static inline IObjFactory* GetFactory()
		{
			static ObjFactory<T> oFactory;
			return &oFactory;
		}
	};

	template <class T>
	static inline IObjFactory* GetClassFactory()
	{
		return ObjFactoryFactory<T>::GetFactory();
	}

	enum { __ST_END = 0, __ST_OK, __ST_FAIL };

	struct ClassEntry
	{
		const char* name;
		IObjFactory* pFactory;
	};

	struct ClassEntry2
	{
		REGCOOKIE cookie;
		int status;
	};

	status_t InternalRegister(ClassEntry* pMap);
	status_t InternalUnregister(ClassEntry* pMap);

	#define FTL_BEGIN_CLASS_MAP(ClassQ) public: \
		static inline ClassEntry* _GetClassMap() \
		{ \
			static ClassEntry x[] = \
			{

	#define FTL_CLASS_MAP_ENTRY(ClassName) \
				{ ClassName::_GetClassName(), GetClassFactory<ClassName>() },

	#define FTL_END_CLASS_MAP() \
				{ nullptr, nullptr } \
			}; \
			return x; \
		} \
		static inline status_t RegisterModule() \
		{ \
			status_t rc = InternalRegister(_GetClassMap()); \
			return rc; \
		} \
		static inline status_t UnregisterModule() \
		{ \
			status_t rc = InternalUnregister(_GetClassMap()); \
			return rc; \
		}

	#define FTL_IMPLEMENT_LIB_INTERFACE(Class) \
		static Class libObject; \
		LibraryBase* FTL::thisLibrary = &libObject; \
		extern "C" FEOS_EXPORT status_t FOMLIB_RegisterModule() \
		{ \
			return libObject.RegisterModule(); \
		} \
		extern "C" FEOS_EXPORT status_t FOMLIB_UnregisterModule() \
		{ \
			return libObject.UnregisterModule(); \
		} \
		extern "C" FEOS_EXPORT bool FOMLIB_CanUnload() \
		{ \
			return libObject.CanUnload(); \
		}
}
