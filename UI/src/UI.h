#pragma once
#ifndef UI_H
#define UI_H
#include <map>
#include <memory>
#include <string>
#include <set>
#include <windows.h> 
#include <shlobj.h> 
#include <commdlg.h> 
#include <tlhelp32.h> 
#include <AppCore/AppCore.h>
#include "FileManager.h"
#include "ProcessManager.h"



using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

class UI : public WindowListener,
    public LoadListener,
    public ViewListener {
public:
    UI(RefPtr<Window> window);
    ~UI();

    // Inherited from WindowListener
    virtual void OnClose(ultralight::Window* window) override;

    // Inherited from ViewListener
    virtual void OnChangeURL(ultralight::View* caller, const String& url) override;

    // Called by UI JavaScript
    JSValue LoadPage(const JSObject& thisObject, const JSArgs& args);

	JSValue UpdateFeatureStatus(const JSObject& thisObject, const JSArgs& args);
    JSValue GetFeatureStatus(const JSObject& thisObject, const JSArgs& args);

    JSValue GetHiddenList(const JSObject& thisObject, const JSArgs& args);
    JSValue GetProtectedList(const JSObject& thisObject, const JSArgs& args);
    JSValue UpdateHiddenStatus(const JSObject& thisObject, const JSArgs& args);
    JSValue UpdateProtectedStatus(const JSObject& thisObject, const JSArgs& args);
    JSValue RemoveFromHiddenList(const JSObject& thisObject, const JSArgs& args);
    JSValue RemoveFromProtectedList(const JSObject& thisObject, const JSArgs& args);
    JSValue AddToHiddenList(const JSObject& thisObject, const JSArgs& args);
    JSValue AddToProtectedList(const JSObject& thisObject, const JSArgs& args);

	JSValue GetProctectedProcessList(const JSObject& thisObject, const JSArgs& args);
	JSValue UpdateProtectedProcessStatus(const JSObject& thisObject, const JSArgs& args);
	JSValue AddToProctectedProcessList(const JSObject& thisObject, const JSArgs& args);
	JSValue RemoveFromProctectedProcessList(const JSObject& thisObject, const JSArgs& args);

	JSValue GetProcessWhiteList(const JSObject& thisObject, const JSArgs& args);
	JSValue AddToProcessWhiteList(const JSObject& thisObject, const JSArgs& args);
	JSValue RemoveFromProcessWhiteList(const JSObject& thisObject, const JSArgs& args);

    JSValue GetFolderName(const JSObject& thisObject, const JSArgs& args);
	JSValue GetFileName(const JSObject& thisObject, const JSArgs& args);

	JSValue GetProcessList(const JSObject& thisObject, const JSArgs& args);
	JSValue GetProcessName(const JSObject& thisObject, const JSArgs& args);
	JSValue GetExePath(const JSObject& thisObject, const JSArgs& args);

    RefPtr<Window> window() { return window_; }
    RefPtr<View> view() { return overlay_->view(); }

protected:

    RefPtr<Window> window_;
    RefPtr<Overlay> overlay_;
	FileManager file_manager_;
	ProcessManager process_manager_;
};

#endif // UI_H