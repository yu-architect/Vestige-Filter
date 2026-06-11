cmake_minimum_required(VERSION 3.15)
project(VestigeFilter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/SocketServer.cpp
    src/LogGenerator.cpp
)

add_executable(VestigeFilter ${SOURCES})

if(WIN32)
    target_link_libraries(VestigeFilter ws2_32)
endif()
