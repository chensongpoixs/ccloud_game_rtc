project(cmediasoup_push)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS( -g -Wall  )
#SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


ADD_DEFINITIONS(-std=c++11 )


INCLUDE_DIRECTORIES( 
			./
			./sdp
			/Users/mac/Work/webrtc_google/webrtc/src
			/Users/mac/Work/webrtc_google/webrtc/src/third_party/abseil-cpp
		)
add_definitions(-DWEBRTC_MAC)
file(GLOB ALL_C_SRCS
        ./*.cpp
        ./*.h
        ./*.hpp
        ./sdp/*.h
        ./sdp/*.hpp
	./*.cc
	./*.c
	./sdp/*.cpp
	./sdp/*.cc
	./sdp/*.c
        )
LINK_DIRECTORIES(/Users/mac/Work/webrtc_google/webrtc/src/out/mac_x64_debug/obj)
add_executable(${PROJECT_NAME} ${ALL_C_SRCS})
target_link_libraries(${PROJECT_NAME} libwebrtc.a)
