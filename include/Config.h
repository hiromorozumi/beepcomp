#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

using namespace std;

class Config
{

public:

	bool versionDifferent;
	string version;
	string appDataPath;
	string userDocPath;
	string userdataParentPath;
	string userdataPath;
	string currentDir;
	string userdataPathOrigin;
	string configFilePath;

	Config()
	{
		userDocPath = "";
		userdataParentPath = "";
		userdataPath = "";
		currentDir = "";
		userdataPathOrigin = "";
		configFilePath = "";
	}
	~Config(){}

	void setVersion(const std::string &v);
	std::string setup();
	
	std::string getAppDataPath();	
	std::string getCurrentDir();
	vector<std::string> getFileNamesInFolder(string folder);
	std::string getUserDocPathFromSystem();
	std::string getUserDocPath();
	std::string getUserdataPath();
	bool configFileExists(std::string cfgPath);
	bool readConfigFile();
	bool writeConfigFile();
	bool dirExists(const std::string& dirName_in);
	bool makeUninstallBatchFile(const std::string &batchFileName, const std::string &targetFolder);
};

#endif