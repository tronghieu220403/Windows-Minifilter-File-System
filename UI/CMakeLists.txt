project(FileSecure C CXX)
cmake_minimum_required(VERSION 3.3.2)

include(cmake/App.cmake)

set(SOURCES "src/FileSecureApp.h"
            "src/FileSecureApp.cpp"
            "src/UI.h"
            "src/UI.cpp"
            "src/ulti.h"
            "src/main.cpp"
            "src/FileManager.h"
            "src/FileManager.cpp"
            "src/DriverComm.h"
            "src/DriverComm.cpp"
            "src/ProcessManager.h"
            "src/ProcessManager.cpp"
)

add_app("${SOURCES}")