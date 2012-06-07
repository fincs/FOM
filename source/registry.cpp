#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fom.h>
#include <ustl.h>
#include "registry.h"

using namespace ustl;
using namespace FOM;

static char strBuf[256+1];

class StrCat
{
	char* aBuf;
	char* orgBuf;
	int size;
public:
	StrCat(char* _buf, int _size) : aBuf(_buf), orgBuf(_buf), size(_size) { }
	bool AddPortion(const char* x, int len)
	{
		if (size < len) return false;
		memcpy(aBuf, x, len);
		aBuf += len;
		size -= len;
		return true;
	}
	bool AddString(const char* x)
	{
		return AddPortion(x, strlen(x));
	}
	bool AddChar(int c)
	{
		if (!size) return false;
		*aBuf++ = c;
		size --;
		return true;
	}
	const char* Terminate()
	{
		*aBuf = 0;
		return orgBuf;
	}
};

static status_t Reg_LoadRegFile(const char* regfile);

status_t Reg_LoadModuleForClass(const char* name)
{
	StrCat c(strBuf, sizeof(strBuf)-1);
	if (!c.AddString("/data/FeOS/FOMreg/"))
		return E_OUTOFMEMORY;
	const char* aPos;
	while ((aPos = strrchr(name, '.')))
	{
		if (!c.AddPortion(name, aPos-name))
			return E_OUTOFMEMORY;
		if (!c.AddChar('/'))
			return E_OUTOFMEMORY;
		name = aPos + 1;
	}
	if (!c.AddString(name))
		return E_OUTOFMEMORY;
	if (!c.AddString(".reg"))
		return E_OUTOFMEMORY;
	return Reg_LoadRegFile(c.Terminate());
}

status_t Reg_LoadRegFile(const char* regfile)
{
	//printf("%s\n", regfile);
	FILE* f = fopen(regfile, "r");
	if (!f)
		return E_FAIL;

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	if (size > (sizeof(strBuf)-1))
	{
		fclose(f);
		return E_OUTOFMEMORY;
	}

	rewind(f);
	fread(strBuf, 1, size, f);
	fclose(f);

	strBuf[size] = 0;

	return FOM_LinkModule(strBuf);
}
