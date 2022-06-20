project(cx11_demo)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS(-pipe -O2 -pedantic -Wall -W -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Winline -Wdisabled-optimization -Wno-missing-field-initializers -g    -std=c99    -c  )

#SET(EXTRA_INC_DIR /home/ext)

INCLUDE_DIRECTORIES(
        /usr/local/include
        #					${EXTRA_INC_DIR}/include
        #					${EXTRA_INC_DIR}/include/mysql
        #					${PROJECT_SOURCE_DIR}/clib
        #					${PROJECT_SOURCE_DIR}/common
        #					${PROJECT_SOURCE_DIR}/common/cmsg
        #					${PROJECT_SOURCE_DIR}/common/cmessage
        #					#${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}
)




#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux/lib )
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/linux)

LINK_DIRECTORIES(
        /usr/local/lib
        #/usr/local/lib64
        #/usr/lib64/mysql/
        #/usr/lib/x86_64-linux-gnu
        #${PROJECT_SOURCE_DIR}/linux/lib
)

file(GLOB ALL_CXX_SRCS
        ./*.cpp  ./*.cc
        ./*.c
        )

add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS})

set(ALL_LIB  	pthread
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

        )


target_link_libraries(${PROJECT_NAME} ${ALL_LIB})

#add_subdirectory(clib)
#add_subdirectory(common)
#add_subdirectory(gateway)
#add_subdirectory(Robot)
