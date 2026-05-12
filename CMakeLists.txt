cmake_minimum_required(VERSION 3.16)
project(file_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(SOURCES
        src/main.cpp
        src/MainWindow.cpp
        src/FilePanel.cpp
        src/FileOperationsFacade.cpp
        src/FileCommand.cpp
        src/PanelMediator.cpp
)

set(HEADERS
        include/MainWindow.h
        include/FilePanel.h
        include/FileOperationsFacade.h
        include/FileCommand.h
        include/PanelMediator.h
)

add_executable(file_manager ${SOURCES} ${HEADERS})

target_include_directories(file_manager PRIVATE include)

target_link_libraries(file_manager Qt6::Core Qt6::Widgets Qt6::Gui)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(file_manager_tests tests/FileOperationsFacadeTest.cpp src/FileOperationsFacade.cpp)
target_include_directories(file_manager_tests PRIVATE include)
target_link_libraries(file_manager_tests Qt6::Core GTest::gtest)
add_test(NAME file_manager_tests COMMAND file_manager_tests)
