project(rtc)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS( -g -Wall  )
#SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)




INCLUDE_DIRECTORIES( 
			./*.h
			./*.hpp
			./sdp/*.h
			./sdp/*.hpp
		)

file(GLOB ALL_C_SRCS
        ./*.cpp
	./*.cc
	./*.c
	./sdp/*.cpp
	./sdp/*.cc
	./sdp/*.c
        )
#LINK_DIRECTORIES()
add_executable(${PROJECT_NAME} ${ALL_C_SRCS})
target_link_libraries(${PROJECT_NAME})
