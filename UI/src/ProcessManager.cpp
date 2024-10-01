#include "ProcessManager.h"

ProcessManager::ProcessManager(DriverComm* dc) {
	driver_comm_ = dc;
    // Khởi tạo danh sách bảo vệ tiến trình (ví dụ)
    protected_process_list_ = {
        {hash<wstring>{}(L"C:\\Windows\\System32\\notepad.exe"), {L"C:\\Windows\\System32\\notepad.exe", true}},
        {hash<wstring>{}(L"C:\\Program Files\\MyApp\\myapp.exe"), {L"C:\\Program Files\\MyApp\\myapp.exe", true}}
    };

    // Khởi tạo danh sách white list tiến trình (ví dụ)
    whitelist_process_list_ = {
        {hash<std::wstring>{}(L"C:\\Windows\\System32\\svchost.exe"), {L"C:\\Windows\\System32\\svchost.exe", true}},
        {hash<std::wstring>{}(L"C:\\Program Files\\MyApp\\trustedapp.exe"), {L"C:\\Program Files\\MyApp\\trustedapp.exe", true}}
    };
}

ProcessManager::~ProcessManager() {
    protected_process_list_.clear();
    whitelist_process_list_.clear();
}

// Quản lý danh sách process bảo vệ
void ProcessManager::AddProtectedProcess(const ProcessInfo& process) {
    size_t hash_val = hash<std::wstring>{}(process.Path);
    if (protected_process_list_.find(hash_val) == protected_process_list_.end()) {
        protected_process_list_.insert({ hash_val, process });
    }
	driver_comm_->ProtectProcImage(process.Path);
}

void ProcessManager::RemoveProtectedProcess(const std::wstring& name) {
    size_t hash_val = hash<std::wstring>{}(name);
    if (protected_process_list_.find(hash_val) != protected_process_list_.end()) {
        protected_process_list_.erase(hash_val);
    }
	driver_comm_->UnprotectProcImage(name);
}

void ProcessManager::EnableProtectedProcess(const std::wstring& name) {
    size_t hash_val = hash<std::wstring>{}(name);
    if (protected_process_list_.find(hash_val) != protected_process_list_.end()) {
        protected_process_list_[hash_val].isEnable = true;
    }
}

void ProcessManager::DisableProtectedProcess(const std::wstring& name) {
    size_t hash_val = hash<std::wstring>{}(name);
    if (protected_process_list_.find(hash_val) != protected_process_list_.end()) {
        protected_process_list_[hash_val].isEnable = false;
    }
}

const std::map<size_t, ProcessInfo>& ProcessManager::GetProtectedProcessList() {
    return protected_process_list_;
}

// Quản lý danh sách white list process
void ProcessManager::AddProcessToWhitelist(const ProcessInfo& process) {
    size_t hash_val = hash<std::wstring>{}(process.Path);
    if (whitelist_process_list_.find(hash_val) == whitelist_process_list_.end()) {
        whitelist_process_list_.insert({ hash_val, process });
    }
	driver_comm_->WhitelistProcImage(process.Path);
}

void ProcessManager::RemoveProcessFromWhitelist(const std::wstring& name) {
    size_t hash_val = hash<std::wstring>{}(name);
    if (whitelist_process_list_.find(hash_val) != whitelist_process_list_.end()) {
        whitelist_process_list_.erase(hash_val);
    }
	driver_comm_->UnwhitelistProcImage(name);
}

const std::map<size_t, ProcessInfo>& ProcessManager::GetWhitelistProcessList() {
    return whitelist_process_list_;
}
