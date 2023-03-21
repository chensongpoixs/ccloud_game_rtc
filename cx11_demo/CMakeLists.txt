project(cx11_demo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
#ADD_DEFINITIONS(-pipe -O2 -pedantic -Wall -W -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Winline -Wdisabled-optimization -Wno-missing-field-initializers -g    -std=c99    -c  )

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
        ${PROJECT_SOURCE_DIR}
)




#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/old_lib )
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

LINK_DIRECTORIES(
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib64
        #/usr/lib64/mysql/
        #/usr/old_lib/x86_64-linux-gnu
        #${PROJECT_SOURCE_DIR}/linux/old_lib
)



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
       # libwebrtc.a
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


#/usr/old_lib/x86_64-linux-gnu/libX11.so
#/usr/old_lib/x86_64-linux-gnu/libxcb.so
#/usr/old_lib/x86_64-linux-gnu/libxcb-xfixes.so
#/usr/old_lib/x86_64-linux-gnu/libxcb-randr.so
 #/usr/old_lib/x86_64-linux-gnu/libxcb-shm.so
 #/usr/old_lib/x86_64-linux-gnu/libxcb-xinerama.so
 #/usr/old_lib/x86_64-linux-gnu/libxcb-composite.so
 -lpthread  
 #/usr/old_lib/x86_64-linux-gnu/libGLX.so
 #/usr/old_lib/x86_64-linux-gnu/libOpenGL.so
  


        )

file(GLOB ALL_CXX_SRCS
        main.cpp
        )

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})
target_link_libraries(${PROJECT_NAME} ${ALL_LIB})

#################

#file(GLOB ALL_WINDOWS
#        main.cpp
#        )
#
#add_executable(main ${ALL_WINDOWS})
#target_link_libraries(main ${ALL_LIB})



#
#file(GLOB ALL_WINDOWS_X11
#        x11_get_window_id.cpp
#        )
#
#add_executable(x11_get_window_id ${ALL_WINDOWS_X11})
#target_link_libraries(x11_get_window_id ${ALL_LIB})
#
#
#
#
#
file(GLOB  xsendevent
        test_xsendevent.c
        )

add_executable(test_xsendevent ${xsendevent})
target_link_libraries(test_xsendevent ${ALL_LIB})

#add_subdirectory(clib)
#add_subdirectory(common)
#add_subdirectory(gateway)
#add_subdirectory(Robot)
