// BeepComp versionEdit //////////////////////////////////////////////////////
// use this program to make necessary changes across BeepComp project ////////
// files to update to a newer version number /////////////////////////////////

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;

std::string str;
vector<string> files;

void displaySnippet(const std::string &filename, const std::string &strToFind, int nCharsToFollow)
{
	cout << "----------------------------------------\n";
	
	string str;
	string line;
	ifstream inFile;

	// open the file
	inFile.open(filename.c_str());
	if(!inFile)
	{
		cout << "Error opening: " << filename << endl << endl;
		return;
	}
	else
		cout << "In file: " << filename << " ..." << endl;

	// copy the file content to the internal string variable
	while( getline(inFile, line) )
	{
		str += line + "\n";
	}
	inFile.close();
	
	size_t found = str.find(strToFind);
	if(found!=string::npos) // found!
	{
		int len = strToFind.length() + nCharsToFollow;
		if( (found + len) >= str.length())
			len = str.length() - found;
		string strToDisplay = str.substr(found, len);
		cout << "Found:\n" + strToDisplay << endl;
	}
	else
	{
		cout << "ALERT: could not find the search string!\n";
	}
	
	inFile.close();
	
	cout << endl;
	
}

int main()
{
	system("cls");
	
	cout << "********** BeepComp source version check **********\n\n";

	displaySnippet("README.txt", "This version is: v", 8);
	displaySnippet("README.md",  "This version is: v", 8);
	displaySnippet("GUI.cpp",    "STR_VERSION = \"", 8);
	displaySnippet("wix/beepcomp.wxs",    "Version='", 8);
	
	// ShellExecute(NULL, "open", "target_cpp", NULL, NULL, SW_SHOWNORMAL);
	// ShellExecute(NULL, "open", "target_h", NULL, NULL, SW_SHOWNORMAL);
	
	cout << "[ESC] Quit\n\n";
	
	while(!GetAsyncKeyState(VK_ESCAPE)){}
	return 0;
}