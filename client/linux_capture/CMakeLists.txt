project(linux_capture)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")



file(GLOB ALL_C_SRCS
        ./xcomposite.c
        )

# CXXFLAGS="-O3 -fPIC"

#############################################################################
#

#############################################################################
#add_library(${PROJECT_NAME}  SHARED   ${ALL_CXX_SRCS}   ) #SHARED  STATIC
add_executable(${PROJECT_NAME}    ${ALL_C_SRCS})
#add_dependencies(${PROJECT_NAME} input_device)
#input_device
set(ALL_LIB
        pthread
        Xtst
        Xft
        xkbcommon
        xkbcommon-x11
        m
        xkbfile
        X11
        rt
        X11
        pthread
        atomic
        Xcomposite
        Xext
        Xrender
        dl
        pthread
        rt
        pthread
        m
        Xdamage
        Xfixes
        gio-2.0
        gobject-2.0
        glib-2.0
        GL
        pthread
        libwebrtc.a
        X11
        Xtst
        -lX11-xcb
        -lxcb
        -lxkbfile
        -lXt
        -lX11
        -lXtst
        -lX11-xcb
        -lxcb
        -lxkbfile
        -lXt
        Xinerama
        xcb-composite
        libinput_device.a
        )


target_link_libraries(${PROJECT_NAME}  ${ALL_LIB})


file(GLOB ALL_CXX_SRCS
        ./*.cpp
        )


add_executable(xcompcap   ${ALL_CXX_SRCS})
target_link_libraries(xcompcap ${ALL_LIB})


