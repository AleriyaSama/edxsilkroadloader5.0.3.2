#include "FileChooser.h"
#include <windows.h>
#include <stdio.h>

//-------------------------------------------------------------------------

// Private data for the FileChooser class
struct tFileChooserData
{
	// File chooser struct
	OPENFILENAMEA fn;

	// Buffers for the file chooser 
	char setDirectory[2048];
	char setDialogTitle[2048];
	char setDefFileName[2048];
	char setFilter[2048];

	// User buffers to store data
	char filepath[2048];
	char filetitle[2048];
	char filename[2048];
	char fileext[2048];
	char filedir[2048];

	// Index in the filter string since we have to manually create it
	int filterIndex;

	// Can we use the ShowChooseFile function?
	bool canShow;
};

//-------------------------------------------------------------------------

// Constructor
FileChooser::FileChooser()
{
	data = new tFileChooserData;
	memset(data, 0, sizeof(tFileChooserData));

	// Have to set this for the struct
	data->fn.lStructSize = sizeof(OPENFILENAME);

	// We can select a file
	data->canShow = true;
}

//-------------------------------------------------------------------------

// Destructor
FileChooser::~FileChooser()
{
	delete data;
}

//-------------------------------------------------------------------------

// Sets the initial directory the file chooser looks in
void FileChooser::SetInitialDirectory(const char * pDir)
{
	_snprintf_s(data->setDirectory, 2047, "%s", pDir);
}

//-------------------------------------------------------------------------

// Sets the default dialog title of the file chooser
void FileChooser::SetDialogTitle(const char * pTitle)
{
	_snprintf_s(data->setDialogTitle, 2047, "%s", pTitle);
}

//-------------------------------------------------------------------------

// Sets the default data->filename in the file choose dialog
void FileChooser::SetDefaultFileName(const char * pFileName)
{
	_snprintf_s(data->setDefFileName, 2047, "%s", pFileName);
}

//-------------------------------------------------------------------------

// Adds a file browsing filter
// pFilterName - Name of the extension to display, i.e. "Executable Files"
// pFilterExt - Extension of the filter, i.e. "*.exe"
void FileChooser::AddFilter(const char * pFilterName, const char * pFilterExt)
{
	// First part is the name of the filter
	_snprintf_s(data->setFilter + data->filterIndex, 2048 - data->filterIndex, 2047 - data->filterIndex, "%s", pFilterName);
	data->filterIndex += (int)strlen(pFilterName);

	// Separate with a NULL terminator
	data->setFilter[data->filterIndex++] = '\0';

	// Second part is the extension of the filter, *.EXTENSION
	_snprintf_s(data->setFilter + data->filterIndex, 2048 - data->filterIndex, 2047 - data->filterIndex, "%s", pFilterExt);
	data->filterIndex += (int)strlen(pFilterExt);

	// Separate with a NULL terminator
	data->setFilter[data->filterIndex++] = '\0';
}

//-------------------------------------------------------------------------

// Will return a string in this format: "Filename"
const char * FileChooser::GetSelectedFileTitle()
{
	return data->filetitle;
}

//-------------------------------------------------------------------------

// Will return a string in this format: "Filename.Extension"
const char * FileChooser::GetSelectedFileName()
{
	return data->filename;
}

//-------------------------------------------------------------------------

// Will return a string in this format: "Drive:\Path\To\File\Filename.Extension"
const char * FileChooser::GetSelectedFilePath()
{
	return data->filepath;
}

//-------------------------------------------------------------------------

// Will return a string in this format: "Drive:\Path\To\File\"
const char * FileChooser::GetSelectedFileDirectory()
{
	return data->filedir;
}

//-------------------------------------------------------------------------

// Will return a string in this format: "Extension"
const char * FileChooser::GetSelectedFileExtension()
{
	return data->fileext;
}

//-------------------------------------------------------------------------

// Allow the user to select a file, returns true on success and false on failure
bool FileChooser::ShowChooseFile(bool open)
{
	// If we cannot show the file dialog, return failure
	if(!data->canShow)
		return false;

	// Store the current directory before we change it
	char curDir[256] = {0};
	GetCurrentDirectoryA(255, curDir);

	// Have to set this for the struct
	data->fn.lStructSize = sizeof(OPENFILENAME);

	// Default directory
	data->fn.lpstrInitialDir = data->setDirectory;

	// Finish the file filter with the two NULLS it needs at the end
	data->setFilter[data->filterIndex++] = '\0';
	data->setFilter[data->filterIndex++] = '\0';
	data->fn.lpstrFilter = data->setFilter;

	// Tell the chooser to use our buffer
	data->fn.lpstrFile = data->setDefFileName;

	// Max size of buffer
	data->fn.nMaxFile = 2047;

	// Tell the chooser to use our buffer
	data->fn.lpstrFileTitle = data->filename;

	// Max size of buffer
	data->fn.nMaxFileTitle = 2047;

	// Title we wish to display
	data->fn.lpstrTitle = data->setDialogTitle;

	// Display the chooser!
	if(open)
	{
		// Flags for selecting a file
		data->fn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

		if(!GetOpenFileNameA(&data->fn))
		{
			// Restore the old directory
			SetCurrentDirectoryA(curDir);

			// Error or the user canceled
			return FALSE;
		}
	}
	else
	{
		if(!GetSaveFileNameA(&data->fn))
		{
			// Restore the old directory
			SetCurrentDirectoryA(curDir);

			// Error or the user canceled
			return FALSE;
		}
	}

	// Restore the old directory
	SetCurrentDirectoryA(curDir);

	// Copy the file path from the struct into the buffer
	_snprintf_s(data->filepath, 2047, data->fn.lpstrFile);

	// Store the file directory
	_snprintf_s(data->filedir, 2047, "%s", data->fn.lpstrFile);

	// Loop though the string backwards
	for(int x = (int)strlen(data->filedir) - 1; x >= 0; --x)
	{
		// Stop at the first directory separator
		if(data->filedir[x] == '\\')
		{
			// Remove everything after it by setting a NULL terminator in the string
			data->filedir[x + 1] = 0;
			break;
		}
	}

	// Store the filetitle
	_snprintf_s(data->filetitle, 2047, "%s", data->filename);

	// Loop though the string forwards
	for(int x = (int)strlen(data->filetitle) - 1; x > 0; --x)
	{
		// Stop at the last extension separator
		if(data->filetitle[x] == '.')
		{
			// Remove it and everything past it
			data->filetitle[x] = 0;
			break;
		}
	}

	// Temp buffer to store the filename with extension
	char temp[2048] = {0};
	_snprintf_s(temp, 2047, "%s", GetSelectedFileName());
	_snprintf_s(data->fileext, 2047, "%s", temp + strlen(data->filetitle) + 1);

	// We can no longer use this function again
	data->canShow = false;

	// Success
	return TRUE;
}

//-------------------------------------------------------------------------
