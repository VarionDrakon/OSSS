cmake_minimum_required(VERSION 3.10)

# Create the lib mongoose
add_library(mongoose STATIC mongoose.c)

# Set include directories
target_include_directories(mongoose PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(mongoose ws2_32)
endif()