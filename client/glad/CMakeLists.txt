#project(glad)
#
#find_package(OpenGL REQUIRED)
#
#add_library(glad SHARED)
#add_library(OBS::obsglad ALIAS glad)
#
#target_sources(glad PRIVATE src/glad.c include/glad/glad.h)
#
#target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
#
#target_compile_definitions(glad PRIVATE GLAD_GLAPI_EXPORT_BUILD)
#
#target_link_libraries(glad PUBLIC OpenGL::GL)
#
#set_target_properties(
#  glad
#  PROPERTIES OUTPUT_NAME obsglad
#             FOLDER "deps"
#             VERSION "${OBS_VERSION_MAJOR}"
#             SOVERSION "1")
#
#if(OS_WINDOWS)
#  set(MODULE_DESCRIPTION "Glad OpenGL Loading Library")
#
#  configure_file(${CMAKE_SOURCE_DIR}/cmake/bundle/windows/obs-module.rc.in
#                 obsglad.rc)
#
#  target_sources(glad PRIVATE src/glad_wgl.c include/glad/glad_wgl.h obsglad.rc)
#
#elseif(OS_POSIX AND NOT OS_MACOS)
#  find_package(OpenGL REQUIRED)
#
#  if(TARGET OpenGL::EGL)
#    target_sources(glad PRIVATE src/glad_egl.c include/EGL/eglplatform.h
#                                include/glad/glad_egl.h)
#
#    target_link_libraries(glad PRIVATE OpenGL::EGL)
#  endif()
#
#endif()
#
#if(NOT MSVC)
#  target_compile_options(glad PRIVATE -fvisibility=hidden)
#endif()
#
#setup_binary_target(glad)



#project(crobot)
project(glad)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")

# RTC
#SET(RTC_INC_DIR /home/chensong/Work/ccloud_game_rtc)
# /client/rtc

set(CMAKE_CXX_FLAGS         "-std=c++11   -fno-rtti   ")  #注意调试的话,此行代码一定要添加!-fno-rtti
#add_definitions(-D WEBRTC_POSIX) # -fPIC
#add_definitions(-D WEBRTC_LINUX)
#


#set(CMAKE_CXX_FLAGS         "-std=c++11 -g    -fno-rtti  ")  #注意调试的话,此行代码一定要添加!
#add_definitions(-D WEBRTC_POSIX) # -fPIC
#add_definitions(-D WEBRTC_LINUX)


#SET(WEBRTC_DIR /home/chensong/dep/webrtc)

INCLUDE_DIRECTORIES(
        /usr/local/include
	./include
       #${WEBRTC_DIR}/include
       #${WEBRTC_DIR}/include/third_party/libyuv/include
       #${WEBRTC_DIR}/include/third_party/abseil-cpp
       #${WEBRTC_DIR}/include/third_party/jsoncpp/source/include
       #${WEBRTC_DIR}/include/third_party/jsoncpp/generated
       ##${WEBRTC_DIR}/include/third_party/googletest/src/googletest/include
       #${WEBRTC_DIR}/gen
      #  ${RTC_INC_DIR}/client/rtc
       # ../ccloud_game_rtc/client/rtc
       # ../ccloud_game_rtc/client/sdp
       # ../ccloud_game_rtc/client
       # ../Robot

)


#LINK_DIRECTORIES(
#        /usr/local/lib
#        /usr/local/lib64
#        /usr/lib/x86_64-linux-gnu
#        ${PROJECT_SOURCE_DIR}/linux/lib
#
#)

#INCLUDE_DIRECTORIES(
#        ${RTC_INC_DIR}/client/rtc)
LINK_DIRECTORIES(
        #../ccloud_game_rtc/client
/usr/local/lib
         /usr/local/lib64
         /usr/lib/x86_64-linux-gnu
      #  ${RTC_INC_DIR}/linux/lib
     #   ${WEBRTC_DIR}/lib/
)

file(GLOB ALL_CXX_SRCS
         ./src/glad.c
		 ./src/glad_egl.c
	./src/main.c

        )

#add_library(${PROJECT_NAME}      ${ALL_CXX_SRCS}   ) #SHARED  STATIC
add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})
#add_dependencies(${PROJECT_NAME} common)
#add_dependencies(${PROJECT_NAME} clib)

set(ALL_LIB  	pthread
#        libcommon.a
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
        #################
        pthread
#        libclib.a

#        pthread
        libprotobuf-lite.so
     # libprotobuf-lite.a
#        /home/chensong/Work/lib/protobuf-3.7.0/build/libprotobuf-lite.a
#        libthrift.a
#        libmysqlclient.so
#        libjsoncpp.a
        libboost_locale.a
        libboost_regex.a
        libboost_system.so
         libboost_system.a
        libboost_filesystem.a

#        librtc.so
#        pthread
#        Xtst
#        Xft
#        xkbcommon
#        xkbcommon-x11
#        m
#        xkbfile
#        X11
#        rt
#        X11
#        pthread
#        atomic
#        Xcomposite
#        Xext
#        Xrender
#        dl
#        pthread
#        rt
#        pthread
#        m
#        Xdamage
#        Xfixes
#        gio-2.0
#        gobject-2.0
#        glib-2.0
#        GL
#        pthread
#        libwebrtc.a
#        X11
#        Xtst
#        -lX11-xcb
#        -lxcb
#        -lxkbfile
#        -lXt
#        -lX11
#        -lXtst
#        -lX11-xcb
#        -lxcb
#        -lxkbfile
#        -lXt
#        Xinerama
#        xcb-composite


##############################################

        )


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
