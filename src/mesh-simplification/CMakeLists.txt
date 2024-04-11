add_library(src-mesh-simplification STATIC
    MeshSimplification.cpp
)

target_include_directories(src-mesh-simplification PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-mesh-simplification PRIVATE
    src-io
    CGAL::CGAL
)
