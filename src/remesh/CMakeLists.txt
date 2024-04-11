add_library(src-remesh STATIC
    Remesh.cpp
)

target_include_directories(src-remesh PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-remesh PRIVATE
    src-io
    CGAL::CGAL
)
