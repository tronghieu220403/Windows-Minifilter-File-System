#ifndef FILESECUREAPP_H
#define FILESECUREAPP_H

#include <vector>
#include <string>
#include <Ultralight/Ultralight.h>
#include "AppCore/AppCore.h"
#include "FileManager.h"
#include "UI.h"

using namespace ultralight;

class FileSecureApp : public WindowListener, public LoadListener {
public:
    FileSecureApp();
    ~FileSecureApp();

    void Run();

private:
    RefPtr<App> app_;
    RefPtr<Window> window_;
    std::unique_ptr<UI> ui_;
};

#endif // FILESECUREAPP_H
