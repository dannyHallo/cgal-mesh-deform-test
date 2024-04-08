cmake_minimum_required(VERSION 3.5.0)

project(cgal-mesh-deform-test)

# glfw
# find_package(glfw3 CONFIG REQUIRED)

# # glm
# find_package(glm CONFIG REQUIRED)

# # daxa::daxa
# find_package(daxa CONFIG REQUIRED)

# CGAL::CGAL
find_package(CGAL CONFIG REQUIRED)

set(dep_INCLUDE_DIRS "")
list(APPEND dep_INCLUDE_DIRS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/")

# list(APPEND dep_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/dep/tinyobjloader/")
add_subdirectory(src/)
add_subdirectory(apps/)
