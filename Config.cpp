#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <shlobj.h>
#include <string>
#include <sstream>

#include "Config.h"

#pragma comment(lib, "shell32.lib")

// takes the version number from GUI class
// if installed version is different from the number written in config file...
// then we're updating the program - we should rewrite everything in userdata folder!
void Config::setVersion(const std::string &v)
{
	version = v;
}

// this is used to do initial setup of BeepComp USERDOC userdata folder
// if config file is not found in the installation directory that means program is being run
// for the first time <- if so, copy all the txt files in userdata origin to USERDOC destination

std::string Config::setup()
{
	versionDifferent = false;

	bool configFileFound = false;
	bool configFileValid = false;
	bool firstTime = false;
	bool setupDone = false;
	string strSetupResult = "";
	
	// get currentDir path
	currentDir = getCurrentDir(); // current directory
	cout << "currentDir = " << currentDir << endl;
	
	// get ENV's appdata path PLUS 'BeepComp'
	appDataPath = getAppDataPath();
	
	bool appDataCreateResult;
	
	//
	// create the parent folder in appdata
	//
	appDataCreateResult = CreateDirectory(appDataPath.c_str(), NULL);
	cout << "Create %APPDATA%\\BeepComp folder:  " << appDataPath << endl;

	// success-fail check
	if(appDataCreateResult)
		cout << "success!\n";
	else
	{
		cout << "fail.\n";
		if(GetLastError()==ERROR_ALREADY_EXISTS) cout << "The folder already exists.\n";
	}	
	
	// now check for the config file's existance
	cout << "check if beepcomp.config exists...\n";
	configFilePath = appDataPath + "\\" + "beepcomp.config";
	cout << "configFilePath = " << configFilePath << endl;
	
	// if config file found ... get Path information from that
	configFileFound = configFileExists(configFilePath);
	
	// if config file found -> the program has already run once after installation	
	// we will check for the validity of config file...
	
	// if config file was found, let's try to get path data from it 
	if(configFileFound)
	{
		cout << "Config file exists, reading contents and check data validity...\n";
		configFileValid = readConfigFile();
		
		// read config file ... you get boolean result for data validity
		if(configFileValid)
		{
			cout << "Config file's data is good!.\n";
			cout << "Path information is now:\n";
			cout << "userDocPath: " << userDocPath << endl;
			cout << "userdataParentPath: " << userdataParentPath << endl;
			cout << "userdataPath: " << userdataPath << endl;
			setupDone = true; // congrats, all done here
		}
		else
			cout << "Config file's data is NOT good...\n";
	}
	// config file NOT found. first time here
	else
	{
		cout << "Config file NOT found. first time here...\n";
		firstTime = true;
	}
	
	if(versionDifferent)
		cout << "Installed version is different - we should overwrite userdata.\n";
	
	// if config file is found and also its data is valid... 
	// we have read all the valid path information - we will exit without setup
	if(setupDone)
	{
		strSetupResult = "SETUP SKIPPED - Config file found and is valid";
		return strSetupResult;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	
	// File not found - means program is being run for the first time after installation
	// - will do initial setup!
	
	setupDone = false;
	if(firstTime)
	{
		cout << "Config file does not exist. Must be fresh after installation!\n";
		strSetupResult = "Initial setup - ";
	}
	else
	{
		cout << "Config file corrupt - need to rebuild! (or possibly dest folders erased?)\n";
		strSetupResult = "Rebuild corrupt config file (or reconstructing dest folders) - ";
	}

	// get the userDoc path from env var
	userDocPath = getUserDocPathFromSystem();
	cout << "User's 'Documents' path = " << userDocPath << endl;
	
	// if this EVER fails... safeguard: set it to USERPROFILE folder...
	if(userDocPath.empty())
	{
		userDocPath = string(getenv("USERPROFILE"));
		cout << "User's Documents path corrected to USERPROFILE folder:\n";
		cout << userDocPath << endl;
	}
	
	bool result;
	
	//
	// create the parent folder in userdata
	//
	userdataParentPath = userDocPath + "\\" + "BeepComp";
	result = CreateDirectory(userdataParentPath.c_str(), NULL);
	cout << "Create parent folder:  " << userdataParentPath << endl;

	// success-fail check
	if(result)
		cout << "success!\n";
	else
	{
		cout << "fail.\n";
		if(GetLastError()==ERROR_ALREADY_EXISTS) cout << "The folder already exists.\n";
	}	
	
	//
	// create the userdata folder in the parent folder
	//
	userdataPath = userdataParentPath + "\\" + "userdata";
	result = CreateDirectory(userdataPath.c_str(), NULL);
	cout << "Create userdata folder:  " << userdataPath << endl;

	// success-fail check
	if(result)
		cout << "success!\n";
	else
	{
		cout << "fail.\n";
		if(GetLastError()==ERROR_ALREADY_EXISTS) cout << "The folder already exists.\n";
	}
	
	// now get ready to copy all the txt files to destination...
	cout << "Okay, first time here. Let's set up your userdata folder...\n";
	
	// get ready to copy all the txt files from userdata source
	userdataPathOrigin = currentDir + "\\" + "userdata";
	vector<string> dataFiles = getFileNamesInFolder(userdataPathOrigin);
	
	int nFiles = dataFiles.size();
	
	cout << "Listing files to copy...\n";
	for(int i=0; i<nFiles; i++)
		cout << dataFiles[i] << endl;
	
	// now copy all these files to to the newly created userdata directory!
	for(int i=0; i<nFiles; i++)
	{
		string source = userdataPathOrigin + "\\" + dataFiles[i];
		string destination = userdataPath + "\\" + dataFiles[i];
		cout << "Copying...\n";
		cout << "From: " << source << endl;
		cout << "To:   " << destination << endl;
		
		int copyResult;
		copyResult = CopyFile(source.c_str(), destination.c_str(), FALSE); // will force overwrite
		
		if(copyResult==0)
			cout << "...failed\n";
		else
			cout << "...success!\n";
	}
	
	// write all path information to config file...
	
	result = writeConfigFile();
	if(!result)
	{
		cout << "Unable to write: " << configFilePath << endl;
		strSetupResult += "error writing config file";
	}
	else
	{
		cout << "beepcomp.config file created!\n";
		strSetupResult += "SUCCESS";
	}
	
	return strSetupResult;
}

// get current directory
std::string Config::getCurrentDir()
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

// get all file names in folder
vector<std::string> Config::getFileNamesInFolder(string folder)
{
    vector<string> names;
    char search_path[200];
    sprintf(search_path, "%s/*.*", folder.c_str());
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path, &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}


// get current user's documents path from system...
std::string Config::getUserDocPathFromSystem()
{
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

	string strResult = "";
	
	if (result != S_OK)
	{
		cout << "Error getting the User Documents path...\n";
		return strResult;
	}
	stringstream ss;
	ss << my_documents;
	strResult = ss.str();
	return strResult;
}

// get the appdata path plus 'BeepComp'
std::string Config::getAppDataPath()
{
	string pathToReturn = string(getenv("APPDATA"));
	pathToReturn += "\\BeepComp";
	return pathToReturn;
}

// get current user's documents path
std::string Config::getUserDocPath()
{
	return userDocPath;
}

std::string Config::getUserdataPath()
{
	return userdataPath;
}

// checks if config file is present in current dir
bool Config::configFileExists(std::string cfgPath)
{
	bool exists = false;
	GetFileAttributes(cfgPath.c_str()); // from winbase.h
	if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(cfgPath.c_str()) && GetLastError()==	ERROR_FILE_NOT_FOUND)
	{
		//File not found
		exists = false;
	}
	else
	{
		// File found!
		exists = true;
	}
	return exists;
}

