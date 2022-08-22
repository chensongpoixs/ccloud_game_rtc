project(webrtc_demo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")

#set(CMAKE_C_COMPILER "/home/chensong/Work/webrtc/src/third_party/llvm-build/Release+Asserts/bin/clang")

#set(CMAKE_CXX_COMPILER "/home/chensong/Work/webrtc/src/third_party/llvm-build/Release+Asserts/bin/clang++")
SET(CMAKE_VERBOSE_MAKEFILE ON) 
#set(CMAKE_CXX_FLAGS         "-std=c++11 -g -fpermissive -fno-rtti -nostdinc++")  #注意调试的话,此行代码一定要添加!



set(CMAKE_CXX_FLAGS         "-std=c++11 -g    -fno-rtti  ")  #注意调试的话,此行代码一定要添加!
add_definitions(-D WEBRTC_POSIX) # -fPIC
add_definitions(-D WEBRTC_LINUX)

SET(WEBRTC_DIR /home/chensong/dep/webrtc)

INCLUDE_DIRECTORIES(
		/usr/local/include

${WEBRTC_DIR}/include
${WEBRTC_DIR}/include/third_party/libyuv/include
${WEBRTC_DIR}/include/third_party/abseil-cpp
${WEBRTC_DIR}/include/third_party/jsoncpp/source/include
${WEBRTC_DIR}/include/third_party/jsoncpp/generated
#${WEBRTC_DIR}/include/third_party/googletest/src/googletest/include
${WEBRTC_DIR}/gen
)


LINK_DIRECTORIES(
		/usr/local/lib
		/usr/local/lib64
		/usr/lib/x86_64-linux-gnu
		${PROJECT_SOURCE_DIR}/linux/old_lib
		${WEBRTC_DIR}/lib/
)

#ADD_DEFINITIONS(
#
#-DUSE_UDEV -DUSE_AURA=1
#-DUSE_GLIB=1
#-DUSE_NSS_CERTS=1
#-DUSE_X11=1
#-D_FILE_OFFSET_BITS=64
#-D_LARGEFILE_SOURCE
#-D_LARGEFILE64_SOURCE
#-D_GNU_SOURCE
#-DCR_CLANG_REVISION=\"373424-64a362e7-1\"
#-D__STDC_CONSTANT_MACROS
#-D__STDC_FORMAT_MACROS
#-D_FORTIFY_SOURCE=2
#-D_LIBCPP_ABI_UNSTABLE
#-D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS
#-D_LIBCXXABI_DISABLE_VISIBILITY_ANNOTATIONS
#-D_LIBCPP_ENABLE_NODISCARD
#-DCR_LIBCXX_REVISION=361348
##-DCR_SYSROOT_HASH=bcc994cc6e5d4d6f0eec8b44e7f0a65f5a1a7b90
#-DNDEBUG -DNVALGRIND
#-DDYNAMIC_ANNOTATIONS_ENABLED=0
#-DWEBRTC_ENABLE_PROTOBUF=1
#-DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
#-DRTC_ENABLE_VP9
#-DHAVE_SCTP
#-DWEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=1
#-DWEBRTC_POSIX
#-DWEBRTC_LINUX
#-DABSL_ALLOCATOR_NOTHROW=1
#-DABSL_FLAGS_STRIP_NAMES=0
#-DGTEST_API_=
#-DGTEST_HAS_POSIX_RE=0
#-DGTEST_LANG_CXX11=1
#-DGTEST_HAS_TR1_TUPLE=0
#-DGTEST_HAS_ABSL=1
#-DHAVE_WEBRTC_VIDEO
########################################################
#-std=c++14
#-fno-exceptions
#-fno-rtti
##-nostdinc++
#-fvisibility-inlines-hidden
#-Wnon-virtual-dtor
#-Woverloaded-virtual
#-Wno-deprecated-declarations
#-Wno-deprecated-declarations
#-Wno-undef
#
##-Wl,--fatal-warnings -fPIC -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,-z,defs -Wl,--as-needed -fuse-ld=lld -Wl,--icf=all -Wl,--color-diagnostics -m64 -Werror -Wl,-O2 -Wl,--gc-sections -rdynamic
#
#
#
#
#
#
#
#)

file(GLOB ALL_CXX_SRCS
                       ./*.cpp  ./*.cc
)

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})
#add_dependencies(${PROJECT_NAME} common)
#add_dependencies(${PROJECT_NAME} clib)

set(ALL_LIB
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
				
				)


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})
