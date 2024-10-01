#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include <string>
#include <map>

#include "DriverComm.h"

using std::hash;

struct FileInfo {
    std::wstring Path;
	bool isFile = true;
    bool isEnable = true;
};


class FileManager {
private:

	bool kHiddenFileEnable = true;
	bool kProtectedFileEnable = true;

	std::map<size_t, FileInfo> hidden_list_;
	std::map<size_t, FileInfo> protected_list_;
	DriverComm* driver_comm_;
public:
	FileManager(DriverComm* dc);
	~FileManager();

	void AddHiddenFile(const FileInfo& file);
	void AddProtectedFile(const FileInfo& file);

	void RemoveHiddenFile(const std::wstring& name);
	void RemoveProtectedFile(const std::wstring& name);

	void EnableHiddenFile(const std::wstring& name);
	void EnableProtectedFile(const std::wstring& name);

	void DisableHiddenFile(const std::wstring& name);
	void DisableProtectedFile(const std::wstring& name);

	void EnableHiddenFileMode();
	void DisableHiddenFileMode();
	bool IsHiddenFileModeEnabled();

	void EnableProtectedFileMode();
	void DisableProtectedFileMode();
	bool IsProtectedFileModeEnabled();

	const std::map<size_t, FileInfo>& GetHiddenList();
	const std::map<size_t, FileInfo>& GetProtectedList();
};

#endif // FILEMANAGER_H
