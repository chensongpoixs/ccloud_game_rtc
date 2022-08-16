project(x11_demo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS(-pipe -O2 -pedantic -Wall -W -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Winline -Wdisabled-optimization -Wno-missing-field-initializers -g    -std=c99    -c  )

#SET(EXTRA_INC_DIR /home/ext)

INCLUDE_DIRECTORIES(
        /usr/local/include
        #					${EXTRA_INC_DIR}/include
        #					${EXTRA_INC_DIR}/include/mysql
        #					${PROJECT_SOURCE_DIR}/clib
        #					${PROJECT_SOURCE_DIR}/common
        #					${PROJECT_SOURCE_DIR}/common/cmsg
        #					${PROJECT_SOURCE_DIR}/common/cmessage
        #					#${PROJECT_SOURCE_DIR}/
        ../..
)




#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/lib )
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

LINK_DIRECTORIES(
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib64
        #/usr/lib64/mysql/
        #/usr/lib/x86_64-linux-gnu
        #${PROJECT_SOURCE_DIR}/linux/lib
)

file(GLOB ALL_CXX_SRCS
        ./*.cpp  ./*.cc
        ./*.c
        )

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})

set(ALL_LIB  	pthread
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
        #libwebrtc.a
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
        )


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})

#add_subdirectory(clib)
#add_subdirectory(common)
#add_subdirectory(gateway)
#add_subdirectory(Robot)
