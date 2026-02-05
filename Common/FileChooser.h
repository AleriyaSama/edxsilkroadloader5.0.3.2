#pragma once

#ifndef FILECHOOSER_H_
#define FILECHOOSER_H_

//-------------------------------------------------------------------------

// Forward declaration for the FileChooser class data so it is not exposed.
struct tFileChooserData;

// File chooser class
class FileChooser
{
private:
	tFileChooserData* data;

public:
	FileChooser();
	~FileChooser();

	// Sets the initial directory the file chooser looks in.
	void SetInitialDirectory(const char * pDir);

	// Sets the default dialog title of the file chooser component.
	void SetDialogTitle(const char * pTitle);

	// Sets the default filename in the file choose dialog.
	void SetDefaultFileName(const char * pFileName);

	// Adds a file browsing filter.
	void AddFilter(const char * pFilterName, const char * pFilterExt);

	// Allow the user to select a file. (open => true for param, save => false for param)
	bool ShowChooseFile(bool open);

	// Returns the file path of the selected file.
	const char * GetSelectedFilePath();

	// Returns the file directory of the selected file.
	const char * GetSelectedFileDirectory();

	// Returns the filename of the selected file.
	const char * GetSelectedFileName();

	// Returns the file title of the selected file.
	const char * GetSelectedFileTitle();

	// Returns the file extension of the selected file.
	const char * GetSelectedFileExtension();
};

//-------------------------------------------------------------------------

#endif
