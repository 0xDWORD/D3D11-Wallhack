#pragma once
#include <fstream>
#include <Windows.h>
using namespace std;

class Utils
{
private:
	void SetDllPath(HINSTANCE hModule);
	char *GetDirectoryFile(char *filename);
	char dlldir[MAX_PATH];

public:
	Utils::Utils(HINSTANCE hModule);
	~Utils();

	void Log(const char *fmt, ...);
	void LogF(char *filename, const char *fmt, ...);
};

