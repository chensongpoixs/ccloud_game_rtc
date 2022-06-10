project(Server)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS(-std=c++11 -g -Wall )

SET(EXTRA_INC_DIR /home/ext)

INCLUDE_DIRECTORIES( 
					/usr/local/include
					${EXTRA_INC_DIR}/include
					${PROJECT_SOURCE_DIR}/rtc
					)


				
				
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/lib )
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

LINK_DIRECTORIES(
				/usr/local/lib 
				/usr/local/lib64
				${PROJECT_SOURCE_DIR}/linux/lib
				)
				
				
add_subdirectory(rtc)
