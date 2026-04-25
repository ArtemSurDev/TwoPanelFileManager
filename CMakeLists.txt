cmake_minimum_required(VERSION 3.10)

project(TwoPanelFileManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(file_manager
        src/main.cpp
        src/MainWindow.cpp
        src/FilePanel.cpp
        src/FileOperations.cpp

        include/MainWindow.h
        include/FilePanel.h
        include/FileOperations.h
)

target_include_directories(file_manager PRIVATE include)

target_link_libraries(file_manager Qt6::Widgets)