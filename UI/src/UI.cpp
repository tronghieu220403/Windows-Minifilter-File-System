#include "UI.h"
#include "FileManager.h" // Bao gồm FileManager để sử dụng các phương thức của nó
#include <AppCore/JSHelpers.h>
#include <Ultralight/Ultralight.h>
#include <iostream>

static UI* g_ui = 0;

UI::UI(RefPtr<Window> window) : window_(window), driver_comm_(HIEU_DEVICE_LINK), file_manager_(&driver_comm_), process_manager_(&driver_comm_){
    
    g_ui = this;

    overlay_ = Overlay::Create(window, window->width(), window->height(), 0, 0);
    view()->set_load_listener(this);
    view()->set_view_listener(this);
    view()->LoadURL("file:///files_hidden.html");
}

UI::~UI() {
    view()->set_load_listener(nullptr);
    view()->set_view_listener(nullptr);
    g_ui = nullptr;
}

void UI::OnClose(ultralight::Window* window) {
    App::instance()->Quit();
}

// Phương thức lấy danh sách file ẩn và trả về cho JavaScript
JSValue UI::GetHiddenList(const JSObject& thisObject, const JSArgs& args) {
    JSArray result;

    // Lấy danh sách file ẩn từ FileManager
    const auto& hiddenList = file_manager_.GetHiddenList();
    for (const auto& it : hiddenList) {
        size_t hash = it.first;
        FileInfo file = it.second;
        JSObject obj;
        obj["path"] = JSString(String16(file.Path.c_str(), file.Path.size()));
        obj["isFile"] = JSValue(file.isFile);
        obj["isEnable"] = JSValue(file.isEnable);
        result.push(JSValue(obj));
    }

    return JSValue(result);
}

// Phương thức lấy danh sách file bảo vệ và trả về cho JavaScript
JSValue UI::GetProtectedList(const JSObject& thisObject, const JSArgs& args) {
    JSArray result;

    // Lấy danh sách file bảo vệ từ FileManager
    const auto& protectedList = file_manager_.GetProtectedList();
    for (const auto& it : protectedList) {
        size_t hash = it.first;
        FileInfo file = it.second;
        JSObject obj;
        obj["path"] = JSString(String16(file.Path.c_str(), file.Path.size()));
        obj["isFile"] = JSValue(file.isFile);
        obj["isEnable"] = JSValue(file.isEnable);
        result.push(JSValue(obj));
    }

    return JSValue(result);
}

// Cập nhật trạng thái file ẩn trong C++
JSValue UI::UpdateHiddenStatus(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 2) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();
    bool newEnableStatus = args[1].ToBoolean();

    if (newEnableStatus) {
        file_manager_.EnableHiddenFile(path); // Kích hoạt file ẩn
    }
    else {
        file_manager_.DisableHiddenFile(path); // Vô hiệu hóa file ẩn
    }

    return JSValue("Status updated successfully");
}

// Cập nhật trạng thái file bảo vệ trong C++
JSValue UI::UpdateProtectedStatus(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 2) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();
    bool newEnableStatus = args[1].ToBoolean();

    if (newEnableStatus) {
        file_manager_.EnableProtectedFile(path); // Kích hoạt file bảo vệ
    }
    else {
        file_manager_.DisableProtectedFile(path); // Vô hiệu hóa file bảo vệ
    }

    return JSValue("Status updated successfully");
}

// Xóa một mục khỏi danh sách file ẩn
JSValue UI::RemoveFromHiddenList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 1) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();

    file_manager_.RemoveHiddenFile(path, args[1].ToBoolean()); // Xóa file ẩn khỏi FileManager

    return JSValue("File removed successfully");
}

// Xóa một mục khỏi danh sách file bảo vệ
JSValue UI::RemoveFromProtectedList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 1) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();

    file_manager_.RemoveProtectedFile(path, args[1].ToBoolean()); // Xóa file bảo vệ khỏi FileManager

    return JSValue("File removed successfully");
}

// Thêm một mục vào danh sách file ẩn
JSValue UI::AddToHiddenList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 3) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();
    bool isFile = args[1].ToBoolean();
    bool isEnable = args[2].ToBoolean();

    FileInfo file = { path, isFile, isEnable };
    file_manager_.AddHiddenFile(file); // Thêm file vào danh sách ẩn trong FileManager

    return JSValue("File added successfully");
}

