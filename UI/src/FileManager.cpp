#include "FileManager.h"

FileManager::FileManager() {
	hidden_list_ = {
	{hash<std::wstring>{}(L"E:\\Download\\Blitz-2.1.206.exe"),{L"E:\\Download\\Blitz-2.1.206.exe", true, true}},
	{hash<std::wstring>{}(L"E:\\Download\\debug.txt"),{L"E:\\Download\\debug.txt", true, false}},
	{hash<std::wstring>{}(L"C:\\Users\\hieu_admin\\AppData\\Local"),{L"C:\\Users\\hieu_admin\\AppData\\Local", false, true}}
	};
	protected_list_ = {
	{hash<std::wstring>{}(L"E:\\Download\\Blitz-2.1.206.exe"),{L"E:\\Download\\Blitz-2.1.206.exe", true, true}},
	{hash<std::wstring>{}(L"E:\\Download\\debug.txt"),{L"E:\\Download\\debug.txt", true, true}},
	{hash<std::wstring>{}(L"C:\\Users\\hieu_admin\\AppData\\Local"),{L"C:\\Users\\hieu_admin\\AppData\\Local", false, true}}
	};
}

FileManager::~FileManager() {
	hidden_list_.clear();
	protected_list_.clear();
}

void FileManager::AddHiddenFile(const FileInfo& file) {
	size_t hash_val = hash<std::wstring>{}(file.Path);
	if (hidden_list_.find(hash_val) == hidden_list_.end())
	{
		hidden_list_.insert({ hash_val, file });
	}
}

void FileManager::AddProtectedFile(const FileInfo& file) {
	size_t hash_val = hash<std::wstring>{}(file.Path);
	if (protected_list_.find(hash_val) == protected_list_.end())
	{
		protected_list_.insert({ hash_val, file });
	}
}

void FileManager::RemoveHiddenFile(const std::wstring& name) {
	size_t hash_val = hash<std::wstring>{}(name);
	if (hidden_list_.find(hash_val) != hidden_list_.end())
	{
		hidden_list_.erase(hash_val);
	}
}

void FileManager::RemoveProtectedFile(const std::wstring& name) {
	size_t hash_val = hash<std::wstring>{}(name);
	if (protected_list_.find(hash_val) != protected_list_.end())
	{
		protected_list_.erase(hash_val);
	}
}

void FileManager::EnableHiddenFile(const std::wstring& name) {
	size_t hash_val = hash<std::wstring>{}(name);
	if (hidden_list_.find(hash_val) != hidden_list_.end())
	{
		hidden_list_[hash_val].isEnable = true;
	}
}

void FileManager::EnableProtectedFile(const std::wstring& name) {
	size_t hash_val = hash<std::wstring>{}(name);
	if (protected_list_.find(hash_val) != protected_list_.end())
	{
		protected_list_[hash_val].isEnable = true;
	}
}

void FileManager::DisableHiddenFile(const std::wstring& name) {
	size_t hash_val = hash<std::wstring>{}(name);
	if (hidden_list_.find(hash_val) != hidden_list_.end())
	{
		hidden_list_[hash_val].isEnable = false;
	}
}

void FileManager::DisableProtectedFile(const std::wstring& name) {
	size_t hash_val = hash<std::wstring>{}(name);
	if (protected_list_.find(hash_val) != protected_list_.end())
	{
		protected_list_[hash_val].isEnable = false;
	}
}

const std::map<size_t, FileInfo>& FileManager::GetHiddenList() {
	return hidden_list_;
}

const std::map<size_t, FileInfo>& FileManager::GetProtectedList() {
	return protected_list_;
}
