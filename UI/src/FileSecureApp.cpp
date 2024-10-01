#include "FileSecureApp.h"

FileSecureApp::FileSecureApp() {
	app_ = App::Create();
	window_ = Window::Create(app_->main_monitor(), 1024, 500, false, kWindowFlags_Titled);
	window_->SetTitle("File Secure");
	// Create the UI
	ui_.reset(new UI(window_));
	window_->set_listener(ui_.get());
}

FileSecureApp::~FileSecureApp() {
	window_->set_listener(nullptr);
	ui_.reset();
	window_ = nullptr;
	app_ = nullptr;
}

void FileSecureApp::Run() {
    app_->Run();
}
