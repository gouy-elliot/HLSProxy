cmake_minimum_required(VERSION 3.10)
project(HLSProxy)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(EX1_EXECUTABLE_NAME "hls-proxy-ex1")
set(EX2_EXECUTABLE_NAME "hls-proxy-ex2")
set(EX3_EXECUTABLE_NAME "hls-proxy-ex3")

add_executable(${EX1_EXECUTABLE_NAME}
        ex1/src/main.cpp
        ex1/include/proxy.h
        ex1/src/proxy.cpp
        ex1/include/url.h
        ex1/src/url.cpp
        ex1/include/client.h
        ex1/src/client.cpp)

add_executable(${EX2_EXECUTABLE_NAME}
        ex2/src/main.cpp
        ex2/include/proxy.h
        ex2/src/proxy.cpp
        ex2/include/url.h
        ex2/src/url.cpp
        ex2/include/client.h
        ex2/src/client.cpp
        ex2/include/manifest_parser.h
        ex2/src/manifest_parser.cpp
        ex2/include/manifest.h
        ex2/src/manifest.cpp)

add_executable(${EX3_EXECUTABLE_NAME}
        ex3/src/main.cpp
        ex3/include/proxy_config.h
        ex3/include/proxy.h
        ex3/src/proxy.cpp
        ex3/include/url.h
        ex3/src/url.cpp
        ex3/include/client.h
        ex3/src/client.cpp
        ex3/include/manifest_parser.h
        ex3/src/manifest_parser.cpp
        ex3/include/manifest.h
        ex3/src/manifest.cpp)

target_include_directories(${EX1_EXECUTABLE_NAME} PRIVATE ex1/include/)
target_include_directories(${EX2_EXECUTABLE_NAME} PRIVATE ex2/include/)
target_include_directories(${EX3_EXECUTABLE_NAME} PRIVATE ex3/include/)
