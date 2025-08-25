cmake_minimum_required(VERSION 3.10)
project(OSSS VERSION 1.0)

# Settings C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CRYPTOPP_SOURCES "${CMAKE_SOURCE_DIR}/lib/cryptopp/sources" CACHE PATH "Path to local Crypto++ sources")
add_subdirectory(lib/cryptopp)

# Automatic search for all sources
set(SRC_FILES
    MVC/main.cpp
    MVC/Model/FileUtility.cpp
    MVC/Model/WebUtility.cpp
)

set(LIB_FILES
    lib/mongoose/mongoose.c
)

# Create an executable file
add_executable(osss ${SRC_FILES} ${LIB_FILES})

# Directories include
target_include_directories(osss PRIVATE
    MVC
    MVC/Model/Header
    lib/mongoose
    lib/cryptopp
)

# Link cryptopp library
target_link_libraries(osss PRIVATE cryptopp)

set_target_properties(osss PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(osss ws2_32)
    set_target_properties(osss PROPERTIES SUFFIX ".exe")
else()
    set_target_properties(osss PROPERTIES SUFFIX ".bin")
endif()
