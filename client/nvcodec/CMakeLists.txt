#project(crobot)
project(OpenGLEncDemo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")

# RTC
SET(RTC_INC_DIR /home/chensong/Work/ccloud_game_rtc)
# /client/rtc

set(CMAKE_CXX_FLAGS         "-std=c++11   -g -Wall   ")  #注意调试的话,此行代码一定要添加!-fno-rtti
#add_definitions(-D WEBRTC_POSIX) # -fPIC
#add_definitions(-D WEBRTC_LINUX)
#


#set(CMAKE_CXX_FLAGS         "-std=c++11 -g    -fno-rtti  ")  #注意调试的话,此行代码一定要添加!
#add_definitions(-D WEBRTC_POSIX) # -fPIC
#add_definitions(-D WEBRTC_LINUX)


SET(WEBRTC_DIR /home/chensong/dep/webrtc)

INCLUDE_DIRECTORIES( 
		/usr/include 
		/usr/local/include 
		./

)


 
LINK_DIRECTORIES(
        /usr/local/cuda/lib64/stubs
		/usr/lib 
		/usr/local/lib 
)

file(GLOB ALL_CXX_SRCS
        ./*.cc 
		./*.cpp 

        )

#add_library(${PROJECT_NAME}  SHARED   ${ALL_CXX_SRCS}   ) #SHARED  STATIC
add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})
#add_dependencies(${PROJECT_NAME} common)
#add_dependencies(${PROJECT_NAME} clib)

set(ALL_LIB  	pthread
 
	dl
	X11 
	GL 
	glut 
	GLEW

        )


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
