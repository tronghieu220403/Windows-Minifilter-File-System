#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <functional> // std::hash
using std::hash;
using std::wstring;


struct ProcessInfo {
    std::wstring Path;
    bool isEnable = true;
};

extern inline bool kProtectedProcessEnable = true;
extern inline bool kWhitelistProcessEnable = true;

class ProcessManager {
private:
    std::map<size_t, ProcessInfo> protected_process_list_;
    std::map<size_t, ProcessInfo> whitelist_process_list_;

public:
    ProcessManager();
    ~ProcessManager();

    // Quản lý danh sách bảo vệ process
    void AddProtectedProcess(const ProcessInfo& process);
    void RemoveProtectedProcess(const std::wstring& name);
    void EnableProtectedProcess(const std::wstring& name);
    void DisableProtectedProcess(const std::wstring& name);
    const std::map<size_t, ProcessInfo>& GetProtectedProcessList();

    // Quản lý danh sách white list process
    void AddProcessToWhitelist(const ProcessInfo& process);
    void RemoveProcessFromWhitelist(const std::wstring& name);
    const std::map<size_t, ProcessInfo>& GetWhitelistProcessList();
};

#endif // PROCESSMANAGER_H
