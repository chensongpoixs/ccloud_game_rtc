project(rtc)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
 
 

file(GLOB ALL_CXX_SRCS
        ./input_device/*.h
        ./input_device/*.c
        ./*.h
        ./*.c
                       ./*.cpp 
                        ./*.cc

        ./rtc/*.cc
        ./rtc/*.h
        ./rtc/*.cpp
        ./rtc/*.hpp
        ./sdp/*.hpp
        ./sdp/*.hpp
        ./sdp/*.cpp

)

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})
add_dependencies(${PROJECT_NAME} input_device)

set(ALL_LIB   input_device
pthread
Xtst
        Xft
        xkbcommon
        #xtst 
        #xinerama 
        Xinerama
        xkbcommon
        m 
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
       ## glib-2.35.0
       glib-2.0
        GL
        pthread
        libwebrtc.a
        pthread
        #libcommon.a
        #libclib.a
        #libprotobuf-lite.a
        #libthrift.a
        #libmysqlclient.so
        #libjsoncpp.a
        #libboost_locale.a
        #libboost_regex.a
        #libboost_system.a
        #libboost_filesystem.a
        #-lX11 -lXcomposite -lXext -lXrender -latomic -ldl -lpthread -lrt -lm -lXdamage -lXfixes -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lGL
        )


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
 