// read path data from config file...
bool Config::readConfigFile()
{
	ifstream inFile;
	inFile.open(configFilePath.c_str());
	string line;
	
	string tempAppDataPath = "";
	string tempDataParentPath = "";
	string tempUserdataPath = "";
	string tempVersion = "";
	size_t found;
	
	if(inFile.is_open())
	{
		// read a line from file, and try parsing
		while( getline(inFile, line) )
		{
			int lineLen = line.length();
			int searchStrLen = 0;
			int nCharsToRead = 0;
			
			string searchStr;
			searchStr = "USERDOCPATH=";
			
			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempAppDataPath = line.substr(searchStrLen, nCharsToRead);
				
				cout << "USERDOCPATH found:\n" << tempAppDataPath << endl;
			}
			
			searchStr = "USERDATAPARENTPATH=";
			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempDataParentPath = line.substr(searchStrLen, nCharsToRead);
				
				cout << "USERDATAPARENTPATH found:\n" << tempDataParentPath << endl;
			}
			
			searchStr = "USERDATAPATH=";

			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempUserdataPath = line.substr(searchStrLen, nCharsToRead);
				
				cout << "USERDATAPATH found:\n" << tempUserdataPath << endl;
			}

			searchStr = "VERSION=";

			found = line.find(searchStr);
			if(found!=string::npos) // found!
			{
				// extract the data part following '='
				searchStrLen = searchStr.length();
				nCharsToRead = lineLen - searchStrLen;
				tempVersion = line.substr(searchStrLen, nCharsToRead);
				
				cout << "VERSION found:\n" << tempVersion << endl;
			}			

			
		}
	}
	else
	{
		cout << "Error opening config file: " << configFilePath << endl;
		return false;
	}
	
	// file op done - close
	inFile.close();
	
	// let's check the data validity...
	
	bool allDataValid = false;
	bool userDocPathValid, userdataParentPathValid, userdataPathValid;
	bool versionCurrent;
	
	userDocPathValid = dirExists(tempAppDataPath);
	userdataParentPathValid = dirExists(tempDataParentPath);
	userdataPathValid = dirExists(tempUserdataPath);
	
	if(tempUserdataPath.find("userdata")==string::npos)
	{
		cout << "USERDATAPATH does not contain the world 'userdata'!\n";
		userdataPathValid = false;
	}
	
	if(version==tempVersion)
	{
		cout << "Installed version and version number in config match!\n";
		versionDifferent = false;
	}
	else
		versionDifferent = true;
	
	// check validity of all path data... if good, go ahead and rewrite.
	if(userDocPathValid && userdataParentPathValid && userdataPathValid && !versionDifferent)
	{
		allDataValid = true;
		userDocPath = tempAppDataPath;
		userdataParentPath = tempDataParentPath;
		userdataPath = tempUserdataPath;
		version = tempVersion;
	}
	else // not valid, set all paths to empty
	{
		allDataValid = false;
		userDocPath = "";
		userdataParentPath = "";
		userdataPath = "";
	}
	
	return allDataValid;
}

