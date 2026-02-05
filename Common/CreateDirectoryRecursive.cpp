#include "CreateDirectoryRecursive.h"
#include <windows.h>
#include <string>

//-----------------------------------------------------------------------------

void CreateDirectoryRecursive(const char * path)
{
	if(CreateDirectoryA(path, 0) != FALSE)
	{
		return;
	}
	std::string path_ = path;
	if(path_.empty())
	{
		return;
	}
	for(size_t x = 0; x < path_.size(); ++x)
	{
		char & ch = path_[x];
		if(ch == '/')
		{
			ch = '\\';
		}
	}
	if(path_[path_.size() - 1] != '\\')
	{
		path_ += "\\";
	}
	size_t ind = 0;
	while((ind = path_.find_first_of('\\', ind)) != std::string::npos)
	{
		std::string cpath = path_.substr(0, ind);
		ind++;
		CreateDirectoryA(cpath.c_str(), 0);
	}
}

//-----------------------------------------------------------------------------
