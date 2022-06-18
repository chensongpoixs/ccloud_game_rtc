project(Server)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_COMPILER "/home/chensong/Work/webrtc/src/third_party/llvm-build/Release+Asserts/bin/clang")

set(CMAKE_CXX_COMPILER "/home/chensong/Work/webrtc/src/third_party/llvm-build/Release+Asserts/bin/clang++")
SET(CMAKE_VERBOSE_MAKEFILE ON) 
set(CMAKE_CXX_FLAGS         "-std=c++14 -g -fpermissive -fno-rtti -nostdinc++")  #注意调试的话,此行代码一定要添加!
#SET(CMAKE_C_COMPILER /usr/bin/gcc)
#SET(CMAKE_CXX_COMPILER /usr/bin/g++)

Link_directories(  /home/chensong/Work/webrtc/src/out/Default/obj/buildtools/third_party/libc++/libc++)
Link_directories(  /home/chensong/Work/webrtc/src/out/Default/obj/buildtools/third_party/libc++abi/libc++abi)

SET(EXTRA_INC_DIR /home/ext)
SET(WEBRTC_DIR /home/chensong/Work/webrtc/src)
include_directories(${WEBRTC_DIR}/buildtools/third_party/libc++/trunk/include)
  include_directories(${WEBRTC_DIR}/third_party/llvm-build/Release+Asserts/lib/clang/6.0.0/include)
INCLUDE_DIRECTORIES( 
					/usr/local/include
					${EXTRA_INC_DIR}/include
					${PROJECT_SOURCE_DIR}/rtc
					${WEBRTC_DIR}
					${WEBRTC_DIR}/third_party/libyuv/include
					${WEBRTC_DIR}/third_party/abseil-cpp
					${WEBRTC_DIR}/third_party/jsoncpp/source/include 
					${WEBRTC_DIR}/third_party/jsoncpp/generated
					#${WEBRTC_DIR}/third_party/googletest/src/googletest/include
					${WEBRTC_DIR}/out/Release/gen
					)


				
				
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/lib )
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

#LINK_DIRECTORIES(
#				/usr/local/lib 
#				/usr/local/lib64
#				#/home/chensong/Work/webrtc/src/out/Release/obj
#				/home/chensong/Work/webrtc/src/out/Default/obj
#				${PROJECT_SOURCE_DIR}/linux/lib
#				)
	
link_libraries(
    -lwebrtc
#/home/chensong/Work/webrtc/src/out/Default/obj/libwebrtc.a
    -lpthread
        -lc++
        -lc++abi
)
add_subdirectory(webrtc_demo)			
				
#add_subdirectory(rtc)

