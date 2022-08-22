project(cloud_game_rtc)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)

#SET(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS         "-std=c++11 -g    -fno-rtti  ")  #注意调试的话,此行代码一定要添加!
add_definitions(-D WEBRTC_POSIX) # -fPIC
add_definitions(-D WEBRTC_LINUX)

#add_definitions(-D TEST_APP_CAPTUER)

SET(WEBRTC_DIR /home/chensong/dep/webrtc)
#if (WIN32 OR WIN64)
#    set(HOOK_DIR "windows")
#elseif (APPLE)
#    set(HOOK_DIR "darwin")
#else()
#    set(HOOK_DIR "x11")
#endif()




INCLUDE_DIRECTORIES( 
          /usr/local/include

          ${WEBRTC_DIR}/include
          ${WEBRTC_DIR}/include/third_party/libyuv/include
          ${WEBRTC_DIR}/include/third_party/abseil-cpp
          ${WEBRTC_DIR}/include/third_party/jsoncpp/source/include 
          ${WEBRTC_DIR}/include/third_party/jsoncpp/generated
          #${WEBRTC_DIR}/include/third_party/googletest/src/googletest/include
         ${WEBRTC_DIR}/gen
#        ${PROJECT_SOURCE_DIR}/client/input_device
        ${PROJECT_SOURCE_DIR}/client/rtc
        ${PROJECT_SOURCE_DIR}/client
#        ${PROJECT_SOURCE_DIR}/client/input_device
#        ${PROJECT_SOURCE_DIR}/client/input_device/${HOOK_DIR}
          )


        
        
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

LINK_DIRECTORIES(
       /usr/local/lib 
       /usr/local/lib64
        /usr/lib/x86_64-linux-gnu
        ${PROJECT_SOURCE_DIR}/linux/lib
       ${WEBRTC_DIR}/lib/
       )
add_subdirectory(hook)
add_subdirectory(client/linux_capture)
# add_subdirectory(client/input_device)
 add_subdirectory(client)




 

 
 

