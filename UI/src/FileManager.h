#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include <string>
#include <map>

using std::hash;

struct FileInfo {
    std::wstring Path;
	bool isFile = true;
    bool isEnable = true;
};

extern inline bool kHiddenFileEnable = true;
extern inline bool kProtectedFileEnable = true;

class FileManager {
private:
	std::map<size_t, FileInfo> hidden_list_;
	std::map<size_t, FileInfo> protected_list_;
public:
	FileManager();
	~FileManager();

	void AddHiddenFile(const FileInfo& file);
	void AddProtectedFile(const FileInfo& file);

	void RemoveHiddenFile(const std::wstring& name);
	void RemoveProtectedFile(const std::wstring& name);

	void EnableHiddenFile(const std::wstring& name);
	void EnableProtectedFile(const std::wstring& name);

	void DisableHiddenFile(const std::wstring& name);
	void DisableProtectedFile(const std::wstring& name);

	const std::map<size_t, FileInfo>& GetHiddenList();
	const std::map<size_t, FileInfo>& GetProtectedList();
};

#endif // FILEMANAGER_H
