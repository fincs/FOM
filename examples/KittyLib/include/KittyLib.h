#pragma once
#pragma interface
#include <fom.h>

namespace Meow
{
	// {65D3561A-7DD0-4740-BD4E-EA82DF1B8E9D}
	struct ICat : public IObject
	{
		FOM_BIND_GUID(0x65D3561A, 0x7DD0, 0x4740, 0xBD, 0x4E, 0xEA, 0x82, 0xDF, 0x1B, 0x8E, 0x9D);

		virtual status_t Meow() = 0;
		virtual status_t Purr() = 0;
	};

	// {D7ADEB3B-E6D6-42fd-B6B7-23556A5C9309}
	struct IEntertain : public IObject
	{
		FOM_BIND_GUID(0xD7ADEB3B, 0xE6D6, 0x42FD, 0xB6, 0xB7, 0x23, 0x55, 0x6A, 0x5C, 0x93, 0x9);

		virtual status_t Entertain() = 0;
	};

	// {609FF987-2608-40f5-914A-B3760331D240}
	struct IKitten : public ICat
	{
		FOM_BIND_GUID(0x609ff987, 0x2608, 0x40f5, 0x91, 0x4a, 0xb3, 0x76, 0x3, 0x31, 0xd2, 0x40);
		virtual status_t DoCuteThings() = 0;
	};

	// {A1737E5D-C7CD-4791-88CA-82BB0BEBBBAB}
	struct IFatCat : public ICat
	{
		FOM_BIND_GUID(0xa1737e5d, 0xc7cd, 0x4791, 0x88, 0xca, 0x82, 0xbb, 0xb, 0xeb, 0xbb, 0xab);
		virtual status_t EatALot() = 0;
	};

	// This structure holds pointers to the static methods of the Cat class.
	struct CatStatics
	{
		status_t (* GeneralMeow)();
	};

	FOM_DECLARE_CLASS_WITH_STATICS(Cat, "Meow.Cat", CatStatics);
	FOM_DECLARE_CLASS(MysteryCat, "Meow.MysteryCat");
}
