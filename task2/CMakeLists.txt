project (task2)

find_package(OpenGL REQUIRED)

# task 2
add_executable(task2
        main.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.cpp
        ${CMAKE_SOURCE_DIR}/common/shader.hpp
        )
target_link_libraries(task2 ${ALL_LIBS})
# Xcode and Visual working directories
set_target_properties(task2 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/task2/")
create_target_launcher(task2 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/task2/")

add_custom_command(
        TARGET task2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/task2${CMAKE_EXECUTABLE_SUFFIX}"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

