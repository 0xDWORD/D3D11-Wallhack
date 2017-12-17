#include "Utils.h"

Utils::Utils(HINSTANCE hModule)
{
	SetDllPath(hModule);
}

Utils::~Utils() 
{
}

void Utils::Log(const char *fmt, ...)
{
	LogF("log.txt", fmt);
}

void Utils::LogF(char *filename, const char *fmt, ...)
{
	if (!fmt)	return;

	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	ofstream logfile(GetDirectoryFile(filename), ios::app);
	if (logfile.is_open() && text)	logfile << text << endl;
	logfile.close();
}



char* Utils::GetDirectoryFile(char *filename)
{
	char _dlldir[MAX_PATH];

	strcpy_s(_dlldir, Utils::dlldir);
	strcpy_s(_dlldir, filename);

	return _dlldir;
}

void Utils::SetDllPath(HINSTANCE hModule)
{
	GetModuleFileName(hModule, Utils::dlldir, _countof(Utils::dlldir));
}

