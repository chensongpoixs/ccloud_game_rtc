project(cmediasoup_client)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS "#########################")
MESSAGE(STATUS "${PROJECT_NAME}")
file(GLOB ALL_CXX_SRCS
                       ./*.cpp  ./*.cc
					   ./sdp/*.cpp  ./sdp/*.cc
)

add_library(${PROJECT_NAME} ${ALL_CXX_SRCS})
target_link_libraries(${PROJECT_NAME} )