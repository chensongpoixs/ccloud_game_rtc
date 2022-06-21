project(input_device)
#cmake_minimum_required(VERSION 3.10)
MESSAGE(STATUS "#########################")

if (WIN32 OR WIN64)
    set(INPUT_DEVICE_SOURCE_DIR "windows")
elseif (APPLE)
    set(INPUT_DEVICE_SOURCE_DIR "darwin")
else()
    set(INPUT_DEVICE_SOURCE_DIR "x11")
endif()
########################################################################
set(BUILD_SHARED_LIBS ON)
set(BUILD_DEMO ON)
#set(USE_XTEST ON)
# generic input driver
#set(USE_EVDEV ON)

if(UNIX AND NOT APPLE)
#    find_package(PkgConfig REQUIRED)

#    pkg_check_modules(X11 REQUIRED x11)
#    target_include_directories(${PROJECT_NAME} PRIVATE "${X11_INCLUDE_DIRS}")
#    target_link_libraries(${PROJECT_NAME} "${X11_LDFLAGS}")
#
#    pkg_check_modules(XTST REQUIRED xtst)
#    target_include_directories(${PROJECT_NAME} PRIVATE "${XTST_INCLUDE_DIRS}")
#    target_link_libraries(${PROJECT_NAME} "${XTST_LDFLAGS}")
#
#    include(CheckLibraryExists)
#    check_library_exists(Xtst XRecordQueryVersion "" HAVE_XRECORD)
#
#    include(CheckIncludeFile)
#    check_include_file(X11/extensions/record.h HAVE_RECORD_H "-include X11/Xlib.h")

    option(USE_XKB_COMMON "X Keyboard Common Extension (default: ON)" ON)
    if(USE_XKB_COMMON)
#        pkg_check_modules(XKB_COMMON REQUIRED xkbcommon-x11)
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XKB_COMMON)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${XKB_COMMON_INCLUDE_DIRS}")
#        target_link_libraries(${PROJECT_NAME} "${XKB_COMMON_LDFLAGS}")

#        pkg_check_modules(X11_XCB REQUIRED x11-xcb)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${X11_XCB_INCLUDE_DIRS}")
#        target_link_libraries(${PROJECT_NAME} "${X11_XCB_LDFLAGS}")
    endif()

    option(USE_XKB_FILE "X Keyboard File Extension (default: ON)" ON)
    if(USE_XKB_FILE)
#        pkg_check_modules(XKB_FILE REQUIRED xkbfile)
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XKB_FILE)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${XKB_FILE_INCLUDE_DIRS}")
#        target_link_libraries(${PROJECT_NAME} "${XKB_FILE_LDFLAGS}")
    endif()

    option(USE_XT "X Toolkit Extension (default: ON)" ON)
    if(USE_XT)
#        pkg_check_modules(XT REQUIRED xt)
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XT)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${XT_INCLUDE_DIRS}")
#        target_link_libraries(${PROJECT_NAME} "${XT_LDFLAGS}")
    endif()


    option(USE_XF86MISC "XFree86-Misc X Extension (default: OFF)" OFF)
    if(USE_XF86MISC)
#        pkg_check_modules(XF86MISC REQUIRED Xxf86misc)
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XF86MISC)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${XF86MISC_INCLUDE_DIRS}")
#        target_link_libraries(${PROJECT_NAME} "${XF86MISC_LDFLAGS}")
    endif()

#    option(USE_XRANDR "XRandR Extension (default: OFF)" OFF)
#    if(USE_XRANDR)
##        pkg_check_modules(XRANDR REQUIRED xrandr)
#        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XRANDR)
##        target_include_directories(${PROJECT_NAME} PRIVATE "${XRANDR_INCLUDE_DIRS}")
##        target_link_libraries(${PROJECT_NAME} "${XRANDR_LDFLAGS}")
#    endif()

    option(USE_XINERAMA "Xinerama Extension (default: ON)" ON)
    if(USE_XINERAMA)
