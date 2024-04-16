add_library(src-benchmark STATIC
    Benchmark.cpp
)

target_include_directories(src-benchmark PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-benchmark PRIVATE
    src-io
    CGAL::CGAL
)
