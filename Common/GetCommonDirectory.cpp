#include "GetCommonDirectory.h"
#include "CreateDirectoryRecursive.h"
#include <sstream>
#include <shlwapi.h>
#include <shlobj.h>

//-----------------------------------------------------------------------------

std::string GetCommonDirectory(const std::string & baseDir)
{
	std::stringstream ss;
	char strPath[MAX_PATH + 1] = {0};
	if(SHGetSpecialFolderPathA(0, strPath, CSIDL_APPDATA, FALSE) == FALSE)
	{
		throw std::exception("Unable to retrieve the path to the special folder AppData");
	}
	else
	{
		ss << strPath << "\\edxLabs" << "\\" << baseDir << "\\";
		CreateDirectoryRecursive(ss.str().c_str());
	}
	return ss.str();
}

//-----------------------------------------------------------------------------