// write path data from config file... based on currently set path variables
bool Config::writeConfigFile()
{
	string userDocPathField = "USERDOCPATH=" + userDocPath;
	string userdataParentPathField = "USERDATAPARENTPATH=" + userdataParentPath;
	string userdataPathField = "USERDATAPATH=" + userdataPath;
	string versionField = "VERSION=" + version;

	// create the config file now...
	ofstream outFile;
	outFile.open(configFilePath.c_str(), ios::trunc);
	
	if (outFile.is_open())
	{
		outFile << userDocPathField << endl;
		outFile << userdataParentPathField << endl;
		outFile << userdataPathField << endl;
		outFile << versionField << endl;
		outFile.close();
	}
	else
		return false; // unable to open config file!

	return true;
}

// check if a directory of the passed name exists
bool Config::dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

// make a batch file to let user completely remove the ghost folder
bool Config::makeUninstallBatchFile(const std::string &batchFileName, const std::string &targetFolder)
{
	ofstream outFile;
	outFile.open(batchFileName.c_str(), ios::trunc);
	
	if(!outFile.is_open())
	{
		cout << "Error opening: " << batchFileName << endl;
		return false;
	}
	
	string strToWrite = "@RD /S /Q \"" + targetFolder + "\"";
	outFile << strToWrite << endl;
	outFile.close();
	
	return true;
}

