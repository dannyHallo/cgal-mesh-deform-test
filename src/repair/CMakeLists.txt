add_library(src-repair STATIC
    Repair.cpp
)

target_include_directories(src-repair PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-repair PRIVATE
    src-io
    CGAL::CGAL
)