// Thêm một mục vào danh sách file bảo vệ
JSValue UI::AddToProtectedList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 3) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();
    bool isFile = args[1].ToBoolean();
    bool isEnable = args[2].ToBoolean();

    FileInfo file = { path, isFile, isEnable };
    file_manager_.AddProtectedFile(file); // Thêm file vào danh sách bảo vệ trong FileManager

    return JSValue("File added successfully");
}

JSValue UI::LoadPage(const JSObject& thisObject, const JSArgs& args) {
    ultralight::String wpage(args[0].ToString());
    std::string page = std::string("file:///") + wpage.utf8().data();

    view()->LoadURL(page.c_str());

    RefPtr<JSContext> lock(view()->LockJSContext());

    return JSValue();
}

JSValue UI::UpdateFeatureStatus(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 2) {
        return JSValue("Invalid arguments");
    }

    // Lấy tên chức năng từ đối số đầu tiên
    std::wstring featureName = String(args[0].ToString()).utf16().data();
    // Lấy trạng thái (bật/tắt) từ đối số thứ hai
    std::wstring status = String(args[1].ToString()).utf16().data();

    // Thực hiện hành động dựa trên tên chức năng và trạng thái
    // Tùy chỉnh các hành động cho từng chức năng khác nhau
    if (featureName == L"Files hidden") {
        if (status == L"on") {
			file_manager_.EnableHiddenFileMode();
        }
        else {
			file_manager_.DisableHiddenFileMode();
        }
    }
    else if (featureName == L"Files protection") {
        if (status == L"on") {
			file_manager_.EnableProtectedFileMode();
        }
        else {
			file_manager_.DisableProtectedFileMode();
        }
    }
    else if (featureName == L"Processes protection") {
        if (status == L"on") {
			process_manager_.EnableProtectedProcessMode();
        }
        else {
			process_manager_.DisableProtectedProcessMode();
        }
    }
    return JSValue("Feature status updated successfully");
}

JSValue UI::GetFeatureStatus(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 1) {
        return JSValue("Invalid arguments");
    }

    // Lấy tên chức năng từ đối số đầu tiên
    std::wstring featureName = String(args[0].ToString()).utf16().data();

    // Trả về trạng thái tương ứng dựa vào tên chức năng
    if (featureName == L"Files hidden") {
        return JSValue(file_manager_.IsHiddenFileModeEnabled() ? "on" : "off");
    }
    else if (featureName == L"Files protection") {
        return JSValue(file_manager_.IsProtectedFileModeEnabled() ? "on" : "off");
    }
    else if (featureName == L"Processes protection") {
        return JSValue(process_manager_.IsProtectedProcessModeEnabled() ? "on" : "off");
    }

    return JSValue("off"); // Trả về "off" cho các chức năng không được biết
}

// Hàm lấy danh sách tiến trình bảo vệ và trả về cho JavaScript
JSValue UI::GetProctectedProcessList(const JSObject& thisObject, const JSArgs& args) {
    JSArray result;

    // Lấy danh sách tiến trình bảo vệ từ ProcessManager
    const auto& protectedProcessList = process_manager_.GetProtectedProcessList();
    for (const auto& it : protectedProcessList) {
        size_t hash = it.first;
        ProcessInfo process = it.second;
        JSObject obj;
        obj["path"] = JSString(String16(process.Path.c_str(), process.Path.size()));
        obj["isEnable"] = JSValue(process.isEnable);
        result.push(JSValue(obj));
    }

    return JSValue(result);
}

// Cập nhật trạng thái bảo vệ tiến trình trong C++
JSValue UI::UpdateProtectedProcessStatus(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 2) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();
    bool newEnableStatus = args[1].ToBoolean();

    if (newEnableStatus) {
        process_manager_.EnableProtectedProcess(path); // Kích hoạt bảo vệ tiến trình
    }
    else {
        process_manager_.DisableProtectedProcess(path); // Vô hiệu hóa bảo vệ tiến trình
    }

    return JSValue("Status updated successfully");
}

// Thêm một tiến trình vào danh sách bảo vệ
JSValue UI::AddToProctectedProcessList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 2) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();

    ProcessInfo process = { path, true };
    process_manager_.AddProtectedProcess(process); // Thêm tiến trình vào danh sách bảo vệ trong ProcessManager

    return JSValue("Process added successfully");
}

// Xóa một tiến trình khỏi danh sách bảo vệ
JSValue UI::RemoveFromProctectedProcessList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 1) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();

    process_manager_.RemoveProtectedProcess(path); // Xóa tiến trình khỏi ProcessManager

    return JSValue("Process removed successfully");
}

