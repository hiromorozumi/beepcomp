// OpenFileDialog.h ////////////////////////////////
// OpenFileDialog class - definition ///////////////

#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <Windows.h>
#include <Commdlg.h>
#include <tchar.h>

class OpenFileDialog
{
public:
    OpenFileDialog(void);
	~OpenFileDialog()
	{}

    TCHAR* DefaultExtension;
    TCHAR* FileName;
    TCHAR* Filter;
    int FilterIndex;
    int Flags;
    TCHAR* InitialDir;
    HWND Owner;
    TCHAR* Title;

    bool ShowDialog(int type);
};

#endif