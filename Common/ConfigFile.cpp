#include "ConfigFile.h"
#include <windows.h>

//-------------------------------------------------------------------------

ConfigFile::ConfigFile()
{
}

//-------------------------------------------------------------------------

ConfigFile::~ConfigFile()
{
}

//-------------------------------------------------------------------------

// Opens a file to work with
bool ConfigFile::Open(const std::string & filename)
{
	char fullDir[MAX_PATH + 1] = {0};
	GetFullPathNameA(filename.c_str(), MAX_PATH, fullDir, 0);
	mFileName = fullDir;

	bool fileExists = (GetFileAttributes(fullDir) != INVALID_FILE_ATTRIBUTES);

	// Force the system to read the mapping into shared memory, so that future invocations of the application will see it without the user having to reboot the system 
	WritePrivateProfileStringA(NULL, NULL, NULL, filename.c_str());

	return fileExists;
}

//-------------------------------------------------------------------------

// Writes to any section
void ConfigFile::Write(const std::string& section, const std::string& key, const char * text, ...)
{
	char txtbuf[65536] = {0};
	va_list args;
	va_start(args, text);
	vsnprintf_s(txtbuf, sizeof(txtbuf), (sizeof(txtbuf) / sizeof(txtbuf[0])) - 1, text, args);
	va_end(args);
	WritePrivateProfileStringA(section.c_str(), key.c_str(), txtbuf, mFileName.c_str());
}

//-------------------------------------------------------------------------

// Read from any section
std::string ConfigFile::Read(const std::string& section, const std::string& key)
{
	char buffer[65536] = {0};
	GetPrivateProfileStringA(section.c_str(), key.c_str(), NULL, buffer, 65535, mFileName.c_str());
	return std::string(buffer);
}

//-------------------------------------------------------------------------

// Deletes an entire section and all keys
void ConfigFile::DeleteSection(const std::string & section)
{
	WritePrivateProfileStringA(section.c_str(), NULL, NULL, mFileName.c_str());
}

//-------------------------------------------------------------------------

// Deletes a key from the specified section
void ConfigFile::DeleteKey(const std::string & section, const std::string & key)
{
	WritePrivateProfileStringA(section.c_str(), key.c_str(), NULL, mFileName.c_str());
}

//-------------------------------------------------------------------------
