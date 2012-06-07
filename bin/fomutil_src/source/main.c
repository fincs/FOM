#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef FEOS
#include <sys/stat.h>
#else
// This is done in order to avoid trouble (MinGW's mkdir is b0rked)
int mkdir(const char*, int);
#endif

char lineBuf[1024];
char otherBuf[128];
char yetAnotherBuf[256];

static inline char* rstrip(char* s)
{
	char* p = s + strlen(s);
	for (; p > s && isspace(*--p); *p = 0);
	return s;
}

static inline char* lskip(const char* s)
{
	for (; *s && isspace(*s); s ++);
	return (char*) s;
}

int usage(const char* name)
{
	printf("Usage:\n  %s reg [fomregFile]\n  %s unreg [fomregFile]\n", name, name);
	return 0;
}

void getFOMregDir(char* buf, int x)
{
#ifdef FEOS
	*buf = 0;
#else
	strncpy(buf, getenv("FEOSDEST"), x-1);
#ifdef WIN32
	if (*buf == '/')
	{
		buf[0] = buf[1];
		buf[1] = ':';
	}
#endif
#endif
	strncat(buf, "/data/FeOS/FOMreg/", x-1);
}

int regClass(const char* clsName, const char* moduleName)
{
	getFOMregDir(yetAnotherBuf, sizeof(yetAnotherBuf)-1);
	char* aPos = yetAnotherBuf + strlen(yetAnotherBuf);
	strncat(yetAnotherBuf, clsName, sizeof(yetAnotherBuf)-1);
	while((aPos = strrchr(aPos, '.')))
	{
		*aPos = 0;
		if (mkdir(yetAnotherBuf, 0777) == -1 && errno != EEXIST)
		{
			fprintf(stderr, "Can't make dir %s\n", yetAnotherBuf);
			return 1;
		}
		*aPos = '/';
	}
	strncat(yetAnotherBuf, ".reg", sizeof(yetAnotherBuf)-1);
	yetAnotherBuf[sizeof(yetAnotherBuf)-1] = 0;
	FILE* f = fopen(yetAnotherBuf, "w");
	if (!f) return 1;
	fputs(moduleName, f);
	fclose(f);
	return 0;
}

int unregClass(const char* clsName)
{
	getFOMregDir(yetAnotherBuf, sizeof(yetAnotherBuf)-1);
	char* aPos = yetAnotherBuf + strlen(yetAnotherBuf);
	strncat(yetAnotherBuf, clsName, sizeof(yetAnotherBuf)-1);
	for (; (aPos = strrchr(aPos, '.')); *aPos = '/');
	strncat(yetAnotherBuf, ".reg", sizeof(yetAnotherBuf)-1);
	yetAnotherBuf[sizeof(yetAnotherBuf)-1] = 0;

	if (remove(yetAnotherBuf) != 0)
	{
		fprintf(stderr, "Can't del file %s\n", yetAnotherBuf);
		return 1;
	}
	return 0;
}

enum { ST_WANTLIBNAME, ST_READCLASS };

int cmdReg(const char* aFile)
{
	getFOMregDir(yetAnotherBuf, sizeof(yetAnotherBuf)-1);
	mkdir(yetAnotherBuf, 0777); // just in case

	FILE* f = fopen(aFile, "r");
	if (!f)
	{
		fprintf(stderr, "Can't open %s: %s\n", aFile, strerror(errno));
		return 1;
	}

	enum { ST_WANTLIBNAME, ST_READCLASS };
	int status = ST_WANTLIBNAME;
	int lineno = 0;

	while (fgets(lineBuf, sizeof(lineBuf), f) != NULL)
	{
		lineno ++;
		const char* line = lskip(rstrip(lineBuf));

		if (!*line) continue;
		switch (status)
		{
			case ST_WANTLIBNAME:
			{
				if (strnicmp(line, "LIBRARY", 7) != 0)
				{
					fprintf(stderr, "Syntax error at line %d: need LIBRARY command\n", lineno);
					fclose(f);
					return 1;
				}
				line = lskip(line + 7);
				if (strlen(line) > (sizeof(otherBuf)-1))
				{
					fprintf(stderr, "Syntax error at line %d: name too long: %s\n", lineno, line);
					fclose(f);
					return 1;
				}
				strncpy(otherBuf, line, sizeof(otherBuf));
				status = ST_READCLASS;
				break;
			}
			case ST_READCLASS:
			{
				int r = regClass(line, otherBuf);
				if (r != 0)
				{
					fprintf(stderr, "Error at line %d: can't register class %s\n", lineno, line);
					fclose(f);
					return r;
				}
				break;
			}
		}
	}

	fclose(f);
	return 0;
}

int cmdUnreg(const char* aFile)
{
	FILE* f = fopen(aFile, "r");
	if (!f)
	{
		fprintf(stderr, "Can't open %s: %s\n", aFile, strerror(errno));
		return 1;
	}

	int status = ST_WANTLIBNAME;
	int lineno = 0;

	while (fgets(lineBuf, sizeof(lineBuf), f) != NULL)
	{
		lineno ++;
		const char* line = lskip(rstrip(lineBuf));

		if (!*line) continue;
		switch (status)
		{
			case ST_WANTLIBNAME:
			{
				if (strnicmp(line, "LIBRARY", 7) != 0)
				{
					fprintf(stderr, "Syntax error at line %d: need LIBRARY command\n", lineno);
					fclose(f);
					return 1;
				}
				status = ST_READCLASS;
				break;
			}
			case ST_READCLASS:
			{
				int r = unregClass(line);
				if (r != 0)
				{
					fprintf(stderr, "Error at line %d: can't unregister class %s\n", lineno, line);
					fclose(f);
					return r;
				}
				break;
			}
		}
	}

	fclose(f);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 3) return usage(argv[0]);
	if (stricmp(argv[1], "reg") == 0) return cmdReg(argv[2]);
	if (stricmp(argv[1], "unreg") == 0) return cmdUnreg(argv[2]);
	return usage(argv[0]);
}
