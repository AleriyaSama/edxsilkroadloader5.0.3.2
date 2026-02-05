#pragma once

#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

//-------------------------------------------------------------------------

#include <string>
#include <sstream>

//-------------------------------------------------------------------------

class ConfigFile
{
private:
	std::string mFileName;

public:
	ConfigFile();
	~ConfigFile();

	// Opens a file to work with. Returns true if the file exists or false if it does not. If
	// the file does not exist, it will be created once Write or WriteTo is called.
	bool Open(const std::string & filename);

	// Deletes an entire section and all keys
	void DeleteSection(const std::string & section);

	// Deletes a key from the specified section
	void DeleteKey(const std::string & section, const std::string & key);

	// Writes to any section
	void Write(const std::string & section, const std::string & key, const char * text, ...);

	// Read from any section
	std::string Read(const std::string & section, const std::string & key);

	// Read from any section
	template <typename type>
	type ReadType(const std::string & section, const std::string & key)
	{
		type t;
		std::stringstream ss;
		ss << Read(section, key);
		ss >> t;
		return t;
	}
};

//-------------------------------------------------------------------------

#endif
