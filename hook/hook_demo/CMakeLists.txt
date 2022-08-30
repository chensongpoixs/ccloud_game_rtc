project(hook_demo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")



file(GLOB ALL_C_SRCS
        #        ./input_device/*.h
        #        ./input_device/*.c
        ./prehook.c
        ./prehook.h

        )


file(GLOB ALL_SO_SRCS
        #        ./input_device/*.h
        #        ./input_device/*.c
        ./test.c


        )

# CXXFLAGS="-O3 -fPIC"
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

LINK_DIRECTORIES(
        /usr/local/lib
        /usr/local/lib64
        /usr/lib/x86_64-linux-gnu
        ${PROJECT_SOURCE_DIR}/linux/lib

)
#############################################################################
#

#############################################################################
add_library(${PROJECT_NAME}  SHARED   ${ALL_C_SRCS}   ) #SHARED  STATIC
add_library(test  SHARED   ${ALL_SO_SRCS}   )


#add_executable(${PROJECT_NAME}    ${ALL_CXX_SRCS})
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
#        libwebrtc.a
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
        -lhook_demo
        )


target_link_libraries(${PROJECT_NAME} )



target_link_libraries(test   ${ALL_LIB} )
