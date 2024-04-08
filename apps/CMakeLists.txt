add_executable(run main.cpp)

target_include_directories(run PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(run PRIVATE
    src-application
)
