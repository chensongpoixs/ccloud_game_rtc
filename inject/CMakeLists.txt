project(cloud_inject)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)


set(CMAKE_C_FLAGS         "--std=gnu99 -ggdb")  #注意调试的话,此行代码一定要添加!