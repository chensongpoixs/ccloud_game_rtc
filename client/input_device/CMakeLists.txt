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
set(USE_XTEST ON)
# generic input driver
set(USE_EVDEV ON)
##############################

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