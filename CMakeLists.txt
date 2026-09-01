cmake_minimum_required(VERSION 3.10)

if(NOT WIN32)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_C_COMPILER "/usr/bin/clang")
endif()

project(OSSS VERSION 1.0)

# Settings C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    cryptopp-cmake
    GIT_REPOSITORY https://github.com/abdes/cryptopp-cmake.git
    GIT_TAG CRYPTOPP_8_9_0
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    mongoose
    GIT_REPOSITORY https://github.com/cesanta/mongoose.git
    GIT_TAG 7.23
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(cryptopp-cmake mongoose)

add_library(mongoose STATIC)
target_sources(mongoose PRIVATE ${mongoose_SOURCE_DIR}/mongoose.c)
target_include_directories(mongoose PUBLIC ${mongoose_SOURCE_DIR})

set(MONGOOSE_INCLUDE_ROOT ${CMAKE_CURRENT_BINARY_DIR}/mongoose_includes)
file(COPY ${mongoose_SOURCE_DIR}/mongoose.h DESTINATION ${MONGOOSE_INCLUDE_ROOT}/mongoose)

# Automatic search for all sources
set(SRC_FILES
    MVC/main.cpp
    MVC/Model/FileUtility.cpp
    MVC/Model/WebUtility.cpp
    MVC/Model/HashUtility.cpp
)

set(HEADER_FILES
    MVC/Model/Headers/FileUtility.h
    MVC/Model/Headers/WebUtility.h
    MVC/Model/Headers/HashUtility.h
)

# Create an executable file
add_executable(osss ${SRC_FILES} ${HEADER_FILES})

target_include_directories(osss PRIVATE ${MONGOOSE_INCLUDE_ROOT})

target_link_libraries(osss PRIVATE 
    cryptopp::cryptopp
    mongoose
)

# Platform-specific settings
if(WIN32)
    set(PLATFORM "windows")
    target_link_libraries(osss ws2_32)
    set_target_properties(osss PROPERTIES SUFFIX ".exe")
elseif(APPLE)
    set(PLATFORM "osx")
    set_target_properties(osss PROPERTIES SUFFIX ".bin")
else(LINUX)
    set(PLATFORM "linux")
    set_target_properties(osss PROPERTIES SUFFIX ".bin")
endif()

set_target_properties(osss PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${PLATFORM}/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${PLATFORM}/release"
)