// Hàm lấy danh sách tiến trình trong danh sách trắng và trả về cho JavaScript
JSValue UI::GetProcessWhiteList(const JSObject& thisObject, const JSArgs& args) {
    JSArray result;

    // Lấy danh sách tiến trình trong danh sách trắng từ ProcessManager
    const auto& whitelistProcessList = process_manager_.GetWhitelistProcessList();
    for (const auto& it : whitelistProcessList) {
        size_t hash = it.first;
        ProcessInfo process = it.second;
        JSObject obj;
        obj["path"] = JSString(String16(process.Path.c_str(), process.Path.size()));
        obj["isEnable"] = JSValue(process.isEnable);
        result.push(JSValue(obj));
    }

    return JSValue(result);
}

// Thêm một tiến trình vào danh sách trắng
JSValue UI::AddToProcessWhiteList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 2) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();

    ProcessInfo process = { path, true };
    process_manager_.AddProcessToWhitelist(process); // Thêm tiến trình vào danh sách trắng trong ProcessManager

    return JSValue("Process added to whitelist successfully");
}

// Xóa một tiến trình khỏi danh sách trắng
JSValue UI::RemoveFromProcessWhiteList(const JSObject& thisObject, const JSArgs& args) {
    if (args.size() != 1) {
        return JSValue("Invalid arguments");
    }

    std::wstring path = String(args[0].ToString()).utf16().data();

    process_manager_.RemoveProcessFromWhitelist(path); // Xóa tiến trình khỏi danh sách trắng trong ProcessManager

    return JSValue("Process removed from whitelist successfully");
}

// Hàm tiện ích để chuyển đổi std::wstring sang JSString
JSString ConvertToJSString(const std::wstring& str) {
    return JSString(String16(str.c_str(), str.size()));
}

// Hàm mở hộp thoại chọn thư mục
JSValue UI::GetFolderName(const JSObject& thisObject, const JSArgs& args) {
    BROWSEINFOW bi = { 0 };
    bi.lpszTitle = L"Select a folder";
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi); // Mở hộp thoại chọn thư mục

    if (pidl != 0) {
        WCHAR path[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, path)) { // Lấy đường dẫn thư mục đã chọn
            CoTaskMemFree(pidl); // Giải phóng bộ nhớ đã sử dụng
            return JSValue(ConvertToJSString(path)); // Trả về đường dẫn thư mục dưới dạng JSString
        }
    }
    return JSValue(); // Trả về giá trị mặc định nếu không chọn được thư mục
}

// Hàm mở hộp thoại chọn file
JSValue UI::GetFileName(const JSObject& thisObject, const JSArgs& args) {
    OPENFILENAMEW ofn;       // Cấu trúc cho hộp thoại mở file
    WCHAR fileName[MAX_PATH] = L""; // Bộ đệm cho tên file

    ZeroMemory(&ofn, sizeof(ofn)); // Khởi tạo bộ nhớ cho cấu trúc ofn
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Nếu muốn chỉ định một cửa sổ cha, hãy thay thế NULL bằng handle của cửa sổ cha
    ofn.lpstrFile = fileName; // Đặt bộ đệm cho tên file
    ofn.nMaxFile = MAX_PATH; // Kích thước tối đa của tên file
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0"; // Bộ lọc cho các loại file
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // Chỉ cho phép chọn file tồn tại

    if (GetOpenFileNameW(&ofn)) { // Mở hộp thoại chọn file
        return JSValue(ConvertToJSString(fileName)); // Trả về đường dẫn file đã chọn dưới dạng JSString
    }
    return JSValue(); // Trả về giá trị mặc định nếu không chọn được file
}

// Hàm lấy danh sách tiến trình đang chạy
JSValue UI::GetProcessList(const JSObject& thisObject, const JSArgs& args) {
    JSArray result;
    HANDLE hProcessSnap;
    PROCESSENTRY32W pe64;
    pe64.dwSize = sizeof(PROCESSENTRY32W);

    // Lấy danh sách tiến trình
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return JSValue(); // Trả về rỗng nếu không thể lấy danh sách tiến trình
    }

    std::set<size_t> s;

    if (Process32FirstW(hProcessSnap, &pe64)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pe64.th32ProcessID);
            if (hProcess) {
                WCHAR processPath[MAX_PATH] = { 0 };
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, processPath, &size)) {
                    JSObject process;
                    process["shortName"] = ConvertToJSString(pe64.szExeFile); // Tên ngắn
                    process["fullPath"] = ConvertToJSString(processPath); // Đường dẫn đầy đủ
					size_t processHash = hash<std::wstring>{}(processPath);
                    if (s.find(processHash) == s.end())
                    {
						s.insert(processHash);
                        result.push(JSValue(process)); // Thêm từng tiến trình vào kết quả
                    }
                }
                CloseHandle(hProcess);
            }
        } while (Process32NextW(hProcessSnap, &pe64));
    }
    CloseHandle(hProcessSnap);

    return JSValue(result);
}


