project(vaapi_h264_encode_demo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")


INCLUDE_DIRECTORIES(
	/usr/include
	/usr/local/include
	/usr/include/libdrm
	./common
  #      /usr/lib/jvm/java-8-openjdk-amd64/include
	#/usr/lib/jvm/java-8-openjdk-amd64/include/linux
        )

file(GLOB ALL_CXX_SRCS
      #  ../clib/*.cpp 
      #  ../clib/*.cc 
      #  ../clib/*.c
      #  ../clib/*.h
      #  ../cloud_thrift/ccloudRender_Agent_mgr.cpp
      #  #CloudAgent.h
      #  ../cloud_thrift/CloudAgent.cpp
      # # CloudAgent_constants.h
      #  ../cloud_thrift/CloudAgent_constants.cpp
      # # CloudAgent_types.h
      #  ../cloud_thrift/CloudAgent_types.cpp
      #  #CloudRender.h
      #  ../cloud_thrift/CloudRender.cpp
      #  #CloudRender_client.h
      #  ../cloud_thrift/CloudRender_client.cpp
      #  #CloudRender_constants.h
      #  ../cloud_thrift/CloudRender_constants.cpp
      #  #CloudRender_types.h
      #  ../cloud_thrift/CloudRender_types.cpp
      #  ../cloud_thrift/CloudServer_constants.cpp 
      #  ../cloud_thrift/CloudServer_types.cpp 
      #  ../common/*.cpp  
      #  ../common/*.cc
      #  ../common/*.h
      #  ../common/cclient_msg/*.cc
      #  ../common/cclient_msg/*.h
      #  ../common/cclient_msg/*.cpp
      #  ./*.cpp  ./*.cc
		./common/*.h
		./common/*.c 
		./encode/h264encode.c
        )
#add_library(${PROJECT_NAME}  SHARED   ${ALL_CXX_SRCS}   ) #
add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})
#add_dependencies(${PROJECT_NAME} cloud_thrift)
#add_dependencies(${PROJECT_NAME} common)
#add_dependencies(${PROJECT_NAME} clib)

set(ALL_LIB  	pthread
	m
	#libva.a
	#x11
	X11 
	va-x11 
	drm 
	va-drm 
	va-wayland #.so.2 => /lib/x86_64-linux-gnu/libva-wayland.so.2 (0x00007fa7871f5000)
	va #.so.2 => /lib/x86_64-linux-gnu/libva.so.2 (0x00007fa7871ca000)
	wayland-client #.so.0 => /lib/x86_64-linux-gnu/libwayland-client.so.0 (0x00007fa7871b9000)
	#pthread #.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007fa787196000)
	#libm.so #.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fa787047000)
	#libc.so #.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fa786e55000)
	xcb #.so.1 => /lib/x86_64-linux-gnu/libxcb.so.1 (0x00007fa786e2b000)
 	dl #.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fa786e23000)
	Xext #.so.6 => /lib/x86_64-linux-gnu/libXext.so.6 (0x00007fa786e0e000)
	Xfixes #.so.3 => /lib/x86_64-linux-gnu/libXfixes.so.3 (0x00007fa786e06000)
	ffi #.so.7 => /lib/x86_64-linux-gnu/libffi.so.7 (0x00007fa786dfa000)
	#/lib64/ld-linux-x86-64.so.2 (0x00007fa7873f6000)
	Xau #.so.6 => /lib/x86_64-linux-gnu/libXau.so.6 (0x00007fa786df4000)
	Xdmcp #.so.6 => /lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007fa786dea000)
	#bsd #.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0
        )


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
