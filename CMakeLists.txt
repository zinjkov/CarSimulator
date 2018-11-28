cmake_minimum_required(VERSION 2.8)

project(CarSimulator)

set(CMAKE_CXX_STANDARD 11)

set (ENV{URHO3D_HOME} C:/libs/Urho3D-1.7/build_gl64)
set (CMAKE_PREFIX_PATH C:/Qt/5.9.1/msvc2015_64; D:/libs/opencv-3.4.2/release/install)
#Qt
#set(CMAKE_BUILD_TYPE "Release")
CMAKE_POLICY(SET CMP0002 OLD)
## Automatically link Qt executables to qtmain target on Windows - don't need it.
cmake_policy (SET CMP0020 NEW)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#Qt packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)


set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
set (URHO3D_PREFIX_PATH ${CMAKE_SOURCE_DIR}/bin/CoreData)

include (UrhoCommon)

#Src
set(SOURCES
        mainwindow.hpp
        mainwindow.cpp
        keycontroller.hpp
        keycontroller.cpp
        urho3dbase.hpp
        urho3dbase.cpp
        utils.cpp
        utils.hpp
        Vehicle.cpp
        Vehicle.h
        )

set (TARGET_NAME CarSimulator)

define_source_files ()
setup_main_executable()

include_directories(D:/libs/msvc/ZeroMQ/include)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(CarSimulator ${SOURCES} ${CONN_SOURCES})
target_link_libraries(CarSimulator ${OpenCV_LIBS} Qt5::Core Qt5::Widgets D:/libs/msvc/ZeroMQ/lib/libzmq-v120-mt-4_0_4.lib)# ${ZeroMQ_LIBRARY})
