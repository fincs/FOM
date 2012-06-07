#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <feos.h>
#include <KittyLib.h>
#include <ftl.h>

using namespace FOM;
using namespace FTL;
using namespace Meow;

class CatImpl: public ICat, public IEntertain, public ObjectImpl
{
protected:
	~CatImpl() { printf("~CatImpl()\n"); } // test destructor

public:
	FTL_IMPLEMENT_IOBJECT(clsnameof<Cat>());
	FTL_BEGIN_IFACE_MAP(CatImpl)
		FTL_IFACE_MAP_ENTRY(ICat)
		FTL_IFACE_MAP_ENTRY(IEntertain)
	FTL_END_IFACE_MAP()

	// This construct exposes the class' static methods.
	FTL_STATIC_TABLE_BEGIN(CatStatics)
		FTL_STATIC_TABLE_ENTRY(GeneralMeow)
	FTL_STATIC_TABLE_END()

	status_t Meow() { printf("meowwww!\n"); return S_OK; }
	status_t Purr() { printf("purrrrrr\n"); return S_OK; }
	status_t Entertain() { printf("*plays with a wool ball*\n"); return S_OK; }

	CatImpl() { }

	static status_t GeneralMeow() { printf("GeneralMeowww\n"); return S_OK; }
};

class MysteryCatImpl: public IKitten, public IFatCat, public ObjectImpl
{
public:
	FTL_IMPLEMENT_IOBJECT(clsnameof<MysteryCat>());
	FTL_BEGIN_IFACE_MAP(MysteryCatImpl)
		FTL_IFACE_MAP_ENTRY(IKitten)
		FTL_IFACE_MAP_ENTRY(IFatCat)
		// FTL_IFACE_MAP_ENTRY_BRANCH() tells FTL to use IKitten's inheritance branch when
		// exposing the ICat interface, otherwise it's ambiguous
		FTL_IFACE_MAP_ENTRY_BRANCH(ICat, IKitten)
	FTL_END_IFACE_MAP()

	status_t Meow() { printf("MEOWWWWWWW\n"); return S_OK; }
	status_t Purr() { printf("PURRRRRRRRR\n"); return S_OK; }
	status_t DoCuteThings() { printf("can I b0rk your DS? :3\n"); return S_OK; }
	status_t EatALot() { printf("nomnom\n"); return S_OK; }
};
