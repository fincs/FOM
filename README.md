FOM - FeOS Object Model
=======================

Introduction
------------

FOM is a model for inter-operable FeOS OOP programming that allows for an easy way to produce a consistent interface to
classes and functionality. It is loosely modelled on Microsoft's Component Object Model (COM) technology, but in a
drastically simplified and eased way.

Currently it's only possible to create and consume FOM components in C++, but in the future you will be able to access
said components from scripting languages, such as [Lua](https://github.com/fincs/FeOSLua).

How to install
--------------

First, make sure you have the following prerequisites:

- [FeOS](http://feos.mtheall.com/) and its build prerequisites, including the host C compiler.
- [uSTL for FeOS](http://feos.mtheall.com/uSTL-FeOS/).

Afterwards, clone this repository to <FeOSSDK>/userlib/FOM. Then issue this command:

	make install

You should then be ready to create and consume FOM libraries.

Linking to the FOM runtime
--------------------------

FOM follows the standard userlib system. Your Makefile could look like this:

	CONF_USERLIBS := FOM
	CONF_LIBS := -lFOM

What's contained in this repo
-----------------------------

This repo contains:

- FeOS-side FOM runtime library, which handles the FOM API.
- FOM header file
- FOM Template Library (FTL), a must-have header library that provides ready-to-use
  implementations required for easy and quick development of FOM components.
- `fomutil`, FOM component registration utility that works both on FeOS and on the host (in order to
  register components while developing).

FOM primer
----------

FOM objects are based on C++ pure virtual classes, which have a widely known layout. All interfaces are tied to
a unique 16-byte identifier ("GUID") normally called IID (Interface ID). The base interface all FOM interfaces
must (at some point) inherit from is `IObject`, shown below:

	struct IObject // IID: {57ADE01C-8181-448B-AE6C-4153E0D41ED0}
	{
		virtual status_t QueryInterface(REFIID riid, void** ppObject) = 0;
		virtual word_t AddRef() = 0;
		virtual word_t Release() = 0;
		virtual const char* GetClassName() = 0;
	};

The first three methods work exactly the same as COM's `IUnknown` interface ([MSDN link](http://msdn.microsoft.com/en-us/library/windows/desktop/ms680509.aspx)).
The fourth method returns a pointer to a string containing the class name the object is an instance of or NULL if it doesn't belong to a public class.
Make sure you read the [rules of QueryInterface and object identity](http://msdn.microsoft.com/en-us/library/windows/desktop/ms682521.aspx).
Normally you don't have to implement this interface yourself, and instead use the standard implementation shipped with FTL, like so:

	class SomethingImpl : public ISomething, public FTL::ObjectImpl // ISomething is the interface you want to implement
	{
	public:
		// Implement the base IObject methods (the class name is passed)
		FTL_IMPLEMENT_IOBJECT("Namespace.Class");

		// Table of interfaces to expose (required by the default QueryInterface implementation)
		FTL_BEGIN_IFACE_MAP(SomethingImpl)
			FTL_IFACE_MAP_ENTRY(ISomething)
		FTL_END_IFACE_MAP()

		// ISomething method implementations here...
	};

When using standardly-formatted FOM library header files, creating instances of FOM classes is a piece of cake:

	ISomething* pObj;
	if (SUCCEEDED(Namespace::Class::New(&pObj)))
	{
		// Do something with pObj...

		// Delete object
		pObj->Release();
	} else { /* Error... */ }

FOM libraries are shared libraries that implement FOM classes. The FOM runtime library accesses these objects thanks
to a common interface implemented by FOM libraries. FTL provides a standard implementation which automatically takes
care of both creating instances of classes and managing the lifetime of the library. It is used like this:

	class MyLibrary : public FTL::LibraryBase
	{
		// Add here the classes you want to expose
		FTL_BEGIN_CLASS_MAP(MyLibrary)
			FTL_CLASS_MAP_ENTRY(SomethingImpl)
			// ... more classes here
		FTL_END_CLASS_MAP()
	};

	// Implement the FOM library interface
	FTL_IMPLEMENT_LIB_INTERFACE(MyLibrary)

For more details, read the examples and (in the future) the documentation.
