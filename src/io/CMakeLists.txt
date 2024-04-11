add_library(src-io STATIC
    Io.cpp
)

target_include_directories(src-io PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)
