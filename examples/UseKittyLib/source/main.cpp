#include <stdio.h>
#include <KittyLib.h>

using namespace FOM;
using namespace Meow;

int main()
{
	InitToken tok;
	if (!tok.Succeeded())
	{
		printf("FOM init FAIL\n");
		return 1;
	}

	ICat* pCat = nullptr;
	if (SUCCEEDED(Cat::New(&pCat)))
	{
		pCat->Meow();
		pCat->Release();
	}else
	{
		printf("Failed to create object!\n");
		return 1;
	}

	// Call a static method
	Cat::StaticMethods()->GeneralMeow();

	return 0;
}
