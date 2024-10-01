#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <functional> // std::hash
#include "DriverComm.h"


using std::hash;
using std::wstring;


struct ProcessInfo {
    std::wstring Path;
    bool isEnable = true;
};


class ProcessManager {
private:
    bool kProtectedProcessEnable = true;
    bool kWhitelistProcessEnable = true;

    std::map<size_t, ProcessInfo> protected_process_list_;
    std::map<size_t, ProcessInfo> whitelist_process_list_;
	DriverComm* driver_comm_;

public:
    ProcessManager(DriverComm* dc);
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

	// Bật/Tắt chế độ bảo vệ process
	void EnableProtectedProcessMode();
	void DisableProtectedProcessMode();
	bool IsProtectedProcessModeEnabled();
};

#endif // PROCESSMANAGER_H