// Hàm mở hộp thoại chọn process từ danh sách các tiến trình
JSValue UI::GetProcessName(const JSObject& thisObject, const JSArgs& args) {
    // Hiển thị một cửa sổ chọn process (có thể tự xây dựng UI riêng cho việc này nếu cần)
    // Tuy nhiên, để đơn giản trong ví dụ này, chúng ta trả về một tên giả lập.
    return JSValue(ConvertToJSString(L"DummyProcess.exe")); // Trả về tên process mẫu
}

// Hàm mở hộp thoại chọn đường dẫn file .exe
JSValue UI::GetExePath(const JSObject& thisObject, const JSArgs& args) {
    OPENFILENAMEW ofn;
    WCHAR fileName[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Nếu muốn chỉ định một cửa sổ cha, hãy thay thế NULL bằng handle của cửa sổ cha
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Executable Files\0*.exe\0"; // Chỉ cho phép chọn file .exe
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) { // Mở hộp thoại chọn file
        return JSValue(ConvertToJSString(fileName)); // Trả về đường dẫn file đã chọn dưới dạng JSString
    }

    return JSValue(); // Trả về giá trị mặc định nếu không chọn được file
}


void UI::OnChangeURL(ultralight::View* caller, const String& url) {
    RefPtr<JSContext> context = caller->LockJSContext();
    SetJSContext(context->ctx());

    JSObject global = JSGlobalObject();

    global["LoadPage"] = BindJSCallbackWithRetval(&UI::LoadPage);

    global["GetHiddenList"] = BindJSCallbackWithRetval(&UI::GetHiddenList);
    global["GetProtectedList"] = BindJSCallbackWithRetval(&UI::GetProtectedList);
    global["UpdateHiddenStatus"] = BindJSCallbackWithRetval(&UI::UpdateHiddenStatus);
    global["UpdateProtectedStatus"] = BindJSCallbackWithRetval(&UI::UpdateProtectedStatus);
    global["RemoveFromHiddenList"] = BindJSCallbackWithRetval(&UI::RemoveFromHiddenList);
    global["RemoveFromProtectedList"] = BindJSCallbackWithRetval(&UI::RemoveFromProtectedList);
    global["AddToHiddenList"] = BindJSCallbackWithRetval(&UI::AddToHiddenList);
    global["AddToProtectedList"] = BindJSCallbackWithRetval(&UI::AddToProtectedList);

    global["GetFeatureStatus"] = BindJSCallbackWithRetval(&UI::GetFeatureStatus);
    global["UpdateFeatureStatus"] = BindJSCallbackWithRetval(&UI::UpdateFeatureStatus);

    global["GetProctectedProcessList"] = BindJSCallbackWithRetval(&UI::GetProctectedProcessList);
    global["UpdateProtectedProcessStatus"] = BindJSCallbackWithRetval(&UI::UpdateProtectedProcessStatus);
    global["AddToProctectedProcessList"] = BindJSCallbackWithRetval(&UI::AddToProctectedProcessList);
    global["RemoveFromProctectedProcessList"] = BindJSCallbackWithRetval(&UI::RemoveFromProctectedProcessList);

    global["GetProcessWhiteList"] = BindJSCallbackWithRetval(&UI::GetProcessWhiteList);
    global["AddToProcessWhiteList"] = BindJSCallbackWithRetval(&UI::AddToProcessWhiteList);
    global["RemoveFromProcessWhiteList"] = BindJSCallbackWithRetval(&UI::RemoveFromProcessWhiteList);

	global["GetFolderName"] = BindJSCallbackWithRetval(&UI::GetFolderName);
	global["GetFileName"] = BindJSCallbackWithRetval(&UI::GetFileName);

	global["GetProcessList"] = BindJSCallbackWithRetval(&UI::GetProcessList);
	global["GetProcessName"] = BindJSCallbackWithRetval(&UI::GetProcessName);
	global["GetExePath"] = BindJSCallbackWithRetval(&UI::GetExePath);
}
