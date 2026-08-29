cmake_minimum_required(VERSION 3.10)
project(OSSS VERSION 1.0)

include(FetchContent)

# Settings C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

set(MONGOOSE_INCLUDE_ROOT ${CMAKE_CURRENT_BINARY_DIR}/mongoose_includes)
file(MAKE_DIRECTORY ${MONGOOSE_INCLUDE_ROOT}/mongoose)

file(CREATE_LINK
    ${mongoose_SOURCE_DIR}/mongoose.h
    ${MONGOOSE_INCLUDE_ROOT}/mongoose/mongoose.h
    SYMBOLIC
)

target_include_directories(mongoose PUBLIC
    $<BUILD_INTERFACE:${MONGOOSE_INCLUDE_ROOT}>
)

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
)

# Create an executable file
add_executable(osss ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(osss PRIVATE 
    cryptopp::cryptopp
    mongoose
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(osss ws2_32)
    set_target_properties(osss PROPERTIES SUFFIX ".exe")
else()
    set_target_properties(osss PROPERTIES SUFFIX ".bin")
endif()