#        pkg_check_modules(XINERAMA REQUIRED xinerama)
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XINERAMA)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${XINERAMA_INCLUDE_DIRS}")
#        target_link_libraries(${PROJECT_NAME} "${XINERAMA_LDFLAGS}")
    endif()

    option(USE_XRECORD_ASYNC "XRecord Asynchronous API (default: OFF)" OFF)
    if(USE_XRECORD_ASYNC)
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XRECORD_ASYNC)
    endif()

    option(USE_XTEST "XTest API (default: ON)" ON)
    if(USE_XTEST)
        # XTest API is provided by Xtst
        add_compile_definitions(${PROJECT_NAME} PRIVATE USE_XTEST)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        option(USE_EVDEV "Generic Linux input driver (default: ON)" ON)
        if(USE_EVDEV)
            add_compile_definitions(${PROJECT_NAME} PRIVATE USE_EVDEV)
        endif()
    endif()
elseif(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.5")

#    find_package(Threads REQUIRED)
#    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
#
#    find_library(CARBON Carbon REQUIRED)
#    target_include_directories(${PROJECT_NAME} PRIVATE "${CARBON}")
#    target_link_libraries(${PROJECT_NAME} "${CARBON}")

    option(USE_APPLICATION_SERVICES "ApplicationServices framework (default: ON)" ON)
    if(USE_APPLICATION_SERVICES)
#        find_library(APPLICATION_SERVICES ApplicationServices REQUIRED)
        add_compile_definitions(USE_APPLICATION_SERVICES)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${APPLICATION_SERVICES}")
#        target_link_libraries(${PROJECT_NAME} "${APPLICATION_SERVICES}")
    endif()

    option(USE_IOKIT "IOKit framework (default: ON)" ON)
    if(USE_IOKIT)
#        find_library(IOKIT IOKit REQUIRED)
        add_compile_definitions(USE_IOKIT)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${IOKIT}")
#        target_link_libraries(${PROJECT_NAME} "${IOKIT}")
    endif()

    # FIXME Change USE_OBJC flag to USE_APPKIT
    #option(USE_APPKIT "AppKit framework (default: ON)" ON)
    option(USE_OBJC "Objective-C API (default: ON)" ON)
    if(USE_OBJC)
        # FIXME Drop USE_OBJC as it is included in AppKit
#        find_library(OBJC objc REQUIRED)
        add_compile_definitions(USE_OBJC)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${OBJC}")
#        target_link_libraries(${PROJECT_NAME} "${OBJC}")

#        find_library(APPKIT AppKit REQUIRED)
#        add_compile_definitions(USE_APPKIT)
#        target_include_directories(${PROJECT_NAME} PRIVATE "${APPKIT}")
#        target_link_libraries(${PROJECT_NAME} "${APPKIT}")
    endif()

    option(USE_CARBON_LEGACY "Legacy Carbon framework functionality (default: OFF)" OFF)
    if(USE_CARBON_LEGACY)
        message(DEPRECATION "Legacy Carbon functionality has been deprecated.")
        add_compile_definitions(USE_CARBON_LEGACY)

        if(USE_CARBON_LEGACY AND CMAKE_SIZEOF_VOID_P EQUAL 8)
            message(WARNING "Legacy Carbon functionality should not be used with 64-bit targets.")
        endif()
    endif()
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} Advapi32)
endif()
#add_library(${PROJECT_NAME}
#        "logger.c"
#        "${UIOHOOK_SOURCE_DIR}/input_helper.c"
#        "${UIOHOOK_SOURCE_DIR}/input_hook.c"
#        "${UIOHOOK_SOURCE_DIR}/post_event.c"
#        "${UIOHOOK_SOURCE_DIR}/system_properties.c"
#        )

file(GLOB ALL_C_SRCS
        ./*.c
        ./${INPUT_DEVICE_SOURCE_DIR}/*.c
        )

add_library(${PROJECT_NAME} STATIC ${ALL_C_SRCS})
target_link_libraries(${PROJECT_NAME} )